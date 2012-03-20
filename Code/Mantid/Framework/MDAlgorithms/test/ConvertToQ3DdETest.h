#ifndef MANTID_MD_CONVERT2_QxyzDE_TEST_H_
#define MANTID_MD_CONVERT2_QxyzDE_TEST_H_

#include "MantidDataObjects/EventWorkspace.h"
#include "MantidKernel/System.h"
#include "MantidKernel/Timer.h"
#include "MantidMDAlgorithms/ConvertToMDEvents.h"
#include "MantidGeometry/Crystal/OrientedLattice.h"
#include "MantidTestHelpers/ComponentCreationHelper.h"
#include "MantidTestHelpers/MDEventsTestHelper.h"
#include "MantidTestHelpers/WorkspaceCreationHelper.h"
#include "MantidMDEvents/MDWSDescription.h"
#include <cxxtest/TestSuite.h>
#include <iomanip>
#include <iostream>

using namespace Mantid;
using namespace Mantid::Kernel;
using Mantid::Geometry::OrientedLattice;
using namespace Mantid::API;
using namespace Mantid::DataObjects;
using namespace Mantid::MDAlgorithms;
using namespace Mantid::MDEvents;

class ConvertTo3DdETestHelper: public ConvertToMDEvents
{
public:
    ConvertTo3DdETestHelper(){};
   // private (PROTECTED) methods, exposed for testing:
   std::vector<double> getTransfMatrix(const std::string &wsName,MDWSDescription &TargWSDescription)const
   {
       return ConvertToMDEvents::getTransfMatrix(wsName,TargWSDescription);
   }
   /// construct meaningful dimension names:
   void buildDimNames(MDEvents::MDWSDescription &TargWSDescription)
   {
        ConvertToMDEvents::buildDimensions(TargWSDescription);
   }

 
};

// Test is transformed from ConvetToQ3DdE but actually tests some aspects of ConvertToMDEvents algorithm. 
class ConvertToQ3DdETest : public CxxTest::TestSuite
{
 std::auto_ptr<ConvertTo3DdETestHelper> pAlg;
public:
static ConvertToQ3DdETest *createSuite() { return new ConvertToQ3DdETest(); }
static void destroySuite(ConvertToQ3DdETest * suite) { delete suite; }    

void testInit(){
  
    TS_ASSERT( pAlg->isInitialized() )
 
}

void testExecThrow(){
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::CreateGroupedWorkspace2DWithRingsAndBoxes();

    AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

 
    TSM_ASSERT_THROWS(" the workspace X axis does not have units ",pAlg->setPropertyValue("InputWorkspace", ws2D->getName()),std::invalid_argument);
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));

}


//TODO:  this check has to be implemented !!!!
void t__tWithExistingLatticeTrowsLowEnergy(){
    // create model processed workpsace with 10x10 cylindrical detectors, 10 energy levels and oriented lattice
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(100,10,true);
    // add workspace energy
     ws2D->mutableRun().addProperty("Ei",2.,"meV",true);

    AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QDimensions","Q3D"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("dEAnalysisMode", "Inelastic"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MinValues", "-50.,-50.,-50,-2"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MaxValues", " 50., 50.,-50,10"));



    pAlg->execute();
    TSM_ASSERT("Should be not-successful as input energy was lower then obtained",!pAlg->isExecuted());

}
void testExecFailsOnNewWorkspaceNoLimits(){
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(100,10,true);
    // add workspace energy
     ws2D->mutableRun().addProperty("Ei",12.,"meV",true);


    AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

 
    TSM_ASSERT_THROWS_NOTHING("the inital is not in the units of energy transfer",pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
  

    pAlg->execute();
    TSM_ASSERT("Should fail as no  min-max limits were specied ",!pAlg->isExecuted());

}
void testExecFailsOnNewWorkspaceNoMaxLimits(){
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(100,10,true);
 // add workspace energy
     ws2D->mutableRun().addProperty("Ei",12.,"meV",true);


    AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QDimensions","Q3D"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("dEAnalysisMode", "Indirect"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
 
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MinValues", "-50.,-50.,-50,-2"));

    pAlg->execute();
    TSM_ASSERT("Should fail as no max limits were specified ",!pAlg->isExecuted());

}
void testExecFailsLimits_MinGeMax(){
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(100,10,true);
 // add workspace energy
     ws2D->mutableRun().addProperty("Ei",12.,"meV",true);

    AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

 
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QDimensions","Q3D"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("dEAnalysisMode", "Indirect"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
 
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MinValues", "-50.,-50.,-50,-2"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MaxValues", " 50., 50.,-50,-2"));

    pAlg->execute();
    TSM_ASSERT("Should fail as wrong max limits were specified ",!pAlg->isExecuted());

}
void testExecFine(){
    // create model processed workpsace with 10x10 cylindrical detectors, 10 energy levels and oriented lattice
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(100,10,true);
 // add workspace energy
     ws2D->mutableRun().addProperty("Ei",12.,"meV",true);


    AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QDimensions","Q3D"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("dEAnalysisMode", "Indirect"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
 
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MinValues", "-50.,-50.,-50,-2"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MaxValues", " 50., 50., 50, 20"));

    pAlg->execute();
    TSM_ASSERT("Should be successful ",pAlg->isExecuted());

}
void testExecAndAdd(){
    // create model processed workpsace with 10x10 cylindrical detectors, 10 energy levels and oriented lattice
    Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(100,10,true);

  // rotate the crystal by twenty degrees back;
     ws2D->mutableRun().getGoniometer().setRotationAngle(0,20);
     // add workspace energy
     ws2D->mutableRun().addProperty("Ei",13.,"meV",true);
 //  

     AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QDimensions","Q3D"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("dEAnalysisMode", "Indirect"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
 
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MinValues", "-50.,-50.,-50,-2"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("MaxValues", " 50., 50., 50, 20"));


    pAlg->execute();
    TSM_ASSERT("Should fail as no adding to existing ws yet ",pAlg->isExecuted());
    //TSM_ASSERT("Should be successful ",pAlg->isExecuted());
 

}

void testTransfMat1()
{
     MDEvents::MDWSDescription TWS(4);
     TWS.pLatt = std::auto_ptr<OrientedLattice>(new OrientedLattice(10.4165,3.4165,10.4165, 90., 90., 90.));
     V3D u(1,0,0);
     V3D v(0,0,1);
     Kernel::Matrix<double> U0=TWS.pLatt->setUFromVectors(u,v);
     std::vector<double> rot0=U0.get_vector();
     //ws2D->mutableSample().setOrientedLattice(latt);

     TWS.convert_to_hkl=false;
     TWS.is_uv_default=true;
     TWS.emode=1;

     // get transformation matrix from oriented lattice. 
     std::vector<double> rot=pAlg->getTransfMatrix("TestWS",TWS);
  
     for(int i=0;i<9;i++){
        TS_ASSERT_DELTA(rot0[i],rot[i],1.e-6);
     }
     Kernel::Matrix<double> rez(rot);
     V3D ez = rez*u;
     ez.normalize();
     V3D ex = rez*v;
     ex.normalize();
     TS_ASSERT_EQUALS(V3D(0,0,1),ez);
     TS_ASSERT_EQUALS(V3D(1,0,0),ex);

     // to allow recalculate axis names specific for Q3D mode
     TWS.AlgID="WSEventQ3DPowdDirectCnvFromTOF";
     pAlg->buildDimNames(TWS);
     TS_ASSERT_EQUALS("[Q1,0,0]",TWS.dimNames[0]);
     TS_ASSERT_EQUALS("[0,Q2,0]",TWS.dimNames[1]);
     TS_ASSERT_EQUALS("[0,0,Q3]",TWS.dimNames[2]);
 

}

// COMPARISON WITH HORACE:  --->
void xtestTransfMat1()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,2,3, 90., 90., 90.);
     ws2D->mutableSample().setOrientedLattice(latt);
     MDWSDescription TWS(4);


     std::vector<double> rot;
//    std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,0,0),Kernel::V3D(0,1,0));
     Kernel::Matrix<double> unit = Kernel::Matrix<double>(3,3, true);
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(unit.equals(rez,1.e-4));
}
void xtestTransfMat2()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,2,3, 75., 45., 35.);
     ws2D->mutableSample().setOrientedLattice(latt);

      std::vector<double> rot;
    //std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,0,0),Kernel::V3D(0,1,0));
     Kernel::Matrix<double> unit = Kernel::Matrix<double>(3,3, true);
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(unit.equals(rez,1.e-4));
}
void xtestTransfMat3()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,2,3, 75., 45., 35.);
     ws2D->mutableSample().setOrientedLattice(latt);

      std::vector<double> rot;
     //std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,0,0),Kernel::V3D(0,-1,0));
     Kernel::Matrix<double> unit = Kernel::Matrix<double>(3,3, true);
     unit[1][1]=-1;
     unit[2][2]=-1;
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(unit.equals(rez,1.e-4));
}
void xtestTransfMat4()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,1,3, 90., 90., 90.);
     ws2D->mutableSample().setOrientedLattice(latt);
     ws2D->mutableRun().getGoniometer().setRotationAngle(0,0);
     ws2D->mutableRun().getGoniometer().setRotationAngle(1,0);
     ws2D->mutableRun().getGoniometer().setRotationAngle(2,0);

        std::vector<double> rot;
    //std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,1,0),Kernel::V3D(1,-1,0));
     Kernel::Matrix<double> sample = Kernel::Matrix<double>(3,3, true);
     sample[0][0]= sqrt(2.)/2 ;
     sample[0][1]= sqrt(2.)/2 ;
     sample[1][0]= sqrt(2.)/2 ;
     sample[1][1]=-sqrt(2.)/2 ;
     sample[2][2]= -1 ;
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(sample.equals(rez,1.e-4));
}
void xtestTransfMat5()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,2,3, 75., 45., 90.);
     ws2D->mutableSample().setOrientedLattice(latt);
     ws2D->mutableRun().getGoniometer().setRotationAngle(0,0);
     ws2D->mutableRun().getGoniometer().setRotationAngle(1,0);
     ws2D->mutableRun().getGoniometer().setRotationAngle(2,0);

       std::vector<double> rot;
     //std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,1,0),Kernel::V3D(1,-1,0));
     Kernel::Matrix<double> sample = Kernel::Matrix<double>(3,3, true);
     //aa=[0.9521 0.3058  0.0000;  0.3058   -0.9521    0.0000;   0         0   -1.000];
     sample[0][0]= 0.9521 ;
     sample[0][1]= 0.3058 ;
     sample[1][0]= 0.3058 ;
     sample[1][1]=-0.9521 ;
     sample[2][2]= -1 ;
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(sample.equals(rez,1.e-4));
}
void xtestTransf_PSI_DPSI()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,1,1, 90., 90., 90.);
     ws2D->mutableSample().setOrientedLattice(latt);
     ws2D->mutableRun().getGoniometer().setRotationAngle(0,0); 
     ws2D->mutableRun().getGoniometer().setRotationAngle(1,-20); // Psi, dPsi
     ws2D->mutableRun().getGoniometer().setRotationAngle(2,0);

        std::vector<double> rot;
    //std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,0,0),Kernel::V3D(0,1,0));
     Kernel::Matrix<double> sample = Kernel::Matrix<double>(3,3, true);   
     sample[0][0]= 0.9397 ;
     sample[0][1]= 0.3420 ;
     sample[1][0]=-0.3420 ;
     sample[1][1]= 0.9397 ;
     sample[2][2]=  1 ;
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(sample.equals(rez,1.e-4));
}
void xtestTransf_GL()
{
     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedWorkspaceWithCylComplexInstrument(16,10,true);
     OrientedLattice * latt = new OrientedLattice(1,1,1, 90., 90., 90.);
     ws2D->mutableSample().setOrientedLattice(latt);
     ws2D->mutableRun().getGoniometer().setRotationAngle(0,20);  //gl
     ws2D->mutableRun().getGoniometer().setRotationAngle(1,0);
     ws2D->mutableRun().getGoniometer().setRotationAngle(2,0);

        std::vector<double> rot;
   // std::vector<double> rot=pAlg->get_transf_matrix(ws2D,Kernel::V3D(1,0,0),Kernel::V3D(0,1,0));
     Kernel::Matrix<double> sample = Kernel::Matrix<double>(3,3, true);
     
     sample[0][0]= 0.9397 ;
     sample[0][2]= 0.3420 ;
     sample[2][0]=-0.3420 ;
     sample[2][2]= 0.9397 ;
     sample[1][1]=  1 ;
     Kernel::Matrix<double> rez(rot);
     TS_ASSERT(sample.equals(rez,1.e-4));
}
// check the results;
void t__tResult(){
     std::vector<double> L2(3,10),polar(3,0),azim(3,0);
     polar[1]=1;
     polar[2]=2;
     azim[0]=-1;
     azim[2]= 1;

     Mantid::API::MatrixWorkspace_sptr ws2D =WorkspaceCreationHelper::createProcessedInelasticWS(L2,polar,azim,3,-1,2,10);

     ws2D->mutableRun().getGoniometer().setRotationAngle(0,0);  //gl
     ws2D->mutableRun().getGoniometer().setRotationAngle(1,0);
     ws2D->mutableRun().getGoniometer().setRotationAngle(2,0);
  
     AnalysisDataService::Instance().addOrReplace("testWSProcessed", ws2D);

 
    TSM_ASSERT_THROWS_NOTHING("the inital is not in the units of energy transfer",pAlg->setPropertyValue("InputWorkspace", ws2D->getName()));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("OutputWorkspace", "EnergyTransfer4DWS"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("EnergyInput", "12."));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QdEValuesMin", "-10.,-10.,-10,-2"));
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("QdEValuesMax", " 10., 10., 10, 8"))
    TS_ASSERT_THROWS_NOTHING(pAlg->setPropertyValue("UsePreprocessedDetectors","0"));

    pAlg->execute();
    TSM_ASSERT("Should be successful ",pAlg->isExecuted());

    Mantid::API::Workspace_sptr wsOut =  AnalysisDataService::Instance().retrieve("EnergyTransfer4DWS");
    TSM_ASSERT("Can not retrieve resulting workspace from the analysis data service ",wsOut);
}


// COMPARISON WITH HORACE: END  <---



ConvertToQ3DdETest(){
    pAlg = std::auto_ptr<ConvertTo3DdETestHelper>(new ConvertTo3DdETestHelper());
    pAlg->initialize();
}

};


#endif /* MANTID_MDEVENTS_MAKEDIFFRACTIONMDEVENTWORKSPACETEST_H_ */

