import numpy as np

from CrystalField import CrystalField, Function


def makeWorkspace(xArray, yArray):
    """Create a workspace that doesn't appear in the ADS"""
    from mantid.api import AlgorithmManager
    alg = AlgorithmManager.createUnmanaged('CreateWorkspace')
    alg.initialize()
    alg.setChild(True)
    alg.setProperty('DataX', xArray)
    alg.setProperty('DataY', yArray)
    alg.setProperty('OutputWorkspace', 'dummy')
    alg.execute()
    return alg.getProperty('OutputWorkspace').value


def get_parameters_for_add(cf, new_ion_index):
    """get params from crystalField object to append"""
    params = {}
    ion_prefix = 'ion%s.' % new_ion_index
    for bparam in CrystalField.field_parameter_names:
        params[ion_prefix + bparam] = cf[bparam]
    return params


def get_parameters_for_add_from_multisite(cfms, new_ion_index):
    """get params from crystalFieldMultiSite object to append"""
    params = {}
    for i in range(len(cfms.Ions)):
        ion_prefix = 'ion%s.' % (new_ion_index + i)
        if cfms._isMultiSite():
            existing_prefix = 'ion%s.' % i
        else:
            existing_prefix = ''
        for bparam in CrystalField.field_parameter_names:
            params[ion_prefix + bparam] = cfms[existing_prefix + bparam]
        return params


def read_and_del(key, dict):
    value = None
    if key in dict:
        value = dict[key]
        del dict[key]
    return value, dict


def iterable_to_string(iterable):
    values_as_string = ""
    for element in iterable:
        values_as_string += ","
        values_as_string += element
    values_as_string = values_as_string[1:]
    return values_as_string


class CrystalFieldMultiSite(object):

    def __init__(self, Ions, Symmetries, **kwargs):
        from collections import OrderedDict

        self._makeFunction()

        backgroundPeak, kwargs = read_and_del('BackgroundPeak', kwargs)
        background, kwargs = read_and_del('Background', kwargs)
        if background is not None or backgroundPeak is not None:
            self._setBackground(backgroundPeak, background)

        self.Ions = Ions
        self.Symmetries = Symmetries
        self._plot_window = {}
        self.chi2 = None

        parameter_dict, kwargs = read_and_del('parameters', kwargs)
        attribute_dict, kwargs = read_and_del('attributes', kwargs)

        kwargs = self._setMandatoryArguments(kwargs)

        self._abundances = OrderedDict()
        abundances, kwargs = read_and_del('abundances', kwargs)
        self._makeAbundances(abundances)

        self._setRemainingArguments(kwargs)

        if attribute_dict is not None:
            for name, value in attribute_dict.items():
                self.function.setAttributeValue(name, value)
        if parameter_dict is not None:
            for name, value in parameter_dict.items():
                self.function.setParameter(name, value)

    def _setMandatoryArguments(self, kwargs):
        if 'Temperatures' in kwargs:
            self.Temperatures = kwargs['Temperatures']
            del kwargs['Temperatures']
            if 'FWHMs' in kwargs:
                self.FWHMs = kwargs['FWHMs']
                del kwargs['FWHMs']
            elif 'ResolutionModel' in kwargs:
                self.ResolutionModel = kwargs['ResolutionModel']
                del kwargs['ResolutionModel']
            else:
                raise RuntimeError("If temperatures are set, must also set FWHMs or ResolutionModel")
        return kwargs

    def _setRemainingArguments(self, kwargs):
        possible_args = ['ToleranceEnergy', 'ToleranceIntensity', 'NPeaks', 'FWHMVariation', 'FixAllPeaks',
                         'PeakShape', 'PhysicalProperty']
        for attribute in possible_args:
            value, kwargs = read_and_del(attribute, kwargs)
            if value is not None:
                setattr(self, attribute, value)

        for key in kwargs:  # Crystal field parameters remain - must be set last
            self.function.setParameter(key, kwargs[key])

    def _isMultiSite(self):
        return len(self.Ions) > 1

    def _makeFunction(self):
        from mantid.simpleapi import FunctionFactory
        self.function = FunctionFactory.createFunction('CrystalFieldFunction')

    def getParameter(self, param):
        self.function.getParameterValue(param)

    def _getSpectrumTwoArgs(self, arg1, arg2):
        if isinstance(arg1, int):
            i = arg1
            ws = arg2
            ws_index = 0
            if self.Temperatures[i] < 0:
                raise RuntimeError('You must first define a temperature for the spectrum')
        elif isinstance(arg2, int):
            i = 0
            ws = arg1
            ws_index = arg2
        else:
            raise TypeError('expected int for one argument in GetSpectrum, got %s and %s' %
                            (arg1.__class__.__name__, arg2.__class__.__name__))

        if isinstance(ws, list) or isinstance(ws, np.ndarray):
            ws = self._convertToWS(ws)

        return self._calcSpectrum(i, ws, ws_index)

    def getSpectrum(self, *args):
        """
        Get a specified spectrum calculated with the current field and peak parameters.

        Alternatively can be called getSpectrum(workspace, ws_index). Spectrum index is assumed zero.

        Examples:
            cf.getSpectrum(1, ws, 5) # Calculate the second spectrum using the x-values from the 6th spectrum
                                     # in workspace ws.
            cf.getSpectrum(ws) # Calculate the first spectrum using the x-values from the 1st spectrum
                               # in workspace ws.
            cf.getSpectrum(ws, 3) # Calculate the first spectrum using the x-values from the 4th spectrum
                                  # in workspace ws.
            cf.getSpectrum(3, ws) # Calculate the third spectrum using the x-values from the 1st spectrum
                                  # in workspace ws.

        @return: A tuple of (x, y) arrays
        """
        if len(args) == 3:
            ws = args[1]
            if self.Temperatures[args[0]] < 0:
                raise RuntimeError('You must first define a temperature for the spectrum')
            if isinstance(ws, list) or isinstance(ws, np.ndarray):
                ws = self._convertToWS(ws)
            return self._calcSpectrum(args[0], ws, args[2])

        elif len(args) == 1:
            ws = args[0]
            if isinstance(ws, list) or isinstance(ws, np.ndarray):
                ws = self._convertToWS(ws)
            return self._calcSpectrum(0, ws, 0)

        elif len(args) == 2:
            return self._getSpectrumTwoArgs(*args)
        else:
            raise RuntimeError('getSpectrum expected 1-3 arguments, got %s' % len(args))


    def _convertToWS(self, wksp_list):
        """converts a list or numpy array to workspace"""
        xArray = wksp_list
        yArray = np.zeros_like(xArray)
        return makeWorkspace(xArray, yArray)

    def _calcSpectrum(self, i, workspace, ws_index, funstr=None):
        """Calculate i-th spectrum.

        @param i: Index of a spectrum or function string
        @param workspace: A workspace used to evaluate the spectrum function.
        @param ws_index:  An index of a spectrum in workspace to use.
        """
        from mantid.api import AlgorithmManager
        alg = AlgorithmManager.createUnmanaged('EvaluateFunction')
        alg.initialize()
        alg.setChild(True)
        alg.setProperty('Function', self.makeSpectrumFunction(i))
        alg.setProperty("InputWorkspace", workspace)
        alg.setProperty('WorkspaceIndex', ws_index)
        alg.setProperty('OutputWorkspace', 'dummy')
        alg.execute()
        out = alg.getProperty('OutputWorkspace').value
        # Create copies of the x and y because `out` goes out of scope when this method returns
        # and x and y get deallocated
        return np.array(out.readX(0)), np.array(out.readY(1))

    def makeSpectrumFunction(self, i=0):
        """Form a definition string for the CrystalFieldSpectrum function
        @param i: Index of a spectrum.
        """
        if self.NumberOfSpectra == 1:
            return str(self.function)
        else:
            funs = self.function.createEquivalentFunctions()
            return str(funs[i])

    def _makeAbundances(self, abundances):
        """Create dict for ion intensity scalings"""
        if abundances is not None:
            for ion_index in range(len(self.Ions)):
                self._abundances['ion%s' % ion_index]  = abundances[ion_index]
            max_ion = max(self._abundances, key=lambda key: self._abundances[key])
            ties = {}
            for ion in self._abundances.keys():
                if ion is not max_ion:
                    factor = self._abundances[ion] / self._abundances[max_ion]
                    tie_key = ion + '.IntensityScaling'
                    tie_value = str(factor) + '*' + max_ion + ".IntensityScaling"
                    ties[tie_key] = tie_value
                    self.ties(ties)
        else:
            for ion_index in range(len(self.Ions)):
                self._abundances['ion%s' % ion_index]  = 1.0

    def update(self, func):
        """
        Update values of the fitting parameters.
        @param func: A IFunction object containing new parameter values.
        """
        self.function = func

    def fix(self, *args):
        for a in args:
            self.function.fixParameter(a)

    def ties(self, *args, **kwargs):
        """
        Set ties on the field parameters.

        @param kwargs: Ties as name=value pairs: name is a parameter name,
            the value is a tie string or a number. For example:
                tie(B20 = 0.1, IB23 = '2*B23')
        """
        for arg in args:
            if isinstance(arg, dict):
                kwargs.update(arg)
            else:
                raise TypeError("")
        for tie in kwargs:
            self.function.tie(tie, str(kwargs[tie]))

    def constraints(self, *args):
        """
        Set constraints for the field parameters.

        @param args: A list of constraints. For example:
                constraints('B00 > 0', '0.1 < B43 < 0.9')
        """
        self.function.addConstraints(','.join(args))

    def plot(self, i=0, workspace=None, ws_index=0, name=None):
        """Plot a spectrum. Parameters are the same as in getSpectrum(...)"""
        from mantidplot import plotSpectrum
        from mantid.api import AlgorithmManager
        createWS = AlgorithmManager.createUnmanaged('CreateWorkspace')
        createWS.initialize()

        xArray, yArray = self.getSpectrum(i, workspace, ws_index)
        ws_name = name if name is not None else 'CrystalFieldMultiSite_%s' % self.Ions

        if isinstance(i, int):
            if workspace is None:
                if i > 0:
                    ws_name += '_%s' % i
                createWS.setProperty('DataX', xArray)
                createWS.setProperty('DataY', yArray)
                createWS.setProperty('OutputWorkspace', ws_name)
                createWS.execute()
                plot_window = self._plot_window[i] if i in self._plot_window else None
                self._plot_window[i] = plotSpectrum(ws_name, 0, window=plot_window, clearWindow=True)
            else:
                ws_name += '_%s' % workspace
                if i > 0:
                    ws_name += '_%s' % i
                createWS.setProperty('DataX', xArray)
                createWS.setProperty('DataY', yArray)
                createWS.setProperty('OutputWorkspace', ws_name)
                createWS.execute()
                plotSpectrum(ws_name, 0)
        else:
            ws_name += '_%s' % i
            createWS.setProperty('DataX', xArray)
            createWS.setProperty('DataY', yArray)
            createWS.setProperty('OutputWorkspace', ws_name)
            createWS.execute()
            plotSpectrum(ws_name, 0)

    def _setBackground(self, peak=None, background=None):
        """
        Set background function(s).

        Can provide one argument or both. Each argument can be a string or FunctionWrapper object.
        Can also pass two functions as one argument by passing a single string or CompositeFunctionWrapper.

        Examples:
        setBackground(Gaussian())
        setBackground(background=LinearBackground())
        setBackground(peak='name=Gaussian,Height=1', background='name=LinearBackground')
        setBackground(Gaussian(), 'name=LinearBackground')
        setBackground(Gaussian() + LinearBackground())
        setBackground('name=Gaussian,Height=0,PeakCentre=1,Sigma=0;name=LinearBackground,A0=0,A1=0')

        @param peak: A function passed as the peak. Can be a string or FunctionWrapper e.g.
                'name=Gaussian,Height=0,PeakCentre=1,Sigma=0' or Gaussian(PeakCentre=1)
        @param background: A function passed as the background. Can be a string or FunctionWrapper e.g.
                'name=LinearBackground,A0=1' or LinearBackground(A0=1)
        """
        from mantid.fitfunctions import FunctionWrapper, CompositeFunctionWrapper

        self._background = Function(self.function, prefix='bg.')
        property_name = "peak"
        if background is not None and peak is None: #swap arguments, then do single arg case
            peak = background
            background = None
            property_name = "background"
        if peak is not None and background is None: #single arg case
            if isinstance(peak, str):
                number_of_functions = peak.count(';') + 1
                if number_of_functions == 2:
                    peak, background = peak.split(';')
                elif number_of_functions == 1:
                    setattr(self._background, property_name, Function(self.function, prefix='bg.'))
                    self.function.setAttributeValue('Background', '%s' % peak)
                else:
                    raise ValueError("argument passed to background must have exactly 1 or 2 functions, got %s"
                                     % number_of_functions)

            elif isinstance(peak, CompositeFunctionWrapper):
                if len(peak) == 2:
                    peak, background = str(peak).split(';')
                else:
                    raise ValueError("composite function passed to background must have exactly 2 functions, got %s"
                                     % len(peak))
            elif isinstance(peak, FunctionWrapper):
                setattr(self._background, property_name, Function(self.function, prefix='bg.'))
                self.function.setAttributeValue('Background', '%s' % peak)
            else:
                raise TypeError("background argument(s) must be string or function object(s)")

        if background is not None and peak is not None:
            self._background.peak = Function(self.function, prefix='bg.f0.')
            self._background.background = Function(self.function, prefix='bg.f1.')
            self.function.setAttributeValue('Background', '%s;%s' % (peak, background))

    def _combine_multisite(self, other):
        """Used to add two CrystalFieldMultiSite"""
        ions = self.Ions + other.Ions
        symmetries = self.Symmetries + other.Symmetries
        abundances = list(self._abundances.values()) + list(other._abundances.values())
        params = get_parameters_for_add_from_multisite(self, 0)
        params.update(get_parameters_for_add_from_multisite(other, len(self.Ions)))
        new_cf = CrystalFieldMultiSite(Ions=ions, Symmetries=symmetries, Temperatures=self.Temperatures,
                                       FWHMs=self.FWHMs, parameters=params, abundances=abundances)
        return new_cf

    def __getitem__(self, item):
        if self.function.hasAttribute(item):
            return self.function.getAttributeValue(item)
        else:
            return self.function.getParameterValue(item)

    def __setitem__(self, key, value):
        self.function.setParameter(key, value)

    def __add__(self, other):
        scale_factor = 1.0
        if hasattr(other, 'abundance'): # is CrystalFieldSite
            scale_factor = other.abundance
            other = other.crystalField
        elif isinstance(other, CrystalFieldMultiSite):
            return self._combine_multisite(other)
        if not isinstance(other, CrystalField):
            raise TypeError('Unsupported operand type(s) for +: CrystalFieldMultiSite and %s'% other.__class__.__name__)
        ions = self.Ions + [other.Ion]
        symmetries = self.Symmetries + [other.Symmetry]
        abundances = list(self._abundances.values()) + [scale_factor]
        params = get_parameters_for_add_from_multisite(self, 0)
        params.update(get_parameters_for_add(other, len(self.Ions)))
        new_cf = CrystalFieldMultiSite(Ions=ions, Symmetries=symmetries, Temperatures=self.Temperatures,
                                       FWHMs=self.FWHMs, parameters=params, abundances=abundances)
        return new_cf

    def __radd__(self, other):
        scale_factor = 1.0
        if hasattr(other, 'abundance'):  # is CrystalFieldSite
            scale_factor = other.abundance
            other = other.crystalField
        if not isinstance(other, CrystalField):
            raise TypeError('Unsupported operand type(s) for +: CrystalFieldMultiSite and %s'% other.__class__.__name__)
        ions = [other.Ion] + self.Ions
        symmetries = [other.Symmetry] + self.Symmetries
        abundances = [scale_factor] + list(self._abundances.values())
        params = get_parameters_for_add(other, 0)
        params.update(get_parameters_for_add_from_multisite(self, 1))
        new_cf = CrystalFieldMultiSite(Ions=ions, Symmetries=symmetries, Temperatures=self.Temperatures,
                                       FWHMs=self.FWHMs, parameters=params, abundances=abundances)
        return new_cf

    @property
    def background(self):
        return self._background

    @property
    def Ions(self):
        string_ions = self.function.getAttributeValue('Ions')
        string_ions = string_ions[1:-1]
        return string_ions.split(",")

    @Ions.setter
    def Ions(self, value):
        if isinstance(value, str):
            self.function.setAttributeValue('Ions', value)
        else:
            self.function.setAttributeValue('Ions', iterable_to_string(value))

    @property
    def Symmetries(self):
        string_symmetries = self.function.getAttributeValue('Symmetries')
        string_symmetries = string_symmetries[1:-1]
        return string_symmetries.split(",")

    @Symmetries.setter
    def Symmetries(self, value):
        if isinstance(value, str):
            self.function.setAttributeValue('Symmetries', value)
        else:
            self.function.setAttributeValue('Symmetries', iterable_to_string(value))

    @property
    def ToleranceEnergy(self):
        """Get energy tolerance"""
        return self.function.getAttributeValue('ToleranceEnergy')

    @ToleranceEnergy.setter
    def ToleranceEnergy(self, value):
        """Set energy tolerance"""
        self.function.setAttributeValue('ToleranceEnergy', float(value))

    @property
    def ToleranceIntensity(self):
        """Get intensity tolerance"""
        return self.function.getAttributeValue('ToleranceIntensity')

    @ToleranceIntensity.setter
    def ToleranceIntensity(self, value):
        """Set intensity tolerance"""
        self.function.setAttributeValue('ToleranceIntensity', float(value))

    @property
    def Temperatures(self):
        return list(self.function.getAttributeValue("Temperatures"))

    @Temperatures.setter
    def Temperatures(self, value):
        self.function.setAttributeValue('Temperatures', value)

    @property
    def FWHMs(self):
        fwhm = self.function.getAttributeValue('FWHMs')
        nDatasets = len(self.Temperatures)
        if len(fwhm) != nDatasets:
            return list(fwhm) * nDatasets
        return list(fwhm)

    @FWHMs.setter
    def FWHMs(self, value):
        if len(value) == 1:
            value = value[0]
            value = [value] * len(self.Temperatures)
        self.function.setAttributeValue('FWHMs', value)

    @property
    def FWHMVariation(self):
        return self.function.getAttributeValue('FWHMVariation')

    @FWHMVariation.setter
    def FWHMVariation(self, value):
        self.function.setAttributeValue('FWHMVariation', float(value))

    @property
    def FixAllPeaks(self):
        return self.function.getAttributeValue('FixAllPeaks')

    @FixAllPeaks.setter
    def FixAllPeaks(self, value):
        self.function.setAttributeValue('FixAllPeaks', value)

    @property
    def PeakShape(self):
        return self.function.getAttributeValue('PeakShape')

    @PeakShape.setter
    def PeakShape(self, value):
        self.function.setAttributeValue('PeakShape', value)

    @property
    def NumberOfSpectra(self):
        return self.function.getNumberDomains()

    @property
    def NPeaks(self):
        return self.function.getAttributeValue('NPeaks')

    @NPeaks.setter
    def NPeaks(self, value):
        self.function.setAttributeValue('NPeaks', value)
