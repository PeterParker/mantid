#ifndef MANTID_GEOMETRY_INSTRUMENTVISITORVISITORTEST_H_
#define MANTID_GEOMETRY_INSTRUMENTVISITORVISITORTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidKernel/V3D.h"
#include "MantidKernel/EigenConversionHelpers.h"
#include "MantidGeometry/IComponent.h"
#include "MantidGeometry/Instrument/InstrumentVisitor.h"
#include "MantidGeometry/Instrument/ComponentHelper.h"
#include "MantidGeometry/Instrument/Detector.h"
#include "MantidBeamline/Beamline.h"
#include "MantidBeamline/ComponentInfo.h"
#include "MantidBeamline/DetectorInfo.h"
#include "MantidTestHelpers/ComponentCreationHelper.h"
#include "MantidGeometry/Instrument/ParameterMap.h"
#include <set>
#include <algorithm>
#include <boost/make_shared.hpp>

using namespace Mantid::Geometry;
using Mantid::Kernel::V3D;
using namespace ComponentCreationHelper;
using Mantid::detid_t;

namespace {

boost::shared_ptr<const Instrument>
makeParameterized(boost::shared_ptr<const Instrument> baseInstrument) {
  return boost::make_shared<const Instrument>(
      baseInstrument, boost::make_shared<ParameterMap>());
}
}

class InstrumentVisitorTest : public CxxTest::TestSuite {
public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static InstrumentVisitorTest *createSuite() {
    return new InstrumentVisitorTest();
  }
  static void destroySuite(InstrumentVisitorTest *suite) { delete suite; }

  void test_visitor_basic_sanity_check() {

    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);
    // Create the visitor.
    InstrumentVisitor visitor(makeParameterized(visitee));
    // Visit everything
    visitee->registerContents(visitor);

    size_t expectedSize = 0;
    ++expectedSize; // source
    ++expectedSize; // sample
    ++expectedSize; // Detector
    ++expectedSize; // instrument

    TSM_ASSERT_EQUALS("Should have registered 4 components", visitor.size(),
                      expectedSize);
  }

  void test_visitor_purges_parameter_map_basic_check() {

    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/,
                                           V3D(11, 0, 0) /*detector position*/);
    auto pmap = boost::make_shared<ParameterMap>();
    auto detector = visitee->getDetector(visitee->getDetectorIDs()[0]);
    pmap->addV3D(detector->getComponentID(), "pos",
                 Mantid::Kernel::V3D{12, 0, 0});
    pmap->addV3D(visitee->getComponentID(), "pos",
                 Mantid::Kernel::V3D{13, 0, 0});

    TS_ASSERT_EQUALS(pmap->size(), 2);

    // Create the visitor.
    InstrumentVisitor visitor(
        boost::make_shared<const Instrument>(visitee, pmap));

    // Visit everything. Purging should happen.
    visitor.walkInstrument();

    TSM_ASSERT_EQUALS("Detectors positions are purged by visitor at present",
                      pmap->size(), 0);
  }

  void test_visitor_purges_parameter_map_safely() {
    /* We need to check that the purging process does not actually result in
     *things
     * that are subsequently read being misoriented or mislocated.
     *
     * In detail: Purging must be depth-first because of the way that lower
     *level
     * components calculate their positions/rotations from their parents.
     */
    using namespace ComponentHelper;

    const V3D sourcePos(0, 0, 0);
    const V3D samplePos(10, 0, 0);
    const V3D detectorPos(11, 0, 0);
    // Create a very basic instrument to visit
    auto baseInstrument = ComponentCreationHelper::createMinimalInstrument(
        sourcePos, samplePos, detectorPos);
    auto paramMap = boost::make_shared<Mantid::Geometry::ParameterMap>();
    auto parInstrument = boost::make_shared<Mantid::Geometry::Instrument>(
        baseInstrument, paramMap);

    TSM_ASSERT_EQUALS("Expect 0 items in the parameter map to start with",
                      paramMap->size(), 0);
    auto source = parInstrument->getComponentByName("source");
    const V3D newInstrumentPos(-10, 0, 0);
    ComponentHelper::moveComponent(*parInstrument, *paramMap, newInstrumentPos,
                                   TransformType::Absolute);
    const V3D newSourcePos(-1, 0, 0);
    ComponentHelper::moveComponent(*source, *paramMap, newSourcePos,
                                   TransformType::Absolute);

    // Test the moved things are where we expect them to be an that the
    // parameter map is populated.
    TS_ASSERT_EQUALS(newSourcePos,
                     parInstrument->getComponentByName("source")->getPos());
    TS_ASSERT_EQUALS(newInstrumentPos, parInstrument->getPos());
    TSM_ASSERT_EQUALS("Expect 2 items in the parameter map", paramMap->size(),
                      2);

    InstrumentVisitor visitor(parInstrument);
    visitor.walkInstrument();

    TSM_ASSERT_EQUALS("Expect 0 items in the purged parameter map",
                      paramMap->size(), 0);

    // Now we check that thing are located where we expect them to be.
    auto beamline = visitor.beamline();

    TSM_ASSERT("Check source position",
               beamline.componentInfo().position(1).isApprox(
                   Mantid::Kernel::toVector3d(newSourcePos)));
    TSM_ASSERT("Check instrument position",
               beamline.componentInfo().position(3).isApprox(
                   Mantid::Kernel::toVector3d(newInstrumentPos)));
  }

  void test_visitor_detector_sanity_check() {

    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);

    // Create the visitor.
    const size_t detectorIndex =
        0; // Internally we expect detector index to start at 0

    const size_t instrumentIndex = 3; // Instrument is always hightest index.

    InstrumentVisitor visitor(makeParameterized(visitee));
    // Visit everything
    visitor.walkInstrument();

    auto beamline = visitor.beamline();

    TSM_ASSERT_EQUALS("Detector has parent of instrument",
                      beamline.componentInfo().parent(detectorIndex),
                      instrumentIndex);
    TSM_ASSERT_EQUALS(
        "Instrument has single detector",
        beamline.componentInfo().detectorsInSubtree(instrumentIndex),
        std::vector<size_t>{detectorIndex});
  }

  void test_visitor_component_check() {
    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);

    InstrumentVisitor visitor(makeParameterized(visitee));

    // Visit everything
    visitor.walkInstrument();

    std::set<Mantid::Geometry::ComponentID> componentIds(
        visitor.componentIds()->begin(), visitor.componentIds()->end());

    auto componentIdToIndexMap = visitor.componentIdToIndexMap();

    TSM_ASSERT_EQUALS("Expect 4 component Ids", componentIds.size(), 4);
    TSM_ASSERT_EQUALS("Expect 4 component Ids in map",
                      componentIdToIndexMap->size(), 4);

    TSM_ASSERT_EQUALS("Should contain the instrument id", 1,
                      componentIds.count(visitee->getComponentID()));
    TSM_ASSERT_EQUALS(
        "Should contain the sample id", 1,
        componentIds.count(visitee->getComponentByName("some-surface-holder")
                               ->getComponentID()));
    TSM_ASSERT_EQUALS("Should contain the source id", 1,
                      componentIds.count(visitee->getComponentByName("source")
                                             ->getComponentID()));

    auto detectorComponentId =
        visitee->getComponentByName("point-detector")->getComponentID();
    TSM_ASSERT_EQUALS("Should contain the detector id", 1,
                      componentIds.count(detectorComponentId));
    TSM_ASSERT_EQUALS(
        "Detectors are guaranteed to occupy the lowest component range",
        componentIdToIndexMap->at(detectorComponentId), 0);

    std::set<size_t> uniqueIndices;
    for (auto id : componentIds) {
      uniqueIndices.insert(componentIdToIndexMap->at(id));
    }
    TSM_ASSERT_EQUALS("We should have unique index values in our map",
                      uniqueIndices.size(), componentIds.size());
    TSM_ASSERT_EQUALS(
        "Indices are out of range",
        *std::max_element(uniqueIndices.begin(), uniqueIndices.end()),
        componentIds.size() - 1);
  }

  void test_visitor_detector_ranges_check() {
    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);

    // Create the visitor.
    InstrumentVisitor visitor(makeParameterized(visitee));

    // Visit everything
    visitor.walkInstrument();

    auto beamline = visitor.beamline();

    TS_ASSERT_EQUALS(beamline.componentInfo().detectorsInSubtree(3),
                     std::vector<size_t>{0});
  }

  void test_visitor_component_ranges_check() {
    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);

    InstrumentVisitor visitor(makeParameterized(visitee));
    // Visit everything
    visitor.walkInstrument();

    auto beamline = visitor.beamline();
    const auto &compInfo = beamline.componentInfo();
    const auto &detInfo = beamline.detectorInfo();

    TS_ASSERT_EQUALS(compInfo.size(), 4); // 4 components in total
    TS_ASSERT_EQUALS(detInfo.size(), 1);  // 1 component is a detector

    auto subTreeOfRoot = compInfo.componentsInSubtree(3);
    TS_ASSERT_EQUALS(
        std::set<size_t>(subTreeOfRoot.begin(), subTreeOfRoot.end()),
        (std::set<size_t>({0, 1, 2, 3})));

    auto subTreeOfNonRoot = compInfo.componentsInSubtree(1);
    TS_ASSERT_EQUALS(
        std::set<size_t>(subTreeOfNonRoot.begin(), subTreeOfNonRoot.end()),
        (std::set<size_t>({1})));
  }
  void test_visitor_collects_detector_id_to_index_mappings() {

    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);

    InstrumentVisitor visitor(makeParameterized(visitee));
    // Visit everything
    visitor.walkInstrument();

    TS_ASSERT_EQUALS(visitor.detectorIdToIndexMap()->size(), 1);
    TS_ASSERT_EQUALS(visitor.detectorIdToIndexMap()->at(1),
                     0); // ID 1 to index 0

    TS_ASSERT_EQUALS(visitor.detectorIds()->size(), 1);
    TS_ASSERT_EQUALS(visitor.detectorIds()->at(0), 1); // index 0 is ID 1
  }

  void test_visitor_drops_detectors_without_id() {
    /*
     We have to go via DetectorInfo::indexOf to get the index of a detector.
     if this throws because the detector has an invalid id, we are forced to
     drop it.
     Some IDFs i.e. SNAP have montiors with detector ids <  0.
    */

    // Create a very basic instrument to visit
    auto visitee = createMinimalInstrument(V3D(0, 0, 0) /*source pos*/,
                                           V3D(10, 0, 0) /*sample pos*/
                                           ,
                                           V3D(11, 0, 0) /*detector position*/);

    // Create an add a duplicate detector
    Detector *det =
        new Detector("invalid_detector", 1 /*DUPLICATE detector id*/, nullptr);
    visitee->add(det);
    visitee->markAsDetector(det);

    InstrumentVisitor visitor(makeParameterized(visitee));

    // Visit everything
    visitor.walkInstrument();

    size_t expectedSize = 0;
    ++expectedSize; // only detector
    ++expectedSize; // source
    ++expectedSize; // sample
    ++expectedSize; // instrument
    // Note no second detector counted
    TS_ASSERT_EQUALS(visitor.size(), expectedSize);
  }

  void test_visitation_of_rectangular_detector() {

    // Need confidence that this works properly for RectangularDetectors
    const int nPixelsWide = 10; // Gives 10*10 detectors in total
    auto instrument = ComponentCreationHelper::createTestInstrumentRectangular(
        1 /*n banks*/, nPixelsWide, 1 /*sample-bank distance*/);
    InstrumentVisitor visitor(makeParameterized(instrument));
    visitor.walkInstrument();

    TSM_ASSERT_EQUALS("Wrong number of detectors registered",
                      visitor.detectorIds()->size(), nPixelsWide * nPixelsWide);
  }

  void test_parent_indices() {

    const int nPixelsWide = 10; // Gives 10*10 detectors in total
    auto instrument = ComponentCreationHelper::createTestInstrumentRectangular(
        1 /*n banks*/, nPixelsWide, 1 /*sample-bank distance*/);

    InstrumentVisitor visitor(makeParameterized(instrument));

    // Visit everything
    visitor.walkInstrument();

    auto beamline = visitor.beamline();
    const auto &compInfo = beamline.componentInfo();

    TS_ASSERT_EQUALS(compInfo.parent(compInfo.source()), compInfo.root());
    TS_ASSERT_EQUALS(compInfo.parent(compInfo.sample()), compInfo.root());
    TS_ASSERT_EQUALS(compInfo.parent(compInfo.root()), compInfo.root());
  }
};

class InstrumentVisitorTestPerformance : public CxxTest::TestSuite {
private:
  const int m_nPixels = 1000;
  boost::shared_ptr<const Mantid::Geometry::Instrument> m_instrument;

public:
  // This pair of boilerplate methods prevent the suite being created statically
  // This means the constructor isn't called when running other tests
  static InstrumentVisitorTestPerformance *createSuite() {
    return new InstrumentVisitorTestPerformance();
  }
  static void destroySuite(InstrumentVisitorTestPerformance *suite) {
    delete suite;
  }

  InstrumentVisitorTestPerformance() {
    m_instrument = makeParameterized(
        ComponentCreationHelper::createTestInstrumentRectangular(
            1 /*n banks*/, m_nPixels, 1 /*sample-bank distance*/));
  }

  void test_process_rectangular_instrument() {
    InstrumentVisitor visitor(m_instrument);
    visitor.walkInstrument();
    TS_ASSERT(visitor.size() >= size_t(m_nPixels * m_nPixels));
  }
};
#endif /* MANTID_API_INSTRUMENTVISITORVISITORTEST_H_ */