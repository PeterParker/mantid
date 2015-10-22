#pylint: disable=no-init
from IndirectImport import *
if is_supported_f2py_platform():
    QLr     = import_f2py("QLres")
    QLd     = import_f2py("QLdata")
    Qse     = import_f2py("QLse")
    Que     = import_f2py("Quest")
    resnorm = import_f2py("ResNorm")
else:
    logger.error('F2Py functionality not currently available on your platform.')

from mantid.api import PythonAlgorithm, AlgorithmFactory, MatrixWorkspaceProperty, PropertyMode, WorkspaceGroupProperty
from mantid.kernel import StringListValidator, Direction
from mantid.simpleapi import *
from mantid import config, logger
import os
import numpy as np

class QLRun(PythonAlgorithm):

    _program = None
    _samWS = None
    _resWS = None
    _resnormWS = None
    _e_min = None
    _e_max = None
    _sam_bins = None
    _res_bins = None
    _elastic = None
    _background = None
    _width = None
    _res_norm = None
    _wfile = None
    _Loop = None
    _Save = None
    _Plot = None

    def category(self):
        return "Workflow\\MIDAS;PythonAlgorithms"

    def summary(self):
        return "This algorithm runs the Fortran QLines programs which fits a Delta function of"+\
               " amplitude 0 and Lorentzians of amplitude A(j) and HWHM W(j) where j=1,2,3. The"+\
               " whole function is then convoled with the resolution function."

    def version(self):
        return 1

    def PyInit(self):
        self.declareProperty(name='Program', defaultValue='QL',
                             validator=StringListValidator(['QL','QSe']),
                             doc='The type of program to run (either QL or QSe)')

        self.declareProperty(MatrixWorkspaceProperty('SampleWorkspace', '', direction=Direction.Input),
                             doc='Name of the Sample input Workspace')

        self.declareProperty(MatrixWorkspaceProperty('ResolutionWorkspace', '', direction=Direction.Input),
                             doc='Name of the resolution input Workspace')

        self.declareProperty(MatrixWorkspaceProperty('ResNormWorkspace', '',
                             optional=PropertyMode.Optional, direction=Direction.Input),
                             doc='Name of the ResNorm input Workspace')

        self.declareProperty(name='MinRange', defaultValue=-0.2,
                             doc='The start of the fit range. Default=-0.2')

        self.declareProperty(name='MaxRange', defaultValue=0.2,
                             doc='The end of the fit range. Default=0.2')

        self.declareProperty(name='SampleBins', defaultValue=1,
                             doc='The number of sample bins')

        self.declareProperty(name='ResolutionBins', defaultValue=1,
                             doc='The number of resolution bins')

        self.declareProperty(name='Elastic', defaultValue=True,
                             doc='Fit option for using the elastic peak')

        self.declareProperty(name='Background', defaultValue='Flat',
                             validator=StringListValidator(['Sloping','Flat','Zero']),
                             doc='Fit option for the type of background')

        self.declareProperty(name='FixedWidth', defaultValue=True,
                             doc='Fit option for using FixedWidth')

        self.declareProperty(name='UseResNorm', defaultValue=False,
                             doc='fit option for using ResNorm')

        self.declareProperty(name='WidthFile', defaultValue='', doc='The name of the fixedWidth file')

        self.declareProperty(name='Loop', defaultValue=True, doc='Switch Sequential fit On/Off')

        self.declareProperty(name='Plot', defaultValue='', doc='Plot options')

        self.declareProperty(name='Save', defaultValue=False, doc='Switch Save result to nxs file Off/On')

        self.declareProperty(WorkspaceGroupProperty('OutputWorkspaceFit', '', direction=Direction.Output),
                             doc='The name of the fit output workspaces')

        self.declareProperty(MatrixWorkspaceProperty('OutputWorkspaceResult', '', direction=Direction.Output),
                             doc='The name of the result output workspaces')

        self.declareProperty(MatrixWorkspaceProperty('OutputWorkspaceProb', '', optional=PropertyMode.Optional,
                             direction=Direction.Output),
                             doc='The name of the probability output workspaces')


    def validateInputs(self):
        self._get_properties()
        issues = dict()

        # Validate fitting range in energy
        if self._e_min > self._e_max:
            issues['MaxRange'] = 'Must be less than EnergyMin'

        return issues


    def _get_properties(self):
        self._program = self.getPropertyValue('Program')
        self._samWS = self.getPropertyValue('SampleWorkspace')
        self._resWS = self.getPropertyValue('ResolutionWorkspace')
        self._resnormWS = self.getPropertyValue('ResNormWorkspace')
        self._e_min = self.getProperty('MinRange').value
        self._e_max = self.getProperty('MaxRange').value
        self._sam_bins = self.getPropertyValue('SampleBins')
        self._res_bins = self.getPropertyValue('ResolutionBins')
        self._elastic = self.getProperty('Elastic').value
        self._background = self.getPropertyValue('Background')
        self._width = self.getProperty('FixedWidth').value
        self._res_norm = self.getProperty('UseResNorm').value
        self._wfile = self.getPropertyValue('WidthFile')
        self._Loop = self.getProperty('Loop').value
        self._Save = self.getProperty('Save').value
        self._Plot = self.getPropertyValue('Plot')


    def PyExec(self):
        #from IndirectImport import run_f2py_compatibility_test, is_supported_f2py_platform

        if is_supported_f2py_platform():
            import IndirectBayes as Main

        run_f2py_compatibility_test()

        from IndirectBayes import CalcErange, GetXYE, ReadNormFile, ReadWidthFile, QLAddSampleLogs, C2Fw, C2Se, QuasiPlot
        from IndirectCommon import getDefaultWorkingDirectory, CheckXrange, CheckAnalysers, getEfixed, GetThetaQ, CheckHistZero, CheckHistSame

        self.log().information('QLRun input')

        erange = [self._e_min, self._e_max]
        nbins = [self._sam_bins, self._res_bins]

        #convert true/false to 1/0 for fortran
        o_el = 1 if self._elastic else 0
        o_w1 = 1 if self._width else 0
        o_res = 1 if self._res_norm else 0

        #fortran code uses background choices defined using the following numbers
        if self._background == 'Sloping':
            o_bgd = 2
        elif self._background == 'Flat':
            o_bgd = 1
        elif self._background == 'Zero':
            o_bgd = 0

        fitOp = [o_el, o_bgd, o_w1, o_res]

        workdir = getDefaultWorkingDirectory()

        array_len = 4096                           # length of array in Fortran
        CheckXrange(erange,'Energy')

        nbin,nrbin = nbins[0], nbins[1]

        logger.information('Sample is ' + self._samWS)
        logger.information('Resolution is ' + self._resWS)

        CheckAnalysers(self._samWS,self._resWS)
        efix = getEfixed(self._samWS)
        theta, Q = GetThetaQ(self._samWS)

        nsam,ntc = CheckHistZero(self._samWS)

        totalNoSam = nsam

        #check if we're performing a sequential fit
        if self._Loop != True:
            nsam = 1

        nres = CheckHistZero(self._resWS)[0]

        if self._program == 'QL':
            if nres == 1:
                prog = 'QLr'                        # res file
            else:
                prog = 'QLd'                        # data file
                CheckHistSame(self._samWS,'Sample',self._resWS,'Resolution')
        elif self._program == 'QSe':
            if nres == 1:
                prog = 'QSe'                        # res file
            else:
                raise ValueError('Stretched Exp ONLY works with RES file')

        logger.information('Version is ' +prog)
        logger.information(' Number of spectra = '+str(nsam))
        logger.information(' Erange : '+str(erange[0])+' to '+str(erange[1]))

        Wy,We = ReadWidthFile(self._width,self._wfile,totalNoSam)
        dtn,xsc = ReadNormFile(self._res_norm,self._resnormWS,totalNoSam)

        fname = self._samWS[:-4] + '_'+ prog
        probWS = fname + '_Prob'
        fitWS = fname + '_Fit'
        wrks=os.path.join(workdir, self._samWS[:-4])
        logger.information(' lptfile : '+wrks+'_'+prog+'.lpt')
        lwrk=len(wrks)
        wrks.ljust(140,' ')
        wrkr=self._resWS
        wrkr.ljust(140,' ')

        # initialise probability list
        if self._program == 'QL':
            prob0 = []
            prob1 = []
            prob2 = []
        xQ = np.array([Q[0]])
        for m in range(1,nsam):
            xQ = np.append(xQ,Q[m])
        xProb = xQ
        xProb = np.append(xProb,xQ)
        xProb = np.append(xProb,xQ)
        eProb = np.zeros(3*nsam)

        group = ''
        for m in range(0,nsam):
            logger.information('Group ' +str(m)+ ' at angle '+ str(theta[m]))
            nsp = m+1
            nout,bnorm,Xdat,Xv,Yv,Ev = CalcErange(self._samWS,m,erange,nbin)
            Ndat = nout[0]
            Imin = nout[1]
            Imax = nout[2]
            if prog == 'QLd':
                mm = m
            else:
                mm = 0
            Nb,Xb,Yb,Eb = GetXYE(self._resWS,mm,array_len)     # get resolution data
            numb = [nsam, nsp, ntc, Ndat, nbin, Imin, Imax, Nb, nrbin]
            rscl = 1.0
            reals = [efix, theta[m], rscl, bnorm]

            if prog == 'QLr':
                nd,xout,yout,eout,yfit,yprob=QLr.qlres(numb,Xv,Yv,Ev,reals,fitOp,
                                                       Xdat,Xb,Yb,Wy,We,dtn,xsc,
                                                       wrks,wrkr,lwrk)
                message = ' Log(prob) : '+str(yprob[0])+' '+str(yprob[1])+' '+str(yprob[2])+' '+str(yprob[3])
                logger.information(message)
            if prog == 'QLd':
                nd,xout,yout,eout,yfit,yprob=QLd.qldata(numb,Xv,Yv,Ev,reals,fitOp,
                                                        Xdat,Xb,Yb,Eb,Wy,We,
                                                        wrks,wrkr,lwrk)
                message = ' Log(prob) : '+str(yprob[0])+' '+str(yprob[1])+' '+str(yprob[2])+' '+str(yprob[3])
                logger.information(message)
            if prog == 'QSe':
                nd,xout,yout,eout,yfit,yprob=Qse.qlstexp(numb,Xv,Yv,Ev,reals,fitOp,\
                                                        Xdat,Xb,Yb,Wy,We,dtn,xsc,\
                                                        wrks,wrkr,lwrk)
            dataX = xout[:nd]
            dataX = np.append(dataX,2*xout[nd-1]-xout[nd-2])
            yfit_list = np.split(yfit[:4*nd],4)
            dataF1 = yfit_list[1]
            if self._program == 'QL':
                dataF2 = yfit_list[2]
            dataG = np.zeros(nd)
            datX = dataX
            datY = yout[:nd]
            datE = eout[:nd]
            datX = np.append(datX,dataX)
            datY = np.append(datY,dataF1[:nd])
            datE = np.append(datE,dataG)
            res1 = dataF1[:nd] - yout[:nd]
            datX = np.append(datX,dataX)
            datY = np.append(datY,res1)
            datE = np.append(datE,dataG)
            nsp = 3
            names = 'data,fit.1,diff.1'
            res_plot = [0, 1, 2]
            if self._program == 'QL':
                datX = np.append(datX,dataX)
                datY = np.append(datY,dataF2[:nd])
                datE = np.append(datE,dataG)
                res2 = dataF2[:nd] - yout[:nd]
                datX = np.append(datX,dataX)
                datY = np.append(datY,res2)
                datE = np.append(datE,dataG)
                nsp += 2
                names += ',fit.2,diff.2'
                res_plot.append(4)
                prob0.append(yprob[0])
                prob1.append(yprob[1])
                prob2.append(yprob[2])

            # create result workspace
            fitWS = fname+'_Workspaces'
            fout = fname+'_Workspace_'+ str(m)

            CreateWorkspace(OutputWorkspace=fout, DataX=datX, DataY=datY, DataE=datE,\
                Nspec=nsp, UnitX='DeltaE', VerticalAxisUnit='Text', VerticalAxisValues=names)

            # append workspace to list of results
            group += fout + ','

        GroupWorkspaces(InputWorkspaces=group,OutputWorkspace=fitWS)

        if self._program == 'QL':
            yPr0 = np.array([prob0[0]])
            yPr1 = np.array([prob1[0]])
            yPr2 = np.array([prob2[0]])
            for m in range(1,nsam):
                yPr0 = np.append(yPr0,prob0[m])
                yPr1 = np.append(yPr1,prob1[m])
                yPr2 = np.append(yPr2,prob2[m])
            yProb = yPr0
            yProb = np.append(yProb,yPr1)
            yProb = np.append(yProb,yPr2)
            probWs = CreateWorkspace(OutputWorkspace=probWS, DataX=xProb, DataY=yProb, DataE=eProb,\
                Nspec=3, UnitX='MomentumTransfer')
            outWS = C2Fw(self._samWS[:-4],fname)
            if self._Plot != 'None':
                QuasiPlot(fname,self._Plot,res_plot,self._Loop)
        if self._program == 'QSe':
            outWS = C2Se(fname)
            if self._Plot != 'None':
                QuasiPlot(fname,self._Plot,res_plot,self._Loop)

        #Add some sample logs to the output workspaces
        CopyLogs(InputWorkspace=self._samWS, OutputWorkspace=outWS)
        QLAddSampleLogs(outWS, self._resWS, prog, self._background, self._elastic, erange,
                        (nbin, nrbin), self._resnormWS, self._wfile)
        CopyLogs(InputWorkspace=self._samWS, OutputWorkspace=fitWS)
        QLAddSampleLogs(fitWS, self._resWS, prog, self._background, self._elastic, erange,
                        (nbin, nrbin), self._resnormWS, self._wfile)

        if self._Save:
            fit_path = os.path.join(workdir,fitWS+'.nxs')
            SaveNexusProcessed(InputWorkspace=fitWS, Filename=fit_path)
            out_path = os.path.join(workdir, outWS+'.nxs')                    # path name for nxs file
            SaveNexusProcessed(InputWorkspace=outWS, Filename=out_path)
            logger.information('Output fit file created : ' + fit_path)
            logger.information('Output paramter file created : ' + out_path)

        self.setProperty('OutputWorkspaceFit', fitWS)
        self.setProperty('OutputWorkspaceResult', outWS)

        if self._program == 'QL':
            self.setProperty('OutputWorkspaceProb', probWS)


if is_supported_f2py_platform():
    # Register algorithm with Mantid
    AlgorithmFactory.subscribe(QLRun)
