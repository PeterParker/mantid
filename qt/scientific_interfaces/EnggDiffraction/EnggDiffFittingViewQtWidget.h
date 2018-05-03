#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEWQTWIDGET_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEWQTWIDGET_H_

#include "MantidAPI/IPeakFunction.h"
#include "DllConfig.h"
#include "IEnggDiffFittingPresenter.h"
#include "IEnggDiffFittingView.h"
#include "IEnggDiffMultiRunFittingWidgetView.h"
#include "IEnggDiffractionPythonRunner.h"

#include "ui_EnggDiffractionQtTabFitting.h"

// Qt classes forward declarations
class QMessageBox;
class QMutex;

class QwtPlotCurve;
class QwtPlotZoomer;

namespace MantidQt {

namespace MantidWidgets {
class PeakPicker;
}

namespace CustomInterfaces {

/**
Qt-based view of the Engineering Diffraction (EnggDiff) fitting
widget/tab. Provides a concrete view that is Qt-based and is probably
the only one that will be implemented in a foreseeable horizon. The
interface of this class is given by IEnggDiffFittingView so that it
fits in the MVP (Model-View-Presenter) design of this GUI.

Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory, NScD
Oak Ridge National Laboratory & European Spallation Source

This file is part of Mantid.

Mantid is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Mantid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

File change history is stored at: <https://github.com/mantidproject/mantid>
Code Documentation is available at: <http://doxygen.mantidproject.org>
*/
class MANTIDQT_ENGGDIFFRACTION_DLL EnggDiffFittingViewQtWidget
    : public QWidget,
      public IEnggDiffFittingView {
  Q_OBJECT

public:
  EnggDiffFittingViewQtWidget(
      QWidget *parent, boost::shared_ptr<IEnggDiffractionUserMsg> mainMsg,
      boost::shared_ptr<IEnggDiffractionSettings> mainSettings,
      boost::shared_ptr<IEnggDiffractionCalibration> mainCalib,
      boost::shared_ptr<IEnggDiffractionParam> mainParam,
      boost::shared_ptr<IEnggDiffractionPythonRunner> pythonRunner);
  ~EnggDiffFittingViewQtWidget() override;

  void addWidget(IEnggDiffMultiRunFittingWidgetView *widget) override;

  /// From the IEnggDiffractionUserMsg interface
  void showStatus(const std::string &sts) override;

  void userWarning(const std::string &warn,
                   const std::string &description) override;

  void userError(const std::string &err,
                 const std::string &description) override;
  void enableCalibrateFocusFitUserActions(bool enable) override;

  /// From the IEnggDiffractionSettings interface
  EnggDiffCalibSettings currentCalibSettings() const override;

  std::string focusingDir() const override;

  void enable(bool enable);

  std::vector<std::string> logMsgs() const override { return m_logMsgs; }

  void setFocusedFileNames(const std::string &paths) override;

  std::string getFocusedFileNames() const override;

  void enableFitAllButton(bool enable) const override;

  std::string getExpectedPeaksInput() const override;

  void setPeakList(const std::string &peakList) const override;

  std::vector<std::string> getFittingRunNumVec() override;

  void setFittingRunNumVec(std::vector<std::string> assignVec) override;

  bool getFittingMultiRunMode() override;

  void setFittingMultiRunMode(bool mode) override;

  bool getFittingSingleRunMode() override;

  void setFittingSingleRunMode(bool mode) override;

  std::string getPreviousDir() const override;

  void setPreviousDir(const std::string &path) override;

  std::string getOpenFile(const std::string &prevPath) override;

  std::string getSaveFile(const std::string &prevPath) override;

  std::string getCurrentInstrument() const override { return m_currentInst; }

  void setCurrentInstrument(const std::string &newInstrument) override {
    m_currentInst = newInstrument;
  }

protected:
  void initLayout();

signals:
  void getBanks();
  void setBank();

private slots:
  // slot of the fitting peaks per part of the interface
  void browseFitFocusedRun();
  void resetFittingMode();
  void clearPeakList();
  void loadClicked();
  void fitClicked();
  void fitAllClicked();
  void addClicked();
  void browseClicked();
  void saveClicked();
  void showToolTipHelp();

private:
  /// Setup the interface (tab UI)
  void doSetup();

  /// Load saved/default interface settings
  void readSettings();
  /// save settings (before closing)
  void saveSettings() const override;

  /// converts QList to a vector
  std::vector<std::string> qListToVector(QStringList list,
                                         bool validator) const;

  // path/name for the persistent settings group of this interface
  static const std::string g_settingsGroup;

  static const std::string g_peaksListExt;

  /// indentifier for fitting multi-run or single run input
  static bool m_fittingMutliRunMode;

  /// indentifier for fitting multi-run or single run input
  static bool m_fittingSingleRunMode;

  // vector holding directory of focused bank file
  static std::vector<std::string> m_fitting_runno_dir_vec;

  Ui::EnggDiffractionQtTabFitting m_ui;

  // here the view puts messages before notifying the presenter to show them
  std::vector<std::string> m_logMsgs;

  /// user messages interface provided by a main view/widget
  boost::shared_ptr<IEnggDiffractionUserMsg> m_mainMsgProvider;

  /// settings from the user
  boost::shared_ptr<IEnggDiffractionSettings> m_mainSettings;

  boost::shared_ptr<IEnggDiffMultiRunFittingWidgetView> m_multiRunWidgetView;

  /// presenter as in the model-view-presenter
  boost::shared_ptr<IEnggDiffFittingPresenter> m_presenter;

  /// current selected instrument
  /// updated from the EnggDiffractionPresenter processInstChange
  std::string m_currentInst = "";
};

} // namespace CustomInterfaces
} // namespace MantidQt

#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEWQTWIDGET_H_
