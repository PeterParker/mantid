#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEW_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEW_H_

#include "IEnggDiffMultiRunFittingWidgetOwner.h"
#include "IEnggDiffractionSettings.h"
#include "IEnggDiffractionUserMsg.h"

#include "MantidKernel/Unit.h"

#include <string>
#include <vector>
#include <boost/optional.hpp>

class QwtData;

namespace MantidQt {
namespace CustomInterfaces {

/**
Engineering diffraction custom interface / GUI. This is the base class
(interface) for the view of the the fitting tab/widget (view in the
sense of the Model-View-Presenter, MVP pattern). This class is
Qt-free. Qt specific functionality/dependencies are added in a class
derived from this.

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
class IEnggDiffFittingView : public IEnggDiffractionUserMsg,
                             public IEnggDiffractionSettings,
                             public IEnggDiffMultiRunFittingWidgetOwner {

public:
  virtual ~IEnggDiffFittingView() = default;

  virtual void
  addWidget(IEnggDiffMultiRunFittingWidgetView *widget) override = 0;

  /**
   * Get value of the input files text box
   *
   * @return (hopefully) comma-separated list of focused files to load for
   * fitting
   */
  virtual std::string getFocusedFileNames() const = 0;

  /**
   * A list of dSpacing values to be translated into TOF
   * to find expected peaks.
   *
   * @return list of dSpacing values as std::string
   */
  virtual std::string getExpectedPeaksInput() const = 0;

  /**
   * Sets the peak list according to the string given
   *
   * @param peakList list of expected peaks to be fitted as std::string
   */
  virtual void setPeakList(const std::string &peakList) const = 0;

  /**
   * enables the Fit All button when multi-run number given
   *
   * @param enable the button to Fit multi-run number
   */
  virtual void enableFitAllButton(bool enable) const = 0;

  /**
  * gets the previously used directory path by the user
  *
  * @return directory of previously used directory by user,
  * may return empty if no previous history
  */
  virtual std::string getPreviousDir() const = 0;

  /**
  * sets the previously used directory path
  *
  * @param path is set according to the file selected by user
  */
  virtual void setPreviousDir(const std::string &path) = 0;

  /**
  * gets the path as string which required when browsing the file
  *
  * @param prevPath path set according to the previously file selected
  *
  * @return string of the browsed file path
  */
  virtual std::string getOpenFile(const std::string &prevPath) = 0;

  /**
  * gets the path as string which is required when saving the file
  *
  * @param prevPath path set according to the previously selected file
  *
  * @return string of the saved file
  */
  virtual std::string getSaveFile(const std::string &prevPath) = 0;

  /**
   * Set value of the text box for input filenames
   *
   * @param path Comma-separated list of files to add
   */
  virtual void setFocusedFileNames(const std::string &path) = 0;

  /**
   * gets the global vector in view containing focused file directory
   *
   * @return std::vector<std::string> containing the focused bank files
   */
  virtual std::vector<std::string> getFittingRunNumVec() = 0;

  /**
   * sets the global vector in view containing focused file directory
   *
   * @param assignVec of the all the focused bank files
   *  per run number
   */
  virtual void setFittingRunNumVec(std::vector<std::string> assignVec) = 0;

  /**
   * to determine whether the current loop is multi-run or single to avoid
   * regenerating the list-view widget when not required
   *
   * @return bool whether given multi-run or singular file
   */
  virtual bool getFittingMultiRunMode() = 0;

  /**
   * sets the fitting mode to multi-run or single to avoid
   * regenerating the list-view widget when not required
   *
   * @param mode true if its multi-run
   */
  virtual void setFittingMultiRunMode(bool mode) = 0;

  /**
  * to determine whether the current loop is multi-run or single to avoid
  * regenerating the list-view widget when not required
  *
  * @return bool whether given multi-run or singular file
  */
  virtual bool getFittingSingleRunMode() = 0;

  /**
  * sets the fitting mode to multi-run or single to avoid
  * regenerating the list-view widget when not required
  *
  * @param mode true if its multi-run
  */
  virtual void setFittingSingleRunMode(bool mode) = 0;

  /**
   * Messages that this view wants to send to the logging system.
   *
   * @return list of messages to log, one by one.
   */
  virtual std::vector<std::string> logMsgs() const = 0;

  /**
   * Save user settings (normally when closing the interface).
   */
  virtual void saveSettings() const = 0;

  /**
  * Gets the current selected instrument
  */
  virtual std::string getCurrentInstrument() const = 0;

  /**
   * Sets the currently selected instrument
   * @param newInstrument the new instrument that is selected
   */
  virtual void setCurrentInstrument(const std::string &newInstrument) = 0;
};

} // namespace CustomInterfaces
} // namespace MantidQt

#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGVIEW_H_
