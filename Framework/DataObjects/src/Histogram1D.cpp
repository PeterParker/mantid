#include "MantidDataObjects/Histogram1D.h"
#include "MantidKernel/Exception.h"
#include "MantidAPI/WorkspaceFactory.h"

namespace Mantid {
namespace DataObjects {

/// Construct from ISpectrum.
Histogram1D::Histogram1D(const ISpectrum &other)
    : ISpectrum(other), m_histogram(HistogramData::getHistogramXMode(
                            other.readX().size(), other.readY().size())) {
  dataY() = other.readY();
  dataE() = other.readE();
}

/// Assignment from ISpectrum.
Histogram1D &Histogram1D::operator=(const ISpectrum &rhs) {
  m_histogram = HistogramData::Histogram(
      HistogramData::getHistogramXMode(rhs.readX().size(), rhs.readY().size()));
  ISpectrum::operator=(rhs);
  dataY() = rhs.readY();
  dataE() = rhs.readE();
  return *this;
}

void Histogram1D::clearData() {
  MantidVec &yValues = this->dataY();
  std::fill(yValues.begin(), yValues.end(), 0.0);
  MantidVec &eValues = this->dataE();
  std::fill(eValues.begin(), eValues.end(), 0.0);
}

/// Sets the x data.
/// @param X :: vector of X data
void Histogram1D::setX(const Kernel::cow_ptr<HistogramData::HistogramX> &X) {
  m_histogram.setX(X);
}

/// Returns the x data
MantidVec &Histogram1D::dataX() { return m_histogram.dataX(); }

/// Returns the x data const
const MantidVec &Histogram1D::dataX() const { return m_histogram.dataX(); }

/// Returns the x data const
const MantidVec &Histogram1D::readX() const { return m_histogram.readX(); }

/// Returns a pointer to the x data
Kernel::cow_ptr<HistogramData::HistogramX> Histogram1D::ptrX() const {
  return m_histogram.ptrX();
}

MantidVec &Histogram1D::dataDx() { return m_histogram.dataDx(); }
const MantidVec &Histogram1D::dataDx() const { return m_histogram.dataDx(); }
const MantidVec &Histogram1D::readDx() const { return m_histogram.readDx(); }

} // namespace DataObjects
} // namespace Mantid
