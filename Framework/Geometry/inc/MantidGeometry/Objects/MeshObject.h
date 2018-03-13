#ifndef MANTID_GEOMETRY_MESHOBJECT_H_
#define MANTID_GEOMETRY_MESHOBJECT_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidGeometry/DllConfig.h"
#include "MantidGeometry/Objects/IObject.h"
#include "MantidKernel/Material.h"
#include "BoundingBox.h"
#include <map>
#include <memory>

namespace Mantid {
//----------------------------------------------------------------------
// Forward declarations
//----------------------------------------------------------------------
namespace Kernel {
class PseudoRandomNumberGenerator;
class V3D;
}

namespace Geometry {
class CacheGeometryHandler;
class CompGrp;
class GeometryHandler;
class Track;
class vtkGeometryCacheReader;
class vtkGeometryCacheWriter;

/**
\class MeshObject
\brief Triangular Mesh Object
\version 1.0
\date Dec 2017
\author Karl Palmen

Mesh Object of Triangles assumed to form one or more
non-intersecting closed surfaces enclosing separate volumes.
The number of vertices is limited to 65535.

Copyright &copy; 2017-2018 ISIS Rutherford Appleton Laboratory, NScD Oak Ridge
National Laboratory & European Spallation Source

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
class MANTID_GEOMETRY_DLL MeshObject : public IObject {
public:
  /// Constructor
  MeshObject(const std::vector<uint16_t> &faces,
             const std::vector<Mantid::Kernel::V3D> &vertices,
             const Kernel::Material &material);
  /// Constructor
  MeshObject(std::vector<uint16_t> &&faces,
             std::vector<Mantid::Kernel::V3D> &&vertices,
             const Kernel::Material &&material);

  /// Copy constructor
  MeshObject(const MeshObject &) = delete;
  /// Assignment operator
  MeshObject &operator=(const MeshObject &) = delete;
  /// Destructor
  virtual ~MeshObject() = default;
  /// Clone
  IObject *clone() const override {
    return new MeshObject(m_triangles, m_vertices, m_material);
  }
  IObject *cloneWithMaterial(const Kernel::Material &material) const override {
    return new MeshObject(m_triangles, m_vertices, material);
  }

  const std::string &id() const override { return m_id; }

  int getName() const override { return 0; }

  const Kernel::Material material() const override;

  /// Return whether this object has a valid shape
  bool hasValidShape() const override;

  bool
  isValid(const Kernel::V3D &) const override; ///< Check if a point is inside
  bool isOnSide(const Kernel::V3D &) const override;
  int calcValidType(const Kernel::V3D &Pt,
                    const Kernel::V3D &uVec) const override;

  // INTERSECTION
  int interceptSurface(Geometry::Track &) const override;

  // Solid angle - uses triangleSolidAngle unless many (>30000) triangles
  double solidAngle(const Kernel::V3D &observer) const override;
  // Solid angle with a scaling of the object
  double solidAngle(const Kernel::V3D &observer,
                    const Kernel::V3D &scaleFactor) const override;

  /// Calculates the volume of this object.
  double volume() const override;

  /// Calculate (or return cached value of) Axis Aligned Bounding box
  /// (DEPRECATED)
  void getBoundingBox(double &xmax, double &ymax, double &zmax, double &xmin,
                      double &ymin, double &zmin) const override;

  /// Return cached value of axis-aligned bounding box
  const BoundingBox &getBoundingBox() const override;

  // find internal point to object
  int getPointInObject(Kernel::V3D &point) const override;

  /// Select a random point within the object
  Kernel::V3D generatePointInObject(Kernel::PseudoRandomNumberGenerator &rng,
                                    const size_t) const override;
  Kernel::V3D generatePointInObject(Kernel::PseudoRandomNumberGenerator &rng,
                                    const BoundingBox &activeRegion,
                                    const size_t) const override;

  // Rendering member functions
  void draw() const override;
  // Initialize Drawing
  void initDraw() const override;
  // Get Geometry Handler
  boost::shared_ptr<GeometryHandler> getGeometryHandler() const override;
  /// Set Geometry Handler
  void setGeometryHandler(boost::shared_ptr<GeometryHandler> h);

  void GetObjectGeom(int &type, std::vector<Kernel::V3D> &vectors,
                     double &myradius, double &myheight) const override;

  /// Read access to mesh object for rendering
  int numberOfVertices() const;
  double *getVertices() const;
  int numberOfTriangles() const;
  int *getTriangles() const;

  void updateGeometryHandler();

private:
  void initialize();
  /// Get intersections
  void getIntersections(const Kernel::V3D &start, const Kernel::V3D &direction,
                        std::vector<Kernel::V3D> &intersectionPoints,
                        std::vector<int> &entryExitFlags) const;
  /// Determine intersection between ray and an one triangle
  bool rayIntersectsTriangle(const Kernel::V3D &start,
                             const Kernel::V3D &direction,
                             const Kernel::V3D &v1, const Kernel::V3D &v2,
                             const Kernel::V3D &v3, Kernel::V3D &intersection,
                             int &entryExit) const;
  /// Get triangle
  bool getTriangle(const size_t index, Kernel::V3D &v1, Kernel::V3D &v2,
                   Kernel::V3D &v3) const;
  /// Search object for valid point
  bool searchForObject(Kernel::V3D &point) const;

  /// Cache for object's bounding box
  mutable BoundingBox m_boundingBox;

  /// Tolerence distance
  const double M_TOLERANCE = 0.000001;

  /// Geometry Handle for rendering
  boost::shared_ptr<GeometryHandler> m_handler;

  // String from which object may be defined
  std::string m_string;

  /// string to return as ID
  std::string m_id;

  /// a pointer to a class for reading from the geometry cache
  boost::shared_ptr<vtkGeometryCacheReader> m_vtk_cache_reader;
  /// a pointer to a class for writing to the geometry cache
  boost::shared_ptr<vtkGeometryCacheWriter> m_vtk_cache_writer;

  /// Contents
  /// Triangles are specified by indices into a list of vertices.
  std::vector<uint16_t> m_triangles;
  std::vector<Kernel::V3D> m_vertices;
  /// material composition
  Kernel::Material m_material;
};

} // NAMESPACE Geometry
} // NAMESPACE Mantid

#endif /*MANTID_GEOMETRY_MESHOBJECT_H_*/