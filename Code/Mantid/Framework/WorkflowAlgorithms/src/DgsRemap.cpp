/*WIKI*

This algorithm is responsible for masking and grouping the given input workspace.

*WIKI*/

#include "MantidWorkflowAlgorithms/DgsRemap.h"
#include "MantidDataObjects/GroupingWorkspace.h"

using namespace Mantid::Kernel;
using namespace Mantid::API;
using namespace Mantid::DataObjects;

namespace Mantid
{
namespace WorkflowAlgorithms
{

  // Register the algorithm into the AlgorithmFactory
  DECLARE_ALGORITHM(DgsRemap)
  
  //----------------------------------------------------------------------------------------------
  /** Constructor
   */
  DgsRemap::DgsRemap()
  {
  }
    
  //----------------------------------------------------------------------------------------------
  /** Destructor
   */
  DgsRemap::~DgsRemap()
  {
  }

  //----------------------------------------------------------------------------------------------
  /// Algorithm's name for identification. @see Algorithm::name
  const std::string DgsRemap::name() const { return "DgsRemap"; };
  
  /// Algorithm's version for identification. @see Algorithm::version
  int DgsRemap::version() const { return 1; };
  
  /// Algorithm's category for identification. @see Algorithm::category
  const std::string DgsRemap::category() const { return "Workflow\\Inelastic"; }

  //----------------------------------------------------------------------------------------------
  /// Sets documentation strings for this algorithm
  void DgsRemap::initDocs()
  {
    this->setWikiSummary("Mask and/or group the given workspace.");
    this->setOptionalMessage("Mask and/or group the given workspace.");
  }

  //----------------------------------------------------------------------------------------------
  /** Initialize the algorithm's properties.
   */
  void DgsRemap::init()
  {
    this->declareProperty(new WorkspaceProperty<MatrixWorkspace>("InputWorkspace",
        "", Direction::Input), "An input workspace to mask and group.");
    this->declareProperty(new WorkspaceProperty<MatrixWorkspace>("MaskWorkspace",
        "", Direction::Input), "A workspace containing masking information.");
    this->declareProperty(new WorkspaceProperty<MatrixWorkspace>("GroupingWorkspace",
        "", Direction::Input), "A workspace containing grouping information");
    this->declareProperty(new WorkspaceProperty<MatrixWorkspace>("OutputWorkspace",
        "", Direction::Output), "The resulting workspace.");
  }

  //----------------------------------------------------------------------------------------------
  /** Execute the algorithm.
   */
  void DgsRemap::exec()
  {
    MatrixWorkspace_sptr inputWS = this->getProperty("InputWorkspace");
    MatrixWorkspace_sptr maskWS = this->getProperty("MaskWorkspace");
    MatrixWorkspace_sptr groupWS = this->getProperty("GroupingWorkspace");
    MatrixWorkspace_sptr outputWS = this->getProperty("OutputWorkspace");

    if (maskWS)
    {
      IAlgorithm_sptr mask = this->createSubAlgorithm("MaskDetectors");
      mask->setProperty("Workspace", inputWS);
      mask->setProperty("MaskedWorkspace", maskWS);
      mask->executeAsSubAlg();
    }

    if (groupWS)
    {
      int64_t ngroups = 0;
      std::vector<int> groupDetIdList;
      GroupingWorkspace_sptr gWS = boost::dynamic_pointer_cast<GroupingWorkspace>(groupWS);
      gWS->makeDetectorIDToGroupVector(groupDetIdList, ngroups);

      IAlgorithm_sptr group = this->createSubAlgorithm("GroupDetectors");
      group->setProperty("InputWorkspace", inputWS);
      group->setProperty("OutputWorkspace", outputWS);
      group->setProperty("DetectorList", groupDetIdList);
      group->setProperty("Behaviour", "Average");
      group->executeAsSubAlg();
    }

    this->setProperty("OutputWorkspace", outputWS);
  }
} // namespace WorkflowAlgorithms
} // namespace Mantid
