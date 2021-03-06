// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2007 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_ALGORITHM_MULTIPLY_H_
#define MANTID_ALGORITHM_MULTIPLY_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAlgorithms/CommutativeBinaryOperation.h"

namespace Mantid {
namespace Algorithms {
/**
Multiply performs the product of two input workspaces.
It inherits from the Algorithm class, and overrides
the init()&+ exec()  methods.

Required Properties:
<UL>
<LI> InputWorkspace1 - The name of the workspace </LI>
<LI> InputWorkspace2 - The name of the workspace </LI>
<LI> OutputWorkspace - The name of the workspace in which to store the product
data </LI>
</UL>

@author Nick Draper
@date 14/12/2007
*/
class DLLExport Multiply : public CommutativeBinaryOperation {
public:
  /// Algorithm's name for identification overriding a virtual method
  const std::string name() const override { return "Multiply"; }
  /// Summary of algorithms purpose
  const std::string summary() const override {
    return "The Multiply algorithm will multiply the data values and calculate "
           "the corresponding error values of two compatible workspaces.  ";
  }

  /// Algorithm's version for identification overriding a virtual method
  int version() const override { return 1; }
  const std::vector<std::string> seeAlso() const override {
    return {"Plus", "Minus", "Divide"};
  }

private:
  // Overridden BinaryOperation methods
  void performBinaryOperation(const MantidVec &lhsX, const MantidVec &lhsY,
                              const MantidVec &lhsE, const MantidVec &rhsY,
                              const MantidVec &rhsE, MantidVec &YOut,
                              MantidVec &EOut) override;
  void performBinaryOperation(const MantidVec &lhsX, const MantidVec &lhsY,
                              const MantidVec &lhsE, const double rhsY,
                              const double rhsE, MantidVec &YOut,
                              MantidVec &EOut) override;

  void setOutputUnits(const API::MatrixWorkspace_const_sptr lhs,
                      const API::MatrixWorkspace_const_sptr rhs,
                      API::MatrixWorkspace_sptr out) override;

  void performEventBinaryOperation(DataObjects::EventList &lhs,
                                   const DataObjects::EventList &rhs) override;

  void performEventBinaryOperation(DataObjects::EventList &lhs,
                                   const MantidVec &rhsX, const MantidVec &rhsY,
                                   const MantidVec &rhsE) override;

  void performEventBinaryOperation(DataObjects::EventList &lhs,
                                   const double &rhsY,
                                   const double &rhsE) override;

  void checkRequirements() override;
  std::string checkSizeCompatibility(
      const API::MatrixWorkspace_const_sptr lhs,
      const API::MatrixWorkspace_const_sptr rhs) const override;
};

} // namespace Algorithms
} // namespace Mantid

#endif /*MANTID_ALGORITHM_MULTIPLY_H_*/
