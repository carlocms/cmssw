// -*- C++ -*-
//
// Package:    Validation/MtdValidation
// Class:      BtlLocalRecoValidation
//
/**\class BtlTimeMonitoring BtlTimeMonitoring.cc Validation/MtdValidation/plugins/BtlTimeMonitoring.cc

 Description: BTL Time Monitoring 

 Implementation:
     [Notes on implementation]
*/

#include <string>
#include <fstream>
#include <iostream> 
#include <cmath>
#include <vector>
#include <algorithm>
#include <TH1F.h>
#include <TFile.h>
#include <random>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/Common/interface/ValidHandle.h"
#include "DataFormats/Math/interface/GeantUnits.h"
#include "DataFormats/ForwardDetId/interface/BTLDetId.h"
#include "DataFormats/FTLRecHit/interface/FTLRecHitCollections.h"
#include "DataFormats/FTLRecHit/interface/FTLClusterCollections.h"
#include "DataFormats/TrackerRecHit2D/interface/MTDTrackingRecHit.h"

#include "SimDataFormats/CaloAnalysis/interface/MtdSimLayerCluster.h"
#include "SimDataFormats/Associations/interface/MtdRecoClusterToSimLayerClusterAssociationMap.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include "Geometry/Records/interface/MTDDigiGeometryRecord.h"
#include "Geometry/Records/interface/MTDTopologyRcd.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDTopology.h"

#include "Geometry/MTDGeometryBuilder/interface/ProxyMTDTopology.h"
#include "Geometry/MTDGeometryBuilder/interface/RectangularMTDTopology.h"

#include "Geometry/MTDCommonData/interface/MTDTopologyMode.h"

#include "RecoLocalFastTime/Records/interface/MTDCPERecord.h"
#include "RecoLocalFastTime/FTLClusterizer/interface/MTDClusterParameterEstimator.h"

#include "MTDHit.h"

//-----------------------------------------------------
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h" 
#include "DataFormats/Candidate/interface/Candidate.h"
#include <CLHEP/Units/GlobalPhysicalConstants.h>
#include <SimDataFormats/Track/interface/SimTrack.h>
#include <SimDataFormats/Track/interface/CoreSimTrack.h>
#include <SimDataFormats/Track/interface/SimTrackContainer.h>
#include <SimDataFormats/Vertex/interface/CoreSimVertex.h>
#include <SimDataFormats/Vertex/interface/SimVertex.h>
#include <SimDataFormats/Vertex/interface/SimVertexContainer.h>
//-----------------------------------------------------



class BtlTimeMonitoring : public DQMEDAnalyzer {
public:
  explicit BtlTimeMonitoring(const edm::ParameterSet&);
  ~BtlTimeMonitoring() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
 
  void analyze(const edm::Event&, const edm::EventSetup&) override;
 
  bool isSameCluster(const FTLCluster&, const FTLCluster&);
  
 
 // ------------ member data ------------

  const std::string folder_;
  const double hitMinEnergy_;
  const bool optionalPlots_;
  const bool uncalibRecHitsPlots_;
  const double hitMinAmplitude_;

  edm::EDGetTokenT<FTLRecHitCollection> btlRecHitsToken_;
  edm::EDGetTokenT<FTLUncalibratedRecHitCollection> btlUncalibRecHitsToken_;
  edm::EDGetTokenT<CrossingFrame<PSimHit>> btlSimHitsToken_;
  edm::EDGetTokenT<FTLClusterCollection> btlRecCluToken_;
  edm::EDGetTokenT<MTDTrackingDetSetVector> mtdTrackingHitToken_;
  edm::EDGetTokenT<MtdRecoClusterToSimLayerClusterAssociationMap> r2sAssociationMapToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  
  //--------  
  edm::EDGetTokenT<std::vector<reco::GenParticle>> genParticlesToken_;
  edm::EDGetTokenT<edm::SimTrackContainer> simTracksToken_;
  edm::EDGetTokenT<edm::SimVertexContainer> simVerticesToken_;
  //-------

  const edm::ESGetToken<MTDGeometry, MTDDigiGeometryRecord> mtdgeoToken_;
  const edm::ESGetToken<MTDTopology, MTDTopologyRcd> mtdtopoToken_;
  const edm::ESGetToken<MTDClusterParameterEstimator, MTDCPERecord> cpeToken_;

  // --- histograms declaration

  MonitorElement* meNevents_;

  MonitorElement* meNhits_;

  MonitorElement* meHitEnergy_;
  MonitorElement* meHitLogEnergy_;
  MonitorElement* meHitTime_;
  MonitorElement* meHitTimeError_;

  MonitorElement* meOccupancy_;

  static constexpr int nTR_ = 36;
  static constexpr int nRU_ = 6;
  static constexpr int nSM_ = 8;
  static constexpr int nCR_ = 128;
  static constexpr int nRU_tot_ = 12;
  static constexpr int nSMphi_ = 96;
  static constexpr int nSMinRU_ = 24;
  static constexpr double MinRecoHitEnergy_ = 2.;
  static constexpr double MaxRecoHitEnergy_ = 16.;
  static constexpr double MaxRecoHitTime_ = 19.;
  static constexpr double MinRecoHitTime_ = -2.;
/*
  MonitorElement* meHitTimeCorr_;
  MonitorElement* meHitTimeRuSlice_corr_[nRU_tot_];
  MonitorElement* meHitTimeSingleRU_[nRU_tot_][nTR_];
  MonitorElement* meHitTimeSmSlice_[nSMphi_];
  MonitorElement* meHitTimeSingleSmInRu_[nRU_tot_][nSMinRU_];
*/


  //local position monitoring
  MonitorElement* meLocalOccupancy_;
  MonitorElement* meHitXlocal_;
  MonitorElement* meHitYlocal_;
  MonitorElement* meHitZlocal_;

  MonitorElement* meHitZ_;
  MonitorElement* meHitPhi_;
  MonitorElement* meHitEta_;

  MonitorElement* meHitTvsE_;
  MonitorElement* meHitEvsPhi_;
  MonitorElement* meHitEvsEta_;
  MonitorElement* meHitEvsZ_;
  MonitorElement* meHitTvsPhi_;
  MonitorElement* meHitTvsEta_;
  MonitorElement* meHitTvsZ_;
  MonitorElement* meHitLongPos_;

  MonitorElement* meTimeRes_;
  MonitorElement* meTimeResVsE_;
  MonitorElement* meEnergyRes_;
  MonitorElement* meEnergyRelResVsE_;

  MonitorElement* meLongPosPull_;
  MonitorElement* meLongPosPullvsE_;
  MonitorElement* meLongPosPullvsEta_;

  MonitorElement* meTPullvsE_;
  MonitorElement* meTPullvsEta_;
  MonitorElement* meUnmatchedRecHit_;

  MonitorElement* meNclusters_;

  MonitorElement* meCluTime_;
  MonitorElement* meCluTimeError_;
  MonitorElement* meCluEnergy_;
  MonitorElement* meCluPhi_;
  MonitorElement* meCluEta_;
  MonitorElement* meCluHits_;
  MonitorElement* meCluZvsPhi_;
  MonitorElement* meCluEnergyvsEta_;
  MonitorElement* meCluHitsvsEta_;

  MonitorElement* meCluTimeRes_;
  MonitorElement* meCluEnergyRes_;
  MonitorElement* meCluTResvsE_;
  MonitorElement* meCluTResvsEta_;
  MonitorElement* meCluTPullvsE_;
  MonitorElement* meCluTPullvsEta_;
  MonitorElement* meCluRhoRes_;
  MonitorElement* meCluPhiRes_;
  MonitorElement* meCluLocalXRes_;

  MonitorElement* meCluLocalYResZGlobPlus_;
  MonitorElement* meCluLocalYResZGlobMinus_;

  MonitorElement* meCluZRes_;
  MonitorElement* meCluLocalXPull_;

  MonitorElement* meCluLocalYPullZGlobPlus_;
  MonitorElement* meCluLocalYPullZGlobMinus_;

  MonitorElement* meCluSingCrystalLocalYRes_;
  MonitorElement* meCluSingCrystalLocalYResZGlobPlus_;
  MonitorElement* meCluSingCrystalLocalYResZGlobMinus_;

  MonitorElement* meCluMultiCrystalLocalYRes_;
  MonitorElement* meCluMultiCrystalLocalYResZGlobPlus_;
  MonitorElement* meCluMultiCrystalLocalYResZGlobMinus_;

  MonitorElement* meCluCentralLocalYRes_;
  MonitorElement* meCluCentralLocalYResZGlobPlus_;
  MonitorElement* meCluCentralLocalYResZGlobMinus_;

  MonitorElement* meCluForwardLocalYRes_;
  MonitorElement* meCluForwardPlusLocalYRes_;
  MonitorElement* meCluForwardMinusLocalYRes_;

  MonitorElement* meCluZPull_;
  MonitorElement* meCluYXLocal_;
  MonitorElement* meCluYXLocalSim_;
  MonitorElement* meCluXLocalErr_;
  MonitorElement* meCluYLocalErr_;

  // resolution wrt to MtdSimLayerClusters
  MonitorElement* meCluTrackIdOffset_;

  MonitorElement* meCluTimeRes_simLC_;
  MonitorElement* meCluEnergyRes_simLC_;
  MonitorElement* meCluTResvsE_simLC_;
  MonitorElement* meCluTResvsEta_simLC_;
  MonitorElement* meCluTPullvsE_simLC_;
  MonitorElement* meCluTPullvsEta_simLC_;
  MonitorElement* meCluRhoRes_simLC_;
  MonitorElement* meCluPhiRes_simLC_;
  MonitorElement* meCluLocalXRes_simLC_;

  MonitorElement* meCluLocalYResZGlobPlus_simLC_;
  MonitorElement* meCluLocalYResZGlobMinus_simLC_;

  MonitorElement* meCluZRes_simLC_;
  MonitorElement* meCluLocalXPull_simLC_;

  MonitorElement* meCluLocalYPullZGlobPlus_simLC_;
  MonitorElement* meCluLocalYPullZGlobMinus_simLC_;

  MonitorElement* meCluSingCrystalLocalYRes_simLC_;
  MonitorElement* meCluSingCrystalLocalYResZGlobPlus_simLC_;
  MonitorElement* meCluSingCrystalLocalYResZGlobMinus_simLC_;

  MonitorElement* meCluMultiCrystalLocalYRes_simLC_;
  MonitorElement* meCluMultiCrystalLocalYResZGlobPlus_simLC_;
  MonitorElement* meCluMultiCrystalLocalYResZGlobMinus_simLC_;

  MonitorElement* meCluCentralLocalYRes_simLC_;
  MonitorElement* meCluCentralLocalYResZGlobPlus_simLC_;
  MonitorElement* meCluCentralLocalYResZGlobMinus_simLC_;

  MonitorElement* meCluForwardLocalYRes_simLC_;
  MonitorElement* meCluForwardPlusLocalYRes_simLC_;
  MonitorElement* meCluForwardMinusLocalYRes_simLC_;

  MonitorElement* meCluZPull_simLC_;
  MonitorElement* meCluYXLocalSim_simLC_;

  MonitorElement* meCluTimeRes_simLC_fromIndirectHits_;
  MonitorElement* meCluEnergyRes_simLC_fromIndirectHits_;
  MonitorElement* meCluTResvsE_simLC_fromIndirectHits_;
  MonitorElement* meCluTResvsEta_simLC_fromIndirectHits_;
  MonitorElement* meCluTPullvsE_simLC_fromIndirectHits_;
  MonitorElement* meCluTPullvsEta_simLC_fromIndirectHits_;
  MonitorElement* meCluRhoRes_simLC_fromIndirectHits_;
  MonitorElement* meCluPhiRes_simLC_fromIndirectHits_;
  MonitorElement* meCluLocalXRes_simLC_fromIndirectHits_;

  MonitorElement* meCluLocalYResZGlobPlus_simLC_fromIndirectHits_;
  MonitorElement* meCluLocalYResZGlobMinus_simLC_fromIndirectHits_;

  MonitorElement* meCluZRes_simLC_fromIndirectHits_;
  MonitorElement* meCluLocalXPull_simLC_fromIndirectHits_;

  MonitorElement* meCluLocalYPullZGlobPlus_simLC_fromIndirectHits_;
  MonitorElement* meCluLocalYPullZGlobMinus_simLC_fromIndirectHits_;

  MonitorElement* meCluSingCrystalLocalYRes_simLC_fromIndirectHits_;
  MonitorElement* meCluSingCrystalLocalYResZGlobPlus_simLC_fromIndirectHits_;
  MonitorElement* meCluSingCrystalLocalYResZGlobMinus_simLC_fromIndirectHits_;

  MonitorElement* meCluMultiCrystalLocalYRes_simLC_fromIndirectHits_;
  MonitorElement* meCluMultiCrystalLocalYResZGlobPlus_simLC_fromIndirectHits_;
  MonitorElement* meCluMultiCrystalLocalYResZGlobMinus_simLC_fromIndirectHits_;

  MonitorElement* meCluCentralLocalYRes_simLC_fromIndirectHits_;
  MonitorElement* meCluCentralLocalYResZGlobPlus_simLC_fromIndirectHits_;
  MonitorElement* meCluCentralLocalYResZGlobMinus_simLC_fromIndirectHits_;

  MonitorElement* meCluForwardLocalYRes_simLC_fromIndirectHits_;
  MonitorElement* meCluForwardPlusLocalYRes_simLC_fromIndirectHits_;
  MonitorElement* meCluForwardMinusLocalYRes_simLC_fromIndirectHits_;

  MonitorElement* meCluZPull_simLC_fromIndirectHits_;
  MonitorElement* meCluYXLocalSim_simLC_fromIndirectHits_;

  MonitorElement* meUnmatchedCluEnergy_;

  // --- Gen Particle histograms
  MonitorElement* meGenPt_;
  MonitorElement* meGenEne_;
  MonitorElement* meGenEta_;
  MonitorElement* meGenTheta_;
  MonitorElement* meGenPhi_;
  MonitorElement* meGenPdg_;


  // --- UncalibratedRecHits histograms

  static constexpr double calib_EneRecoHit_ = 0.03125;
  static constexpr double hitMaxTime_= 19.;
  static constexpr double hitMaxAmplitude_= 450.;
  static constexpr double hitMinimumAmplitude_ = 55.; //55. default
  static constexpr double tof_internal_ = 2.85 / 13.846235;
  static constexpr double p0_ = 2.21103;
  static constexpr double p1_ = -0.933552;
  static constexpr double simUnit_ = 1e9;

  MonitorElement* meUncEneLVsX_;
  MonitorElement* meUncEneRVsX_;
  MonitorElement* meUncTimeLVsX_;
  MonitorElement* meUncTimeRVsX_; 
  
  MonitorElement* meUncTimeMean_;
  MonitorElement* meUncTimeMean_corr_;
  MonitorElement* meUncTimeRUSlice_[nRU_tot_];
  MonitorElement* meUncTimeRUSlice_corr_[nRU_tot_];
  MonitorElement* meUncTime_SingleRU_[nRU_tot_][nTR_];
  MonitorElement* meUncTimePhiSlice_[nSMphi_];
  MonitorElement* meUncAmpl_global_;
  MonitorElement* meUncEne_global_;
  MonitorElement* meUncTime_Left_;
  MonitorElement* meUncTime_Right_;
  MonitorElement* meUncTimeCorr_Left_;
  MonitorElement* meUncTimeCorr_Right_;
  MonitorElement* meUncTimeSingleSmInRu_[nRU_tot_][nSMinRU_];
  
  MonitorElement* meUncTimeCrystalRing_RU1_;
  MonitorElement* meUncTimeCrystalRing_RU2_;
  MonitorElement* meUncTimeCrystalRing_RU3_;
  MonitorElement* meUncTimeCrystalRing_RU4_;
  MonitorElement* meUncTimeCrystalRing_RU5_;
  MonitorElement* meUncTimeCrystalRing_RU6_;
  MonitorElement* meUncTimeCrystalRing_RU7_;
  MonitorElement* meUncTimeCrystalRing_RU8_;
  MonitorElement* meUncTimeCrystalRing_RU9_;
  MonitorElement* meUncTimeCrystalRing_RU10_;
  MonitorElement* meUncTimeCrystalRing_RU11_;
  MonitorElement* meUncTimeCrystalRing_RU12_;

  MonitorElement* meUncHitTcorr_vs_A_Ctl_;
  MonitorElement* meUncHitT_vs_A_Ctl_;
  MonitorElement* meUncHitTcorr_vs_A_Forward_;
  MonitorElement* meUncHitT_vs_A_Forward_;

  MonitorElement* meUncHitT_L_corr_vs_A_Ctl_;
  MonitorElement* meUncHitT_R_corr_vs_A_Ctl_;
 // MonitorElement* meUncHitT_L_corr_vs_A_Forward_;
 // MonitorElement* meUncHitT_R_corr_vs_A_Forward_;
  MonitorElement* meUncHitT_L_vs_A_Ctl_;
  MonitorElement* meUncHitT_R_vs_A_Ctl_;
  //MonitorElement* meUncHitT_L_vs_A_Forward_;
  //MonitorElement* meUncHitT_R_vs_A_Forward_;
  //MonitorElement* meUncHitTcorr_vs_A_Forward_noWT_;
  MonitorElement* meUncHitTcorr_vs_A_Ctl_noWT_;
  MonitorElement* meUncHitT_vs_A_Ctl_noWT_;
  //MonitorElement* meUncHitT_vs_A_Forward_noWT_;
  MonitorElement* meUncHitT_L_corr_vs_A_Ctl_noWT_;
  //MonitorElement* meUncHitT_L_corr_vs_A_Forward_noWT_;
  MonitorElement* meUncHitT_L_vs_A_Ctl_noWT_;
  //MonitorElement* meUncHitT_L_vs_A_Forward_noWT_;
  MonitorElement* meUncHitT_R_corr_vs_A_Ctl_noWT_;
  //MonitorElement* meUncHitT_R_corr_vs_A_Forward_noWT_;
  MonitorElement* meUncHitT_R_vs_A_Ctl_noWT_;
  //MonitorElement* meUncHitT_R_vs_A_Forward_noWT_;
  MonitorElement* meUncHit_DeltaT_R_Ctl_;
  MonitorElement* meUncHit_DeltaT_L_Ctl_;


  //Test sample PI:
  MonitorElement* meSimHitTime_;
  MonitorElement* meSimHitTimeCorr_;


  static constexpr int nBinsQ_ = 20;
  static constexpr float binWidthQ_ = 30.;
  static constexpr int nBinsQEta_ = 3;
  static constexpr float binsQEta_[nBinsQEta_ + 1] = {0., 0.65, 1.15, 1.55};

  MonitorElement* meTimeResQ_[nBinsQ_];
  MonitorElement* meTimeResQvsEta_[nBinsQ_][nBinsQEta_];

  static constexpr int nBinsEta_ = 31;
  static constexpr float binWidthEta_ = 0.05;
  static constexpr int nBinsEtaQ_ = 7;
  static constexpr float binsEtaQ_[nBinsEtaQ_ + 1] = {0., 30., 60., 90., 120., 150., 360., 600.};

  MonitorElement* meTimeResEta_[nBinsEta_];
  MonitorElement* meTimeResEtavsQ_[nBinsEta_][nBinsEtaQ_];
};

bool BtlTimeMonitoring::isSameCluster(const FTLCluster& clu1, const FTLCluster& clu2) {
  return clu1.id() == clu2.id() && clu1.size() == clu2.size() && clu1.x() == clu2.x() && clu1.y() == clu2.y() &&
         clu1.time() == clu2.time();
}

// ------------ constructor and destructor --------------
BtlTimeMonitoring::BtlTimeMonitoring(const edm::ParameterSet& iConfig)
    : folder_(iConfig.getParameter<std::string>("folder")),
      hitMinEnergy_(iConfig.getParameter<double>("HitMinimumEnergy")),
      optionalPlots_(iConfig.getParameter<bool>("optionalPlots")),
      uncalibRecHitsPlots_(iConfig.getParameter<bool>("UncalibRecHitsPlots")),
      hitMinAmplitude_(iConfig.getParameter<double>("HitMinimumAmplitude")),
      mtdgeoToken_(esConsumes<MTDGeometry, MTDDigiGeometryRecord>()),
      mtdtopoToken_(esConsumes<MTDTopology, MTDTopologyRcd>()),
      cpeToken_(esConsumes<MTDClusterParameterEstimator, MTDCPERecord>(edm::ESInputTag("", "MTDCPEBase"))) {
  btlRecHitsToken_ = consumes<FTLRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitsTag"));
  btlUncalibRecHitsToken_ =
      consumes<FTLUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>("uncalibRecHitsTag"));
  btlSimHitsToken_ = consumes<CrossingFrame<PSimHit>>(iConfig.getParameter<edm::InputTag>("simHitsTag"));
  btlRecCluToken_ = consumes<FTLClusterCollection>(iConfig.getParameter<edm::InputTag>("recCluTag"));
  mtdTrackingHitToken_ = consumes<MTDTrackingDetSetVector>(iConfig.getParameter<edm::InputTag>("trkHitTag"));
  r2sAssociationMapToken_ = consumes<MtdRecoClusterToSimLayerClusterAssociationMap>(
      iConfig.getParameter<edm::InputTag>("r2sAssociationMapTag"));
   beamSpotToken_ = consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));
   genParticlesToken_ = consumes<std::vector<reco::GenParticle>>(edm::InputTag("genParticles", "", "HLT"));
   simTracksToken_ = consumes<edm::SimTrackContainer>(edm::InputTag("g4SimHits"));
   simVerticesToken_ = consumes<edm::SimVertexContainer>(edm::InputTag("g4SimHits"));

      }

BtlTimeMonitoring::~BtlTimeMonitoring() {}

// ------------ method called for each event  ------------
void BtlTimeMonitoring::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;
  using namespace geant_units::operators;

  auto geometryHandle = iSetup.getTransientHandle(mtdgeoToken_);
  const MTDGeometry* geom = geometryHandle.product();
  auto topologyHandle = iSetup.getTransientHandle(mtdtopoToken_);
  const MTDTopology* topology = topologyHandle.product();

  auto const& cpe = iSetup.getData(cpeToken_);

  // Recupera il BeamSpot--------------------------------------------------------
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(beamSpotToken_, beamSpotHandle);

  if (!beamSpotHandle.isValid()) {
    throw cms::Exception("BtlTimeMonitoring") << "BeamSpot is not available in the event!";
  }
  const reco::BeamSpot& beamSpot = *beamSpotHandle;

  auto btlRecHitsHandle = makeValid(iEvent.getHandle(btlRecHitsToken_));
  auto btlSimHitsHandle = makeValid(iEvent.getHandle(btlSimHitsToken_));
  auto btlRecCluHandle = makeValid(iEvent.getHandle(btlRecCluToken_));
  auto mtdTrkHitHandle = makeValid(iEvent.getHandle(mtdTrackingHitToken_));
  const auto& r2sAssociationMap = iEvent.get(r2sAssociationMapToken_);
  MixCollection<PSimHit> btlSimHits(btlSimHitsHandle.product());
  auto genParticleHandle = makeValid(iEvent.getHandle(genParticlesToken_));
  auto simVerticesHandle = makeValid(iEvent.getHandle(simVerticesToken_));
  edm::Handle<edm::SimTrackContainer> simTracks;
  iEvent.getByToken(simTracksToken_, simTracks);
  if (!simTracks.isValid()) {
	  edm::LogError("SimTrack") << "Error: SimTrack collection not found!";
	  return;
  }


#ifdef EDM_ML_DEBUG
  for (const auto& hits : *mtdTrkHitHandle) {
    if (MTDDetId(hits.id()).mtdSubDetector() == MTDDetId::MTDType::BTL) {
      LogDebug("BtlTimeMonitoring") << "MTD cluster DetId " << hits.id() << " # cluster " << hits.size();
      for (const auto& hit : hits) {
        LogDebug("BtlTimeMonitoring")
            << "MTD_TRH: " << hit.localPosition().x() << "," << hit.localPosition().y() << " : "
            << hit.localPositionError().xx() << "," << hit.localPositionError().yy() << " : " << hit.time() << " : "
            << hit.timeError();
      }
    }
  }
#endif

/*
  //test per campione PI:
  std::unordered_map<int, float> trackIdToVtxTime;
  for (const auto& simTrack : *simTracks) {
	  int vtxIndex = simTrack.vertIndex();
	  if (vtxIndex >= 0 && (unsigned)vtxIndex < simVerticesHandle->size()) {
		  float t_vtx = (*simVerticesHandle)[vtxIndex].position().t() * simUnit_;  // in ns
		  std::cout << " t_vtx:" << t_vtx << std::endl;
		  trackIdToVtxTime[simTrack.trackId()] = t_vtx;
	  }
  }
*/



  // --- Loop over the BTL SIM hits
  std::unordered_map<uint32_t, MTDHit> m_btlSimHits;
  std::unordered_map<uint32_t, int> m_btlSimTrackId; //potrebbe essere utile per simHit types 


  for (auto const& simHit : btlSimHits) {
    // --- Use only hits compatible with the in-time bunch-crossing
    if (simHit.tof() < 0 || simHit.tof() > 25.)
      continue; 

    DetId id = simHit.detUnitId();

    auto simHitIt = m_btlSimHits.emplace(id.rawId(), MTDHit()).first;

    // --- Accumulate the energy (in MeV) of SIM hits in the same detector cell
    (simHitIt->second).energy += convertUnitsTo(0.001_MeV, simHit.energyLoss());

    // --- Get the time of the first SIM hit in the cell
    if ((simHitIt->second).time == 0 || simHit.tof() < (simHitIt->second).time) {
      (simHitIt->second).time = simHit.tof();

      auto hit_pos = simHit.localPosition();
      (simHitIt->second).x = hit_pos.x();
      (simHitIt->second).y = hit_pos.y();
      (simHitIt->second).z = hit_pos.z();

      m_btlSimTrackId[id.rawId()] = simHit.offsetTrackId();

      /*
      //Test sample PI:
      int trackId = simHit.trackId();
      float tof = simHit.tof();
      float vtxTime = 0.;
      auto it = trackIdToVtxTime.find(trackId);

      if (it != trackIdToVtxTime.end()) {
	      vtxTime = it->second;
      }
      float correctedTime_SIM = tof - vtxTime;
      meSimHitTime_->Fill(tof);
      meSimHitTimeCorr_->Fill(correctedTime_SIM);
*/      
    }

  }  // simHit loop

//---------------------------------------------------------------------
 
  for (const auto& genParticle : *genParticleHandle) {

	  if (genParticle.status() == 1) {

      	  float pt = genParticle.pt();
	  int pdgId = genParticle.pdgId();
	  float gen_ene = genParticle.energy();
	  float gen_theta = genParticle.theta();
	  float gen_phi = genParticle.phi();
          float gen_eta = genParticle.eta();

	  meGenPt_->Fill(pt);
	  meGenEne_->Fill(gen_ene);
	  meGenEta_->Fill(gen_eta);
	  meGenPhi_->Fill(gen_phi);
	  meGenTheta_->Fill(gen_theta);
	  meGenPdg_->Fill(pdgId);

	  }
  }

//--------------------------------------------------------------------
  
  // --- Loop over the BTL RECO hits
  unsigned int n_reco_btl = 0;
  unsigned int n_reco_btl_nosimhit = 0;
  for (const auto& recHit : *btlRecHitsHandle) {
    LogTrace("BtlTimeMonitoring") << "@RH detid " << recHit.id().rawId() << " r/c/X/dX " << recHit.row() << " "
                                       << recHit.column() << " " << recHit.position() << " " << recHit.positionError()
                                       << " E,T,dT " << recHit.energy() << " " << recHit.time() << " "
                                       << recHit.timeError();
/*
    //-----------------------------------------
    //Energy selection RecoHit:
    if (recHit.energy() > MaxRecoHitEnergy_)
    	    continue;
    if (recHit.energy() < MinRecoHitEnergy_)  
      	    continue;

    //Time cut for the saturation peak:
    if (recHit.time()>MaxRecoHitTime_)
      	    continue;
    //----------------------------------------
*/
    BTLDetId detId = recHit.id();
    DetId geoId = detId.geographicalId(MTDTopologyMode::crysLayoutFromTopoMode(topology->getMTDTopologyMode()));
    const MTDGeomDet* thedet = geom->idToDet(geoId);
    if (thedet == nullptr)
      throw cms::Exception("BtlTimeMonitoring") << "GeographicalID: " << std::hex << geoId.rawId() << " ("
                                                     << detId.rawId() << ") is invalid!" << std::dec << std::endl;
    const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(thedet->topology());
    const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());

    Local3DPoint local_point(0., 0., 0.);
    local_point = topo.pixelToModuleLocalPoint(local_point, detId.row(topo.nrows()), detId.column(topo.nrows()));
    const auto& global_point = thedet->toGlobal(local_point);

    meHitEnergy_->Fill(recHit.energy());
    meHitLogEnergy_->Fill(log10(recHit.energy()));
    meHitTime_->Fill(recHit.time());
    meHitTimeError_->Fill(recHit.timeError());
    meHitLongPos_->Fill(recHit.position());

    meOccupancy_->Fill(global_point.z(), global_point.phi());

/*
    //--------------------------------------------------------------------------------------
    //Distance beam spot<-> center of the hitted crystal:
    float DistanceFromBS = std::sqrt(std::pow(global_point.x() - beamSpot.x0(), 2) +
		       std::pow(global_point.y() - beamSpot.y0(), 2) +
		       std::pow(global_point.z() - beamSpot.z0(), 2));

    constexpr float c_cm_ns = geant_units::operators::convertMmToCm(CLHEP::c_light);  // [mm/ns] -> [cm/ns]

    //--- TOF photon-like:
    float TOF_FromBS = DistanceFromBS / c_cm_ns;

    //--- Hit Time-Stamp correction:  
    float RecoHitTime_corr = recHit.time() - TOF_FromBS;


    //Time cut on RecoHitTime_corr:
    if(RecoHitTime_corr < MinRecoHitTime_)
            continue;



    //--- Sensor Module Index extrapolation:  
    auto index = topology->btlIndex(geoId.rawId());
    //uint32_t SmPhiIndex = index.first;
    uint32_t SmEtaIndex = index.second;

    //--- RU Slice Index 1->First at the left side extreme to 12-> last at the right side extreme 
    int RU_index = (detId.mtdSide() == 0) ? (7 - detId.runit()) : (6 + detId.runit());

    //--- SM index in a Single RU: 0->23:
    int SMinRU_index = detId.dmodule() + (detId.dmodule() + detId.smodule()); 

    //int crystal_index = detId.crystal() + (SMeta_index-1)*16;
    //int adjustedSmEtaIndex = SmEtaIndex - (RU_index-1)*8;

      //----- HISTOGRAMS ----
      // --- RecoHIt Time w/ TOF correction:
      meHitTimeCorr_->Fill(RecoHitTime_corr);

      // --- Histograms per fixed RU slice:
      meHitTimeRuSlice_corr_[RU_index-1]->Fill(RecoHitTime_corr);

      // --- Histograms per single RU [RU slice - Tray number]:
      meHitTimeSingleRU_[RU_index-1][detId.mtdRR()-1]->Fill(RecoHitTime_corr);

      // --- Histograms for each SM slice:
      meHitTimeSmSlice_[SmEtaIndex-1]->Fill(RecoHitTime_corr);

      // --- Histograms for each SM in a single RU:
      if (detId.mtdRR() == 1){
	      meHitTimeSingleSmInRu_[RU_index-1][SMinRU_index]->Fill(RecoHitTime_corr);
      }

      

  
    std::cout << "RU NUMBER: " << detId.runit() << std::endl;
    std::cout << "TRAY NUMBER: " << detId.mtdRR() << std::endl;
    std::cout << "DM NUMBER: " << detId.dmodule() << std::endl;
    std::cout << "SM NUMNBER: " <<detId.smodule() << std::endl;
    std::cout << "Global z: " << global_point.z() << std::endl;
    std::cout << "CRYSTAL NUMBER: " << detId.crystal() << std::endl;
    std::cout << "-----------------------------------------------"<< std::endl;
   */
    //--------------------------------------------------------------------------------------------


    if (optionalPlots_) {
      meLocalOccupancy_->Fill(local_point.x() + recHit.position(), local_point.y());
    }
    meHitXlocal_->Fill(local_point.x());
    meHitYlocal_->Fill(local_point.y());
    meHitZlocal_->Fill(local_point.z());
    meHitZ_->Fill(global_point.z());
    meHitPhi_->Fill(global_point.phi());
    meHitEta_->Fill(global_point.eta());

    meHitTvsE_->Fill(recHit.energy(), recHit.time());
    meHitEvsPhi_->Fill(global_point.phi(), recHit.energy());
    meHitEvsEta_->Fill(global_point.eta(), recHit.energy());
    meHitEvsZ_->Fill(global_point.z(), recHit.energy());
    meHitTvsPhi_->Fill(global_point.phi(), recHit.time());
    meHitTvsEta_->Fill(global_point.eta(), recHit.time());
    meHitTvsZ_->Fill(global_point.z(), recHit.time());

    // Resolution histograms
    LogDebug("BtlTimeMonitoring") << "RecoHit DetId= " << detId.rawId()
                                       << " sim hits in id= " << m_btlSimHits.count(detId.rawId());
    if (m_btlSimHits.count(detId.rawId()) == 1 && m_btlSimHits[detId.rawId()].energy > hitMinEnergy_) {
      float longpos_res = recHit.position() - convertMmToCm(m_btlSimHits[detId.rawId()].x);
      float time_res = recHit.time() - m_btlSimHits[detId.rawId()].time;
      float energy_res = recHit.energy() - m_btlSimHits[detId.rawId()].energy;

      Local3DPoint local_point_sim(convertMmToCm(m_btlSimHits[detId.rawId()].x),
                                   convertMmToCm(m_btlSimHits[detId.rawId()].y),
                                   convertMmToCm(m_btlSimHits[detId.rawId()].z));
      local_point_sim =
          topo.pixelToModuleLocalPoint(local_point_sim, detId.row(topo.nrows()), detId.column(topo.nrows()));
      const auto& global_point_sim = thedet->toGlobal(local_point_sim);

      meTimeRes_->Fill(time_res);
      meTimeResVsE_->Fill(recHit.energy(), time_res);
      meEnergyRes_->Fill(energy_res);
      meEnergyRelResVsE_->Fill(recHit.energy(), energy_res / recHit.energy());

      meLongPosPull_->Fill(longpos_res / recHit.positionError());
      meLongPosPullvsEta_->Fill(std::abs(global_point_sim.eta()), longpos_res / recHit.positionError());
      meLongPosPullvsE_->Fill(m_btlSimHits[detId.rawId()].energy, longpos_res / recHit.positionError());

      meTPullvsEta_->Fill(std::abs(global_point_sim.eta()), time_res / recHit.timeError());
      meTPullvsE_->Fill(m_btlSimHits[detId.rawId()].energy, time_res / recHit.timeError());
    } else if (m_btlSimHits.count(detId.rawId()) == 0) {
      n_reco_btl_nosimhit++;
      LogDebug("BtlTimeMonitoring") << "BTL rec hit with no corresponding sim hit in crystal, DetId= "
                                         << detId.rawId() << " geoId= " << geoId.rawId() << " ene= " << recHit.energy()
                                         << " time= " << recHit.time();
    }

    n_reco_btl++;

  }  // recHit loop

  if (n_reco_btl > 0) {
    meNhits_->Fill(std::log10(n_reco_btl));
  }
  if (n_reco_btl_nosimhit == 0) {
    meUnmatchedRecHit_->Fill(-1.5);
  } else {
    meUnmatchedRecHit_->Fill(std::log10(n_reco_btl_nosimhit));
  }

  // --- Loop over the BTL RECO clusters ---
  unsigned int n_clus_btl(0);
  for (const auto& DetSetClu : *btlRecCluHandle) {
    for (const auto& cluster : DetSetClu) {
      if (cluster.energy() < hitMinEnergy_)
        continue;
      BTLDetId cluId = cluster.id();
      DetId detIdObject(cluId);
      const auto& genericDet = geom->idToDetUnit(detIdObject);
      if (genericDet == nullptr) {
        throw cms::Exception("BtlTimeMonitoring")
            << "GeographicalID: " << std::hex << cluId << " is invalid!" << std::dec << std::endl;
      }
      n_clus_btl++;

      const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(genericDet->topology());
      const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());

      MTDClusterParameterEstimator::ReturnType tuple = cpe.getParameters(cluster, *genericDet);

      // --- Cluster position in the module reference frame
      LocalPoint local_point(std::get<0>(tuple));
      const auto& global_point = genericDet->toGlobal(local_point);

      meCluEnergy_->Fill(cluster.energy());
      meCluTime_->Fill(cluster.time());
      meCluTimeError_->Fill(cluster.timeError());
      meCluPhi_->Fill(global_point.phi());
      meCluEta_->Fill(global_point.eta());
      meCluZvsPhi_->Fill(global_point.z(), global_point.phi());
      meCluHits_->Fill(cluster.size());

      // --- Get the SIM hits associated to the cluster and calculate
      //     the cluster SIM energy, time and position

      double cluEneSIM = 0.;
      double cluTimeSIM = 0.;
      double cluLocXSIM = 0.;
      double cluLocYSIM = 0.;
      double cluLocZSIM = 0.;

      if (optionalPlots_) {
        for (int ihit = 0; ihit < cluster.size(); ++ihit) {
          int hit_row = cluster.minHitRow() + cluster.hitOffset()[ihit * 2];
          int hit_col = cluster.minHitCol() + cluster.hitOffset()[ihit * 2 + 1];

          // Match the RECO hit to the corresponding SIM hit
          for (const auto& recHit : *btlRecHitsHandle) {
            BTLDetId hitId(recHit.id().rawId());

            if (m_btlSimHits.count(hitId.rawId()) == 0)
              continue;

            // Check the hit position
            if (hitId.mtdSide() != cluId.mtdSide() || hitId.mtdRR() != cluId.mtdRR() || recHit.row() != hit_row ||
                recHit.column() != hit_col)
              continue;

            // Check the hit energy and time
            if (recHit.energy() != cluster.hitENERGY()[ihit] || recHit.time() != cluster.hitTIME()[ihit])
              continue;

            // SIM hit's position in the module reference frame
            Local3DPoint local_point_sim(convertMmToCm(m_btlSimHits[recHit.id().rawId()].x),
                                         convertMmToCm(m_btlSimHits[recHit.id().rawId()].y),
                                         convertMmToCm(m_btlSimHits[recHit.id().rawId()].z));
            local_point_sim =
                topo.pixelToModuleLocalPoint(local_point_sim, hitId.row(topo.nrows()), hitId.column(topo.nrows()));

            // Calculate the SIM cluster's position in the module reference frame
            cluLocXSIM += local_point_sim.x() * m_btlSimHits[recHit.id().rawId()].energy;
            cluLocYSIM += local_point_sim.y() * m_btlSimHits[recHit.id().rawId()].energy;
            cluLocZSIM += local_point_sim.z() * m_btlSimHits[recHit.id().rawId()].energy;

            // Calculate the SIM cluster energy and time
            cluEneSIM += m_btlSimHits[recHit.id().rawId()].energy;
            cluTimeSIM += m_btlSimHits[recHit.id().rawId()].time * m_btlSimHits[recHit.id().rawId()].energy;

            break;

          }  // recHit loop

        }  // ihit loop
      }

      // Find the MTDTrackingRecHit corresponding to the cluster
      const MTDTrackingRecHit* comp(nullptr);
      bool matchClu = false;
      const auto& trkHits = (*mtdTrkHitHandle)[detIdObject];
      for (const auto& trkHit : trkHits) {
        if (isSameCluster(trkHit.mtdCluster(), cluster)) {
          comp = trkHit.clone();
          matchClu = true;
          break;
        }
      }
      if (!matchClu) {
        edm::LogWarning("BtlTimeMonitoring")
            << "No valid TrackingRecHit corresponding to cluster, detId = " << detIdObject.rawId();
      }

      // --- Fill the cluster resolution histograms
      if (optionalPlots_) {
        if (cluTimeSIM > 0. && cluEneSIM > 0.) {
          cluTimeSIM /= cluEneSIM;

          Local3DPoint cluLocalPosSIM(cluLocXSIM / cluEneSIM, cluLocYSIM / cluEneSIM, cluLocZSIM / cluEneSIM);
          const auto& cluGlobalPosSIM = genericDet->toGlobal(cluLocalPosSIM);

          float time_res = cluster.time() - cluTimeSIM;
          float energy_res = cluster.energy() - cluEneSIM;
          meCluTimeRes_->Fill(time_res);
          meCluEnergyRes_->Fill(energy_res);

          float rho_res = global_point.perp() - cluGlobalPosSIM.perp();
          float phi_res = global_point.phi() - cluGlobalPosSIM.phi();

          meCluRhoRes_->Fill(rho_res);
          meCluPhiRes_->Fill(phi_res);

          float xlocal_res = local_point.x() - cluLocalPosSIM.x();
          float ylocal_res = local_point.y() - cluLocalPosSIM.y();

          float z_res = global_point.z() - cluGlobalPosSIM.z();

          meCluZRes_->Fill(z_res);

          if (matchClu && comp != nullptr) {
            meCluLocalXRes_->Fill(xlocal_res);

            if (global_point.z() > 0) {
              meCluLocalYResZGlobPlus_->Fill(ylocal_res);
              meCluLocalYPullZGlobPlus_->Fill(ylocal_res / std::sqrt(comp->localPositionError().yy()));
            } else {
              meCluLocalYResZGlobMinus_->Fill(ylocal_res);
              meCluLocalYPullZGlobMinus_->Fill(ylocal_res / std::sqrt(comp->localPositionError().yy()));
            }
            if (cluster.size() == 1) {  // single-crystal clusters
              meCluSingCrystalLocalYRes_->Fill(ylocal_res);
              if (global_point.z() > 0) {
                meCluSingCrystalLocalYResZGlobPlus_->Fill(ylocal_res);
              } else {
                meCluSingCrystalLocalYResZGlobMinus_->Fill(ylocal_res);
              }
            }  // end of single-crystal clusters
            else {
              if (cluster.size() > 1) {  // multi-crystal clusters
                meCluMultiCrystalLocalYRes_->Fill(ylocal_res);
                if (global_point.z() > 0) {
                  meCluMultiCrystalLocalYResZGlobPlus_->Fill(ylocal_res);
                } else {
                  meCluMultiCrystalLocalYResZGlobMinus_->Fill(ylocal_res);
                }
              }
            }  // end of multi-crystal clusters

            if (abs(global_point.eta()) < 0.3) {
              meCluCentralLocalYRes_->Fill(ylocal_res);
              if (global_point.z() > 0) {
                meCluCentralLocalYResZGlobPlus_->Fill(ylocal_res);
              } else {
                meCluCentralLocalYResZGlobMinus_->Fill(ylocal_res);
              }

            } else {
              if (abs(global_point.eta()) > 1) {
                meCluForwardLocalYRes_->Fill(ylocal_res);
                if (global_point.z() > 0) {
                  meCluForwardPlusLocalYRes_->Fill(ylocal_res);
                } else {
                  meCluForwardMinusLocalYRes_->Fill(ylocal_res);
                }
              }
            }

            meCluYXLocal_->Fill(local_point.x(), local_point.y());
            meCluYXLocalSim_->Fill(cluLocalPosSIM.x(), cluLocalPosSIM.y());

            meCluLocalXPull_->Fill(xlocal_res / std::sqrt(comp->localPositionError().xx()));
            meCluZPull_->Fill(z_res / std::sqrt(comp->globalPositionError().czz()));
            meCluXLocalErr_->Fill(std::sqrt(comp->localPositionError().xx()));
            meCluYLocalErr_->Fill(std::sqrt(comp->localPositionError().yy()));
          }

          meCluEnergyvsEta_->Fill(std::abs(cluGlobalPosSIM.eta()), cluster.energy());
          meCluHitsvsEta_->Fill(std::abs(cluGlobalPosSIM.eta()), cluster.size());

          meCluTResvsEta_->Fill(std::abs(cluGlobalPosSIM.eta()), time_res);
          meCluTResvsE_->Fill(cluEneSIM, time_res);

          meCluTPullvsEta_->Fill(std::abs(cluGlobalPosSIM.eta()), time_res / cluster.timeError());
          meCluTPullvsE_->Fill(cluEneSIM, time_res / cluster.timeError());

        }  // if ( cluTimeSIM > 0. &&  cluEneSIM > 0. )
        else {
          meUnmatchedCluEnergy_->Fill(std::log10(cluster.energy()));
        }
      }

      // --- Fill the cluster resolution histograms using MtdSimLayerClusters as mtd truth
      edm::Ref<edmNew::DetSetVector<FTLCluster>, FTLCluster> clusterRef = edmNew::makeRefTo(btlRecCluHandle, &cluster);
      auto itp = r2sAssociationMap.equal_range(clusterRef);
      if (itp.first != itp.second) {
        std::vector<MtdSimLayerClusterRef> simClustersRefs =
            (*itp.first).second;  // the range of itp.first, itp.second should be always 1
        for (unsigned int i = 0; i < simClustersRefs.size(); i++) {
          auto simClusterRef = simClustersRefs[i];

          float simClusEnergy = convertUnitsTo(0.001_MeV, (*simClusterRef).simLCEnergy());  // GeV --> MeV
          float simClusTime = (*simClusterRef).simLCTime();
          LocalPoint simClusLocalPos = (*simClusterRef).simLCPos();
          const auto& simClusGlobalPos = genericDet->toGlobal(simClusLocalPos);
          unsigned int idOffset = (*simClusterRef).trackIdOffset();

          float time_res = cluster.time() - simClusTime;
          float energy_res = cluster.energy() - simClusEnergy;
          float rho_res = global_point.perp() - simClusGlobalPos.perp();
          float phi_res = global_point.phi() - simClusGlobalPos.phi();
          float z_res = global_point.z() - simClusGlobalPos.z();
          float xlocal_res = local_point.x() - simClusLocalPos.x();
          float ylocal_res = local_point.y() - simClusLocalPos.y();

          meCluTrackIdOffset_->Fill(float(idOffset));

          // -- Fill for direct hits
          if (idOffset == 0) {
            meCluTimeRes_simLC_->Fill(time_res);
            meCluEnergyRes_simLC_->Fill(energy_res);
            meCluRhoRes_simLC_->Fill(rho_res);
            meCluPhiRes_simLC_->Fill(phi_res);
            meCluZRes_simLC_->Fill(z_res);

            if (matchClu && comp != nullptr) {
              meCluLocalXRes_simLC_->Fill(xlocal_res);

              if (global_point.z() > 0) {
                meCluLocalYResZGlobPlus_simLC_->Fill(ylocal_res);
                meCluLocalYPullZGlobPlus_simLC_->Fill(ylocal_res / std::sqrt(comp->localPositionError().yy()));
              } else {
                meCluLocalYResZGlobMinus_simLC_->Fill(ylocal_res);
                meCluLocalYPullZGlobMinus_simLC_->Fill(ylocal_res / std::sqrt(comp->localPositionError().yy()));
              }
              if (optionalPlots_) {
                if (cluster.size() == 1) {  // single-crystal clusters
                  meCluSingCrystalLocalYRes_simLC_->Fill(ylocal_res);
                  if (global_point.z() > 0) {
                    meCluSingCrystalLocalYResZGlobPlus_simLC_->Fill(ylocal_res);
                  } else {
                    meCluSingCrystalLocalYResZGlobMinus_simLC_->Fill(ylocal_res);
                  }
                }  // end of single-crystal clusters
                else {
                  if (cluster.size() > 1) {  // multi-crystal clusters
                    meCluMultiCrystalLocalYRes_simLC_->Fill(ylocal_res);
                    if (global_point.z() > 0) {
                      meCluMultiCrystalLocalYResZGlobPlus_simLC_->Fill(ylocal_res);
                    } else {
                      meCluMultiCrystalLocalYResZGlobMinus_simLC_->Fill(ylocal_res);
                    }
                  }
                }  // end of multi-crystal clusters

                if (abs(global_point.eta()) < 0.3) {
                  meCluCentralLocalYRes_simLC_->Fill(ylocal_res);
                  if (global_point.z() > 0) {
                    meCluCentralLocalYResZGlobPlus_simLC_->Fill(ylocal_res);
                  } else {
                    meCluCentralLocalYResZGlobMinus_simLC_->Fill(ylocal_res);
                  }
                } else {
                  if (abs(global_point.eta()) > 1) {
                    meCluForwardLocalYRes_simLC_->Fill(ylocal_res);
                    if (global_point.z() > 0) {
                      meCluForwardPlusLocalYRes_simLC_->Fill(ylocal_res);
                    } else {
                      meCluForwardMinusLocalYRes_simLC_->Fill(ylocal_res);
                    }
                  }
                }
              }  //end of optional plots

              meCluLocalXPull_simLC_->Fill(xlocal_res / std::sqrt(comp->localPositionError().xx()));
              meCluZPull_simLC_->Fill(z_res / std::sqrt(comp->globalPositionError().czz()));
            }

            meCluTResvsEta_simLC_->Fill(std::abs(simClusGlobalPos.eta()), time_res);
            meCluTResvsE_simLC_->Fill(simClusEnergy, time_res);

            meCluTPullvsEta_simLC_->Fill(std::abs(simClusGlobalPos.eta()), time_res / cluster.timeError());
            meCluTPullvsE_simLC_->Fill(simClusEnergy, time_res / cluster.timeError());

          }  // if idOffset == 0
          else {
            meCluTimeRes_simLC_fromIndirectHits_->Fill(time_res);
            meCluEnergyRes_simLC_fromIndirectHits_->Fill(energy_res);
            meCluRhoRes_simLC_fromIndirectHits_->Fill(rho_res);
            meCluPhiRes_simLC_fromIndirectHits_->Fill(phi_res);
            meCluZRes_simLC_fromIndirectHits_->Fill(z_res);

            if (matchClu && comp != nullptr) {
              meCluLocalXRes_simLC_fromIndirectHits_->Fill(xlocal_res);

              if (global_point.z() > 0) {
                meCluLocalYResZGlobPlus_simLC_fromIndirectHits_->Fill(ylocal_res);
                meCluLocalYPullZGlobPlus_simLC_fromIndirectHits_->Fill(ylocal_res /
                                                                       std::sqrt(comp->localPositionError().yy()));
              } else {
                meCluLocalYResZGlobMinus_simLC_fromIndirectHits_->Fill(ylocal_res);
                meCluLocalYPullZGlobMinus_simLC_fromIndirectHits_->Fill(ylocal_res /
                                                                        std::sqrt(comp->localPositionError().yy()));
              }
              if (optionalPlots_) {
                if (cluster.size() == 1) {  // single-crystal clusters
                  meCluSingCrystalLocalYRes_simLC_fromIndirectHits_->Fill(ylocal_res);
                  if (global_point.z() > 0) {
                    meCluSingCrystalLocalYResZGlobPlus_simLC_fromIndirectHits_->Fill(ylocal_res);
                  } else {
                    meCluSingCrystalLocalYResZGlobMinus_simLC_fromIndirectHits_->Fill(ylocal_res);
                  }
                }  // end of single-crystal clusters
                else {
                  if (cluster.size() > 1) {  // multi-crystal clusters
                    meCluMultiCrystalLocalYRes_simLC_fromIndirectHits_->Fill(ylocal_res);
                    if (global_point.z() > 0) {
                      meCluMultiCrystalLocalYResZGlobPlus_simLC_fromIndirectHits_->Fill(ylocal_res);
                    } else {
                      meCluMultiCrystalLocalYResZGlobMinus_simLC_fromIndirectHits_->Fill(ylocal_res);
                    }
                  }
                }  // end of multi-crystal clusters

                if (abs(global_point.eta()) < 0.3) {
                  meCluCentralLocalYRes_simLC_fromIndirectHits_->Fill(ylocal_res);
                  if (global_point.z() > 0) {
                    meCluCentralLocalYResZGlobPlus_simLC_fromIndirectHits_->Fill(ylocal_res);
                  } else {
                    meCluCentralLocalYResZGlobMinus_simLC_fromIndirectHits_->Fill(ylocal_res);
                  }
                } else {
                  if (abs(global_point.eta()) > 1) {
                    meCluForwardLocalYRes_simLC_fromIndirectHits_->Fill(ylocal_res);
                    if (global_point.z() > 0) {
                      meCluForwardPlusLocalYRes_simLC_fromIndirectHits_->Fill(ylocal_res);
                    } else {
                      meCluForwardMinusLocalYRes_simLC_fromIndirectHits_->Fill(ylocal_res);
                    }
                  }
                }
              }  //end of optional plots

              meCluLocalXPull_simLC_fromIndirectHits_->Fill(xlocal_res / std::sqrt(comp->localPositionError().xx()));
              meCluZPull_simLC_fromIndirectHits_->Fill(z_res / std::sqrt(comp->globalPositionError().czz()));
            }

            meCluTResvsEta_simLC_fromIndirectHits_->Fill(std::abs(simClusGlobalPos.eta()), time_res);
            meCluTResvsE_simLC_fromIndirectHits_->Fill(simClusEnergy, time_res);

            meCluTPullvsEta_simLC_fromIndirectHits_->Fill(std::abs(simClusGlobalPos.eta()),
                                                          time_res / cluster.timeError());
            meCluTPullvsE_simLC_fromIndirectHits_->Fill(simClusEnergy, time_res / cluster.timeError());
          }

        }  // simLayerClusterRefs loop
      }

    }  // cluster loop

  }  // DetSetClu loop

  if (n_clus_btl > 0)
    meNclusters_->Fill(log10(n_clus_btl));

  // --- This is to count the number of processed events, needed in the harvesting step
  meNevents_->Fill(0.5);



  // --- Loop over the BTL Uncalibrated RECO hits
  if (optionalPlots_) {
    auto btlUncalibRecHitsHandle = makeValid(iEvent.getHandle(btlUncalibRecHitsToken_));
    for (const auto& uRecHit : *btlUncalibRecHitsHandle) {
      BTLDetId detId = uRecHit.id();

      LogTrace("BtlTimeMonitoring") << "@URH detid " << detId.rawId() << " A " << uRecHit.amplitude().first << " "
                                         << uRecHit.amplitude().second << " T " << uRecHit.time().first << " "
                                         << uRecHit.time().second;


      // --- Skip UncalibratedRecHits not matched to SimHits
      //if (m_btlSimHits.count(detId.rawId()) != 1)
      //  continue;

      
      //Combine the information from the left and right BTL cell sides
      double nHits = 0.;
      double hit_amplitude = 0.;
      double hit_time = 0.;
      double t_corr_L = 0.;
      double t_corr_R = 0.;
      double hit_amplitude_L = 0.;
      double hit_amplitude_R = 0.;

      double hit_time_noTW = 0.;
      double t_L_noTW = 0.;
      double t_R_noTW = 0.;
      double t_corr_L_noTW = 0.;
      double t_corr_R_noTW = 0.;
      double delta_time_L = 0.;
      double delta_time_R = 0.;

      // left side:
      if (uRecHit.amplitude().first > 0.) {
        hit_amplitude += uRecHit.amplitude().first;
        hit_time += uRecHit.time().first;
        nHits += 1.;
	t_corr_L = uRecHit.time().first - tof_internal_;
	t_L_noTW = uRecHit.time().first + p0_ * pow(uRecHit.amplitude().first, p1_);
	t_corr_L_noTW = t_L_noTW - tof_internal_;
	hit_time_noTW+= t_corr_L_noTW;
	hit_amplitude_L = uRecHit.amplitude().first;
	delta_time_L = t_corr_L_noTW - t_corr_L;
      }
      // right side:
      if (uRecHit.amplitude().second > 0.) {
        hit_amplitude += uRecHit.amplitude().second;
        hit_time += uRecHit.time().second;
        nHits += 1.;
	t_corr_R = uRecHit.time().second  - tof_internal_;
	t_R_noTW = uRecHit.time().second + p0_ * pow(uRecHit.amplitude().second, p1_);
        t_corr_R_noTW = t_R_noTW - tof_internal_;
	hit_time_noTW+= t_corr_R_noTW;
	hit_amplitude_R = uRecHit.amplitude().second;
	delta_time_R = t_corr_R_noTW - t_corr_R;
      }

      LogDebug("BtlTimeMonitoring") << "#unc " << nHits << " A/T " << hit_amplitude << " " << hit_time;
      if (nHits == 0.) {
        edm::LogWarning("BtlTimeMonitoring") << "Empty uncalibrated hit in DetId " << detId;
        continue;
      }

      hit_amplitude /= nHits;
      hit_time /= nHits;

      hit_time_noTW /= nHits;
      
      //Amplitude selection:hit_amplitude < hitMinimumAmplitude_ || hit_amplitude > hitMaxAmplitude_
      if (hit_amplitude > hitMaxAmplitude_)
        continue;
      if (hit_amplitude < hitMinimumAmplitude_)
        continue;
      
      //Time cut for the saturation peak:
      if (hit_time>hitMaxTime_)
	continue;

      
      DetId geoId = detId.geographicalId(MTDTopologyMode::crysLayoutFromTopoMode(topology->getMTDTopologyMode()));
      const MTDGeomDet* thedet = geom->idToDet(geoId);
      if (thedet == nullptr)
	      throw cms::Exception("BtlTimeMonitoring") << "GeographicalID: " << std::hex << geoId.rawId() << " ("
                                                 << detId.rawId() << ") is invalid!" << std::dec << std::endl;

      const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(thedet->topology());
      const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());

      Local3DPoint local_point(0., 0., 0.);
      local_point = topo.pixelToModuleLocalPoint(local_point, detId.row(topo.nrows()), detId.column(topo.nrows()));
      const auto& global_point = thedet->toGlobal(local_point);
      
//----------------------------------------------------      
      
      //Distance beam spot<-> center of the hitted crystal:
      double DistUnc_BS = std::sqrt(std::pow(global_point.x() - beamSpot.x0(), 2) + 
		                   std::pow(global_point.y() - beamSpot.y0(), 2) + 
		                   std::pow(global_point.z() - beamSpot.z0(), 2));
      constexpr double c_cm_ns = geant_units::operators::convertMmToCm(CLHEP::c_light);  // [mm/ns] -> [cm/ns]
      
      //TOF photon-like:
      double TOFUnc_BS = DistUnc_BS / c_cm_ns;

      //Hit Time-Stamp correction:
      double hit_time_corr = hit_time - TOFUnc_BS;
      double time_L_TOF = t_corr_L - TOFUnc_BS;
      double time_R_TOF = t_corr_R - TOFUnc_BS;

      double hit_time_noTW_corr = hit_time_noTW - TOFUnc_BS;
      double t_L_noTW_TOF = t_corr_L_noTW - TOFUnc_BS;
      double t_R_noTW_TOF = t_corr_R_noTW - TOFUnc_BS; 

      //Time cut on RecoHitTime_corr:
      if(hit_time_corr < MinRecoHitTime_)
            continue;

      //Sensor Module Index extrapolation:
      auto index = topology->btlIndex(geoId.rawId());
      //uint32_t SMphi_index = index.first;
      uint32_t SMeta_index = index.second;

      int RU_index = (detId.mtdSide() == 0) ? (7 - detId.runit()) : (6 + detId.runit());

      //--- SM index in a Single RU: 0->23:
      int SMinRU_index = detId.dmodule() + (detId.dmodule() + detId.smodule());

      int crystal_index = detId.crystal() + (SMeta_index-1)*16;
      int adjusted_SMeta_index =SMeta_index - (RU_index-1)*8;


      //----- HISTOGRAMS ----

      //Global plots:
      meUncTimeMean_->Fill(hit_time);
      meUncTimeMean_corr_->Fill(hit_time_corr);
      meUncAmpl_global_->Fill(hit_amplitude);
      meUncEne_global_->Fill(hit_amplitude*0.03125);
      meUncTime_Left_->Fill(t_corr_L);
      meUncTime_Right_->Fill(t_corr_R);
      meUncTimeCorr_Left_->Fill(time_L_TOF);
      meUncTimeCorr_Right_->Fill(time_R_TOF);

      //Histograms per fixed RU slice:
      meUncTimeRUSlice_[RU_index-1]->Fill(hit_time);
      meUncTimeRUSlice_corr_[RU_index-1]->Fill(hit_time_corr);

      //Histograms per single RU [RU slice - Tray number]:
      meUncTime_SingleRU_[RU_index-1][detId.mtdRR()-1]->Fill(hit_time_corr);		

      //Histograms for each SM slice:
      meUncTimePhiSlice_[SMeta_index-1]->Fill(hit_time_corr);      
      
      // --- Histograms for each SM in a single RU:
      if (detId.mtdRR() == 1){
              meUncTimeSingleSmInRu_[RU_index-1][SMinRU_index]->Fill(hit_time_corr);
      }

      // --- Specific histograms per Crystal ring (central ring per RU):
      if(crystal_index == 65) {
	      meUncTimeCrystalRing_RU1_->Fill(hit_time_corr);
      }
      if(crystal_index == 193) {
              meUncTimeCrystalRing_RU2_->Fill(hit_time_corr);
      }
      if(crystal_index == 321) {
              meUncTimeCrystalRing_RU3_->Fill(hit_time_corr);
      }
      if(crystal_index == 448) {
              meUncTimeCrystalRing_RU4_->Fill(hit_time_corr);
      }
      if(crystal_index == 577) {
              meUncTimeCrystalRing_RU5_->Fill(hit_time_corr);
      }
      if(crystal_index == 705) {
              meUncTimeCrystalRing_RU6_->Fill(hit_time_corr);
      }
      if(crystal_index == 833) {
              meUncTimeCrystalRing_RU7_->Fill(hit_time_corr);
      }
      if(crystal_index == 961) {
              meUncTimeCrystalRing_RU8_->Fill(hit_time_corr);
      }
      if(crystal_index == 1089) {
              meUncTimeCrystalRing_RU9_->Fill(hit_time_corr);
      }
      if(crystal_index == 1217) {
              meUncTimeCrystalRing_RU10_->Fill(hit_time_corr);
      }
      if(crystal_index == 1345) {
              meUncTimeCrystalRing_RU11_->Fill(hit_time_corr);
      }
      if(crystal_index == 1473) {
              meUncTimeCrystalRing_RU12_->Fill(hit_time_corr);
      }

//--------------------------------------------------------------------------------------------

      //Crystal rings in the cetral zone (RU=6, 7):
      if(crystal_index == 833 || crystal_index == 705) {
	      meUncHitTcorr_vs_A_Ctl_->Fill(hit_amplitude, hit_time_corr);
	      meUncHitT_vs_A_Ctl_->Fill(hit_amplitude, hit_time);

	      meUncHitT_L_corr_vs_A_Ctl_->Fill(hit_amplitude_L, time_L_TOF );
	      meUncHitT_L_vs_A_Ctl_->Fill(hit_amplitude_L, t_corr_L);

	      meUncHitT_R_corr_vs_A_Ctl_->Fill(hit_amplitude_R, time_R_TOF );
              meUncHitT_R_vs_A_Ctl_->Fill(hit_amplitude_R, t_corr_R);

	      meUncHitTcorr_vs_A_Ctl_noWT_->Fill(hit_amplitude, hit_time_noTW_corr);
	      meUncHitT_vs_A_Ctl_noWT_->Fill(hit_amplitude, hit_time_noTW);

	      meUncHitT_L_corr_vs_A_Ctl_noWT_->Fill(hit_amplitude_L, t_L_noTW_TOF );
              meUncHitT_L_vs_A_Ctl_noWT_->Fill(hit_amplitude_L, t_corr_L_noTW);

              meUncHitT_R_corr_vs_A_Ctl_noWT_->Fill(hit_amplitude_R, t_R_noTW_TOF);
              meUncHitT_R_vs_A_Ctl_noWT_->Fill(hit_amplitude_R, t_corr_R_noTW);

	      meUncHit_DeltaT_R_Ctl_->Fill(hit_amplitude_R,delta_time_R);
	      meUncHit_DeltaT_L_Ctl_->Fill(hit_amplitude_L,delta_time_L);
      }
/*
      //Crystal rings in the Forward zone (RU=1, 12):
      if(crystal_index == 1473 || crystal_index == 65) {
              meUncHitTcorr_vs_A_Forward_->Fill(hit_amplitude, hit_time_corr);
              meUncHitT_vs_A_Forward_->Fill(hit_amplitude, hit_time);
      
	      meUncHitT_L_corr_vs_A_Forward_->Fill(hit_amplitude_L, time_w_TOF_L );
              meUncHitT_L_vs_A_Forward_->Fill(hit_amplitude_L, t_corr_L);

              meUncHitT_R_corr_vs_A_Forward_->Fill(hit_amplitude_R, time_w_TOF_R );
              meUncHitT_R_vs_A_Forward_->Fill(hit_amplitude_R, t_corr_R);

	      meUncHitTcorr_vs_A_Forward_noWT_->Fill(hit_amplitude, hit_time_noTW_corr);
              meUncHitT_vs_A_Forward_noWT_->Fill(hit_amplitude, hit_time_noTW);

	      meUncHitT_L_corr_vs_A_Forward_noWT_->Fill(hit_amplitude_L, t_corr_L_noTW_TOF );
              meUncHitT_L_vs_A_Forward_noWT_->Fill(hit_amplitude_L, t_corr_L_noTW);

              meUncHitT_R_corr_vs_A_Forward_noWT_->Fill(hit_amplitude_R, t_corr_R_noTW_TOF );
              meUncHitT_R_vs_A_Forward_noWT_->Fill(hit_amplitude_R, t_corr_R_noTW);

	      meUncHit_DeltaT_R_Forward_->Fill(hit_amplitude_R,delta_time_R);
              meUncHit_DeltaT_R_Forward_->Fill(hit_amplitude_L,delta_time_L);
      }
*/
//--------------------------------------------------------------------------------------------      



/*
      std::cout << "=====================================" << std::endl;
      std::cout << " TRAY: " << detId.mtdRR()
                << " | RU: " << detId.globalRunit()->OLD NUMBERING SCHEME
		<< " | RU: " << detID.runit() -> NEW NUMBERING SCHEME
                << " | SM ETA: " << SMeta_index
                << " | SM PHI: " << SMphi_index
                << " | CRYSTAL: " << detId.crystal()
                << std::endl;
      */

//-----------------------------------------------------------------------------------------------------------------------------------------

      // --- Fill the histograms

      meUncEneRVsX_->Fill(uRecHit.position(), uRecHit.amplitude().first - hit_amplitude);
      meUncEneLVsX_->Fill(uRecHit.position(), uRecHit.amplitude().second - hit_amplitude);

      meUncTimeRVsX_->Fill(uRecHit.position(), uRecHit.time().first - hit_time);
      meUncTimeLVsX_->Fill(uRecHit.position(), uRecHit.time().second - hit_time);

      if (uncalibRecHitsPlots_) {
        DetId geoId = detId.geographicalId(MTDTopologyMode::crysLayoutFromTopoMode(topology->getMTDTopologyMode()));
        const MTDGeomDet* thedet = geom->idToDet(geoId);
        if (thedet == nullptr)
          throw cms::Exception("BtlTimeMonitoring") << "GeographicalID: " << std::hex << geoId.rawId() << " ("
                                                         << detId.rawId() << ") is invalid!" << std::dec << std::endl;
        const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(thedet->topology());
        const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());

        Local3DPoint local_point(0., 0., 0.);
        local_point = topo.pixelToModuleLocalPoint(local_point, detId.row(topo.nrows()), detId.column(topo.nrows()));
        const auto& global_point = thedet->toGlobal(local_point);

        float time_res = hit_time - m_btlSimHits[detId.rawId()].time;

        // amplitude histograms

        int qBin = (int)(hit_amplitude / binWidthQ_);
        if (qBin > nBinsQ_ - 1)
          qBin = nBinsQ_ - 1;

        meTimeResQ_[qBin]->Fill(time_res);

        int etaBin = 0;
        for (int ibin = 1; ibin < nBinsQEta_; ++ibin)
          if (fabs(global_point.eta()) >= binsQEta_[ibin] && fabs(global_point.eta()) < binsQEta_[ibin + 1])
            etaBin = ibin;

        meTimeResQvsEta_[qBin][etaBin]->Fill(time_res);

        // eta histograms

        etaBin = (int)(fabs(global_point.eta()) / binWidthEta_);
        if (etaBin > nBinsEta_ - 1)
          etaBin = nBinsEta_ - 1;

        meTimeResEta_[etaBin]->Fill(time_res);

        qBin = 0;
        for (int ibin = 1; ibin < nBinsEtaQ_; ++ibin)
          if (hit_amplitude >= binsEtaQ_[ibin] && hit_amplitude < binsEtaQ_[ibin + 1])
            qBin = ibin;

        meTimeResEtavsQ_[etaBin][qBin]->Fill(time_res);
      }
    }  // uRecHit loop}
  }//optional plots


}//analyzer()

//---------------------------------------------------------------------------------



// ------------ method for histogram booking ------------
void BtlTimeMonitoring::bookHistograms(DQMStore::IBooker& ibook,
                                            edm::Run const& run,
                                            edm::EventSetup const& iSetup) {
  ibook.setCurrentFolder(folder_);

  // --- histograms booking


  //Test sample PI;
  meSimHitTime_ = ibook.book1D("BtlSimHitTime", "Sim Hit timestamp;Time_{RECO} [ns]", 1000, -5., 25.);
  meSimHitTimeCorr_ = ibook.book1D("BtlSimHitTimeCorr", "Sim hit corrected time: timestamp - simVTX time", 1000, -5., 25.);


  meNevents_ = ibook.book1D("BtlNevents", "Number of events", 1, 0., 1.);

  meNhits_ = ibook.book1D("BtlNhits", "Number of BTL RECO hits;log_{10}(N_{RECO})", 100, 0., 5.25);

  meHitEnergy_ = ibook.book1D("BtlHitEnergy", "BTL RECO hits energy;E_{RECO} [MeV]", 100, 0., 20.);
  meHitLogEnergy_ = ibook.book1D("BtlHitLogEnergy", "BTL RECO hits energy;log_{10}(E_{RECO} [MeV])", 16, -0.1, 1.5);
  meHitTime_ = ibook.book1D("BtlHitTime", "BTL RECO hits ToA;ToA_{RECO} [ns]", 100, 0., 25.);
  meHitTimeError_ = ibook.book1D("BtlHitTimeError", "BTL RECO hits ToA error;#sigma^{ToA}_{RECO} [ns]", 50, 0., 0.1);
  meOccupancy_ = ibook.book2D(
      "BtlOccupancy", "BTL RECO hits occupancy;Z_{RECO} [cm]; #phi_{RECO} [rad]", 65, -260., 260., 126, -3.2, 3.2);
 
  //meHitTimeCorr_ = ibook.book1D("BtlHitTimeCorr", "BTL RECO hits ToA with Photon-Like TOF correction;Time_{RECO} [ns]", 1000, -5., 25.); 
  
/*
  //--- RECO HIT READ-OUT LEVEL MONITORING 
  for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
	  std::string name = "BtlHitTimeRuSlice_corr_" + std::to_string(ihistoRU + 1);
	  std::string title = "Reco Hit Time with Photon-Like TOF (RU " + std::to_string(ihistoRU + 1) + ");Time_{RECO} [ns];Entries/30ps";
	  meHitTimeRuSlice_corr_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
  }


  for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
	  for (unsigned int ihistoTR = 0; ihistoTR < nTR_; ++ihistoTR) {
		  std::string name_corr = "BtlHitTimeSingleRU_" + std::to_string(ihistoRU + 1) + "_TR" + std::to_string(ihistoTR + 1);        
		  std::string title_corr = "RECO HIT Corrected Time for RU " + std::to_string(ihistoRU + 1) + " TR " + std::to_string(ihistoTR + 1);
		  meHitTimeSingleRU_[ihistoRU][ihistoTR] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);
		  meHitTimeSingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Time_{RECO} [ns]", 1);
		  meHitTimeSingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Entries/30ps", 2);
	  } 
  }

//--- RECO HIT SENSOR MODULE LEVEL MONITORING

  for (uint32_t i = 0; i < nSMphi_; ++i) {

        meHitTimeSmSlice_[i] = ibook.book1D(
                        Form("BtlHitTimeSmSlice_%d", i),
                        Form("Corrected BTL Calibrated RecoHit Time Distribution for Phi SM Slice %d", i),
                        1000, -5., 25.);
        meHitTimeSmSlice_[i]->setAxisTitle("Time_{RECO} [ns]", 1);
        meHitTimeSmSlice_[i]->setAxisTitle("Entries/30ps", 2);
   }


  for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
          for (unsigned int iSM = 0; iSM < nSMinRU_; ++iSM) {
                  std::string name_corr = "BtlHitimeSingleSmInRu_" + std::to_string(ihistoRU + 1) + "_SM" + std::to_string(iSM + 1);
                  std::string title_corr = "RECO HIT Corrected Time in single SM in single RU - RU slice " + std::to_string(ihistoRU + 1) + " SM " + std::to_string(iSM + 1);
                  meHitTimeSingleSmInRu_[ihistoRU][iSM] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);
                  meHitTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Time_{RECO} [ns]", 1);
                  meHitTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Entries/30ps", 2);
          }
  }
*/  
//-----------------------------------------------


  if (optionalPlots_) {
    meLocalOccupancy_ = ibook.book2D(
        "BtlLocalOccupancy", "BTL RECO hits local occupancy;X_{RECO} [cm]; Y_{RECO} [cm]", 100, 10., 10., 60, -3., 3.);
  }
  meHitXlocal_ = ibook.book1D("BtlHitXlocal", "BTL RECO local X;X_{RECO}^{LOC} [cm]", 100, -10., 10.);
  meHitYlocal_ = ibook.book1D("BtlHitYlocal", "BTL RECO local Y;Y_{RECO}^{LOC} [cm]", 60, -3, 3);
  meHitZlocal_ = ibook.book1D("BtlHitZlocal", "BTL RECO local z;z_{RECO}^{LOC} [cm]", 8, -0.4, 0.4);
  meHitZ_ = ibook.book1D("BtlHitZ", "BTL RECO hits Z;Z_{RECO} [cm]", 100, -260., 260.);
  meHitPhi_ = ibook.book1D("BtlHitPhi", "BTL RECO hits #phi;#phi_{RECO} [rad]", 126, -3.2, 3.2);
  meHitEta_ = ibook.book1D("BtlHitEta", "BTL RECO hits #eta;#eta_{RECO}", 100, -1.55, 1.55);
  meHitTvsE_ =
      ibook.bookProfile("BtlHitTvsE", "BTL RECO ToA vs energy;E_{RECO} [MeV];ToA_{RECO} [ns]", 50, 0., 20., 0., 100.);
  meHitEvsPhi_ = ibook.bookProfile(
      "BtlHitEvsPhi", "BTL RECO energy vs #phi;#phi_{RECO} [rad];E_{RECO} [MeV]", 50, -3.2, 3.2, 0., 100.);
  meHitEvsEta_ = ibook.bookProfile(
      "BtlHitEvsEta", "BTL RECO energy vs #eta;#eta_{RECO};E_{RECO} [MeV]", 50, -1.55, 1.55, 0., 100.);
  meHitEvsZ_ =
      ibook.bookProfile("BtlHitEvsZ", "BTL RECO energy vs Z;Z_{RECO} [cm];E_{RECO} [MeV]", 50, -260., 260., 0., 100.);
  meHitTvsPhi_ = ibook.bookProfile(
      "BtlHitTvsPhi", "BTL RECO ToA vs #phi;#phi_{RECO} [rad];ToA_{RECO} [ns]", 50, -3.2, 3.2, 0., 100.);
  meHitTvsEta_ =
      ibook.bookProfile("BtlHitTvsEta", "BTL RECO ToA vs #eta;#eta_{RECO};ToA_{RECO} [ns]", 50, -1.6, 1.6, 0., 100.);
  meHitTvsZ_ =
      ibook.bookProfile("BtlHitTvsZ", "BTL RECO ToA vs Z;Z_{RECO} [cm];ToA_{RECO} [ns]", 50, -260., 260., 0., 100.);
  meHitLongPos_ = ibook.book1D("BtlLongPos", "BTL RECO hits longitudinal position;long. pos._{RECO}", 50, -5, 5);
  meTimeRes_ = ibook.book1D("BtlTimeRes", "BTL time resolution;T_{RECO}-T_{SIM}", 100, -0.5, 0.5);
  meTimeResVsE_ = ibook.bookProfile(
      "BtlTimeResvsE", "BTL time resolution vs hit energy;E_{RECO} [MeV];T_{RECO}-T_{SIM}", 50, 0., 20., -0.5, 0.5, "S");
  meEnergyRes_ = ibook.book1D("BtlEnergyRes", "BTL energy resolution;E_{RECO}-E_{SIM}", 100, -0.5, 0.5);
  meEnergyRelResVsE_ = ibook.bookProfile("BtlEnergyRelResvsE",
                                         "BTL relative energy resolution vs hit energy;E_{RECO} [MeV];E_{RECO}-E_{SIM}",
                                         50,
                                         0.,
                                         20.,
                                         -0.15,
                                         0.15,
                                         "S");
  meLongPosPull_ = ibook.book1D("BtlLongPosPull",
                                "BTL longitudinal position pull;X^{loc}_{RECO}-X^{loc}_{SIM}/#sigma_{xloc_{RECO}}",
                                100,
                                -5.,
                                5.);
  meLongPosPullvsE_ = ibook.bookProfile(
      "BtlLongposPullvsE",
      "BTL longitudinal position pull vs E;E_{SIM} [MeV];X^{loc}_{RECO}-X^{loc}_{SIM}/#sigma_{xloc_{RECO}}",
      20,
      0.,
      20.,
      -5.,
      5.,
      "S");
  meLongPosPullvsEta_ = ibook.bookProfile(
      "BtlLongposPullvsEta",
      "BTL longitudinal position pull vs #eta;|#eta_{RECO}|;X^{loc}_{RECO}-X^{loc}_{SIM}/#sigma_{xloc_{RECO}}",
      32,
      0,
      1.55,
      -5.,
      5.,
      "S");
  meTPullvsE_ = ibook.bookProfile(
      "BtlTPullvsE", "BTL time pull vs E;E_{SIM} [MeV];(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}", 20, 0., 20., -5., 5., "S");
  meTPullvsEta_ = ibook.bookProfile("BtlTPullvsEta",
                                    "BTL time pull vs #eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
                                    30,
                                    0,
                                    1.55,
                                    -5.,
                                    5.,
                                    "S");

  meUnmatchedRecHit_ = ibook.book1D(
      "UnmatchedRecHit", "log10(#BTL crystals with rechits but no simhit);log10(#BTL rechits)", 80, -2., 6.);

  meNclusters_ = ibook.book1D("BtlNclusters", "Number of BTL RECO clusters;log_{10}(N_{RECO})", 100, 0., 5.25);
  meCluTime_ = ibook.book1D("BtlCluTime", "BTL cluster time ToA;ToA [ns]", 250, 0, 25);
  meCluTimeError_ = ibook.book1D("BtlCluTimeError", "BTL cluster time error;#sigma_{t} [ns]", 100, 0, 0.1);
  meCluEnergy_ = ibook.book1D("BtlCluEnergy", "BTL cluster energy;E_{RECO} [MeV]", 100, 0, 20);
  meCluPhi_ = ibook.book1D("BtlCluPhi", "BTL cluster #phi;#phi_{RECO} [rad]", 144, -3.2, 3.2);
  meCluEta_ = ibook.book1D("BtlCluEta", "BTL cluster #eta;#eta_{RECO}", 100, -1.55, 1.55);
  meCluHits_ = ibook.book1D("BtlCluHitNumber", "BTL hits per cluster; Cluster size", 10, 0, 10);
  meCluZvsPhi_ = ibook.book2D(
      "BtlOccupancy", "BTL cluster Z vs #phi;Z_{RECO} [cm]; #phi_{RECO} [rad]", 144, -260., 260., 50, -3.2, 3.2);

  if (optionalPlots_) {
    meCluEnergyvsEta_ = ibook.bookProfile(
        "BtlCluEnergyVsEta", "BTL cluster energy vs #eta; |#eta_{RECO}|; E_{RECO} [cm]", 30, 0., 1.55, 0., 20., "S");
    meCluHitsvsEta_ = ibook.bookProfile(
        "BtlCluHitsVsEta", "BTL hits per cluster vs #eta; |#eta_{RECO}|;Cluster size", 30, 0., 1.55, 0., 10., "S");

    meCluTimeRes_ = ibook.book1D("BtlCluTimeRes", "BTL cluster time resolution;T_{RECO}-T_{SIM} [ns]", 100, -0.5, 0.5);
    meCluEnergyRes_ =
        ibook.book1D("BtlCluEnergyRes", "BTL cluster energy resolution;E_{RECO}-E_{SIM} [MeV]", 100, -0.5, 0.5);
    meCluTResvsE_ = ibook.bookProfile("BtlCluTResvsE",
                                      "BTL cluster time resolution vs E;E_{SIM} [MeV];(T_{RECO}-T_{SIM}) [ns]",
                                      20,
                                      0.,
                                      20.,
                                      -0.5,
                                      0.5,
                                      "S");
    meCluTResvsEta_ = ibook.bookProfile("BtlCluTResvsEta",
                                        "BTL cluster time resolution vs #eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM}) [ns]",
                                        30,
                                        0,
                                        1.55,
                                        -0.5,
                                        0.5,
                                        "S");
    meCluTPullvsE_ = ibook.bookProfile("BtlCluTPullvsE",
                                       "BTL cluster time pull vs E;E_{SIM} [MeV];(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
                                       20,
                                       0.,
                                       20.,
                                       -5.,
                                       5.,
                                       "S");
    meCluTPullvsEta_ =
        ibook.bookProfile("BtlCluTPullvsEta",
                          "BTL cluster time pull vs #eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
                          30,
                          0,
                          1.55,
                          -5.,
                          5.,
                          "S");
    meCluRhoRes_ =
        ibook.book1D("BtlCluRhoRes", "BTL cluster #rho resolution;#rho_{RECO}-#rho_{SIM} [cm]", 100, -0.5, 0.5);
    meCluPhiRes_ =
        ibook.book1D("BtlCluPhiRes", "BTL cluster #phi resolution;#phi_{RECO}-#phi_{SIM} [rad]", 100, -0.03, 0.03);
    meCluZRes_ = ibook.book1D("BtlCluZRes", "BTL cluster Z resolution;Z_{RECO}-Z_{SIM} [cm]", 100, -0.2, 0.2);
    meCluLocalXRes_ =
        ibook.book1D("BtlCluLocalXRes", "BTL cluster local X resolution;X_{RECO}-X_{SIM} [cm]", 100, -3.1, 3.1);
    meCluLocalYResZGlobPlus_ = ibook.book1D(
        "BtlCluLocalYResZGlobPlus", "BTL cluster local Y resolution (glob Z > 0);Y_{RECO}-Y_{SIM} [cm]", 100, -0.2, 0.2);
    meCluLocalYResZGlobMinus_ = ibook.book1D("BtlCluLocalYResZGlobMinus",
                                             "BTL cluster local Y resolution (glob Z < 0);Y_{RECO}-Y_{SIM} [cm]",
                                             100,
                                             -0.2,
                                             0.2);
    meCluSingCrystalLocalYRes_ =
        ibook.book1D("BtlCluSingCrystalLocalYRes",
                     "BTL cluster local Y resolution (single Crystal clusters);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluSingCrystalLocalYResZGlobPlus_ =
        ibook.book1D("BtlCluSingCrystalLocalYResZGlobPlus",
                     "BTL cluster local Y resolution (single Crystal clusters, Z glob > 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluSingCrystalLocalYResZGlobMinus_ =
        ibook.book1D("BtlCluSingCrystalLocalYResZGlobMinus",
                     "BTL cluster local Y resolution (single Crystal clusters, Z glob < 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluMultiCrystalLocalYRes_ =
        ibook.book1D("BtlCluMultiCrystalLocalYRes",
                     "BTL cluster local Y resolution (Multi-Crystal clusters);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluMultiCrystalLocalYResZGlobPlus_ =
        ibook.book1D("BtlCluMultiCrystalLocalYResZGlobPlus",
                     "BTL cluster local Y resolution (Multi-Crystal clusters, Z glob > 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluMultiCrystalLocalYResZGlobMinus_ =
        ibook.book1D("BtlCluMultiCrystalLocalYResZGlobMinus",
                     "BTL cluster local Y resolution (Multi-Crystal clusters, Z glob < 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluCentralLocalYRes_ = ibook.book1D("BtlCluCentralLocalYRes",
                                          "BTL cluster local Y resolution (central region);Y_{RECO}-Y_{SIM} [cm]",
                                          100,
                                          -0.2,
                                          0.2);
    meCluCentralLocalYResZGlobPlus_ =
        ibook.book1D("BtlCluCentralLocalYResZGlobPlus",
                     "BTL cluster local Y resolution (central region, Z glob > 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluCentralLocalYResZGlobMinus_ =
        ibook.book1D("BtlCluCentralLocalYResZGlobMinus",
                     "BTL cluster local Y resolution (central region, Z glob < 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluForwardLocalYRes_ = ibook.book1D("BtlCluForwardLocalYRes",
                                          "BTL cluster local Y resolution (forward region);Y_{RECO}-Y_{SIM} [cm]",
                                          100,
                                          -0.2,
                                          0.2);
    meCluForwardPlusLocalYRes_ =
        ibook.book1D("BtlCluForwardPlusLocalYRes",
                     "BTL cluster local Y resolution (forward region, Z glob > 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluForwardMinusLocalYRes_ =
        ibook.book1D("BtlCluForwardMinusLocalYRes",
                     "BTL cluster local Y resolution (forward region, Z glob < 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);

    meCluLocalYPullZGlobPlus_ = ibook.book1D("BtlCluLocalYPullZGlobPlus",
                                             "BTL cluster local Y pull (glob Z > 0);Y_{RECO}-Y_{SIM}/sigmaY_[RECO]",
                                             100,
                                             -5.,
                                             5.);
    meCluLocalYPullZGlobMinus_ = ibook.book1D("BtlCluLocalYPullZGlobMinus",
                                              "BTL cluster local Y pull (glob Z < 0);Y_{RECO}-Y_{SIM}/sigmaY_[RECO]",
                                              100,
                                              -5.,
                                              5.);

    meCluLocalXPull_ =
        ibook.book1D("BtlCluLocalXPull", "BTL cluster local X pull;X_{RECO}-X_{SIM}/sigmaX_[RECO]", 100, -5., 5.);

    meCluZPull_ = ibook.book1D("BtlCluZPull", "BTL cluster Z pull;Z_{RECO}-Z_{SIM}/sigmaZ_[RECO]", 100, -5., 5.);
    meCluXLocalErr_ = ibook.book1D("BtlCluXLocalErr", "BTL cluster X local error;sigmaX_{RECO,loc} [cm]", 20, 0., 2.);
    meCluYLocalErr_ = ibook.book1D("BtlCluYLocalErr", "BTL cluster Y local error;sigmaY_{RECO,loc} [cm]", 20, 0., 0.4);
    meCluYXLocal_ = ibook.book2D("BtlCluYXLocal",
                                 "BTL cluster local Y vs X;X^{local}_{RECO} [cm];Y^{local}_{RECO} [cm]",
                                 200,
                                 -9.5,
                                 9.5,
                                 200,
                                 -2.8,
                                 2.8);
    meCluYXLocalSim_ = ibook.book2D("BtlCluYXLocalSim",
                                    "BTL cluster local Y vs X;X^{local}_{SIM} [cm];Y^{local}_{SIM} [cm]",
                                    200,
                                    -9.5,
                                    9.5,
                                    200,
                                    -2.8,
                                    2.8);
    meUnmatchedCluEnergy_ =
        ibook.book1D("BtlUnmatchedCluEnergy", "BTL unmatched cluster log10(energy);log10(E_{RECO} [MeV])", 5, -3, 2);
  }

  // with MtdSimLayerCluster as truth

  meCluTrackIdOffset_ =
      ibook.book1D("BtlCluTrackIdOffset", "BTL cluster category (trackId offset); trackId offset", 4, 0.0, 4.0);
  meCluTimeRes_simLC_ = ibook.book1D("BtlCluTimeRes_simLC",
                                     "BTL cluster time resolution (wrt MtdSimLayerClusters);T_{RECO}-T_{SIM} [ns]",
                                     100,
                                     -0.5,
                                     0.5);
  meCluEnergyRes_simLC_ = ibook.book1D("BtlCluEnergyRes_simLC",
                                       "BTL cluster energy resolution (wrt MtdSimLayerClusters);E_{RECO}-E_{SIM} [MeV]",
                                       100,
                                       -0.5,
                                       0.5);
  meCluTResvsE_simLC_ = ibook.bookProfile(
      "BtlCluTResvsE_simLC",
      "BTL cluster time resolution (wrt MtdSimLayerClusters) vs E;E_{SIM} [MeV];(T_{RECO}-T_{SIM}) [ns]",
      20,
      0.,
      20.,
      -0.5,
      0.5,
      "S");
  meCluTResvsEta_simLC_ = ibook.bookProfile(
      "BtlCluTResvsEta_simLC",
      "BTL cluster time resolution (wrt MtdSimLayerClusters) vs #eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM}) [ns]",
      30,
      0,
      1.55,
      -0.5,
      0.5,
      "S");
  meCluTPullvsE_simLC_ = ibook.bookProfile(
      "BtlCluTPullvsE_simLC",
      "BTL cluster time pull (wrt MtdSimLayerClusters) vs E;E_{SIM} [MeV];(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
      20,
      0.,
      20.,
      -5.,
      5.,
      "S");
  meCluTPullvsEta_simLC_ = ibook.bookProfile(
      "BtlCluTPullvsEta_simLC",
      "BTL cluster time pull (wrt MtdSimLayerClusters) vs #eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
      30,
      0,
      1.55,
      -5.,
      5.,
      "S");
  meCluRhoRes_simLC_ = ibook.book1D("BtlCluRhoRes_simLC",
                                    "BTL cluster #rho resolution (wrt MtdSimLayerClusters);#rho_{RECO}-#rho_{SIM} [cm]",
                                    100,
                                    -0.5,
                                    0.5);
  meCluPhiRes_simLC_ =
      ibook.book1D("BtlCluPhiRes_simLC",
                   "BTL cluster #phi resolution (wrt MtdSimLayerClusters);#phi_{RECO}-#phi_{SIM} [rad]",
                   100,
                   -0.03,
                   0.03);
  meCluZRes_simLC_ = ibook.book1D(
      "BtlCluZRes_simLC", "BTL cluster Z resolution (wrt MtdSimLayerClusters);Z_{RECO}-Z_{SIM} [cm]", 100, -0.2, 0.2);
  meCluLocalXRes_simLC_ = ibook.book1D("BtlCluLocalXRes_simLC",
                                       "BTL cluster local X resolution (wrt MtdSimLayerClusters);X_{RECO}-X_{SIM} [cm]",
                                       100,
                                       -3.1,
                                       3.1);
  meCluLocalYResZGlobPlus_simLC_ =
      ibook.book1D("BtlCluLocalYResZGlobPlus_simLC",
                   "BTL cluster local Y resolution (wrt MtdSimLayerClusters, glob Z > 0);Y_{RECO}-Y_{SIM} [cm]",
                   100,
                   -0.2,
                   0.2);
  meCluLocalYResZGlobMinus_simLC_ =
      ibook.book1D("BtlCluLocalYResZGlobMinus_simLC",
                   "BTL cluster local Y resolution (wrt MtdSimLayerClusters, glob Z < 0);Y_{RECO}-Y_{SIM} [cm]",
                   100,
                   -0.2,
                   0.2);
  if (optionalPlots_) {
    meCluSingCrystalLocalYRes_simLC_ = ibook.book1D(
        "BtlCluSingCrystalLocalYRes_simLC",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, single Crystal clusters);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluSingCrystalLocalYResZGlobPlus_simLC_ =
        ibook.book1D("BtlCluSingCrystalLocalYResZGlobPlus_simLC",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, single Crystal clusters, Z glob > "
                     "0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluSingCrystalLocalYResZGlobMinus_simLC_ =
        ibook.book1D("BtlCluSingCrystalLocalYResZGlobMinus_simLC",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, single Crystal clusters, Z glob < "
                     "0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluMultiCrystalLocalYRes_simLC_ = ibook.book1D(
        "BtlCluMultiCrystalLocalYRes_simLC",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, Multi-Crystal clusters);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluMultiCrystalLocalYResZGlobPlus_simLC_ =
        ibook.book1D("BtlCluMultiCrystalLocalYResZGlobPlus_simLC",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, Multi-Crystal clusters, Z glob > "
                     "0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluMultiCrystalLocalYResZGlobMinus_simLC_ =
        ibook.book1D("BtlCluMultiCrystalLocalYResZGlobMinus_simLC",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, Multi-Crystal clusters, Z glob < "
                     "0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluCentralLocalYRes_simLC_ =
        ibook.book1D("BtlCluCentralLocalYRes_simLC",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, central region);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluCentralLocalYResZGlobPlus_simLC_ = ibook.book1D(
        "BtlCluCentralLocalYResZGlobPlus_simLC",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, central region, Z glob > 0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluCentralLocalYResZGlobMinus_simLC_ = ibook.book1D(
        "BtlCluCentralLocalYResZGlobMinus_simLC",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, central region, Z glob < 0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluForwardLocalYRes_simLC_ =
        ibook.book1D("BtlCluForwardLocalYRes_simLC",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, forward region);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluForwardPlusLocalYRes_simLC_ = ibook.book1D(
        "BtlCluForwardPlusLocalYRes_simLC",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, forward region, Z glob > 0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluForwardMinusLocalYRes_simLC_ = ibook.book1D(
        "BtlCluForwardMinusLocalYRes_simLC",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, forward region, Z glob < 0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
  }
  meCluLocalYPullZGlobPlus_simLC_ = ibook.book1D("BtlCluLocalYPullZGlobPlus_simLC",
                                                 "BTL cluster local Y pull (glob Z > 0);Y_{RECO}-Y_{SIM}/sigmaY_[RECO]",
                                                 100,
                                                 -5.,
                                                 5.);
  meCluLocalYPullZGlobMinus_simLC_ =
      ibook.book1D("BtlCluLocalYPullZGlobMinus_simLC",
                   "BTL cluster local Y pull (wrt MtdSimLayerClusters, glob Z < 0);Y_{RECO}-Y_{SIM}/sigmaY_[RECO]",
                   100,
                   -5.,
                   5.);

  meCluLocalXPull_simLC_ =
      ibook.book1D("BtlCluLocalXPull_simLC",
                   "BTL cluster local X pull (wrt MtdSimLayerClusters);X_{RECO}-X_{SIM}/sigmaX_[RECO]",
                   100,
                   -5.,
                   5.);

  meCluZPull_simLC_ = ibook.book1D(
      "BtlCluZPull_simLC", "BTL cluster Z pull (wrt MtdSimLayerClusters);Z_{RECO}-Z_{SIM}/sigmaZ_[RECO]", 100, -5., 5.);
  if (optionalPlots_) {
    meCluYXLocalSim_simLC_ =
        ibook.book2D("BtlCluYXLocalSim_simLC",
                     "BTL cluster local Y vs X (MtdSimLayerClusters);X^{local}_{SIM} [cm];Y^{local}_{SIM} [cm]",
                     200,
                     -9.5,
                     9.5,
                     200,
                     -2.8,
                     2.8);
  }

  ///

  meCluTimeRes_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluTimeRes_simLC_fromIndirectHits",
                   "BTL cluster time resolution (wrt MtdSimLayerClusters, non-direct hits);T_{RECO}-T_{SIM} [ns]",
                   100,
                   -0.5,
                   0.5);
  meCluEnergyRes_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluEnergyRes_simLC_fromIndirectHits",
                   "BTL cluster energy resolution (wrt MtdSimLayerClusters, non-direct hits);E_{RECO}-E_{SIM} [MeV]",
                   100,
                   -0.5,
                   0.5);
  meCluTResvsE_simLC_fromIndirectHits_ =
      ibook.bookProfile("BtlCluTResvsE_simLC_fromIndirectHits",
                        "BTL cluster time resolution (wrt MtdSimLayerClusters, non-direct hits) vs E;E_{SIM} "
                        "[MeV];(T_{RECO}-T_{SIM}) [ns]",
                        20,
                        0.,
                        20.,
                        -0.5,
                        0.5,
                        "S");
  meCluTResvsEta_simLC_fromIndirectHits_ =
      ibook.bookProfile("BtlCluTResvsEta_simLC_fromIndirectHits",
                        "BTL cluster time resolution (wrt MtdSimLayerClusters, non-direct hits) vs "
                        "#eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM}) [ns]",
                        30,
                        0,
                        1.55,
                        -0.5,
                        0.5,
                        "S");
  meCluTPullvsE_simLC_fromIndirectHits_ =
      ibook.bookProfile("BtlCluTPullvsE_simLC_fromIndirectHits",
                        "BTL cluster time pull (wrt MtdSimLayerClusters, non-direct hits) vs E;E_{SIM} "
                        "[MeV];(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
                        20,
                        0.,
                        20.,
                        -5.,
                        5.,
                        "S");
  meCluTPullvsEta_simLC_fromIndirectHits_ =
      ibook.bookProfile("BtlCluTPullvsEta_simLC_fromIndirectHits",
                        "BTL cluster time pull (wrt MtdSimLayerClusters, non-direct hits) vs "
                        "#eta;|#eta_{RECO}|;(T_{RECO}-T_{SIM})/#sigma_{T_{RECO}}",
                        30,
                        0,
                        1.55,
                        -5.,
                        5.,
                        "S");
  meCluRhoRes_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluRhoRes_simLC_fromIndirectHits",
                   "BTL cluster #rho resolution (wrt MtdSimLayerClusters, non-direct hits);#rho_{RECO}-#rho_{SIM} [cm]",
                   100,
                   -0.5,
                   0.5);
  meCluPhiRes_simLC_fromIndirectHits_ = ibook.book1D(
      "BtlCluPhiRes_simLC_fromIndirectHits",
      "BTL cluster #phi resolution (wrt MtdSimLayerClusters, non-direct hits);#phi_{RECO}-#phi_{SIM} [rad]",
      100,
      -0.03,
      0.03);
  meCluZRes_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluZRes_simLC_fromIndirectHits",
                   "BTL cluster Z resolution (wrt MtdSimLayerClusters, non-direct hits);Z_{RECO}-Z_{SIM} [cm]",
                   100,
                   -0.2,
                   0.2);
  meCluLocalXRes_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluLocalXRes_simLC_fromIndirectHits",
                   "BTL cluster local X resolution (wrt MtdSimLayerClusters, non-direct hits);X_{RECO}-X_{SIM} [cm]",
                   100,
                   -3.1,
                   3.1);
  meCluLocalYResZGlobPlus_simLC_fromIndirectHits_ = ibook.book1D(
      "BtlCluLocalYResZGlobPlus_simLC_fromIndirectHits",
      "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, glob Z > 0);Y_{RECO}-Y_{SIM} [cm]",
      100,
      -0.2,
      0.2);
  meCluLocalYResZGlobMinus_simLC_fromIndirectHits_ = ibook.book1D(
      "BtlCluLocalYResZGlobMinus_simLC_fromIndirectHits",
      "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, glob Z < 0);Y_{RECO}-Y_{SIM} [cm]",
      100,
      -0.2,
      0.2);
  if (optionalPlots_) {
    meCluSingCrystalLocalYRes_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluSingCrystalLocalYRes_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, single Crystal "
                     "clusters);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluSingCrystalLocalYResZGlobPlus_simLC_fromIndirectHits_ = ibook.book1D(
        "BtlCluSingCrystalLocalYResZGlobPlus_simLC_fromIndirectHits",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, single Crystal clusters, Z glob > "
        "0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluSingCrystalLocalYResZGlobMinus_simLC_fromIndirectHits_ = ibook.book1D(
        "BtlCluSingCrystalLocalYResZGlobMinus_simLC_fromIndirectHits",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, single Crystal clusters, Z glob < "
        "0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluMultiCrystalLocalYRes_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluMultiCrystalLocalYRes_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, Multi-Crystal "
                     "clusters);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluMultiCrystalLocalYResZGlobPlus_simLC_fromIndirectHits_ = ibook.book1D(
        "BtlCluMultiCrystalLocalYResZGlobPlus_simLC_fromIndirectHits",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, Multi-Crystal clusters, Z glob > "
        "0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluMultiCrystalLocalYResZGlobMinus_simLC_fromIndirectHits_ = ibook.book1D(
        "BtlCluMultiCrystalLocalYResZGlobMinus_simLC_fromIndirectHits",
        "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, Multi-Crystal clusters, Z glob < "
        "0);Y_{RECO}-Y_{SIM} [cm]",
        100,
        -0.2,
        0.2);
    meCluCentralLocalYRes_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluCentralLocalYRes_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, central "
                     "region);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluCentralLocalYResZGlobPlus_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluCentralLocalYResZGlobPlus_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, central region, Z glob "
                     "> 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluCentralLocalYResZGlobMinus_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluCentralLocalYResZGlobMinus_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, central region, Z glob "
                     "< 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluForwardLocalYRes_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluForwardLocalYRes_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, forward "
                     "region);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluForwardPlusLocalYRes_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluForwardPlusLocalYRes_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, forward region, Z glob "
                     "> 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
    meCluForwardMinusLocalYRes_simLC_fromIndirectHits_ =
        ibook.book1D("BtlCluForwardMinusLocalYRes_simLC_fromIndirectHits",
                     "BTL cluster local Y resolution (wrt MtdSimLayerClusters, non-direct hits, forward region, Z glob "
                     "< 0);Y_{RECO}-Y_{SIM} [cm]",
                     100,
                     -0.2,
                     0.2);
  }
  meCluLocalYPullZGlobPlus_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluLocalYPullZGlobPlus_simLC_fromIndirectHits",
                   "BTL cluster local Y pull (glob Z > 0);Y_{RECO}-Y_{SIM}/sigmaY_[RECO]",
                   100,
                   -5.,
                   5.);
  meCluLocalYPullZGlobMinus_simLC_fromIndirectHits_ = ibook.book1D(
      "BtlCluLocalYPullZGlobMinus_simLC_fromIndirectHits",
      "BTL cluster local Y pull (wrt MtdSimLayerClusters, non-direct hits, glob Z < 0);Y_{RECO}-Y_{SIM}/sigmaY_[RECO]",
      100,
      -5.,
      5.);

  meCluLocalXPull_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluLocalXPull_simLC_fromIndirectHits",
                   "BTL cluster local X pull (wrt MtdSimLayerClusters, non-direct hits);X_{RECO}-X_{SIM}/sigmaX_[RECO]",
                   100,
                   -5.,
                   5.);

  meCluZPull_simLC_fromIndirectHits_ =
      ibook.book1D("BtlCluZPull_simLC_fromIndirectHits",
                   "BTL cluster Z pull (wrt MtdSimLayerClusters, non-direct hits);Z_{RECO}-Z_{SIM}/sigmaZ_[RECO]",
                   100,
                   -5.,
                   5.);
  if (optionalPlots_) {
    meCluYXLocalSim_simLC_fromIndirectHits_ =
        ibook.book2D("BtlCluYXLocalSim_simLC_fromIndirectHits",
                     "BTL cluster local Y vs X (MtdSimLayerClusters);X^{local}_{SIM} [cm];Y^{local}_{SIM} [cm]",
                     200,
                     -9.5,
                     9.5,
                     200,
                     -2.8,
                     2.8);
  }

  // --- Gen Particle histograms
 
  meGenPt_ = ibook.book1D("BtlGenPt", "Pt at generator level particles;P_{t} [GeV];Entries", 1000, 0., 500.);
  meGenEne_ = ibook.book1D("BtlGenEne", "Energy at generator level particles;Energy [GeV];Entries", 800, 0., 500.);
  meGenEta_ = ibook.book1D("BtlGenEta", "#eta at generator level particles; #eta;Entries", 100, -4., 4.);
  meGenTheta_ = ibook.book1D("BtlGenTheta", "#theta at generator level particles; #theta [rad];Entries", 100, -3.14159, 3.14159);
  meGenPhi_ = ibook.book1D("BtlGenPhi", "#phi at generator level particles; #phi [rad];Entries", 100, -3.14159, 3.14159);
  meGenPdg_ = ibook.book1D("BtlGenPdg", "Pdg number at generator level particles; Pdg number;Entries", 2000, -1000, 1000);



  // --- UncalibratedRecHits histograms

  meUncTimeMean_ = ibook.book1D("BtlUncTimeMean","BTL UNCALIBRATED RECO hits ToA;ToA_{UNC RECO} [ns]", 100, 0., 25.);
  meUncTimeMean_corr_ = ibook.book1D("BtlUncTimeMean_corr", "Mean Time of Uncalibrated RECO Hits with TOF correction;Time [ns];Entries", 1000, -3., 18.);
  meUncAmpl_global_ = ibook.book1D("BtlUncAmpl_global", "Hit Amplitude;Hit Amplitude [pC];Entries", 1500, -10., 3000.);
  meUncEne_global_ = ibook.book1D("BtlUncEne_global", "Hit Energy;Hit Energy [MeV];Entries", 150, -10., 30.);
  meUncTime_Left_ = ibook.book1D("BtlUncTime_Left_","BTL UNCALIBRATED Recohits ToA Left side;Time (Left) [ns];Entries", 100, 0., 25.);

  meUncTime_Right_ = ibook.book1D("BtlUncTime_Right_","BTL UNCALIBRATED Recohits ToA right side;Time (Right) [ns];Entries", 100, 0., 25.);
  
  meUncTimeCorr_Left_ = ibook.book1D("BtlUncTimeCorr_Left_","BTL UNCALIBRATED Recohits Time corrected with TOF  - Left side;Time (Left) [ns];Entries", 1000, -3., 18.);  
  
  meUncTimeCorr_Right_ =  ibook.book1D("BtlUncTimeCorr_Right_","BTL UNCALIBRATED Recohits Time corrected with TOF - right side;Time (Right) [ns];Entries", 1000, -3., 18.);

  meUncTimeCrystalRing_RU1_ = ibook.book1D("BtlUncTimeCrystalRing_RU1_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU2_ = ibook.book1D("BtlUncTimeCrystalRing_RU2_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU3_ = ibook.book1D("BtlUncTimeCrystalRing_RU3_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU4_ = ibook.book1D("BtlUncTimeCrystalRing_RU4_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU5_ = ibook.book1D("BtlUncTimeCrystalRing_RU5_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU6_ = ibook.book1D("BtlUncTimeCrystalRing_RU6_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU7_ = ibook.book1D("BtlUncTimeCrystalRing_RU7_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU8_ = ibook.book1D("BtlUncTimeCrystalRing_RU8_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU9_ = ibook.book1D("BtlUncTimeCrystalRing_RU9_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU10_ = ibook.book1D("BtlUncTimeCrystalRing_RU10_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU11_ = ibook.book1D("BtlUncTimeCrystalRing_RU11_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meUncTimeCrystalRing_RU12_ = ibook.book1D("BtlUncTimeCrystalRing_RU12_", "Cor time per crystal ring;Time [ns];Entries", 1000, -5., 25.);


//Profile Uncalibrated RecoHit:
  meUncHitTcorr_vs_A_Ctl_ = ibook.bookProfile(
    "BtlUncHitTcorr_vs_A_Ctl",
    "BTL Uncallibrater recohit Time with TOF correction vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    70, 0., 450., -3., 18.);

  meUncHitT_vs_A_Ctl_ = ibook.bookProfile(
    "BtlUncHitT_vs_A_Ctl",
    "BTL Uncallibrater recohit Time vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    70, 0., 450., 0., 25.);
/*
  meUncHitTcorr_vs_A_Forward_ = ibook.bookProfile(
    "BtlUncHitTcorr_vs_A_Forward",
    "BTL Uncallibrater recohit Time with TOF correction vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    70, 55., 450., -3., 18.);

  meUncHitT_vs_A_Forward_ = ibook.bookProfile(
    "BtlUncHitT_vs_A_Forward",
    "BTL Uncallibrater recohit Time vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    34, 55., 450.,0., 25.);
*/
  meUncHitT_L_corr_vs_A_Ctl_ = ibook.bookProfile(
    "BtlUncHitT_L_corr_vs_A_Ctl",
    "BTL Unc-recohit Time left side  with TOF correction vs hit amplitude;Amplitude - Left [pC];Corrected Time - Left [ns]",
    70, 0., 450., -3., 18.);

  meUncHitT_R_corr_vs_A_Ctl_ = ibook.bookProfile(
    "BtlUncHitT_R_corr_vs_A_Ctl",
    "BTL Unc-recohit Time Right side  with TOF correction vs hit amplitude;Amplitude - Right [pC];Corrected Time - Right [ns]",
    70, 0., 450., -3., 18.);

  meUncHitT_L_vs_A_Ctl_ = ibook.bookProfile(
    "BtlUncHitT_L_vs_A_Ctl",
    "BTL Unc-recohit Time left side vs hit amplitude;Amplitude - Left [pC];Time - Left [ns]",
    70, 0., 450., 0., 25.);

  meUncHitT_R_vs_A_Ctl_ = ibook.bookProfile(
    "BtlUncHitT_R_vs_A_Ctl",
    "BTL Unc-recohit Time right side vs hit amplitude;Amplitude - Right [pC];Time - Right [ns]",
    70, 0., 450., 0., 25.);
/*
  meUncHitT_L_corr_vs_A_Forward_ = ibook.bookProfile(
    "BtlUncHitT_L_corr_vs_A_Forward",
    "BTL Unc-recohit Time left side  with TOF correction vs hit amplitude;Amplitude - Left [pC];Corrected Time - Left [ns]",
    30, 55., 450., -3., 18.);

  meUncHitT_R_corr_vs_A_Forward_ = ibook.bookProfile(
    "BtlUncHitT_R_corr_vs_A_Forward",
    "BTL Unc-recohit Time Right side  with TOF correction vs hit amplitude;Amplitude - Right [pC];Corrected Time - Right [ns]",
    30, 55., 450., -3., 18.);

  meUncHitT_L_vs_A_Forward_ = ibook.bookProfile(
    "BtlUncHitT_L_vs_A_Forward",
    "BTL Unc-recohit Time left side vs hit amplitude;Amplitude - Left [pC];Time - Left [ns]",
    30, 55., 450., 0., 25.);

  meUncHitT_R_vs_A_Forward_ = ibook.bookProfile(
    "BtlUncHitT_R_vs_A_Forward",
    "BTL Unc-recohit Time right side vs hit amplitude;Amplitude - Right [pC];Time - Right [ns]",
    30, 55., 450., 0., 25.);
*/


  //Profile con correzione walk time disattivata:

  meUncHitTcorr_vs_A_Ctl_noWT_ = ibook.bookProfile(
    "BtlUncHitTcorr_vs_A_Ctl_noWT",
    "BTL Unc Rechit Time with TOF correction vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    70, 0., 450., -3., 18.);
 /*
  meUncHitTcorr_vs_A_Forward_noWT_ = ibook.bookProfile(
    "BtlUncHitTcorr_vs_A_Forward_noWT",
    "BTL Unc Rechit Time with TOF correction vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    30, 55., 450., -3., 18.);
*/
  meUncHitT_vs_A_Ctl_noWT_ = ibook.bookProfile(
    "BtlUncHitT_vs_A_Ctl_noWT",
    "BTL Unc RecHit Time vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    70, 0., 450., 0., 25.);
/*
 meUncHitT_vs_A_Forward_noWT_ = ibook.bookProfile(
    "BtlUncHitT_vs_A_Forward_noWT",
    "BTL Unc RecHit Time vs Mean hit amplitude;<Amplitude> [pC];<Time> [ns]",
    30, 55., 450., 0., 25.); 
*/
 meUncHitT_L_corr_vs_A_Ctl_noWT_ = ibook.bookProfile(
    "BtlUncHitT_L_corr_vs_A_Ctl_noWT",
    "BTL Unc Rechit Time left side  with TOF correction vs hit amplitude;Amplitude - Left [pC];Corrected Time - Left [ns]",
    70, 0., 450., -3., 18.);
/*
 meUncHitT_L_corr_vs_A_Forward_noWT_ = ibook.bookProfile(
    "BtlUncHitT_L_corr_vs_A_Forward_noWT",
    "BTL Unc Rechit Time left side  with TOF correction vs hit amplitude;Amplitude - Left [pC];Corrected Time - Left [ns]",
    30, 55., 450., -3., 18.);
*/
 meUncHitT_R_corr_vs_A_Ctl_noWT_ = ibook.bookProfile(
    "BtlUncHitT_R_corr_vs_A_Ctl_noWT",
    "BTL Unc Recohit Time Right side  with TOF correction vs hit amplitude;Amplitude - Right [pC];Corrected Time - Right [ns]",
    70, 0., 450., -3., 18.);
/*
 meUncHitT_R_corr_vs_A_Forward_noWT_ = ibook.bookProfile(
    "BtlUncHitT_R_corr_vs_A_Forward_noWT",
    "BTL Unc Recohit Time Right side  with TOF correction vs hit amplitude;Amplitude - Right [pC];Corrected Time - Right [ns]",
    30, 55., 450., -3., 18.);
*/
 meUncHitT_R_vs_A_Ctl_noWT_ = ibook.bookProfile(
    "BtlUncHitT_R_vs_A_Ctl_noWT",
    "BTL Unc-recohit Time Right side vs hit amplitude;Amplitude - Right [pC];Corrected Time - Right [ns]",
    70, 0., 450., -3., 18.);
/*
 meUncHitT_R_vs_A_Forward_noWT_ = ibook.bookProfile(
    "BtlUncHitT_R_vs_A_Forward_noWT",
    "BTL Unc-recohit Time Right side vs hit amplitude;Amplitude - Right [pC];Corrected Time - Right [ns]",
    30, 55., 450., -3., 18.);
*/
  meUncHitT_L_vs_A_Ctl_noWT_ = ibook.bookProfile(
    "BtlUncHitT_L_vs_A_Ctl_noWT",
    "BTL Unc-recohit Time left side vs hit amplitude;Amplitude - Left [pC];Time - Left [ns]",
    70, 0., 450., 0., 25.);
/*
  meUncHitT_L_vs_A_Forward_noWT_ = ibook.bookProfile(
    "BtlUncHitT_L_vs_A_Forward_noWT",
    "BTL Unc-recohit Time left side vs hit amplitude;Amplitude - Left [pC];Time - Left [ns]",
    34, 0., 450., 0., 25.);
*/
 
  meUncHit_DeltaT_R_Ctl_ = ibook.bookProfile(
    "BtlUncHit_DeltaT_R_Ctl_",
    "#Delta time = Time_R_noTW - Time_R ;Amplitude - Right [pC];#Delta - Right [ns]",
    70, 0., 450., -20., 20.);

  meUncHit_DeltaT_L_Ctl_ = ibook.bookProfile(
    "BtlUncHit_DeltaT_L_Ctl_",
    "#Delta time = Time_L_noTW - Time_L ;Amplitude - Left [pC];#Delta - Left [ns]",
    70, 0., 450., -20., 20.);



   for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeRUSlice_" + std::to_string(ihistoRU + 1);
            std::string title = "Mean Time of Uncalibrated RECO Hits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meUncTimeRUSlice_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }

   for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeRUSlice_corr_" + std::to_string(ihistoRU + 1);
            std::string title = "Mean Time with TOF correction of Uncalibrated RECO Hits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meUncTimeRUSlice_corr_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }


   for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
    for (unsigned int ihistoTR = 0; ihistoTR < nTR_; ++ihistoTR) {

        std::string name_corr = "BtlUncTime_SingleRU_" + std::to_string(ihistoRU + 1) + "_TR" + std::to_string(ihistoTR + 1);
        std::string title_corr = "Corrected Time for RU " + std::to_string(ihistoRU + 1) + " TR " + std::to_string(ihistoTR + 1);

        meUncTime_SingleRU_[ihistoRU][ihistoTR] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);


        meUncTime_SingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Time [ns]", 1); 
        meUncTime_SingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Counts", 2); 
   
    }

   }



   for (uint32_t i = 0; i < nSMphi_; ++i) {

	meUncTimePhiSlice_[i] = ibook.book1D(
			Form("BtlUncTimePhiSlice_%d", i),
			Form("Corrected BTL Hit Time Distribution for Phi Slice %d", i),
			1000, -5., 25.);
	meUncTimePhiSlice_[i]->setAxisTitle("Time [ns]", 1);
	meUncTimePhiSlice_[i]->setAxisTitle("Entries/30ps", 2); 
   }


   for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
          for (unsigned int iSM = 0; iSM < nSMinRU_; ++iSM) {
                  std::string name_corr = "UncBtlTimeSingleSmInRu_" + std::to_string(ihistoRU + 1) + "_SM" + std::to_string(iSM + 1);
                  std::string title_corr = "RECO HIT Corrected Time in single SM in single RU - RU slice " + std::to_string(ihistoRU + 1) + " SM " + std::to_string(iSM + 1);
                  meUncTimeSingleSmInRu_[ihistoRU][iSM] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);
                  meUncTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Time_{RECO} [ns]", 1);
                  meUncTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Entries/30ps", 2);
          }
  }





  if (optionalPlots_) {
    meUncEneLVsX_ = ibook.bookProfile("BTLUncEneLVsX",
                                      "BTL uncalibrated hit amplitude left - average vs X;X [cm];Delta(Q_{left}) [pC]",
                                      20,
                                      -5.,
                                      5.,
                                      -640.,
                                      640.,
                                      "S");
    meUncEneRVsX_ =
        ibook.bookProfile("BTLUncEneRVsX",
                          "BTL uncalibrated hit amplitude right - average vs X;X [cm];Delta(Q_{right}) [pC]",
                          20,
                          -5.,
                          5.,
                          -640.,
                          640.,
                          "S");

    meUncTimeLVsX_ = ibook.bookProfile("BTLUncTimeLVsX",
                                       "BTL uncalibrated hit time left - average vs X;X [cm];Delta(T_{left}) [MeV]",
                                       20,
                                       -5.,
                                       5.,
                                       -25.,
                                       25.,
                                       "S");
    meUncTimeRVsX_ = ibook.bookProfile("BTLUncTimeRVsX",
                                       "BTL uncalibrated hit time right - average vs X;X [cm];Delta(T_{right}) [MeV]",
                                       20,
                                       -5.,
                                       5.,
                                       -25.,
                                       25.,
                                       "S");
    if (uncalibRecHitsPlots_) {
      for (unsigned int ihistoQ = 0; ihistoQ < nBinsQ_; ++ihistoQ) {
        std::string hname = Form("TimeResQ_%d", ihistoQ);
        std::string htitle = Form("BTL time resolution (Q bin = %d);T_{RECO} - T_{SIM} [ns]", ihistoQ);
        meTimeResQ_[ihistoQ] = ibook.book1D(hname, htitle, 200, -0.3, 0.7);

        for (unsigned int ihistoEta = 0; ihistoEta < nBinsQEta_; ++ihistoEta) {
          hname = Form("TimeResQvsEta_%d_%d", ihistoQ, ihistoEta);
          htitle =
              Form("BTL time resolution (Q bin = %d, |#eta| bin = %d);T_{RECO} - T_{SIM} [ns]", ihistoQ, ihistoEta);
          meTimeResQvsEta_[ihistoQ][ihistoEta] = ibook.book1D(hname, htitle, 200, -0.3, 0.7);

        }  // ihistoEta loop

      }  // ihistoQ loop

      for (unsigned int ihistoEta = 0; ihistoEta < nBinsEta_; ++ihistoEta) {
        std::string hname = Form("TimeResEta_%d", ihistoEta);
        std::string htitle = Form("BTL time resolution (|#eta| bin = %d);T_{RECO} - T_{SIM} [ns]", ihistoEta);
        meTimeResEta_[ihistoEta] = ibook.book1D(hname, htitle, 200, -0.3, 0.7);

        for (unsigned int ihistoQ = 0; ihistoQ < nBinsEtaQ_; ++ihistoQ) {
          hname = Form("TimeResEtavsQ_%d_%d", ihistoEta, ihistoQ);
          htitle =
              Form("BTL time resolution (|#eta| bin = %d, Q bin = %d);T_{RECO} - T_{SIM} [ns]", ihistoEta, ihistoQ);
          meTimeResEtavsQ_[ihistoEta][ihistoQ] = ibook.book1D(hname, htitle, 200, -0.3, 0.7);

        }  // ihistoQ loop

      }  // ihistoEta loop
    }
  }
}



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void BtlTimeMonitoring::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<std::string>("folder", "MTD/BTL/RecoTiming");
  desc.add<edm::InputTag>("recHitsTag", edm::InputTag("mtdRecHits", "FTLBarrel"));
  desc.add<edm::InputTag>("uncalibRecHitsTag", edm::InputTag("mtdUncalibratedRecHits", "FTLBarrel"));
  desc.add<edm::InputTag>("simHitsTag", edm::InputTag("mix", "g4SimHitsFastTimerHitsBarrel"));
  desc.add<edm::InputTag>("recCluTag", edm::InputTag("mtdClusters", "FTLBarrel"));
  desc.add<edm::InputTag>("trkHitTag", edm::InputTag("mtdTrackingRecHits"));
  desc.add<edm::InputTag>("r2sAssociationMapTag", edm::InputTag("mtdRecoClusterToSimLayerClusterAssociation"));
  desc.add<double>("HitMinimumEnergy", 1.);  // [MeV]
  desc.add<bool>("optionalPlots", false);
  desc.add<bool>("UncalibRecHitsPlots", false);
  desc.add<double>("HitMinimumAmplitude", 30.);  // [pC]

  descriptions.add("btlTimeMonitoring", desc);
}

DEFINE_FWK_MODULE(BtlTimeMonitoring);
