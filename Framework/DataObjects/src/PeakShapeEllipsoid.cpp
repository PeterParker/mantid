// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#include "MantidDataObjects/PeakShapeEllipsoid.h"
#include "MantidKernel/cow_ptr.h"
#include <json/json.h>

namespace Mantid {
namespace DataObjects {

PeakShapeEllipsoid::PeakShapeEllipsoid(
    const std::vector<Kernel::V3D> &directions,
    const std::vector<double> &abcRadii,
    const std::vector<double> &abcRadiiBackgroundInner,
    const std::vector<double> &abcRadiiBackgroundOuter,
    Kernel::SpecialCoordinateSystem frame, std::string algorithmName,
    int algorithmVersion)
    : PeakShapeBase(frame, algorithmName, algorithmVersion),
      m_directions(directions), m_abc_radii(abcRadii),
      m_abc_radiiBackgroundInner(abcRadiiBackgroundInner),
      m_abc_radiiBackgroundOuter(abcRadiiBackgroundOuter) {

  if (directions.size() != 3) {
    throw std::invalid_argument("directions must be of size 3");
  }
  if (abcRadii.size() != 3) {
    throw std::invalid_argument("radii must be of size 3");
  }
  if (abcRadiiBackgroundInner.size() != 3) {
    throw std::invalid_argument("radii inner must be of size 3");
  }
  if (abcRadiiBackgroundOuter.size() != 3) {
    throw std::invalid_argument("radii outer must be of size 3");
  }
}

bool PeakShapeEllipsoid::operator==(const PeakShapeEllipsoid &other) const {
  return PeakShapeBase::operator==(other) &&
         other.directions() == this->directions() &&
         other.abcRadii() == this->abcRadii() &&
         other.abcRadiiBackgroundInner() == this->abcRadiiBackgroundInner() &&
         other.abcRadiiBackgroundOuter() == this->abcRadiiBackgroundOuter();
}

const std::vector<double> &PeakShapeEllipsoid::abcRadii() const {
  return m_abc_radii;
}

const std::vector<double> &PeakShapeEllipsoid::abcRadiiBackgroundInner() const {
  return m_abc_radiiBackgroundInner;
}

const std::vector<double> &PeakShapeEllipsoid::abcRadiiBackgroundOuter() const {
  return m_abc_radiiBackgroundOuter;
}

const std::vector<Kernel::V3D> &PeakShapeEllipsoid::directions() const {
  return m_directions;
}

std::vector<Kernel::V3D> PeakShapeEllipsoid::getDirectionInSpecificFrame(
    Kernel::Matrix<double> &invertedGoniometerMatrix) const {
  std::vector<Kernel::V3D> directionsInFrame;

  if ((invertedGoniometerMatrix.numCols() != m_directions.size()) ||
      (invertedGoniometerMatrix.numRows() != m_directions.size())) {
    throw std::invalid_argument("The inverted goniometer matrix is not "
                                "compatible with the direction vector");
  }

  for (const auto &direction : m_directions) {
    directionsInFrame.push_back(invertedGoniometerMatrix * direction);
  }

  return directionsInFrame;
}

std::string PeakShapeEllipsoid::toJSON() const {
  Json::Value root;
  PeakShapeBase::buildCommon(root);
  root["radius0"] = Json::Value(m_abc_radii[0]);
  root["radius1"] = Json::Value(m_abc_radii[1]);
  root["radius2"] = Json::Value(m_abc_radii[2]);

  root["background_inner_radius0"] = Json::Value(m_abc_radiiBackgroundInner[0]);
  root["background_inner_radius1"] = Json::Value(m_abc_radiiBackgroundInner[1]);
  root["background_inner_radius2"] = Json::Value(m_abc_radiiBackgroundInner[2]);

  root["background_outer_radius0"] = Json::Value(m_abc_radiiBackgroundOuter[0]);
  root["background_outer_radius1"] = Json::Value(m_abc_radiiBackgroundOuter[1]);
  root["background_outer_radius2"] = Json::Value(m_abc_radiiBackgroundOuter[2]);

  root["direction0"] = m_directions[0].toString();
  root["direction1"] = m_directions[1].toString();
  root["direction2"] = m_directions[2].toString();

  Json::StyledWriter writer;
  return writer.write(root);
}

PeakShapeEllipsoid *PeakShapeEllipsoid::clone() const {
  return new PeakShapeEllipsoid(*this);
}

std::string PeakShapeEllipsoid::shapeName() const {
  return PeakShapeEllipsoid::ellipsoidShapeName();
}

boost::optional<double> PeakShapeEllipsoid::radius(RadiusType type) const {
  std::vector<double>::const_iterator it;
  switch (type) {
  case (RadiusType::Radius):
    it = std::max_element(m_abc_radii.cbegin(), m_abc_radii.cend());
    break;
  case (RadiusType::OuterRadius):
    it = std::max_element(m_abc_radiiBackgroundOuter.cbegin(),
                          m_abc_radiiBackgroundOuter.cend());
    break;
  case (RadiusType::InnerRadius):
    it = std::max_element(m_abc_radiiBackgroundInner.cbegin(),
                          m_abc_radiiBackgroundInner.cend());
    break;
  }
  return boost::optional<double>{*it};
}

const std::string PeakShapeEllipsoid::ellipsoidShapeName() {
  return "ellipsoid";
}

} // namespace DataObjects
} // namespace Mantid
