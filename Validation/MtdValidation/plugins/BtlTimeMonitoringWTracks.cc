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

  static constexpr double etacutGEN_ = 4.;
  static constexpr double etacutREC_ = 3.;
  static constexpr double pTcutBTL_ = 0.7;//defalutl 0.7
  static constexpr double rBTL_ = 110.0;
  static constexpr double zETL_ = 290.0;
  static constexpr bool Debug_ = false;
  
  
  // --- Histograms

  MonitorElement* meTracktmtd_;
  MonitorElement* meBTLTrackEtaTot_;
  MonitorElement* meBTLTrackPhiTot_;
  MonitorElement* meBTLTrackPtTot_;
  MonitorElement* meBTLTrackPTot_;

  MonitorElement* meBTLTrackEtaTotMatchedTP_;
  MonitorElement* meBTLTrackPhiTotMatchedTP_;
  MonitorElement* meBTLTrackPtTotMatchedTP_;
  MonitorElement* meBTLTrackPTotMatchedTP_;

  MonitorElement* meBTLTrackEtaBTLHit_;
  MonitorElement* meBTLTrackPhiBTLHit_;
  MonitorElement* meBTLTrackPtBTLHit_;
  MonitorElement* meBTLTrackPBTLHit_;

  MonitorElement* meBTLTrackEtaBTLHitMatchedTP_;
  MonitorElement* meBTLTrackPhiBTLHitMatchedTP_;
  MonitorElement* meBTLTrackPtBTLHitMatchedTP_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_;

  MonitorElement* meBTLTrackPtBTLHitMatchedTP_Pi_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_Pi_;
  MonitorElement* meBTLTrackPtBTLHitMatchedTP_K_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_K_;
  MonitorElement* meBTLTrackPtBTLHitMatchedTP_p_;
  MonitorElement* meBTLTrackPBTLHitMatchedTP_p_;

  MonitorElement* meHitTime_;
  MonitorElement* meHitTimeCorr_;
  MonitorElement* meHitTimeRUSlice_[nRU_tot_];
  MonitorElement* meHitTimeRUSlice_corr_[nRU_tot_];
  MonitorElement* meHitTime_SingleRU_[nRU_tot_][nTR_];
  MonitorElement* meHitTimePhiSlice_[nSMphi_];
  MonitorElement* meHitEne_;
  MonitorElement* meNumberCluPerTrack_;
  MonitorElement* meTrackNumHits_;

  
  MonitorElement* meHitTimeSingleSmInRu_[nRU_tot_][nSMinRU_];
  std::array<MonitorElement*, 12> meHitTimeCrystalRing_;

  MonitorElement* meHitTimeCorrMatchedTP_; 
  MonitorElement* meHitTimeCorrMatchedTP_Pi_;
  MonitorElement* meHitTimeCorrMatchedTP_K_;
  MonitorElement* meHitTimeCorrMatchedTP_p_;
  MonitorElement* meHitTimeCorrMatchedTP_mu_;

  MonitorElement* meClusterSize_;
  MonitorElement* meClusterSize_vs_TrackEta_;
  MonitorElement* meClusterSize_zone1_;
  MonitorElement* meClusterSize_zone2_;
  MonitorElement* meClusterSize_zone3_;
  MonitorElement* meClusterSize_zone4_;

  MonitorElement* meClusterSizePtCut_;
  MonitorElement* meClusterSizePtCut_vs_TrackEta_;
  MonitorElement* meClusterSizePtCut_zone1_;
  MonitorElement* meClusterSizePtCut_zone2_;
  MonitorElement* meClusterSizePtCut_zone3_;
  MonitorElement* meClusterSizePtCut_zone4_;

  MonitorElement* meHitEnevsTimeCorr_;
  MonitorElement* meHitEta_Pi_BUMP_;
  MonitorElement* meTrackEta_Pi_BUMP_;
  MonitorElement* meNum_Hit_Pi_BUMP_;
  MonitorElement* mePt_Pi_BUMP_;
  MonitorElement* meCluTime_Pi_BUMP_;
  MonitorElement* meSimHitType_Pi_BUMP_;

  MonitorElement* meTrackEta_BUMP_;
  MonitorElement* meHitEta_BUMP_;
  MonitorElement* meHitRU_index_BUMP_;
  MonitorElement* mePt_BUMP_;
  MonitorElement* meTrackPt_;
  MonitorElement* meTrackP_;
  MonitorElement* meHitTimeCorr_NO_PTcut_;
  MonitorElement* meTOF_BUMP_;
  MonitorElement* mePathL_BUMP_;

 MonitorElement* meCluster_time_;
 MonitorElement* meCluster_timePtCut_;
 MonitorElement* meNumbBTLRecHits_;

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

/*
//Modified function with eta BTL end selection on p tot
const bool BtlTimeMonitoringWTracks::trkRecSel(const reco::TrackBase& trk) {
  bool match = false;
  match = std::abs(trk.eta()) <= trackMaxBtlEta_ && trk.p() > pCutBTL_;
  return match;
}
*/

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
  const auto& tMtd = iEvent.get(tmtdToken_);
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


//TENTATIVO LINK: loop track->trovo cluster->loop sui rechit del cluster 

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
	    meTracktmtd_->Fill(tMtd[trackref]);
	    if (std::round(SigmatMtd[trackref] - Sigmat0Pid[trackref]) != 0) {
		   	   std::cout<< "TimeError associated to refitted track is different from TimeError stored in tofPID "<< std::endl;
	    }

    double tof_pi = TofPi[trackref];
    double pathL = pathLength[trackref];
    

    // --- TP MATCHING FLAG ---
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
        if (tof_pi == 0. && pathL < 1.) {
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
		    if(std::abs(trackGen.eta())>=0 && std::abs(trackGen.eta())<= 0.286) meClusterSize_zone1_->Fill(hitCluster.size());
                    if(std::abs(trackGen.eta())>0.286 && std::abs(trackGen.eta())<= 0.598) meClusterSize_zone2_->Fill(hitCluster.size());
		    if(std::abs(trackGen.eta())>0.598 && std::abs(trackGen.eta())<= 0.974) meClusterSize_zone3_->Fill(hitCluster.size());
		    if(std::abs(trackGen.eta())>0.974 && std::abs(trackGen.eta())<= 1.5) meClusterSize_zone4_->Fill(hitCluster.size());

                    nCLuPerTrack++;

		    if (trackGen.pt() < 1.) {
                            meCluster_timePtCut_->Fill(hitCluster.time());
                            meClusterSizePtCut_->Fill(hitCluster.size());
			    meClusterSizePtCut_vs_TrackEta_->Fill(std::abs(trackGen.eta()),hitCluster.size());
			    if(std::abs(trackGen.eta())>=0 && std::abs(trackGen.eta())<= 0.286) meClusterSizePtCut_zone1_->Fill(hitCluster.size());
                            if(std::abs(trackGen.eta())>0.286 && std::abs(trackGen.eta())<= 0.598) meClusterSizePtCut_zone2_->Fill(hitCluster.size());
                            if(std::abs(trackGen.eta())>0.598 && std::abs(trackGen.eta())<= 0.974) meClusterSizePtCut_zone3_->Fill(hitCluster.size());
                            if(std::abs(trackGen.eta())>0.974 && std::abs(trackGen.eta())<= 1.5) meClusterSizePtCut_zone4_->Fill(hitCluster.size());
                    }

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

			    	    //Time cut on RecoHitTime_corr: -> for cut the residual Min Bias of prev BX
		    		    if(hit_time_corr < MinRecoHitTime_)        
	    				    continue;

				    //Sensor Module Index extrapolation:
			    	    auto index_rechit = topology->btlIndex(geoId.rawId());
		    		    //uint32_t SMphi_index = index_rechit.first;
	    			    uint32_t SMeta_index = index_rechit.second;            
    				    int RU_index = (detId.mtdSide() == 0) ? (7 - detId.runit()) : (6 + detId.runit());

				    //--- SM index in a Single RU: 0->23:
				    int SMinRU_index = detId.dmodule() + (detId.dmodule() + detId.smodule());
				    int crystal_index = detId.crystal() + (SMeta_index-1)*16;
				    //int adjusted_SMeta_index =SMeta_index - (RU_index-1)*8;

				    //-------- TEST -----------
				    meHitTimeCorr_NO_PTcut_->Fill(hit_time_corr);
                                    if(hit_time_corr > 8.4 && hit_time_corr < 11.) {

                                            meTrackEta_BUMP_->Fill(trackGen.eta());
                                            meHitEta_BUMP_->Fill(global_point_rechit.eta());
                                            meHitRU_index_BUMP_->Fill(RU_index);
                                            mePt_BUMP_->Fill(trackGen.pt());
                                            meTOF_BUMP_->Fill(TOF_mPi);
                                            mePathL_BUMP_->Fill(pathL);

                                            if(Debug_){
                                            std::cout << "------- EVENT OF INTEREST: ----------" << "\n"
                                                      << "Event: "<< iEvent.id().event()<< "\n"
                                                      << "TRACK ETA: " << trackGen.eta() << "\n"
                                                      << "HIT ETA: " << global_point_rechit.eta() << "\n"
                                                      << "TOF: " << TOF_mPi << "\n"
                                                      << "PATH LENGHT: " << pathL << "\n"
                                                      << "Z position hit: " << global_point_rechit.z()<< "\n"
                                                      << "------------------------------------"<< std::endl;
                                            }
                                    }//test
	    
				
				    ////////////////////////////
				    //   Pt track selection   //
			    	    ////////////////////////////
				
				    if (trackGen.pt() < 1.) continue;
				


				    // HISTOGRAMS

                                    //Global plots:
                                    meHitTime_->Fill(recHit.time());
                                    meHitTimeCorr_->Fill(hit_time_corr);
                                    meHitEne_->Fill(recHit.energy());
		        	    meHitEnevsTimeCorr_->Fill(recHit.energy(), hit_time_corr);
				    meTrackPt_->Fill(trackGen.pt());
				    meTrackP_->Fill(trackGen.p());

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
                                    
			    }//Loop recHit collection
                            
		    }//Loop hit in cluster

	    }//If rechit in track is in BTL
  
    }//Loop recHit in Track

    meTrackNumHits_->Fill(numMTDBtlvalidhits);
    meNumbBTLRecHits_->Fill(numBTLRecHits);
    meNumberCluPerTrack_->Fill(nCLuPerTrack); 

    }//trkRecSel
  
  }//fine loop sulle recoGenTrack
std::cout << " ---> Event = " << iEvent.id().event() << "PRE FILTER TRACKS: " << nTotal << " ----> AFTER FILTER PV: " << nPassFilter << std::endl;
 


}//analyzer()

//---------------------------------------------------------------------------------



// ------------ method for histogram booking ------------
void BtlTimeMonitoringWTracks::bookHistograms(DQMStore::IBooker& ibook,
                                            edm::Run const& run,
                                            edm::EventSetup const& iSetup) {
  ibook.setCurrentFolder(folder_);

  // --- histograms booking

 meTracktmtd_ = ibook.book1D("Tracktmtd", "Track time from TrackExtenderWithMTD;tmtd [ns]", 150, 1, 16); 
 meBTLTrackEtaTot_ = ibook.book1D("TrackBTLEtaTot", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiTot_ = ibook.book1D("TrackBTLPhiTot", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtTot_ = ibook.book1D("TrackBTLPtTot", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10); 
 meBTLTrackPTot_ = ibook.book1D("TrackBTLPTot", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);

 meBTLTrackEtaTotMatchedTP_ = ibook.book1D("TrackBTLEtaTotMatchedTP", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiTotMatchedTP_ = ibook.book1D("TrackBTLPhiTotMatchedTP", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtTotMatchedTP_ = ibook.book1D("TrackBTLPtTotMatchedTP", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPTotMatchedTP_ = ibook.book1D("TrackBTLPTotMatchedTP", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
	 
 meTrackNumHits_ = ibook.book1D("TrackNumHits", "Number of valid MTD hits per track ; Number of hits", 20, -0.5, 19.5);
 meNumbBTLRecHits_ = ibook.book1D("NumbBTLRecHits", "Number of valid MTD hits per track ; Number of hits", 20, -0.5, 19.5);
 meNumberCluPerTrack_ = ibook.book1D("NumberCluPerTrack", "Number of Cluster per Track; Number of Cluster", 20, -0.5, 19.5);

 meCluster_time_ = ibook.book1D("BtlCluster_time","BTL Cluster ToA;ToA_{RECO} [ns]", 100, 0., 25.);
 meCluster_timePtCut_ = ibook.book1D("BtlCluster_timePtCut","BTL Cluster ToA;ToA_{RECO} [ns]", 100, 0., 25.);

 meBTLTrackEtaBTLHit_ = ibook.book1D("TrackBTLEtaBTLHit", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiBTLHit_ = ibook.book1D("TrackBTLPhiBTLHit", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtBTLHit_ = ibook.book1D("TrackBTLPtBTLHit", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHit_ = ibook.book1D("TrackBTLPBTLHit", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 1000);

 meBTLTrackEtaBTLHitMatchedTP_ = ibook.book1D("TrackBTLEtaBTLHitMatchedTP", "Eta of tracks (Tot);#eta_{RECO}", 30, 0., 1.5);
 meBTLTrackPhiBTLHitMatchedTP_ = ibook.book1D("TrackBTLPhiBTLHitMatchedTP", "Phi of tracks (Tot);#phi_{RECO} [rad]", 100, -3.2, 3.2);
 meBTLTrackPtBTLHitMatchedTP_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_ = ibook.book1D("TrackBTLPBTLHitMatchedTP", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 1000);

 meBTLTrackPtBTLHitMatchedTP_Pi_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP_Pi", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_Pi_ = ibook.book1D("TrackBTLPBTLHitMatchedTP_Pi", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
 meBTLTrackPtBTLHitMatchedTP_K_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP_K", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_K_ = ibook.book1D("TrackBTLPBTLHitMatchedTP_K", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);
 meBTLTrackPtBTLHitMatchedTP_p_ = ibook.book1D("TrackBTLPtBTLHitMatchedTP_p", "Pt of tracks (Tot);pt_{RECO} [GeV]", 50, 0, 10);
 meBTLTrackPBTLHitMatchedTP_p_ = ibook.book1D("TrackBTLPBTLHitMatchedTP_p", "P of tracks (Tot);p_{RECO} [GeV]", 250, 0, 500);

  meHitTime_ = ibook.book1D("BtlHitTime","BTL recHit ToA;ToA_{RECO} [ns]", 100, 0., 25.);
  meHitTimeCorr_ = ibook.book1D("BtlHitTimeCorr", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -3., 18.);
  meTrackPt_ = ibook.book1D("BtlTrackPt", "Pt of tracks (BTL) ;pt_{RECO} [GeV]", 100, 0, 20);
  meTrackP_ = ibook.book1D("BtlTrackP", "P of tracks (BTL) ;p_{RECO} [GeV]", 250, 0, 500);
  meHitTimeCorr_NO_PTcut_ = ibook.book1D("BtlHitTimeCorr_NO_PTcut_", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -3., 18.);

  meTrackEta_BUMP_ = ibook.book1D("BtlTrackEta_BUMP", "#eta of the track matched with a TP as Pion; #eta;Entries", 100, -1.55, 1.55);
  meHitEta_BUMP_ = ibook.book1D("BtlHitEta_BUMP", "#eta of the recHits matched with a TP as Pion;#eta;Entries", 100, -1.55, 1.55);  
  meHitRU_index_BUMP_ = ibook.book1D("BtlHitRU_index_BUMP", "Ru index Rechit Bump;RU index;Entries", 15, 0., 15.);
  mePt_BUMP_ = ibook.book1D("BtlPt_BUMP", "Pt of tracks (BTL) - Bump case ;pt_{RECO} [GeV]", 100, 0, 20);
  meTOF_BUMP_ = ibook.book1D("BtlTOF_BUMP_","TOF under the Pion mass hyp;TOF [ns]", 500, 0., 25.);
  mePathL_BUMP_ = ibook.book1D("BtlPathL_BUMP_","Path Length;Path L [cm]", 500, 0., 1000.);

  meHitTimeCorrMatchedTP_ = ibook.book1D("BtlHitTimeCorrMatchedTP_", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorrMatchedTP_Pi_ = ibook.book1D("BtlHitTimeCorrMatchedTP_Pi", "Time of recHit with TOF correction from track(Pi mass) - Only Pi particles;Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorrMatchedTP_K_ = ibook.book1D("BtlHitTimeCorrMatchedTP_K", "Time of recHit with TOF correction from track(Pi mass) - Only K particles;Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorrMatchedTP_p_ = ibook.book1D("BtlHitTimeCorrMatchedTP_p", "Time of recHit with TOF correction from track(Pi mass) - Only p particles;Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorrMatchedTP_mu_ = ibook.book1D("BtlHitTimeCorrMatchedTP_mu", "Time of recHit with TOF correction from track(Pi mass) - Only mu particles;Time [ns];Entries", 1000, -3., 18.);

  
  meHitEne_ = ibook.book1D("BtlHitEne", "recHit Energy;Hit Energy [MeV];Entries", 150, -10., 30.);
  meClusterSize_ = ibook.book1D("BtlClusterSize", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSize_zone1_ = ibook.book1D("BtlClusterSize_zone1", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5); 
  meClusterSize_zone2_ = ibook.book1D("BtlClusterSize_zone2", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSize_zone3_ = ibook.book1D("BtlClusterSize_zone3", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSize_zone4_ = ibook.book1D("BtlClusterSize_zone4", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSize_vs_TrackEta_ = ibook.bookProfile("BtlClusterSize_vs_TrackEta","Track |#eta| vs Cluster size ;|#eta| Reco Track;<Cluster Size>", 25, 0., 1.5, 0., 20.);

  meClusterSizePtCut_ = ibook.book1D("BtlClusterSizePtCut", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSizePtCut_zone1_ = ibook.book1D("BtlClusterSizePtCut_zone1", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSizePtCut_zone2_ = ibook.book1D("BtlClusterSizePtCut_zone2", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSizePtCut_zone3_ = ibook.book1D("BtlClusterSizePtCut_zone3", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSizePtCut_zone4_ = ibook.book1D("BtlClusterSizePtCut_zone4", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  meClusterSizePtCut_vs_TrackEta_ = ibook.bookProfile("BtlClusterSize_vs_TrackEta","Track |#eta| vs Cluster size ;|#eta| Reco Track;<Cluster Size>", 25, 0., 1.5, 0., 20.);  
 
  meHitEnevsTimeCorr_ = ibook.bookProfile("BtlHitEnevsTimeCorr_","Hit Energy vs corrected time;Hit energy [MeV];Time [ns]", 150, 0., 20., -10., 20.);

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


    
  
}



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
