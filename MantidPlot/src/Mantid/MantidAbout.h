// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2009 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTIDABOUT_H
#define MANTIDABOUT_H

//----------------------
// Includes
//----------------------
#include "ui_MantidAbout.h"
#include <MantidQtWidgets/Common/MantidDialog.h>

/**
This class implements About MantidPlot dialog for mnatid help menu.

@author Sofia Antony, ISIS, RAL
@date 13/01/2010
*/

class MantidAbout : public MantidQt::API::MantidDialog {

  Q_OBJECT

public:
  /// constructor
  explicit MantidAbout(QWidget *parent = nullptr);
  /// destructor
  ~MantidAbout() override {}

private:
  /// form generated by QT Designer
  Ui::MantidAbout m_uiForm;
};

#endif /* MANTIDABOUT_H */
