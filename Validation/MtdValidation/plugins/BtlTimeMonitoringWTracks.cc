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
 // ------------ member data ------------

  const std::string folder_;
  const double hitMinEnergy_;
  const bool optionalPlots_;
  const bool uncalibRecHitsPlots_;
  const double hitMinAmplitude_;
  const reco::RecoToSimCollection* r2s_;

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

  edm::EDGetTokenT<edm::ValueMap<float>> pathLengthToken_;
  edm::EDGetTokenT<TrackingParticleCollection> trackingParticleCollectionToken_;
  //edm::EDGetTokenT<reco::SimToRecoCollection> simToRecoAssociationToken_;
  edm::EDGetTokenT<reco::RecoToSimCollection> recoToSimAssociationToken_;
  //edm::EDGetTokenT<reco::TPToSimCollectionMtd> tp2SimAssociationMapToken_;
  //edm::EDGetTokenT<reco::SimToTPCollectionMtd> Sim2tpAssociationMapToken_;
  // --- histograms declaration

  static constexpr int nTR_ = 36;
  static constexpr int nRU_ = 6;
  static constexpr int nSM_ = 8;
  static constexpr int nCR_ = 128;
  static constexpr int nRU_tot_ = 12;
  static constexpr int nSMphi_ = 96;
  static constexpr int nSMinRU_ = 24;
  static constexpr double MinRecoHitEnergy_ = 1.7;
  static constexpr double MaxRecoHitEnergy_ = 16.;
  static constexpr double MaxRecoHitTime_ = 19.;
  static constexpr double MinRecoHitTime_ = -2.;
  static constexpr double trackMaxBtlEta_ = 1.5;
  static constexpr double trackCentralBtlEta_ = 0.65;

  static constexpr double etacutGEN_ = 4.;
  static constexpr double pTcutBTL_ = 0.5;//defalutl 0.7
  static constexpr double rBTL_ = 110.0;
  static constexpr double zETL_ = 290.0;
  static constexpr bool Debug_ = false;
  // --- Histograms

  MonitorElement* meHitTime_;
  MonitorElement* meHitTimeCorr_;
  MonitorElement* meHitTimeRUSlice_[nRU_tot_];
  MonitorElement* meHitTimeRUSlice_corr_[nRU_tot_];
  MonitorElement* meHitTime_SingleRU_[nRU_tot_][nTR_];
  MonitorElement* meHitTimePhiSlice_[nSMphi_];
  MonitorElement* meHitEne_;
  MonitorElement* meHitRadiusBTL_;
  MonitorElement* meNumberTrackGen_;
  MonitorElement* meNumberTrackBTL_;
  MonitorElement* meNumberCluperTrack_;
  MonitorElement* meNumberClusterBTL_;
  MonitorElement* meCluSize_postSel_;
  MonitorElement* meNumberRecHit_preSel_;
  MonitorElement* meNumberRecHit_postSel_;
  MonitorElement* meNumberRecHit_postPtcut_;

  MonitorElement* meHitTimeSingleSmInRu_[nRU_tot_][nSMinRU_];
  MonitorElement* meHitTimeCrystalRing_RU1_;
  MonitorElement* meHitTimeCrystalRing_RU2_;
  MonitorElement* meHitTimeCrystalRing_RU3_;
  MonitorElement* meHitTimeCrystalRing_RU4_;
  MonitorElement* meHitTimeCrystalRing_RU5_;
  MonitorElement* meHitTimeCrystalRing_RU6_;
  MonitorElement* meHitTimeCrystalRing_RU7_;
  MonitorElement* meHitTimeCrystalRing_RU8_;
  MonitorElement* meHitTimeCrystalRing_RU9_;
  MonitorElement* meHitTimeCrystalRing_RU10_;
  MonitorElement* meHitTimeCrystalRing_RU11_;
  MonitorElement* meHitTimeCrystalRing_RU12_;
  
  MonitorElement* meHitTimeCorr_Pi_;
  MonitorElement* meHitTimeCorr_K_;
  MonitorElement* meHitTimeCorr_p_;
  MonitorElement* meHitTimeCorr_mu_;

  MonitorElement* meClusterSize_;
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
  MonitorElement* meCluSize_BUMP_;
  MonitorElement* meTrackPt_;
  MonitorElement* meHitTimeCorrCUT_PT_;
  MonitorElement* meTOF_BUMP_;
  MonitorElement* mePathL_BUMP_;

  MonitorElement* meTOF_Tracks_;
  MonitorElement* meEta_Track_TOFzero_;
  MonitorElement* meTOFvsETA_Tracks_;

 MonitorElement* meCluster_time_;
 MonitorElement* meTrackGen_eta_;
 MonitorElement* meTrackGenBTL_eta_;
 MonitorElement* meTrackExtdBTL_eta_; 
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

   //WTracks:
   GenRecTrackToken_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("inputTagG"));
   RecTrackToken_ = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("inputTagT"));
   trackAssocToken_ = consumes<edm::ValueMap<int>>(iConfig.getParameter<edm::InputTag>("trackAssocSrc"));
   TofPiToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("TofPi"));
   trackingParticleCollectionToken_ = consumes<TrackingParticleCollection>(iConfig.getParameter<edm::InputTag>("SimTag"));
  //simToRecoAssociationToken_ = consumes<reco::SimToRecoCollection>(iConfig.getParameter<edm::InputTag>("TPtoRecoTrackAssoc"));
  recoToSimAssociationToken_ = consumes<reco::RecoToSimCollection>(iConfig.getParameter<edm::InputTag>("TPtoRecoTrackAssoc"));
  //tp2SimAssociationMapToken_ = consumes<reco::TPToSimCollectionMtd>(iConfig.getParameter<edm::InputTag>("tp2SimAssociationMapTag"));
  //Sim2tpAssociationMapToken_ = consumes<reco::SimToTPCollectionMtd>(iConfig.getParameter<edm::InputTag>("Sim2tpAssociationMapTag"));  
  pathLengthToken_ = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("pathLengthSrc"));    
      
      
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

  // Recupera il BeamSpot--------------------------------------------------------
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  iEvent.getByToken(beamSpotToken_, beamSpotHandle);

  if (!beamSpotHandle.isValid()) {
    throw cms::Exception("BtlTimeMonitoringWTracks") << "BeamSpot is not available in the event!";
  }
  const reco::BeamSpot& beamSpot = *beamSpotHandle;

  auto btlRecHitsHandle = makeValid(iEvent.getHandle(btlRecHitsToken_));
  auto btlSimHitsHandle = makeValid(iEvent.getHandle(btlSimHitsToken_));
  auto btlRecCluHandle = makeValid(iEvent.getHandle(btlRecCluToken_));
  auto mtdTrkHitHandle = makeValid(iEvent.getHandle(mtdTrackingHitToken_));
  //const auto& r2sAssociationMap = iEvent.get(r2sAssociationMapToken_);
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
  //const auto& tp2SimAssociationMap = iEvent.get(tp2SimAssociationMapToken_);
  //const auto& Sim2tpAssociationMap = iEvent.get(Sim2tpAssociationMapToken_);

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

    }

  }  // simHit loop

//---------------------------------------------------------------------


//TENTATIVO LINK: loop track->trovo cluster->loop sui rechit del cluster 
// --- Loop over all RECO tracks ---

  unsigned int index = 0;
  unsigned int nTracks_BTL = 0;
  unsigned int nTracks_BTL_extended = 0;
  unsigned int nCLuPerEvt = 0;
  unsigned int nRecHit_preSel_evt = 0;
  unsigned int nRecHit_postSel_evt = 0;
  unsigned int nRecHit_postPtcut_evt = 0;
  std::set<edm::Ref<FTLClusterCollection, FTLCluster>> uniqueClusters;
  
  for (const auto& trackGen : *GenRecTrackHandle) {
    const reco::TrackRef trackref(iEvent.getHandle(GenRecTrackToken_), index);
    index++;

    meTrackGen_eta_->Fill(trackGen.eta());

    //CONTROLLO:
    if (std::abs(trackGen.eta()) >= trackMaxBtlEta_) {
	    nTracks_BTL++;
	    meTrackGenBTL_eta_->Fill(trackGen.eta());
    }


    if (trackAssoc[trackref] == -1) {
      LogWarning("mtdTracks") << "Extended track not associated";
      continue;
    }


    //CONTROLLO:
    if (std::abs(trackGen.eta()) >= trackMaxBtlEta_) {
            nTracks_BTL_extended++;
	    meTrackExtdBTL_eta_->Fill(trackGen.eta());
    }
    

    const reco::TrackRef mtdTrackref = reco::TrackRef(iEvent.getHandle(RecTrackToken_), trackAssoc[trackref]);
    const reco::Track& track = *mtdTrackref;

    double tof_pi = TofPi[trackref];
    double pathL = pathLength[trackref];


    meTOF_Tracks_->Fill(tof_pi);
    
    if (tof_pi == 0) {
	    meEta_Track_TOFzero_->Fill(trackGen.eta());
    }

    meTOFvsETA_Tracks_->Fill(trackGen.eta(),tof_pi);
/*
    //Matching with TrackingParticle:
    //const reco::TrackBaseRef trkrefb(trackref);
    //auto tp_info = getMatchedTP(trkrefb);
    //if (tp_info != nullptr && trkTPSelAll(**tp_info)) {

    //DEBUG:
    if(Debug_){
	    if (tof_pi == 0. && pathL < 1.) {
		    std::cout << "VALID TP matched, but ToF&pathLength still zero! idx="<<index << "\n"
			    << " pdg="<<(*tp_info)->pdgId() << "\n"
			    << " tp.pt="<<(*tp_info)->pt()<< std::endl;
	    }
    }

    bool flag_Pi = false;
    bool flag_K  = false;
    bool flag_p  = false;
    bool flag_mu  = false;

    int pdg = std::abs((*tp_info)->pdgId());
    if (pdg == 211) {
	    flag_Pi = true;
    } else if (pdg == 321) {
	    flag_K = true;
    } else if (pdg == 2212) {
	    flag_p = true;
    } else if (pdg == 13) {
	    flag_mu = true;
    }
  
*/    
    unsigned int numbClu_perTrack = 0;

    //Tracks in BTL region:
    if (std::abs(trackGen.eta()) >= trackMaxBtlEta_) continue;

    //Loop in the track rechits:    
    for (const auto hit : track.recHits()) {

	    if (hit->isValid() == false)        
    		    continue;

	    MTDDetId Hit = hit->geographicalId();

	    //RecHit in BTL subDetector:
	    if ((Hit.det() == 6) && (Hit.subdetId() == 1) && (Hit.mtdSubDetector() == 1)) {
            
    		    const auto* mtdhit = static_cast<const MTDTrackingRecHit*>(hit);
    
		    if (!mtdhit) continue;
           
	 	    const auto& hitCluster = mtdhit->mtdCluster();

		    //Ref to cluster			                     
		    auto recoClusterRef = edmNew::makeRefTo(btlRecCluHandle, &hitCluster);

		    // Only in not considered before
		    if (!uniqueClusters.insert(recoClusterRef).second) continue;

		    //CONTROLLO:
		    numbClu_perTrack++;
		    unsigned int nHitCluster_postSel = 0;
		    meCluster_time_->Fill(hitCluster.time());

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

			    meClusterSize_->Fill(hitCluster.size());

			    //CONTROLLO:
			    nCLuPerEvt++;

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


					    //CONTROLLO:
					    nRecHit_preSel_evt++;

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
                                            if (recHit.energy() > MaxRecoHitEnergy_)
                                                    continue;
                                            if (recHit.energy() < MinRecoHitEnergy_)
                                                    continue;
                                            //Time cut for the saturation peak:
                                            if (recHit.time()>MaxRecoHitTime_)
                                                    continue;

					    //CONTROLLO:
                                            nRecHit_postSel_evt++;
					    nHitCluster_postSel++;

                                            //TOF:
                                            double TOF_mPi = tof_pi;

                                            //Hit Time-Stamp correction:
                                            double hit_time_corr = recHit.time() - TOF_mPi;

                                            //Time cut on RecoHitTime_corr:
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

					    float HitRadiusBTL = sqrt(pow(global_point_rechit.x(),2) + pow(global_point_rechit.y(),2));
					    //----- HISTOGRAMS ----
					    

					    //Pt track selection:
                                            if (trackGen.pt() < 1.)
                                                    continue;


					    //CONTROLLO:
                                            nRecHit_postPtcut_evt++;


                                            //Global plots:
                                            meHitTime_->Fill(recHit.time());
                                            meHitTimeCorr_->Fill(hit_time_corr);
                                            meHitEne_->Fill(recHit.energy());
					    meHitEnevsTimeCorr_->Fill(recHit.energy(), hit_time_corr);
					    meHitRadiusBTL_->Fill(HitRadiusBTL);
					    meTrackPt_->Fill(trackGen.pt());


					    //studio Bump:
					    if(hit_time_corr > 8.4 && hit_time_corr < 11.) {

						    meTrackEta_BUMP_->Fill(trackGen.eta());
						    meHitEta_BUMP_->Fill(global_point_rechit.eta());
						    meHitRU_index_BUMP_->Fill(RU_index);
						    mePt_BUMP_->Fill(trackGen.pt());
						    meTOF_BUMP_->Fill(TOF_mPi);
						    mePathL_BUMP_->Fill(pathL);
						    //meCluSize_BUMP_->Fill(hitCluster.size());
						   
						   if(Debug_){
						     std::cout << "-------   EVENT OF INTEREST: ----------" << "\n"
                                                                    << "Event: "<< iEvent.id().event()<< "\n"
								    << "TRACK ETA: " << trackGen.eta() << "\n"
								    << "HIT ETA: " << global_point_rechit.eta() << "\n"
								    << "TOF: " << TOF_mPi << "\n"
								    << "PATH LENGHT: " << pathL << "\n"
								    << "Z position hit: " << global_point_rechit.z()<< "\n"
                                                                    << "------------------------------------"<< std::endl;
						   }

					    }
/*
					    //Global corrected time per particle PDG type:
					    if(flag_Pi){
						    meHitTimeCorr_Pi_->Fill(hit_time_corr);
						    
						    if(hit_time_corr > 8.4 && hit_time_corr < 11.) {
							    meTrackEta_Pi_BUMP_->Fill(trackGen.eta());
							    meHitEta_Pi_BUMP_->Fill(global_point_rechit.eta());
							    meNum_Hit_Pi_BUMP_->Fill(hitCluster.size());
							    mePt_Pi_BUMP_->Fill(trackGen.pt());
							    meCluTime_Pi_BUMP_->Fill(hitCluster.time());
							    //meSimHitType_Pi_BUMP_->Fill(type_offset);

						    }
					    }
					    if(flag_K){
                                                    meHitTimeCorr_K_->Fill(hit_time_corr);
                                            }
					    if(flag_p){
                                                    meHitTimeCorr_p_->Fill(hit_time_corr);
                                            }

					    if(flag_mu){
                                                    meHitTimeCorr_mu_->Fill(hit_time_corr);
                                            }
*/

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
                                            if(crystal_index == 65) {
                                                    meHitTimeCrystalRing_RU1_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 193) {
                                                   meHitTimeCrystalRing_RU2_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 321) {
                                                    meHitTimeCrystalRing_RU3_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 448) {
                                                    meHitTimeCrystalRing_RU4_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 577) {
                                                    meHitTimeCrystalRing_RU5_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 705) {
                                                    meHitTimeCrystalRing_RU6_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 833) {
                                                    meHitTimeCrystalRing_RU7_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 961) {
                                                    meHitTimeCrystalRing_RU8_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 1089) {
                                                    meHitTimeCrystalRing_RU9_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 1217) {
                                                    meHitTimeCrystalRing_RU10_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 1345) {
                                                    meHitTimeCrystalRing_RU11_->Fill(hit_time_corr);
                                            }
                                            if(crystal_index == 1473) {
                                                    meHitTimeCrystalRing_RU12_->Fill(hit_time_corr);
                                            }


                                    }//Loop recHit collection

                            }//Loop hit in cluster

			    meCluSize_postSel_->Fill(nHitCluster_postSel);

                    }//If rechit in track is in BTL


            }//Loop recHit in Track

     //}//TP matching

    meNumberCluperTrack_->Fill(numbClu_perTrack);
  }//fine loop sulle recoGenTrack

  meNumberTrackGen_->Fill(nTracks_BTL);
  meNumberTrackBTL_->Fill(nTracks_BTL_extended);
  meNumberClusterBTL_->Fill(nCLuPerEvt);
  meNumberRecHit_preSel_->Fill(nRecHit_preSel_evt);
  meNumberRecHit_postSel_->Fill(nRecHit_postSel_evt);
  meNumberRecHit_postPtcut_->Fill(nRecHit_postPtcut_evt);
//--------------------------------------------------------------------------------------------------------------

}//analyzer()

//---------------------------------------------------------------------------------



// ------------ method for histogram booking ------------
void BtlTimeMonitoringWTracks::bookHistograms(DQMStore::IBooker& ibook,
                                            edm::Run const& run,
                                            edm::EventSetup const& iSetup) {
  ibook.setCurrentFolder(folder_);

  // --- histograms booking
 

  meNumberTrackGen_ = ibook.book1D("BtlNumberTrackGen","Number of Track (genTrack) per event;Number of Track per event", 500, 0., 10000.); 
  meNumberTrackBTL_ = ibook.book1D("BtlNumberTrackBTL","Number of Track extended in BTL per event;Number of Track per event", 500, 0., 10000.);
  meNumberCluperTrack_ = ibook.book1D("BtlNumberCluperTrack","Number of Cluster per Track;Number of Cluster per Track", 20, 0.5, 20.5);
  meNumberClusterBTL_ = ibook.book1D("BtlNumberClusterBTL","Number of Cluster per event;Number of Cluster per event", 500, 0., 10000.);
  meCluSize_postSel_ = ibook.book1D("BtlCluSize_postSel","Cluster size - post selection recHit;Cluster size", 20, 0.5, 20.5);
  meNumberRecHit_preSel_ = ibook.book1D("BtlNumberRecHit_preSel","Number of RecHit - pre selection recHit;Number of RecHit", 500, 0., 10000.);
  meNumberRecHit_postSel_ = ibook.book1D("BtlNumberRecHit_postSel","Number of RecHit - post selection recHit;Number of RecHit", 500, 0., 10000.);
  meNumberRecHit_postPtcut_ = ibook.book1D("BtlNumberRecHit_postPtcut","Number of RecHit - post selection Pt cut;Number of RecHit", 500, 0., 10000.);
  meCluster_time_ = ibook.book1D("BtlCluster_time","BTL Cluster ToA;ToA_{RECO} [ns]", 100, 0., 25.);
  meTrackGen_eta_ = ibook.book1D("BtlTrackGen_eta", "#eta of the TrackGen;#eta;Entries", 100, -1.55, 1.55);
  meTrackGenBTL_eta_ = ibook.book1D("BtlTrackGenBTL_eta", "#eta of the TrackGen in BTL region;#eta;Entries", 100, -1.55, 1.55);
  meTrackExtdBTL_eta_ = ibook.book1D("BtlTrackExtdBTL_eta", "#eta of the Track Extended in BTL;#eta;Entries", 100, -1.55, 1.55);

  meHitTime_ = ibook.book1D("BtlHitTime","BTL recHit ToA;ToA_{RECO} [ns]", 100, 0., 25.);
  meHitTimeCorr_ = ibook.book1D("BtlHitTimeCorr", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -3., 18.);
  meHitRadiusBTL_ = ibook.book1D("BtlHitRadiusBTL","BTL recHit Radius ;#rho [cm]", 600, 0., 150.);
  meTrackPt_ = ibook.book1D("BtlTrackPt", "Pt of tracks (BTL) ;pt_{RECO} [GeV]", 100, 0, 20);
  meHitTimeCorrCUT_PT_ = ibook.book1D("BtlHitTimeCorrCUTPT", "Time of recHit with TOF correction from track(Pi mass);Time [ns];Entries", 1000, -3., 18.);

  meTOFvsETA_Tracks_ = ibook.bookProfile(
    "BtlTOFvsETA_Tracks",
    "TOF (Pion mass) vs #eta of the reco track;#eta;TOF (#pi mass) [ns]",
    100, -1.55, 1.55, -1., 25.);

  meTOF_Tracks_ = ibook.book1D("BtlTOF_Tracks","TOF under the Pion mass hyp;TOF [ns]", 500, 0., 25.);
  meEta_Track_TOFzero_ = ibook.book1D("BtlEta_Track_TOFzero", "#eta of the reco track with TOF == 0 ; #eta;Entries", 100, -1.55, 1.55);

  meTrackEta_BUMP_ = ibook.book1D("BtlTrackEta_BUMP", "#eta of the track matched with a TP as Pion; #eta;Entries", 100, -1.55, 1.55);
  meHitEta_BUMP_ = ibook.book1D("BtlHitEta_BUMP", "#eta of the recHits matched with a TP as Pion;#eta;Entries", 100, -1.55, 1.55);  
  meHitRU_index_BUMP_ = ibook.book1D("BtlHitRU_index_BUMP", "Ru index Rechit Bump;RU index;Entries", 15, 0., 15.);
  mePt_BUMP_ = ibook.book1D("BtlPt_BUMP", "Pt of tracks (BTL) - Bump case ;pt_{RECO} [GeV]", 100, 0, 20);
  meCluSize_BUMP_ = ibook.book1D("BtlCluSize_BUMP", "Number of Hits per cluster related to a bump; Number of Hits;Entries", 30, 0, 30);
  meTOF_BUMP_ = ibook.book1D("BtlTOF_BUMP_","TOF under the Pion mass hyp;TOF [ns]", 500, 0., 25.);
  mePathL_BUMP_ = ibook.book1D("BtlPathL_BUMP_","Path Length;Path L [cm]", 500, 0., 1000.);

  meHitEta_Pi_BUMP_ = ibook.book1D("BtlHitEta_Pi_BUMP", "#eta of the recHits matched with a TP as Pion;#eta;Entries", 100, -1.55, 1.55);
  meTrackEta_Pi_BUMP_ = ibook.book1D("BtlTrackEta_Pi_BUMP", "#eta of the track matched with a TP as Pion; #eta;Entries", 100, -1.55, 1.55);
  meNum_Hit_Pi_BUMP_ = ibook.book1D("BtlNum_Hit_Pi_BUMP", "Number of Hits per cluster related to a bump; Number of Hits;Entries", 30, 0, 30); 
  mePt_Pi_BUMP_ = ibook.book1D("BtlPt_Pi_BUMP", "Pt of tracks (BTL) - Bump case ;pt_{RECO} [GeV]", 100, 0, 20); 
  meCluTime_Pi_BUMP_ = ibook.book1D("BtlCluTime_Pi_BUMP","BTL Reco Cluster ToA;ToA_{Cluster} [ns]", 1000, 0., 25.);
  meSimHitType_Pi_BUMP_ = ibook.book1D("BtlSimHitType_Pi_BUMP","BTL simHit typeOffset;SimHit TypeOffset;Entries", 6, -0.5, 5.5);

  meHitTimeCorr_Pi_ = ibook.book1D("BtlHitTimeCorr_Pi", "Time of recHit with TOF correction from track(Pi mass) - Only Pi particles;Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorr_K_ = ibook.book1D("BtlHitTimeCorr_K", "Time of recHit with TOF correction from track(Pi mass) - Only K particles;Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorr_p_ = ibook.book1D("BtlHitTimeCorr_p", "Time of recHit with TOF correction from track(Pi mass) - Only p particles;Time [ns];Entries", 1000, -3., 18.);
  meHitTimeCorr_mu_ = ibook.book1D("BtlHitTimeCorr_mu", "Time of recHit with TOF correction from track(Pi mass) - Only mu particles;Time [ns];Entries", 1000, -3., 18.);


  meHitEne_ = ibook.book1D("BtlHitEne", "recHit Energy;Hit Energy [MeV];Entries", 150, -10., 30.);
  meClusterSize_ = ibook.book1D("ClusterSize", "Cluster Size associated to a reco track;Cluster Size;Entries", 20, -0.5, 19.5);
  
  meHitEnevsTimeCorr_ = ibook.bookProfile(
    "BtlHitEnevsTimeCorr_",
    "Hit Energy vs corrected time;Hit energy [MeV];Time [ns]",
    150, 0., 20., -10., 20.);


  meHitTimeCrystalRing_RU1_ = ibook.book1D("BtlHitTimeCrystalRing_RU1", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU2_ = ibook.book1D("BtlHitTimeCrystalRing_RU2", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU3_ = ibook.book1D("BtlHitTimeCrystalRing_RU3", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU4_ = ibook.book1D("BtlHitTimeCrystalRing_RU4", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU5_ = ibook.book1D("BtlHitTimeCrystalRing_RU5", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU6_ = ibook.book1D("BtlHitTimeCrystalRing_RU6", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU7_ = ibook.book1D("BtlHitTimeCrystalRing_RU7", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU8_ = ibook.book1D("BtlHitTimeCrystalRing_RU8", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU9_ = ibook.book1D("BtlHitTimeCrystalRing_RU9", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU10_ = ibook.book1D("BtlHitTimeCrystalRing_RU10", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU11_ = ibook.book1D("BtlHitTimeCrystalRing_RU11", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);
  meHitTimeCrystalRing_RU12_ = ibook.book1D("BtlHitTimeCrystalRing_RU12", "Corrected time (with TOF) per crystal ring;Time [ns];Entries", 1000, -5., 25.);


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
  desc.add<edm::InputTag>("trackAssocSrc", edm::InputTag("trackExtenderWithMTD:generalTrackassoc"));
  desc.add<edm::InputTag>("TPtoRecoTrackAssoc", edm::InputTag("trackingParticleRecoTrackAsssociation"));
  //desc.add<edm::InputTag>("tp2SimAssociationMapTag", edm::InputTag("mtdSimLayerClusterToTPAssociation"));
  //desc.add<edm::InputTag>("Sim2tpAssociationMapTag", edm::InputTag("mtdSimLayerClusterToTPAssociation"));
  desc.add<edm::InputTag>("pathLengthSrc", edm::InputTag("trackExtenderWithMTD:generalTrackPathLength"));
  desc.add<edm::InputTag>("SimTag", edm::InputTag("mix", "MergedTrackTruth"));
  desc.add<double>("HitMinimumEnergy", 1.);  // [MeV]
  desc.add<bool>("optionalPlots", false);
  desc.add<bool>("UncalibRecHitsPlots", false);
  desc.add<double>("HitMinimumAmplitude", 30.);  // [pC]

  descriptions.add("btlTimeMonitoringWTracks", desc);
}



DEFINE_FWK_MODULE(BtlTimeMonitoringWTracks);
