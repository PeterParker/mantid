from mantid.api import FileProperty, WorkspaceProperty, PythonAlgorithm, NumericAxis, AlgorithmFactory, FileAction
from mantid.kernel import Direction
from mantid.simpleapi import CreateWorkspace, AddSampleLogMultiple, mtd

import numpy
import h5py


class LoadLamp(PythonAlgorithm):
    def name(self):
        return 'LoadLamp'

    def category(self):
        return 'DataHandling\\Nexus'

    def summary(self):
        return 'Loads HDF files exported from LAMP program at the ILL'

    def PyInit(self):
        self.declareProperty(FileProperty(name="InputFile", defaultValue="", action=FileAction.Load, extensions=["hdf"]))
        self.declareProperty(WorkspaceProperty(name="OutputWorkspace", defaultValue="", direction=Direction.Output))

    def PyExec(self):
        input_file = self.getProperty("InputFile").value
        output_ws = self.getPropertyValue("OutputWorkspace")

        with h5py.File(input_file, 'r') as hf:
            DATA = numpy.array(hf.get('entry1/data1/DATA'), dtype='float')
            E = numpy.array(hf.get('entry1/data1/errors'), dtype='float')
            X = numpy.array(hf.get('entry1/data1/X'), dtype='float')
            LOGS = hf.get('entry1/data1/PARAMETERS')[0]
            if len(DATA.shape) > 1:
                Y = numpy.array(hf.get('entry1/data1/Y'), dtype='float')

        nspec = 1
        y_axis = None
        if len(DATA.shape) == 2:
            nspec = DATA.shape[0]
            if len(X.shape) == 1:
                X = numpy.tile(X, nspec)
            y_axis = NumericAxis.create(nspec)
            for i in range(nspec):
                y_axis.setValue(i, Y[i])
        elif len(DATA.shape) > 2:
            raise RuntimeError('Data with more than 2 dimensions are not supported.')

        CreateWorkspace(DataX=X, DataY=DATA, DataE=E, NSpec=nspec, OutputWorkspace=output_ws)
        if len(DATA.shape) == 2:
            mtd[output_ws].replaceAxis(1, y_axis)

        log_names = []
        log_values = []
        for log in LOGS.split("\n"):
            split = log.split("=")
            if len(split) == 2:
                log_names.append(split[0])
                log_values.append(split[1])
        AddSampleLogMultiple(Workspace=output_ws, LogNames=log_names, LogValues=log_values)
        
        self.setProperty('OutputWorkspace', output_ws)

AlgorithmFactory.subscribe(LoadLamp)
