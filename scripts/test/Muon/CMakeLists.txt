#
## Tests for Muon GUIs
##

set ( TEST_PY_FILES
   AxisChangerPresenter_test.py
   AxisChangerView_test.py
   FFTModel_test.py
   FFTPresenter_test.py
   LoadWidgetModel_test.py
   LoadWidgetPresenter_test.py  
   LoadWidgetView_test.py
   MaxEntModel_test.py
   MaxEntPresenter_test.py
   PeriodicTableModel_test.py
   PeriodicTablePresenter_test.py
   PlottingPresenter_test.py
   PlottingUtils_test.py
   PlottingView_test.py
   #thread_model_test.py
   transformWidget_test.py
   subplotObject_test.py
)

check_tests_valid ( ${CMAKE_CURRENT_SOURCE_DIR} ${TEST_PY_FILES} )

# Prefix for test name=PythonAlgorithms
pyunittest_add_test ( ${CMAKE_CURRENT_SOURCE_DIR} python.Muon ${TEST_PY_FILES} )
