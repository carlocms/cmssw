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


#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/Common/interface/AssociationMap.h"
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

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoVertex/PrimaryVertexProducer/interface/HITrackFilterForPVFinding.h"


class BtlTimeMonitoringWTracks : public DQMEDAnalyzer {
public:
  explicit BtlTimeMonitoringWTracks(const edm::ParameterSet&);
  ~BtlTimeMonitoringWTracks() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
 
  void analyze(const edm::Event&, const edm::EventSetup&) override;
 
  bool isSameCluster(const FTLCluster&, const FTLCluster&);
 
 const edm::Ref<std::vector<TrackingParticle>>* getMatchedTP(const reco::TrackBaseRef&);
 
 const bool trkTPSelAll(const TrackingParticle&);

 const bool trkRecSel(const reco::TrackBase&);
 // ------------ member data ------------

  const std::string folder_;
  const double hitMinEnergy_;
  const bool optionalPlots_;
  const bool uncalibRecHitsPlots_;
  const double hitMinAmplitude_;
  const reco::RecoToSimCollection* r2s_;

  std::unique_ptr<TrackFilterForPVFinding> trackFilter_;

  edm::EDGetTokenT<FTLRecHitCollection> btlRecHitsToken_;
  edm::EDGetTokenT<FTLUncalibratedRecHitCollection> btlUncalibRecHitsToken_;
  edm::EDGetTokenT<CrossingFrame<PSimHit>> btlSimHitsToken_;
  edm::EDGetTokenT<FTLClusterCollection> btlRecCluToken_;
  edm::EDGetTokenT<MTDTrackingDetSetVector> mtdTrackingHitToken_;
  edm::EDGetTokenT<MtdRecoClusterToSimLayerClusterAssociationMap> r2sAssociationMapToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  
  edm::EDGetTokenT<std::vector<reco::GenParticle>> genParticlesToken_;
  edm::EDGetTokenT<edm::SimTrackContainer> simTracksToken_;
  edm::EDGetTokenT<edm::SimVertexContainer> simVerticesToken_;

  const edm::ESGetToken<MTDGeometry, MTDDigiGeometryRecord> mtdgeoToken_;
  const edm::ESGetToken<MTDTopology, MTDTopologyRcd> mtdtopoToken_;
  const edm::ESGetToken<MTDClusterParameterEstimator, MTDCPERecord> cpeToken_;

//WTracks
  edm::EDGetTokenT<reco::TrackCollection> GenRecTrackToken_;
  edm::EDGetTokenT<reco::TrackCollection> RecTrackToken_;
  edm::EDGetTokenT<edm::ValueMap<int>> trackAssocToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> TofPiToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> tofKToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> tofPToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> tmtdToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> SigmatmtdToken_;
  edm::EDGetTokenT<edm::ValueMap<float>> Sigmat0PidToken_;

  edm::EDGetTokenT<edm::ValueMap<float>> pathLengthToken_;
  edm::EDGetTokenT<TrackingParticleCollection> trackingParticleCollectionToken_;
  edm::EDGetTokenT<reco::RecoToSimCollection> recoToSimAssociationToken_;

  edm::ESGetToken<TransientTrackBuilder, TransientTrackRecord> ttrackToken_;


  static constexpr int nTR_ = 36;
  static constexpr int nRU_ = 6;
  static constexpr int nSM_ = 8;
  static constexpr int nCR_ = 128;
  static constexpr int nRU_tot_ = 12;
  static constexpr int nSMphi_ = 96;
  static constexpr int nSMinRU_ = 24;
  static constexpr std::array<int, 12> ringStartIndices_ = {{65, 193, 321, 448, 577, 705, 833, 961, 1089, 1217, 1345, 1473}};

  static constexpr double MinRecoHitEnergy_ = 1.7;
  static constexpr double MaxRecoHitEnergy_ = 16.;
  static constexpr double MaxRecoHitTime_ = 19.;
  static constexpr double MinRecoHitTime_ = -2.;
  static constexpr double trackMaxBtlEta_ = 1.5;
  static constexpr double trackCentralBtlEta_ = 0.65;
  static constexpr double TrackPtCut_ = 0.7;

  static constexpr double etacutGEN_ = 4.;
  static constexpr double etacutREC_ = 3.;
  static constexpr double pTcutBTL_ = 0.7;//defalutl 0.7
  static constexpr double rBTL_ = 110.0;
  static constexpr double zETL_ = 290.0;
  static constexpr bool Debug_ = false;
  
  
  // --- Histograms

    
  //General time distributions recHits:
  MonitorElement* meHitTime_;
  MonitorElement* meHitTimeCorr_;
  MonitorElement* meHitEne_;
  MonitorElement* meHitEne_vs_TimeCorr_;

  //Time corrected with TP matching:
  MonitorElement* meBTLTrackEtaBTLHitMatchedTP_;
  MonitorElement* meBTLTrackPhiBTLHitMatchedTP_;
  MonitorElement* meBTLTrackPtBTLHitMatchedTP_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_;
  MonitorElement* meHitTimeCorrMatchedTP_;
  MonitorElement* meHitTimeCorrMatchedTP_Pi_;
  MonitorElement* meHitTimeCorrMatchedTP_K_;
  MonitorElement* meHitTimeCorrMatchedTP_p_;
  MonitorElement* meHitTimeCorrMatchedTP_mu_;


  MonitorElement* meBTLTrackPtBTLHitMatchedTP_Pi_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_Pi_;
  MonitorElement* meBTLTrackPtBTLHitMatchedTP_K_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_K_;
  MonitorElement* meBTLTrackPtBTLHitMatchedTP_p_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_p_;

  //Cluster size studies (track after selections):
  MonitorElement* meClusterSize_;
  MonitorElement* meClusterSize_vs_TrackEta_;
  MonitorElement* meClusterSize_EtaZone1_;
  MonitorElement* meClusterSize_EtaZone2_;
  MonitorElement* meClusterSize_EtaZone3_;
  MonitorElement* meClusterSize_EtaZone4_;
  MonitorElement* meCluster_time_;
  MonitorElement* meNumberCluPerTrack_;
  //MonitorElement* meHitTimeCorr_LOOSE_;
  //MonitorElement* meHitTimeCorr_MEDIAN_;
  //MonitorElement* meHitTimeCorr_TIGHT_;


  //Corrected Time per granularity element in BTL:
  MonitorElement* meHitTimeRUSlice_[nRU_tot_];
  MonitorElement* meHitTimeRUSlice_corr_[nRU_tot_];
  MonitorElement* meHitTime_SingleRU_[nRU_tot_][nTR_];
  MonitorElement* meHitTimePhiSlice_[nSMphi_];
  MonitorElement* meHitTimeSingleSmInRu_[nRU_tot_][nSMinRU_];
  std::array<MonitorElement*, 12> meHitTimeCrystalRing_;

  //Histograms for Reco Tracks (after all the selection and with Rechits in BTL):
  MonitorElement* meTrackPt_;
  MonitorElement* meTrackP_;
  MonitorElement* metrackPLength_;
  MonitorElement* meTrackPL_vs_Eta_;
  MonitorElement* meTrackPL_vs_Tof_;

  MonitorElement* meTrackPLwrong_pT_;
  MonitorElement* meTrackPLwrong_p_;
  MonitorElement* meTrackPLwrong_Eta_;
  MonitorElement* meTrackPLwrong_TOF_;

 // Histograms TAIL characterization:
 MonitorElement* meTrackPt_TAIL_;
 MonitorElement* meTrackP_TAIL_;
 MonitorElement* meTrackEta_TAIL_;
 MonitorElement* meTrackPLength_TAIL_;
 MonitorElement* meTrackPDG_TAIL_;
 MonitorElement* meTrackPL_vs_Eta_TAIL_;
 MonitorElement* meTrackPL_vs_Tof_TAIL_;

//Histograms Reference true TOF:
 MonitorElement* meHitTime_REF_;
 MonitorElement* meHitTimeCorr_REF_;
 //MonitorElement* meHitTimeRUSlice_corr_REF_[nRU_tot_];



 MonitorElement* meNumTrackRecHitPerTrack_;
 MonitorElement* meNumRecHitPerTrack_;
 


//Histograms for DeltaTOF selection studies: 
 MonitorElement* meBtlDeltaTOFtrack_; 
 MonitorElement* meBtlHitTimeCorr_DTof_Cut1_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut2_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut3_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut4_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut5_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut6_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut7_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut8_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut9_;
 MonitorElement* meBtlHitTimeCorr_DTof_Cut10_;
 
 MonitorElement* meTimeRUSlice_DTofCut1_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut2_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut3_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut4_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut5_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut6_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut7_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut8_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut9_[nRU_tot_];
 MonitorElement* meTimeRUSlice_DTofCut10_[nRU_tot_];

MonitorElement* meBtlHitTimeCorr_pTCut1_;
MonitorElement* meBtlHitTimeCorr_pTCut2_;
MonitorElement* meBtlHitTimeCorr_pTCut3_;
MonitorElement* meTimeRUSlice_pTCut1_[nRU_tot_];
MonitorElement* meTimeRUSlice_pTCut2_[nRU_tot_];
MonitorElement* meTimeRUSlice_pTCut3_[nRU_tot_];




 //Histograms for comparison with TrackValidation:
 MonitorElement* meBTLTrackEtaTot_;
 MonitorElement* meBTLTrackPhiTot_;
 MonitorElement* meBTLTrackPtTot_;
 MonitorElement* meBTLTrackPTot_;
 
 //corrispondent histograms but adding the TP matching
 MonitorElement* meBTLTrackEtaTotMatchedTP_;
 MonitorElement* meBTLTrackPhiTotMatchedTP_;
 MonitorElement* meBTLTrackPtTotMatchedTP_;
 MonitorElement* meBTLTrackPTotMatchedTP_;

 //corrispondent histograms but with Hit in BTL:
 MonitorElement* meBTLTrackEtaBTLHit_;
 MonitorElement* meBTLTrackPhiBTLHit_;
 MonitorElement* meBTLTrackPtBTLHit_;
 MonitorElement* meBTLTrackPBTLHit_;
};


bool BtlTimeMonitoringWTracks::isSameCluster(const FTLCluster& clu1, const FTLCluster& clu2) {
  return clu1.id() == clu2.id() && clu1.size() == clu2.size() && clu1.x() == clu2.x() && clu1.y() == clu2.y() &&
         clu1.time() == clu2.time();
}


const edm::Ref<std::vector<TrackingParticle>>* BtlTimeMonitoringWTracks::getMatchedTP(const reco::TrackBaseRef& recoTrack) {
  auto found = r2s_->find(recoTrack);

  // reco track not matched to any TP
  if (found == r2s_->end())
    return nullptr;

  //matched TP equal to any TP associated to in time events
  for (const auto& tp : found->val) {
    if (tp.first->eventId().bunchCrossing() == 0)
      return &tp.first;
  }

  // reco track not matched to any TP from vertex
  return nullptr;
}



const bool BtlTimeMonitoringWTracks::trkTPSelAll(const TrackingParticle& tp) {
  bool match = false;

  auto x_pv = tp.parentVertex()->position().x();
  auto y_pv = tp.parentVertex()->position().y();
  auto z_pv = tp.parentVertex()->position().z();

  auto r_pv = std::sqrt(x_pv * x_pv + y_pv * y_pv);

  match = tp.charge() != 0 && std::abs(tp.eta()) < etacutGEN_ && tp.pt() > pTcutBTL_ && r_pv < rBTL_ &&
          std::abs(z_pv) < zETL_;
  return match;
}


//reference function:
const bool BtlTimeMonitoringWTracks::trkRecSel(const reco::TrackBase& trk) {
  bool match = false;
  match = std::abs(trk.eta()) <= etacutREC_ && trk.pt() > pTcutBTL_;
  return match;
}


// ------------ constructor and destructor --------------
BtlTimeMonitoringWTracks::BtlTimeMonitoringWTracks(const edm::ParameterSet& iConfig)
    : folder_(iConfig.getParameter<std::string>("folder")),
      hitMinEnergy_(iConfig.getParameter<double>("HitMinimumEnergy")),
      optionalPlots_(iConfig.getParameter<bool>("optionalPlots")),
      uncalibRecHitsPlots_(iConfig.getParameter<bool>("UncalibRecHitsPlots")),
      hitMinAmplitude_(iConfig.getParameter<double>("HitMinimumAmplitude")),
      mtdgeoToken_(esConsumes<MTDGeometry, MTDDigiGeometryRecord>()),
      mtdtopoToken_(esConsumes<MTDTopology, MTDTopologyRcd>()),
      cpeToken_(esConsumes<MTDClusterParameterEstimator, MTDCPERecord>(edm::ESInputTag("", "MTDCPEBase"))) {
      btlRecHitsToken_ = consumes<FTLRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitsTag"));
      btlUncalibRecHitsToken_ = consumes<FTLUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>("uncalibRecHitsTag"));
      btlSimHitsToken_ = consumes<CrossingFrame<PSimHit>>(iConfig.getParameter<edm::InputTag>("simHitsTag"));
      btlRecCluToken_ = consumes<FTLClusterCollection>(iConfig.getParameter<edm::InputTag>("recCluTag"));
      mtdTrackingHitToken_ = consumes<MTDTrackingDetSetVector>(iConfig.getParameter<edm::InputTag>("trkHitTag"));
      r2sAssociationMapToken_ = consumes<MtdRecoClusterToSimLayerClusterAssociationMap>(iConfig.getParameter<edm::InputTag>("r2sAssociationMapTag"));
      beamSpotToken_ = consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));
      genParticlesToken_ = consumes<std::vector<reco::GenParticle>>(edm::InputTag("genParticles", "", "HLT"));
      simTracksToken_ = consumes<edm::SimTrackContainer>(edm::InputTag("g4SimHits"));
      simVerticesToken_ = consumes<edm::SimVertexContainer>(edm::InputTag("g4SimHits"));

   //WTracks:
      GenRecTrackToken_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("inputTagG"));
      RecTrackToken_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("inputTagT"));
      trackAssocToken_ = consumes<edm::ValueMap<int>>(iConfig.getParameter<edm::InputTag>("trackAssocSrc"));
      TofPiToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("TofPi"));
      tofKToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("tofK"));
      tofPToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("tofP"));
      trackingParticleCollectionToken_ = consumes<TrackingParticleCollection>(iConfig.getParameter<edm::InputTag>("SimTag"));
      recoToSimAssociationToken_ = consumes<reco::RecoToSimCollection>(iConfig.getParameter<edm::InputTag>("TPtoRecoTrackAssoc"));
      pathLengthToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("pathLengthSrc"));
      tmtdToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("tmtd"));  
      SigmatmtdToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("sigmatmtd"));    
      Sigmat0PidToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("sigmat0PID"));
          


      ttrackToken_ = esConsumes<TransientTrackBuilder, TransientTrackRecord>(
    edm::ESInputTag("", "TransientTrackBuilder"));

 //     ttrackToken_ = esConsumes<TransientTrackBuilder, TransientTrackRecord>("TransientTrackBuilder");

 	
      {
      edm::ParameterSet filterPSet;
      filterPSet.addParameter<double>("maxD0Significance", 4.0);
      filterPSet.addParameter<double>("maxD0Error", 1.0);
      filterPSet.addParameter<double>("maxDzError", 1.0);
      filterPSet.addParameter<double>("minPt", 0.5);
      filterPSet.addParameter<double>("maxEta", 2.4);
      filterPSet.addParameter<double>("maxNormalizedChi2", 10.0);
      filterPSet.addParameter<std::string>("trackQuality", "any");
      filterPSet.addParameter<int>("minSiliconLayersWithHits", 5);
      filterPSet.addParameter<int>("minPixelLayersWithHits", 2);
      filterPSet.addParameter<int>("minValidStripHits", 0);

      trackFilter_ = std::make_unique<TrackFilterForPVFinding>(filterPSet);
}
      
      }

BtlTimeMonitoringWTracks::~BtlTimeMonitoringWTracks() {}

// ------------ method called for each event  ------------
void BtlTimeMonitoringWTracks::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;
  using namespace geant_units::operators;

  auto geometryHandle = iSetup.getTransientHandle(mtdgeoToken_);
  const MTDGeometry* geom = geometryHandle.product();
  auto topologyHandle = iSetup.getTransientHandle(mtdtopoToken_);
  const MTDTopology* topology = topologyHandle.product();

  auto const& cpe = iSetup.getData(cpeToken_);


  auto btlRecHitsHandle = makeValid(iEvent.getHandle(btlRecHitsToken_));
  auto btlSimHitsHandle = makeValid(iEvent.getHandle(btlSimHitsToken_));
  auto btlRecCluHandle = makeValid(iEvent.getHandle(btlRecCluToken_));
  auto mtdTrkHitHandle = makeValid(iEvent.getHandle(mtdTrackingHitToken_));
  MixCollection<PSimHit> btlSimHits(btlSimHitsHandle.product());
  auto genParticleHandle = makeValid(iEvent.getHandle(genParticlesToken_));
  auto simVerticesHandle = makeValid(iEvent.getHandle(simVerticesToken_));
  edm::Handle<edm::SimTrackContainer> simTracks;
  iEvent.getByToken(simTracksToken_, simTracks);
  if (!simTracks.isValid()) {
	  edm::LogError("SimTrack") << "Error: SimTrack collection not found!";
	  return;
  }

  auto GenRecTrackHandle = makeValid(iEvent.getHandle(GenRecTrackToken_));
  const auto& trackAssoc = iEvent.get(trackAssocToken_);
  const auto& TofPi = iEvent.get(TofPiToken_);
  const auto& tofK = iEvent.get(tofKToken_);
  const auto& tofP = iEvent.get(tofPToken_);
  //const auto& tMtd = iEvent.get(tmtdToken_);
  const auto& SigmatMtd = iEvent.get(SigmatmtdToken_);
  const auto& Sigmat0Pid = iEvent.get(Sigmat0PidToken_);

  auto recoToSimH = makeValid(iEvent.getHandle(recoToSimAssociationToken_));
  r2s_ = recoToSimH.product();

  const auto& pathLength = iEvent.get(pathLengthToken_);

#ifdef EDM_ML_DEBUG
  for (const auto& hits : *mtdTrkHitHandle) {
    if (MTDDetId(hits.id()).mtdSubDetector() == MTDDetId::MTDType::BTL) {
      LogDebug("BtlTimeMonitoringWTracks") << "MTD cluster DetId " << hits.id() << " # cluster " << hits.size();
      for (const auto& hit : hits) {
        LogDebug("BtlTimeMonitoringWTracks")
            << "MTD_TRH: " << hit.localPosition().x() << "," << hit.localPosition().y() << " : "
            << hit.localPositionError().xx() << "," << hit.localPositionError().yy() << " : " << hit.time() << " : "
            << hit.timeError();
      }
    }
  }
#endif


//TENTATIVO LINK: loop track->Loop TrackRecHits in BTL -> Associated Cluster-> loop Cluster RecHits 

  unsigned int index = 0;
  int nTotal = 0;
  int nPassFilter = 0;

  std::set<edm::Ref<FTLClusterCollection, FTLCluster>> uniqueClusters;
  
  for (const auto& trackGen : *GenRecTrackHandle) {
    const reco::TrackRef trackref(iEvent.getHandle(GenRecTrackToken_), index);
    index++;

    if (trackAssoc[trackref] == -1) {
      LogWarning("mtdTracks") << "Extended track not associated";
      continue;
    }

    const reco::TrackRef mtdTrackref = reco::TrackRef(iEvent.getHandle(RecTrackToken_), trackAssoc[trackref]);
    const reco::Track& track = *mtdTrackref;

    //Selection on trackGen:
    if (trkRecSel(trackGen)) {
	    if (std::round(SigmatMtd[trackref] - Sigmat0Pid[trackref]) != 0) {
		   	   std::cout<< "TimeError associated to refitted track is different from TimeError stored in tofPID "<< std::endl;
	    }

    double tof_pi = TofPi[trackref];
    double tof_K = tofK[trackref];
    double tof_p = tofP[trackref];
    double pathL = pathLength[trackref];
    
    bool EtaZone_1 = false;
    bool EtaZone_2 = false;
    bool EtaZone_3 = false;
    bool EtaZone_4 = false;
    

    bool flag_tail = false;
    bool flag_track = false;
    float DeltaTOF_track = 0.;

    //////////////////////////////////////
    //    TrackingParticle matching     //
    //////////////////////////////////////
    bool hasTPMatch = false;
    bool flag_Pi = false;
    bool flag_K  = false;
    bool flag_p  = false;
    bool flag_mu = false;
    const reco::TrackBaseRef trkrefb(trackref);
    auto tp_info = getMatchedTP(trkrefb);

    if (tp_info != nullptr && trkTPSelAll(**tp_info)) {
      hasTPMatch = true;

      // DEBUG: Optional TP-based debug printout
      if (Debug_) {
        if (tof_pi == 0. && pathL < 0.) {
          std::cout << "VALID TP matched, but ToF&pathLength still zero! idx = " << index << "\n"
                    << "  pdg = " << (*tp_info)->pdgId() << "\n"
                    << "  tp.pt = " << (*tp_info)->pt() << std::endl;
        }
      }

      int pdg = std::abs((*tp_info)->pdgId());
      if      (pdg == 211)  flag_Pi = true;
      else if (pdg == 321)  flag_K  = true;
      else if (pdg == 2212) flag_p  = true;
      else if (pdg == 13)   flag_mu = true;

    }

    
    //Tracks in BTL region (All BTL tracks with and without hit in MTD) - patological tracks filtered-out (tof==0 and pathL ==-1)
    if (std::abs(trackGen.eta()) >= trackMaxBtlEta_ || tof_pi == 0) continue;

    meBTLTrackEtaTot_->Fill(std::abs(trackGen.eta()));
    meBTLTrackPhiTot_->Fill(trackGen.phi());
    meBTLTrackPtTot_->Fill(trackGen.pt());
    meBTLTrackPTot_->Fill(trackGen.p());


    if(std::abs(trackGen.eta())>=0 && std::abs(trackGen.eta())<= 0.375) EtaZone_1 = true;
    if(std::abs(trackGen.eta())>0.375 && std::abs(trackGen.eta())<= 0.75) EtaZone_2 = true;
    if(std::abs(trackGen.eta())>0.75 && std::abs(trackGen.eta())<= 1.125) EtaZone_3 = true;
    if(std::abs(trackGen.eta())>1.125 && std::abs(trackGen.eta())<= 1.5) EtaZone_4 = true;
 

    if (hasTPMatch) { //Tracks in BTL matched with a TP
	    meBTLTrackEtaTotMatchedTP_->Fill(std::abs(trackGen.eta()));
            meBTLTrackPhiTotMatchedTP_->Fill(trackGen.phi());
            meBTLTrackPtTotMatchedTP_->Fill(trackGen.pt());
            meBTLTrackPTotMatchedTP_->Fill(trackGen.p());
    }


    ///////////////////////////////////////
    //          TRACK FILTER PV          //
    ///////////////////////////////////////
    nTotal++;
    const auto& builder = iSetup.getData(ttrackToken_);
    reco::TransientTrack ttrack = builder.build(track);
    if (!(*trackFilter_)(ttrack)) continue;
    nPassFilter++;


    ////////////////////////////
    //   pT track selection   //
    ////////////////////////////     
    if (trackGen.pt() < TrackPtCut_) continue;

    int numMTDBtlvalidhits = 0;
    int numBTLRecHits = 0;
    unsigned int nCLuPerTrack = 0;

    //Loop in the track rechits:    
    for (const auto hit : track.recHits()) {
	    if (hit->isValid() == false)        
    		    continue;

	    MTDDetId Hit = hit->geographicalId();

	    //RecHit in BTL subDetector:
	    if ((Hit.det() == 6) && (Hit.subdetId() == 1) && (Hit.mtdSubDetector() == 1)) {
		    
		    numMTDBtlvalidhits++;
    		    const auto* mtdhit = static_cast<const MTDTrackingRecHit*>(hit);
		    if (!mtdhit) continue;
	 	    const auto& hitCluster = mtdhit->mtdCluster();
		    if (hitCluster.size() == 0) continue; 

		    //Ref to cluster			                     
		    auto recoClusterRef = edmNew::makeRefTo(btlRecCluHandle, &hitCluster);
		    
		    bool isNewCluster = uniqueClusters.insert(recoClusterRef).second;
		    if (!isNewCluster) {
			    continue;
			    std::cout << "WARNING: Duplicate cluster found:\n"
			        << "  Cluster ID = " << hitCluster.id().rawId() << "\n"
			    	<< "  Event      = " << iEvent.id().event() << std::endl;
		    }

		    meCluster_time_->Fill(hitCluster.time());
                    meClusterSize_->Fill(hitCluster.size());
		    meClusterSize_vs_TrackEta_->Fill(std::abs(trackGen.eta()),hitCluster.size());
		    if(EtaZone_1) meClusterSize_EtaZone1_->Fill(hitCluster.size());
                    if(EtaZone_2) meClusterSize_EtaZone2_->Fill(hitCluster.size());
		    if(EtaZone_3) meClusterSize_EtaZone3_->Fill(hitCluster.size());
		    if(EtaZone_4) meClusterSize_EtaZone4_->Fill(hitCluster.size());

                    nCLuPerTrack++;

		    meBTLTrackEtaBTLHit_->Fill(std::abs(trackGen.eta()));
                    meBTLTrackPhiBTLHit_->Fill(trackGen.phi());
                    meBTLTrackPtBTLHit_->Fill(trackGen.pt());
                    meBTLTrackPBTLHit_->Fill(trackGen.p());
		        
		    if (hasTPMatch) { //Tracks in BTL (with hits in BTL) matched with a TP
			    meBTLTrackEtaBTLHitMatchedTP_->Fill(std::abs(trackGen.eta()));
			    meBTLTrackPhiBTLHitMatchedTP_->Fill(trackGen.phi());
			    meBTLTrackPtBTLHitMatchedTP_->Fill(trackGen.pt());
			    meBTLTrackPBTLHitMatchedTP_->Fill(trackGen.p());

			    if(flag_Pi){
				    meBTLTrackPtBTLHitMatchedTP_Pi_->Fill(trackGen.pt());
				    meBTLTrackPBTLHitMatchedTP_Pi_->Fill(trackGen.p());
			    }
			    if(flag_K){
                                    meBTLTrackPtBTLHitMatchedTP_K_->Fill(trackGen.pt());
                                    meBTLTrackPBTLHitMatchedTP_K_->Fill(trackGen.p());
                            }
			    if(flag_p){
                                    meBTLTrackPtBTLHitMatchedTP_p_->Fill(trackGen.pt());
                                    meBTLTrackPBTLHitMatchedTP_p_->Fill(trackGen.p());
                            }
		    }

		    //TEST:-------------------------------------------
		    if(Debug_){
		    std::cout << "EVENT: " << iEvent.id().event()<< "\n"
	            	    << "TRACK INDEX: " << index << "\n"
			    << "RECHIT ID: " << hit->geographicalId().rawId() << "\n" 
			    << "CLUSTER ID: " << hitCluster.id().rawId() << "\n"
			    << "CLSTER SIZE: " << hitCluster.size()<< "\n" 
			    <<"//////////////////////////////////////////////////" << std::endl;
		    }



		    /////////////////////////////////
		    //    SELECTION CLUSTER SIZE   //
		    /////////////////////////////////
		    /*
		    Cluster size selection configuration:
		    Config |Zone 1 |Zone 2 | Zone 3 | Zone 4 |
		    ------------------------------------------ 
		    Loose  |   2   |   3   |    4   |    4
		    Median |   2   |   2   |    3   |    4
                    Tight  |   1   |   1   |    2   |    2
		    ------------------------------------------
		    */
		    if(EtaZone_1 && hitCluster.size() > 2) continue;
                    if(EtaZone_2 && hitCluster.size() > 2) continue;
		    if(EtaZone_3 && hitCluster.size() > 2) continue;
		    if(EtaZone_4 && hitCluster.size() > 3) continue;


/*
		    //test per cluster size:
		    if ((EtaZone_1 && hitCluster.size() < 2) ||
	                (EtaZone_2 && hitCluster.size() < 2) ||
			(EtaZone_3 && hitCluster.size() < 3) ||
			(EtaZone_4 && hitCluster.size() < 4)) {
			    flag_tight = true;
		    }

		    if ((EtaZone_1 && hitCluster.size() < 3) ||
                        (EtaZone_2 && hitCluster.size() < 3) ||
                        (EtaZone_3 && hitCluster.size() < 4) ||
                        (EtaZone_4 && hitCluster.size() < 5)) {
                            flag_median = true;
                    }

		    if ((EtaZone_1 && hitCluster.size() < 3) ||
                        (EtaZone_2 && hitCluster.size() < 4) ||
                        (EtaZone_3 && hitCluster.size() < 5) ||
                        (EtaZone_4 && hitCluster.size() < 5)) {
                            flag_loose = true;
                    }
*/

		    
		    BTLDetId cluId = hitCluster.id();
		    DetId detIdObject(cluId);

		    const auto& genericDet = geom->idToDetUnit(detIdObject);
		    if (genericDet == nullptr) {
			    throw cms::Exception("BtlLocalRecoValidation")
				    << "GeographicalID: " << std::hex << cluId << " is invalid!" << std::dec << std::endl;
		    }
                            
		    const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(genericDet->topology());
	    	    const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());
    		    MTDClusterParameterEstimator::ReturnType tuple = cpe.getParameters(hitCluster, *genericDet);

		    // --- Cluster position in the module reference frame
	    	    LocalPoint local_point(std::get<0>(tuple));        
    		    const auto& global_point = genericDet->toGlobal(local_point);

		    for (int ihit = 0; ihit < hitCluster.size(); ++ihit) {

			    int hit_row = hitCluster.minHitRow() + hitCluster.hitOffset()[ihit * 2];
			    int hit_col = hitCluster.minHitCol() + hitCluster.hitOffset()[ihit * 2 + 1];

			    //Loop sulle recHit:
			    for (const auto& recHit : *btlRecHitsHandle) {
				    BTLDetId recHitId(recHit.id().rawId());
        
				    // Check the hit position
				    if (recHitId.mtdSide() != cluId.mtdSide() || recHitId.mtdRR() != cluId.mtdRR() || recHit.row() != hit_row || recHit.column() != hit_col)        
					    continue;

				    // Check the hit energy and time
			    	    if (recHit.energy() != hitCluster.hitENERGY()[ihit] || recHit.time() != hitCluster.hitTIME()[ihit])        
		    			    continue;

				    numBTLRecHits++;
				    flag_track = true;

				    //RecHit Position:
                                    BTLDetId detId = recHit.id();
                                    DetId geoId = detId.geographicalId(MTDTopologyMode::crysLayoutFromTopoMode(topology->getMTDTopologyMode()));
                                    const MTDGeomDet* thedet = geom->idToDet(geoId);
        
			    	    if (thedet == nullptr)
					    throw cms::Exception("BtlLocalRecoValidation") << "GeographicalID: " << std::hex << geoId.rawId() << " ("
						    << detId.rawId() << ") is invalid!" << std::dec << std::endl;

				    const ProxyMTDTopology& topoproxy_rechit = static_cast<const ProxyMTDTopology&>(thedet->topology());
				    const RectangularMTDTopology& topo_rechit = static_cast<const RectangularMTDTopology&>(topoproxy_rechit.specificTopology());

				    Local3DPoint local_point_rechit(0., 0., 0.);        
			    	    local_point_rechit = topo_rechit.pixelToModuleLocalPoint(local_point_rechit, detId.row(topo_rechit.nrows()), detId.column(topo_rechit.nrows()));     
				    const auto& global_point_rechit = thedet->toGlobal(local_point_rechit);
                                         
				    //Energy selection RecoHit:
			    	    if (recHit.energy() > MaxRecoHitEnergy_ || recHit.energy() < MinRecoHitEnergy_)
		    			    continue;	    			    
				    
				    //Time cut for the saturation peak:
				    if (recHit.time()>MaxRecoHitTime_)
					    continue;
                                            
				    //Hit Time-Stamp correction:       
			    	    double TOF_mPi = tof_pi;                                  
			  	    double hit_time_corr = recHit.time() - TOF_mPi;


				    if(hit_time_corr >1.5) flag_tail = true;

			    	    //Time cut on RecoHitTime_corr: -> for cut the residual Min Bias of prev BX
		    		    if(hit_time_corr < MinRecoHitTime_)        
	    				    continue;

				    if(pathL < 114.5) continue;

				    //Sensor Module Index extrapolation:
			    	    auto index_rechit = topology->btlIndex(geoId.rawId());
		    		    //uint32_t SMphi_index = index_rechit.first;
	    			    uint32_t SMeta_index = index_rechit.second;            
    				    int RU_index = (detId.mtdSide() == 0) ? (7 - detId.runit()) : (6 + detId.runit());

				    //--- SM index in a Single RU: 0->23:
				    int SMinRU_index = detId.dmodule() + (detId.dmodule() + detId.smodule());
				    int crystal_index = detId.crystal() + (SMeta_index-1)*16;
				    //int adjusted_SMeta_index =SMeta_index - (RU_index-1)*8;

				    //test cluster size:
				    //if(flag_loose == true) meHitTimeCorr_LOOSE_->Fill(hit_time_corr);
				    //if(flag_median == true) meHitTimeCorr_MEDIAN_->Fill(hit_time_corr);
				    //if(flag_tight == true) meHitTimeCorr_TIGHT_->Fill(hit_time_corr);

				
				    
				    /////////////////////
				    //   HISTOGRAMS   ///
                                    /////////////////////
                                    
				    
				    //Global plots:
                                    meHitTime_->Fill(recHit.time());
                                    meHitTimeCorr_->Fill(hit_time_corr);
                                    meHitEne_->Fill(recHit.energy());
		        	    meHitEne_vs_TimeCorr_->Fill(recHit.energy(), hit_time_corr);

				    if (hasTPMatch) { //Corrected time for tracks matched with TP
					    meHitTimeCorrMatchedTP_->Fill(hit_time_corr);
					    if(flag_Pi) meHitTimeCorrMatchedTP_Pi_->Fill(hit_time_corr);
					    if(flag_K) meHitTimeCorrMatchedTP_K_->Fill(hit_time_corr);
					    if(flag_p) meHitTimeCorrMatchedTP_p_->Fill(hit_time_corr);
					    if(flag_mu) meHitTimeCorrMatchedTP_mu_->Fill(hit_time_corr);
				    }
						    
                                            
				    //Histograms per fixed RU slice:
			    	    meHitTimeRUSlice_[RU_index-1]->Fill(recHit.time());                          
		    		    meHitTimeRUSlice_corr_[RU_index-1]->Fill(hit_time_corr);
                    
	    			    //Histograms per single RU [RU slice - Tray number]:
    				    meHitTime_SingleRU_[RU_index-1][detId.mtdRR()-1]->Fill(hit_time_corr);
    
				    //Histograms for each SM slice:
				    meHitTimePhiSlice_[SMeta_index-1]->Fill(hit_time_corr);

				    // --- Histograms for each SM in a single RU:
				    if (detId.mtdRR() == 1){
					    meHitTimeSingleSmInRu_[RU_index-1][SMinRU_index]->Fill(hit_time_corr);
				    }

				    // --- Specific histograms per Crystal ring (central ring per RU):
				    for (size_t i = 0; i < ringStartIndices_.size(); ++i) {
					    if (crystal_index == ringStartIndices_[i]) {
						    meHitTimeCrystalRing_[i]->Fill(hit_time_corr);
						    break;
					    }
				    }


				    //Exercise with TP matching:
				    if (hasTPMatch && (flag_Pi || flag_K || flag_p)) {				
					    double TOF_True = 0.;
					    if (flag_Pi)      TOF_True = tof_pi;
					    else if (flag_K)  TOF_True = tof_K;
					    else if (flag_p)  TOF_True = tof_p;
					    double HitTimeCorr_true = recHit.time() - TOF_True;
						   
					    meHitTime_REF_->Fill(recHit.time());
					    meHitTimeCorr_REF_->Fill(HitTimeCorr_true);

					    //Histograms per fixed RU slice:
						   // meHitTimeRUSlice_corr_REF_[RU_index-1]->Fill(HitTimeCorr_true);
					    
				    }//hasTPMatch 



				    //STUDY ON DELTA TOF SELECTION:

				    DeltaTOF_track = std::abs(tof_p - tof_pi);
				    meBtlDeltaTOFtrack_->Fill(DeltaTOF_track);

				    if(DeltaTOF_track < 0.1){
					    meBtlHitTimeCorr_DTof_Cut1_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut1_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 0.25){
					    meBtlHitTimeCorr_DTof_Cut2_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut2_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 0.5){
					    meBtlHitTimeCorr_DTof_Cut3_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut3_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 0.75){
					    meBtlHitTimeCorr_DTof_Cut4_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut4_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 1.){
					    meBtlHitTimeCorr_DTof_Cut5_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut5_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 1.25){
					    meBtlHitTimeCorr_DTof_Cut6_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut6_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 1.5){
					    meBtlHitTimeCorr_DTof_Cut7_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut7_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 1.75) {
					    meBtlHitTimeCorr_DTof_Cut8_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut8_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 2.){
					    meBtlHitTimeCorr_DTof_Cut9_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut9_[RU_index-1]->Fill(hit_time_corr);
				    }
				    if(DeltaTOF_track < 2.5){ 
					    meBtlHitTimeCorr_DTof_Cut10_->Fill(hit_time_corr);
					    meTimeRUSlice_DTofCut10_[RU_index-1]->Fill(hit_time_corr);
				    }


				    //STUDY WITH SELECTION ON pT:

				    if(trackGen.pt() > 1.0){
                                            meBtlHitTimeCorr_pTCut1_->Fill(hit_time_corr);
                                            meTimeRUSlice_pTCut1_[RU_index-1]->Fill(hit_time_corr);
                                    }

				    if(trackGen.pt() > 0.9){
                                            meBtlHitTimeCorr_pTCut2_->Fill(hit_time_corr);
                                            meTimeRUSlice_pTCut2_[RU_index-1]->Fill(hit_time_corr);
                                    }

				    if(trackGen.pt() > 0.8){
                                            meBtlHitTimeCorr_pTCut3_->Fill(hit_time_corr);
                                            meTimeRUSlice_pTCut3_[RU_index-1]->Fill(hit_time_corr);
                                    }



			    }//Loop recHit collection
                            
		    }//Loop hit in cluster

	    }//If rechit in track is in BTL
	                                      
    }//Loop recHit in Track

    meNumTrackRecHitPerTrack_->Fill(numMTDBtlvalidhits);
    meNumRecHitPerTrack_->Fill(numBTLRecHits);
    meNumberCluPerTrack_->Fill(nCLuPerTrack);

    if(flag_track){
	    meTrackPt_->Fill(trackGen.pt());
	    meTrackP_->Fill(trackGen.p());
	    meTrackPL_vs_Eta_->Fill(pathL,std::abs(trackGen.eta()));
	    meTrackPL_vs_Tof_->Fill(pathL,tof_pi);
	    metrackPLength_->Fill(pathL);

	    if(pathL < 115.){
		    meTrackPLwrong_pT_->Fill(trackGen.pt());
		    meTrackPLwrong_p_->Fill(trackGen.p());
		    meTrackPLwrong_TOF_->Fill(tof_pi);
		    meTrackPLwrong_Eta_->Fill(trackGen.eta());
	    }
    } 

    if(flag_tail) {
	    meTrackPt_TAIL_->Fill(trackGen.pt());
	    meTrackP_TAIL_->Fill(trackGen.p());
	    meTrackEta_TAIL_->Fill(trackGen.eta());
	    meTrackPLength_TAIL_->Fill(pathL);

	    meTrackPL_vs_Eta_TAIL_->Fill(pathL,std::abs(trackGen.eta()));
	    meTrackPL_vs_Tof_TAIL_->Fill(pathL,tof_pi);
	    
	    if (hasTPMatch) meTrackPDG_TAIL_->Fill(std::abs((*tp_info)->pdgId()));
    }


    }//trkRecSel
  
  }//fine loop sulle recoGenTrack
//std::cout << " ---> Event = " << iEvent.id().event() << "PRE FILTER TRACKS: " << nTotal << " ----> AFTER FILTER PV: " << nPassFilter << std::endl;
 


}//analyzer()

//---------------------------------------------------------------------------------



// ------------ method for histogram booking ------------
void BtlTimeMonitoringWTracks::bookHistograms(DQMStore::IBooker& ibook,
                                            edm::Run const& run,
                                            edm::EventSetup const& iSetup) {
  ibook.setCurrentFolder(folder_);

  // --- histograms booking

	 
 meNumTrackRecHitPerTrack_ = ibook.book1D("NumTrackRecHitPerTrack", "Number of Tracking RecHits per Track; Number of Tracking RecHits per Track", 20, -0.5, 19.5);
 meNumRecHitPerTrack_ = ibook.book1D("NumRecHitPerTrack", "Number of RecHits associated of a single Track ; Number of RecHits per Track", 10, -0.5, 9.5);





 //Histograms for general RecHits time distrbution in BTL:
 meHitTime_ = ibook.book1D("BtlHitTime","BTL recHit ToA;ToA_{RECO} [ns]", 500, 0., 25.);
 meHitTimeCorr_ = ibook.book1D("BtlHitTimeCorr", "Time of RecHit with TOF correction from track(#pi mass);Time [ns];Entries", 1000, -5., 25.);
 meHitEne_ = ibook.book1D("BtlHitEne", "RecHit Energy;Hit Energy [MeV];Entries", 150, 0., 30.);
 meHitEne_vs_TimeCorr_ = ibook.bookProfile("BtlHitEne_vs_TimeCorr","Hit Energy vs corrected time;Hit energy [MeV];Time [ns]", 20, 0., 20., -10., 20.);







 //Histograms corrected track matched with TP:
 meBTLTrackEtaBTLHitMatchedTP_ = ibook.book1D("TrackBTLEtaBTLHitMatchedTP", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiBTLHitMatchedTP_ = ibook.book1D("TrackBTLPhiBTLHitMatchedTP", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtBTLHitMatchedTP_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_ = ibook.book1D("TrackBTLPBTLHitMatchedTP", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 1000);
 meHitTimeCorrMatchedTP_ = ibook.book1D("BtlHitTimeCorrMatchedTP_", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -5., 20.);
 meHitTimeCorrMatchedTP_Pi_ = ibook.book1D("BtlHitTimeCorrMatchedTP_Pi", "Time of recHit with TOF correction from track(Pi mass) - Only Pi particles;Time [ns];Entries", 1000, -5., 20.);
 meHitTimeCorrMatchedTP_K_ = ibook.book1D("BtlHitTimeCorrMatchedTP_K", "Time of recHit with TOF correction from track(Pi mass) - Only K particles;Time [ns];Entries", 1000, -5., 20.);
 meHitTimeCorrMatchedTP_p_ = ibook.book1D("BtlHitTimeCorrMatchedTP_p", "Time of recHit with TOF correction from track(Pi mass) - Only p particles;Time [ns];Entries", 1000, -5., 20.);
 meHitTimeCorrMatchedTP_mu_ = ibook.book1D("BtlHitTimeCorrMatchedTP_mu", "Time of recHit with TOF correction from track(Pi mass) - Only mu particles;Time [ns];Entries", 1000, -5., 20.);
 

 meBTLTrackPtBTLHitMatchedTP_Pi_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP_Pi", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_Pi_ = ibook.book1D("TrackBTLPBTLHitMatchedTP_Pi", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
 meBTLTrackPtBTLHitMatchedTP_K_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP_K", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_K_ = ibook.book1D("TrackBTLPBTLHitMatchedTP_K", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
 meBTLTrackPtBTLHitMatchedTP_p_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP_p", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_p_ = ibook.book1D("TrackBTLPBTLHitMatchedTP_p", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);


 //TP matching time distributions reference:
 meHitTime_REF_ = ibook.book1D("BtlHitTime_REF","BTL recHit ToA;ToA_{RECO} [ns]", 100, 0., 25.);
 meHitTimeCorr_REF_ = ibook.book1D("BtlHitTimeCorr_REF", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -3., 18.);
/*
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlHitTimeRUSlice_corr_REF_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meHitTimeRUSlice_corr_TRUE_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
*/





 //Histograms for Reco Tracks (after all the selection and with Rechits in BTL): 
 meTrackPt_ = ibook.book1D("BtlTrackPt", "p_T of tracks (BTL) ;pT_{RECO} [GeV]", 500, 0, 50);
 meTrackP_ = ibook.book1D("BtlTrackP", "P of tracks (BTL) ;p_{RECO} [GeV]", 700, 0, 100);
 metrackPLength_ = ibook.book1D("BtlTrackPLength", "Track Path Length ;Path Length [cm];Entries", 250, 0, 500);
 meTrackPL_vs_Tof_ =  ibook.bookProfile("BtlTrackPL_vs_Tof","Path Length vs Tof (m_{#pi});Path Length [cm];Tof [ns]", 100, 0., 350., 0., 15.);
 meTrackPL_vs_Eta_ =  ibook.bookProfile("BtlTrackPL_vs_Eta","Path Length vs |#eta|;Path Length [cm];|#eta|", 100, 0., 350., 0., 2.);
 
 meTrackPLwrong_pT_ = ibook.book1D("BtlTrackPLwrong_pT_", "p_T of tracks (BTL) ;pT_{RECO} [GeV]", 500, 0, 50);
 meTrackPLwrong_p_ = ibook.book1D("BtlTrackPLwrong_p", "P of tracks (BTL) ;p_{RECO} [GeV]", 700, 0, 100);
 meTrackPLwrong_Eta_ = ibook.book1D("BtlTrackPLwrong_Eta", "#eta of tracks (BTL) ;#eta",100, -1.55, 1.55); 
 meTrackPLwrong_TOF_ = ibook.book1D("BtlTrackPLwrong_TOF", "TOF under Pion mass Hypothesis of tracks (BTL) ;Tof(m_{#pi})",1000, 0., 30);

 // Histograms TAIL characterization:
 meTrackPt_TAIL_ = ibook.book1D("BtlTrackPt_TAIL", "Pt of tracks (BTL) ;pt_{RECO} [GeV]", 500, 0, 50);
 meTrackP_TAIL_ = ibook.book1D("BtlTrackP_TAIL", "P of tracks (BTL) ;p_{RECO} [GeV]", 700, 0, 100);
 meTrackEta_TAIL_ = ibook.book1D("BtlTrackEta_TAIL", "P of tracks (BTL) ;p_{RECO} [GeV]",100, -1.55, 1.55);
 meTrackPLength_TAIL_ = ibook.book1D("BtlTrackPLength_TAIL", "Track Path Length ;Path Length [cm];Entries", 250, 0, 500);
 meTrackPDG_TAIL_ = ibook.book1D("BtlTrackPDG_TAIL", "PDG number of the particle in the tail ;PDG code",5000, 0.,5000);
 meTrackPL_vs_Eta_TAIL_ = ibook.book2D("BtlTrackPL_vs_Eta_TAIL","Path Length vs |#eta|;Path Length [cm];|#eta|", 100, 0., 350., 100,0., 2.);
 meTrackPL_vs_Tof_TAIL_ = ibook.book2D("BtlTrackPL_vs_Tof_TAIL","Path Length vs Tof (m_{#pi});Path Length [cm];Tof [ns]", 100, 0., 350.,500, 0., 15.);




 //Histograms for Cluster studies:
 meClusterSize_ = ibook.book1D("BtlClusterSize", "Cluster Size associated to a reco track after the selections;Cluster Size;Entries", 17, -0.5, 16.5);
 meClusterSize_EtaZone1_ = ibook.book1D("BtlClusterSize_EtaZone1", "Cluster Size associated to a reco track in |#eta| Zone 1:[0.0, 0.375];Cluster Size;Entries", 17, -0.5, 16.5); 
 meClusterSize_EtaZone2_ = ibook.book1D("BtlClusterSize_EtaZone2", "Cluster Size associated to a reco track in |#eta| Zone 2:[0.375, 0.75];Cluster Size;Entries", 17, -0.5, 16.5);
 meClusterSize_EtaZone3_ = ibook.book1D("BtlClusterSize_EtaZone3", "Cluster Size associated to a reco track in |#eta| Zone 3:[0.75, 1.125];Cluster Size;Entries", 17, -0.5, 16.5);
 meClusterSize_EtaZone4_ = ibook.book1D("BtlClusterSize_EtaZone4", "Cluster Size associated to a reco track in |#eta| Zone 4:[1.125, 1.5];Cluster Size;Entries", 17, -0.5, 16.5);
 meClusterSize_vs_TrackEta_ = ibook.bookProfile("BtlClusterSize_vs_TrackEta","Track |#eta| vs Cluster size ;|#eta| Reco Track;<Cluster Size>", 20, 0., 1.5, 0., 20.);
 meCluster_time_ = ibook.book1D("BtlCluster_time","BTL Cluster ToA associated to a selected reco Track;ToA_{RECO} Cluster [ns];Counts", 100, 0., 25.);
 meNumberCluPerTrack_ = ibook.book1D("BtlCluPerTrack", "Number of BTL Clusters per selected Track;# Cluster per Track", 10, -0.5, 9.5);
//meHitTimeCorr_LOOSE_ = ibook.book1D("CLU_SIZE_BtlHitTimeCorr_LOOSE", "Corrected time: |p| cut = 0.5;Time [ns];Entries", 1000, -3., 18.);
//meHitTimeCorr_MEDIAN_ = ibook.book1D("CLU_SIZE_BtlHitTimeCorr_MEDIAN", "Corrected time: |p| cut = 0.5;Time [ns];Entries", 1000, -3., 18.);
//meHitTimeCorr_TIGHT_ = ibook.book1D("CLU_SIZE_BtlHitTimeCorr_TIGHT", "Corrected time: |p| cut = 0.5;Time [ns];Entries", 1000, -3., 18.);





 //Time corr per granularity levels:
  for (size_t i = 0; i < meHitTimeCrystalRing_.size(); ++i) {
  std::string name = "BtlHitTimeCrystalRing_RU" + std::to_string(i + 1);
  std::string title = "Corrected time (with TOF) per crystal ring RU" + std::to_string(i + 1) +
                      ";Time [ns];Entries";
  meHitTimeCrystalRing_[i] = ibook.book1D(name, title, 1000, -5., 25.);
}

   for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlHitTimeRUSlice_" + std::to_string(ihistoRU + 1);
            std::string title = "ToA of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meHitTimeRUSlice_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }

   for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlHitTimeRUSlice_corr_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meHitTimeRUSlice_corr_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }


   for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
    for (unsigned int ihistoTR = 0; ihistoTR < nTR_; ++ihistoTR) {

        std::string name_corr = "BtlHitTime_SingleRU_" + std::to_string(ihistoRU + 1) + "_TR" + std::to_string(ihistoTR + 1);
        std::string title_corr = "Corrected Time for RU " + std::to_string(ihistoRU + 1) + " TR " + std::to_string(ihistoTR + 1);

        meHitTime_SingleRU_[ihistoRU][ihistoTR] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);

        meHitTime_SingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Time [ns]", 1);
        meHitTime_SingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Counts", 2);

    }
   }


   for (uint32_t i = 0; i < nSMphi_; ++i) {

        meHitTimePhiSlice_[i] = ibook.book1D(
                        Form("BtlHitTimePhiSlice_%d", i),
                        Form("Corrected BTL Hit Time Distribution for Phi Slice %d", i),
                        1000, -5., 25.);
        meHitTimePhiSlice_[i]->setAxisTitle("Time [ns]", 1);
        meHitTimePhiSlice_[i]->setAxisTitle("Entries/30ps", 2);
   }


   for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
          for (unsigned int iSM = 0; iSM < nSMinRU_; ++iSM) {
                  std::string name_corr = "BtlHitTimeSingleSmInRu_" + std::to_string(ihistoRU + 1) + "_SM" + std::to_string(iSM + 1);
                  std::string title_corr = "RECO HIT Corrected Time in single SM in single RU - RU slice " + std::to_string(ihistoRU + 1) + " SM " + std::to_string(iSM + 1);
                  meHitTimeSingleSmInRu_[ihistoRU][iSM] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);
                  meHitTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Time_{RECO} [ns]", 1);
                  meHitTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Entries/30ps", 2);
          }
  }





 //Histograms for DeltaTOF selection studies:
meBtlDeltaTOFtrack_ = ibook.book1D("DTOF_BtlDeltaTOFtrack", "Distribution of #Delta TOF = |TOF(m_{#pi}) - TOF(m_{P})|;#Delta TOF[ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut1_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut1", "Corrected time: #Delta TOF  cut = 0.1 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut2_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut2", "Corrected time: #Delta TOF  cut = 0.25 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut3_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut3", "Corrected time: #Delta TOF  cut = 0.5 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut4_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut4", "Corrected time: #Delta TOF  cut = 0.75 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut5_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut5", "Corrected time: #Delta TOF  cut = 1.0 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut6_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut6", "Corrected time: #Delta TOF  cut = 1.25 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut7_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut7", "Corrected time: #Delta TOF  cut = 1.5 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut8_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut8", "Corrected time: #Delta TOF  cut = 1.75 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut9_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut9", "Corrected time: #Delta TOF  cut = 2.0 ns ;Time [ns];Entries", 1000, -5., 20.);
meBtlHitTimeCorr_DTof_Cut10_ = ibook.book1D("DTOF_BtlHitTimeCorr_Cut10", "Corrected time: #Delta TOF  cut = 2.5 ns ;Time [ns];Entries", 1000, -5., 20.);

 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut1_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut1_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }

 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut2_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut2_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut3_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut3_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut4_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut4_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut5_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut5_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut6_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut6_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut7_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut7_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut8_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut8_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut9_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut9_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_DTofCut10_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_DTofCut10_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }




//Histo plot selection in pT:
meBtlHitTimeCorr_pTCut1_ = ibook.book1D("pT_BtlHitTimeCorr_pTCut1", "p_{T}  cut = 1 GeV ;Time [ns];Entries", 1000, -3., 18.);
meBtlHitTimeCorr_pTCut2_ = ibook.book1D("pT_BtlHitTimeCorr_pTCut2", "p_{T}  cut = 0.9 GeV ;Time [ns];Entries", 1000, -3., 18.);
meBtlHitTimeCorr_pTCut3_ = ibook.book1D("pT_BtlHitTimeCorr_pTCut3", "p_{T}  cut = 0.8 GeV;Time [ns];Entries", 1000, -3., 18.);
 
 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_pTCut1_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_pTCut1_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }

 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_pTCut2_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_pTCut2_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }

 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "TimeRUSlice_pTCut3_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Time of recHits (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meTimeRUSlice_pTCut3_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }






//Histograms for comparison with TrackValidation:
 meBTLTrackEtaTot_ = ibook.book1D("TrackBTLEtaTot", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiTot_ = ibook.book1D("TrackBTLPhiTot", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtTot_ = ibook.book1D("TrackBTLPtTot", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPTot_ = ibook.book1D("TrackBTLPTot", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
 meBTLTrackEtaTotMatchedTP_ = ibook.book1D("TrackBTLEtaTotMatchedTP", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiTotMatchedTP_ = ibook.book1D("TrackBTLPhiTotMatchedTP", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtTotMatchedTP_ = ibook.book1D("TrackBTLPtTotMatchedTP", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPTotMatchedTP_ = ibook.book1D("TrackBTLPTotMatchedTP", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
meBTLTrackEtaBTLHit_ = ibook.book1D("TrackBTLEtaBTLHit", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiBTLHit_ = ibook.book1D("TrackBTLPhiBTLHit", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtBTLHit_ = ibook.book1D("TrackBTLPtBTLHit", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHit_ = ibook.book1D("TrackBTLPBTLHit", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 1000);

}//BookHis



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void BtlTimeMonitoringWTracks::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<std::string>("folder", "MTD/BTL/RecoTimingWTracks");
  desc.add<edm::InputTag>("recHitsTag", edm::InputTag("mtdRecHits", "FTLBarrel"));
  desc.add<edm::InputTag>("uncalibRecHitsTag", edm::InputTag("mtdUncalibratedRecHits", "FTLBarrel"));
  desc.add<edm::InputTag>("simHitsTag", edm::InputTag("mix", "g4SimHitsFastTimerHitsBarrel"));
  desc.add<edm::InputTag>("recCluTag", edm::InputTag("mtdClusters", "FTLBarrel"));
  desc.add<edm::InputTag>("trkHitTag", edm::InputTag("mtdTrackingRecHits"));
  desc.add<edm::InputTag>("r2sAssociationMapTag", edm::InputTag("mtdRecoClusterToSimLayerClusterAssociation"));
  desc.add<edm::InputTag>("inputTagG", edm::InputTag("generalTracks"));
  desc.add<edm::InputTag>("inputTagT", edm::InputTag("trackExtenderWithMTD"));
  desc.add<edm::InputTag>("TofPi", edm::InputTag("trackExtenderWithMTD:generalTrackTofPi"));
  desc.add<edm::InputTag>("tofK", edm::InputTag("trackExtenderWithMTD:generalTrackTofK"));
  desc.add<edm::InputTag>("tofP", edm::InputTag("trackExtenderWithMTD:generalTrackTofP"));
  desc.add<edm::InputTag>("tmtd", edm::InputTag("trackExtenderWithMTD:generalTracktmtd"));
  desc.add<edm::InputTag>("sigmatmtd", edm::InputTag("trackExtenderWithMTD:generalTracksigmatmtd"));
  desc.add<edm::InputTag>("sigmat0PID", edm::InputTag("tofPID:sigmat0"));
  desc.add<edm::InputTag>("trackAssocSrc", edm::InputTag("trackExtenderWithMTD:generalTrackassoc"));
  desc.add<edm::InputTag>("TPtoRecoTrackAssoc", edm::InputTag("trackingParticleRecoTrackAsssociation"));
  desc.add<edm::InputTag>("pathLengthSrc", edm::InputTag("trackExtenderWithMTD:generalTrackPathLength"));
  desc.add<edm::InputTag>("SimTag", edm::InputTag("mix", "MergedTrackTruth"));
  desc.add<double>("HitMinimumEnergy", 1.);  // [MeV]
  desc.add<bool>("optionalPlots", false);
  desc.add<bool>("UncalibRecHitsPlots", false);
  desc.add<double>("HitMinimumAmplitude", 30.);  // [pC]

  {
    edm::ParameterSetDescription psd0;
    HITrackFilterForPVFinding::fillPSetDescription(psd0);  // extension of TrackFilterForPVFinding
    desc.add<edm::ParameterSetDescription>("TkFilterParameters", psd0);
  }

  descriptions.add("btlTimeMonitoringWTracks", desc);
}



DEFINE_FWK_MODULE(BtlTimeMonitoringWTracks);
