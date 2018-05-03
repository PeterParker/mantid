#include "EnggDiffFittingViewQtWidget.h"
#include "EnggDiffFittingModel.h"
#include "EnggDiffFittingPresenter.h"
#include "EnggDiffMultiRunFittingQtWidget.h"
#include "EnggDiffMultiRunFittingWidgetModel.h"
#include "EnggDiffMultiRunFittingWidgetPresenter.h"
#include "IEnggDiffMultiRunFittingWidgetView.h"

#include "MantidAPI/FunctionFactory.h"
#include "MantidAPI/IPeakFunction.h"
#include "MantidKernel/make_unique.h"
#include "MantidQtWidgets/Common/AlgorithmInputHistory.h"
#include "MantidQtWidgets/LegacyQwt/PeakPicker.h"

#include <array>
#include <iomanip>
#include <random>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/make_shared.hpp>

#include <Poco/Path.h>

#include <QEvent>
#include <QFileDialog>
#include <QHelpEvent>
#include <QSettings>

#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_symbol.h>

using namespace Mantid::API;
using namespace MantidQt::CustomInterfaces;

namespace MantidQt {
namespace CustomInterfaces {

const std::string EnggDiffFittingViewQtWidget::g_settingsGroup =
    "CustomInterfaces/EnggDiffraction/FittingView";

const std::string EnggDiffFittingViewQtWidget::g_peaksListExt =
    "Peaks list File: CSV "
    "(*.csv *.txt);;"
    "Other extensions/all files (*)";

bool EnggDiffFittingViewQtWidget::m_fittingMutliRunMode = false;
bool EnggDiffFittingViewQtWidget::m_fittingSingleRunMode = false;

std::vector<std::string> EnggDiffFittingViewQtWidget::m_fitting_runno_dir_vec;

EnggDiffFittingViewQtWidget::EnggDiffFittingViewQtWidget(
    QWidget * /*parent*/, boost::shared_ptr<IEnggDiffractionUserMsg> mainMsg,
    boost::shared_ptr<IEnggDiffractionSettings> mainSettings,
    boost::shared_ptr<IEnggDiffractionCalibration> mainCalib,
    boost::shared_ptr<IEnggDiffractionParam> mainParam,
    boost::shared_ptr<IEnggDiffractionPythonRunner> pythonRunner)
    : IEnggDiffFittingView(), m_mainMsgProvider(mainMsg),
      m_mainSettings(mainSettings),
      m_multiRunWidgetView(
          Mantid::Kernel::make_unique<EnggDiffMultiRunFittingQtWidget>(
              pythonRunner)) {

  auto multiRunWidgetPresenter =
      boost::make_shared<EnggDiffMultiRunFittingWidgetPresenter>(
          Mantid::Kernel::make_unique<EnggDiffMultiRunFittingWidgetModel>(),
          m_multiRunWidgetView.get());
  m_multiRunWidgetView->setPresenter(multiRunWidgetPresenter);
  m_multiRunWidgetView->setMessageProvider(m_mainMsgProvider);

  m_presenter = boost::make_shared<EnggDiffFittingPresenter>(
      this, Mantid::Kernel::make_unique<EnggDiffFittingModel>(), mainCalib,
      mainParam, multiRunWidgetPresenter);

  initLayout();
  m_presenter->notify(IEnggDiffFittingPresenter::Start);
}

EnggDiffFittingViewQtWidget::~EnggDiffFittingViewQtWidget() {
  m_presenter->notify(IEnggDiffFittingPresenter::ShutDown);
}

void EnggDiffFittingViewQtWidget::addWidget(
    IEnggDiffMultiRunFittingWidgetView *widget) {
  QWidget *qWidget = dynamic_cast<QWidget *>(widget);
  m_ui.gridLayout_multiRunWidget->addWidget(qWidget, 0, 0);
}

void EnggDiffFittingViewQtWidget::initLayout() {
  m_ui.setupUi(this);

  readSettings();
  doSetup();
}

void EnggDiffFittingViewQtWidget::doSetup() {
  connect(m_ui.pushButton_fitting_browse_run_num, SIGNAL(released()), this,
          SLOT(browseFitFocusedRun()));

  connect(m_ui.lineEdit_pushButton_run_num, SIGNAL(textEdited(const QString &)),
          this, SLOT(resetFittingMode()));

  connect(m_ui.lineEdit_pushButton_run_num, SIGNAL(returnPressed()), this,
          SLOT(loadClicked()));

  connect(m_ui.pushButton_fitting_browse_peaks, SIGNAL(released()), this,
          SLOT(browseClicked()));

  connect(m_ui.pushButton_load, SIGNAL(released()), this, SLOT(loadClicked()));

  connect(m_ui.pushButton_fit, SIGNAL(released()), this, SLOT(fitClicked()));

  connect(m_ui.pushButton_fit_all, SIGNAL(released()), this,
          SLOT(fitAllClicked()));

  // add peak by clicking the button
  connect(m_ui.pushButton_select_peak, SIGNAL(released()), SLOT(setPeakPick()));

  connect(m_ui.pushButton_add_peak, SIGNAL(released()), SLOT(addClicked()));

  connect(m_ui.pushButton_save_peak_list, SIGNAL(released()),
          SLOT(saveClicked()));

  connect(m_ui.pushButton_clear_peak_list, SIGNAL(released()),
          SLOT(clearPeakList()));

  // Tool-tip button
  connect(m_ui.pushButton_tooltip, SIGNAL(released()), SLOT(showToolTipHelp()));
}

void EnggDiffFittingViewQtWidget::readSettings() {
  QSettings qs;
  qs.beginGroup(QString::fromStdString(g_settingsGroup));

  // user params
  m_ui.lineEdit_pushButton_run_num->setText(
      qs.value("user-params-fitting-focused-file", "").toString());
  m_ui.lineEdit_fitting_peaks->setText(
      qs.value("user-params-fitting-peaks-to-fit", "").toString());

  qs.endGroup();
}

void EnggDiffFittingViewQtWidget::saveSettings() const {
  QSettings qs;
  qs.beginGroup(QString::fromStdString(g_settingsGroup));

  qs.setValue("user-params-fitting-focused-file",
              m_ui.lineEdit_pushButton_run_num->text());
  qs.setValue("user-params-fitting-peaks-to-fit",
              m_ui.lineEdit_fitting_peaks->text());

  qs.endGroup();
}

void EnggDiffFittingViewQtWidget::enable(bool enable) {
  m_ui.pushButton_fitting_browse_run_num->setEnabled(enable);
  m_ui.pushButton_load->setEnabled(enable);
  m_ui.lineEdit_pushButton_run_num->setEnabled(enable);
  m_ui.pushButton_fitting_browse_peaks->setEnabled(enable);
  m_ui.lineEdit_fitting_peaks->setEnabled(enable);
  m_ui.pushButton_fit->setEnabled(enable);
  m_ui.pushButton_clear_peak_list->setEnabled(enable);
  m_ui.pushButton_save_peak_list->setEnabled(enable);
  m_ui.groupBox_fitting_preview->setEnabled(enable);
}

void EnggDiffFittingViewQtWidget::showStatus(const std::string &sts) {
  m_mainMsgProvider->showStatus(sts);
}

void EnggDiffFittingViewQtWidget::userWarning(const std::string &err,
                                              const std::string &description) {
  m_mainMsgProvider->userWarning(err, description);
}

void EnggDiffFittingViewQtWidget::userError(const std::string &err,
                                            const std::string &description) {
  m_mainMsgProvider->userError(err, description);
}

void EnggDiffFittingViewQtWidget::enableCalibrateFocusFitUserActions(
    bool enable) {
  m_mainMsgProvider->enableCalibrateFocusFitUserActions(enable);
}

EnggDiffCalibSettings
EnggDiffFittingViewQtWidget::currentCalibSettings() const {
  return m_mainSettings->currentCalibSettings();
}

std::string EnggDiffFittingViewQtWidget::focusingDir() const {
  return m_mainSettings->focusingDir();
}

void EnggDiffFittingViewQtWidget::loadClicked() {
  m_presenter->notify(IEnggDiffFittingPresenter::Load);
}

void EnggDiffFittingViewQtWidget::fitClicked() {
  m_presenter->notify(IEnggDiffFittingPresenter::FitPeaks);
}

void EnggDiffFittingViewQtWidget::fitAllClicked() {
  m_presenter->notify(IEnggDiffFittingPresenter::FitAllPeaks);
}

void EnggDiffFittingViewQtWidget::addClicked() {
  m_presenter->notify(IEnggDiffFittingPresenter::addPeaks);
}

void EnggDiffFittingViewQtWidget::browseClicked() {
  m_presenter->notify(IEnggDiffFittingPresenter::browsePeaks);
}

void EnggDiffFittingViewQtWidget::saveClicked() {
  m_presenter->notify(IEnggDiffFittingPresenter::savePeaks);
}

void EnggDiffFittingViewQtWidget::resetFittingMode() {
  // resets the global variable so the list view widgets
  // adds the run number to for single runs too
  m_fittingMutliRunMode = false;
  m_fittingSingleRunMode = false;
}

std::string EnggDiffFittingViewQtWidget::getPreviousDir() const {

  QString prevPath =
      MantidQt::API::AlgorithmInputHistory::Instance().getPreviousDirectory();

  return prevPath.toStdString();
}

void EnggDiffFittingViewQtWidget::setPreviousDir(const std::string &path) {
  QString qPath = QString::fromStdString(path);
  MantidQt::API::AlgorithmInputHistory::Instance().setPreviousDirectory(qPath);
}

std::string
EnggDiffFittingViewQtWidget::getOpenFile(const std::string &prevPath) {

  QString path(QFileDialog::getOpenFileName(
      this, tr("Open Peaks To Fit"), QString::fromStdString(prevPath),
      QString::fromStdString(g_peaksListExt)));

  return path.toStdString();
}

std::string
EnggDiffFittingViewQtWidget::getSaveFile(const std::string &prevPath) {

  QString path(QFileDialog::getSaveFileName(
      this, tr("Save Expected Peaks List"), QString::fromStdString(prevPath),
      QString::fromStdString(g_peaksListExt)));

  return path.toStdString();
}

void EnggDiffFittingViewQtWidget::browseFitFocusedRun() {
  resetFittingMode();
  QString prevPath = QString::fromStdString(focusingDir());
  if (prevPath.isEmpty()) {
    prevPath =
        MantidQt::API::AlgorithmInputHistory::Instance().getPreviousDirectory();
  }
  std::string nexusFormat = "Nexus file with calibration table: NXS, NEXUS"
                            "(*.nxs *.nexus);;";

  QStringList paths(
      QFileDialog::getOpenFileNames(this, tr("Open Focused File "), prevPath,
                                    QString::fromStdString(nexusFormat)));

  if (paths.isEmpty()) {
    return;
  }

  setFocusedFileNames(paths.join(",").toStdString());
}

void EnggDiffFittingViewQtWidget::setFocusedFileNames(
    const std::string &paths) {
  m_ui.lineEdit_pushButton_run_num->setText(QString::fromStdString(paths));
}

std::string EnggDiffFittingViewQtWidget::getFocusedFileNames() const {
  return m_ui.lineEdit_pushButton_run_num->text().toStdString();
}

void EnggDiffFittingViewQtWidget::enableFitAllButton(bool enable) const {
  m_ui.pushButton_fit_all->setEnabled(enable);
}

void EnggDiffFittingViewQtWidget::showToolTipHelp() {
  // We need a the mouse click position relative to the widget
  // and relative to the screen. We will set the mouse click position
  // relative to widget to 0 as the global position of the mouse
  // is what is considered when the tool tip is displayed
  const QPoint relWidgetPosition(0, 0);
  const QPoint mousePos = QCursor::pos();
  // Now fire the generated event to show a tool tip at the cursor
  QEvent *toolTipEvent =
      new QHelpEvent(QEvent::ToolTip, relWidgetPosition, mousePos);
  QCoreApplication::sendEvent(m_ui.pushButton_tooltip, toolTipEvent);
}

std::string EnggDiffFittingViewQtWidget::getExpectedPeaksInput() const {

  return m_ui.lineEdit_fitting_peaks->text().toStdString();
}

void EnggDiffFittingViewQtWidget::setPeakList(
    const std::string &peakList) const {
  m_ui.lineEdit_fitting_peaks->setText(QString::fromStdString(peakList));
}

std::vector<std::string> EnggDiffFittingViewQtWidget::getFittingRunNumVec() {
  return m_fitting_runno_dir_vec;
}

void EnggDiffFittingViewQtWidget::setFittingRunNumVec(
    std::vector<std::string> assignVec) {
  // holds all the directories required
  m_fitting_runno_dir_vec.clear();
  m_fitting_runno_dir_vec = assignVec;
}

bool EnggDiffFittingViewQtWidget::getFittingSingleRunMode() {
  return m_fittingSingleRunMode;
}

void EnggDiffFittingViewQtWidget::setFittingSingleRunMode(bool mode) {
  m_fittingSingleRunMode = mode;
}

void EnggDiffFittingViewQtWidget::setFittingMultiRunMode(bool mode) {
  m_fittingMutliRunMode = mode;
}

bool EnggDiffFittingViewQtWidget::getFittingMultiRunMode() {
  return m_fittingMutliRunMode;
}

void EnggDiffFittingViewQtWidget::clearPeakList() {
  m_ui.lineEdit_fitting_peaks->clear();
}

} // namespace CustomInterfaces
} // namespace MantidQt
