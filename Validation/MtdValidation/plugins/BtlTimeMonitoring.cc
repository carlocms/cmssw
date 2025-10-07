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
  //edm::EDGetTokenT<MtdRecoClusterToSimLayerClusterAssociationMap> r2sAssociationMapToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  
  //--------  
  edm::EDGetTokenT<std::vector<reco::GenParticle>> genParticlesToken_;
  edm::EDGetTokenT<edm::SimTrackContainer> simTracksToken_;
  edm::EDGetTokenT<edm::SimVertexContainer> simVerticesToken_;
  //-------

  const edm::ESGetToken<MTDGeometry, MTDDigiGeometryRecord> mtdgeoToken_;
  const edm::ESGetToken<MTDTopology, MTDTopologyRcd> mtdtopoToken_;
  //const edm::ESGetToken<MTDClusterParameterEstimator, MTDCPERecord> cpeToken_;

  // --- histograms declaration

  MonitorElement* meNevents_;

  MonitorElement* meNhits_;

  MonitorElement* meHitEnergy_;
  MonitorElement* meHitLogEnergy_;
  MonitorElement* meHitTime_;
  MonitorElement* meHitTimeError_;

  MonitorElement* meOccupancy_;

  // --- Useful parameter (now hardcoded...)
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


  // --- local position monitoring
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



  // --- Gen Particle histograms
  MonitorElement* meGenPt_;
  MonitorElement* meGenEne_;
  MonitorElement* meGenEta_;
  MonitorElement* meGenTheta_;
  MonitorElement* meGenPhi_;
  MonitorElement* meGenPdg_;


  // --- UncalibratedRecHits histograms
  static constexpr double c_cm_ns = geant_units::operators::convertMmToCm(CLHEP::c_light);  // [mm/ns] -> [cm/ns]
  static constexpr double calib_EneRecoHit_ = 0.03125;
  static constexpr double hitMaxTime_= 19.; //default 19.
  static constexpr double hitMaxAmplitude_= 450.;
  static constexpr double hitMinimumAmplitude_ = 60.; //60. default
  static constexpr double tof_internal_ = 2.85 / 13.846235;
  static constexpr double p0_ = 2.21103;
  static constexpr double p1_ = -0.933552;
  static constexpr double simUnit_ = 1e9;
  static constexpr std::array<int, 12> ringStartIndices_ = {{65, 193, 321, 448, 577, 705, 833, 961, 1089, 1217, 1345, 1473}};

  // --- General plots
  MonitorElement* meUncTimeMean_;
  MonitorElement* meUncTimeMean_corr_;
  MonitorElement* meUncAmpl_global_;
  MonitorElement* meUncEne_global_;
  MonitorElement* meUncHitAmpl_vs_TimeCorr_;
  
  MonitorElement* meUncAmpl_global_noSelection_;
  MonitorElement* meUncTimeMean_noSelection_;
  MonitorElement* meUncEne_global_noSelection_;
  MonitorElement* meUncTimeMean_corr_no_selection_;

  // --- Time monitoring studies:
  MonitorElement* meUncTimeRUSlice_[nRU_tot_];
  MonitorElement* meUncTimeRUSlice_corr_[nRU_tot_];
  MonitorElement* meUncTime_SingleRU_[nRU_tot_][nTR_];
  MonitorElement* meUncTimePhiSlice_[nSMphi_]; 
  MonitorElement* meUncTimeSingleSmInRu_[nRU_tot_][nSMinRU_];
  std::array<MonitorElement*, 12> meHitTimeCrystalRing_;


  // --- Walk Time monitoring studies:
  MonitorElement* meUncTime_Left_;
  MonitorElement* meUncTime_Right_;
  MonitorElement* meUncTimeCorr_Left_;
  MonitorElement* meUncTimeCorr_Right_;

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


  // --- SimHit Matching studies:
  MonitorElement* meUncTimeMatched_Id_0_;
  MonitorElement* meUncTimeMatched_Id_1_;
  MonitorElement* meUncTimeMatched_Id_2_;
  MonitorElement* meUncTimeMatched_Id_3_;
 
  MonitorElement* meUncHitAmpl_Matched_Id_0_;
  MonitorElement* meUncHitAmpl_Matched_Id_1_;
  MonitorElement* meUncHitAmpl_Matched_Id_2_;
  MonitorElement* meUncHitAmpl_Matched_Id_3_;

  MonitorElement* meUncTimeCorrRUSlice_Matched_Id_0_[nRU_tot_];
  MonitorElement* meUncTimeCorrRUSlice_Matched_Id_1_[nRU_tot_];
  MonitorElement* meUncTimeCorrRUSlice_Matched_Id_2_[nRU_tot_];
  MonitorElement* meUncTimeCorrRUSlice_Matched_Id_3_[nRU_tot_];

  MonitorElement* meUncHitAmpl_vs_TimeCorr_Matched_Id_3_;


  // --- Test sample PI:
  MonitorElement* meSimHitTime_;
  MonitorElement* meSimHitTimeCorr_;


};


// ------------ constructor and destructor --------------
BtlTimeMonitoring::BtlTimeMonitoring(const edm::ParameterSet& iConfig)
    : folder_(iConfig.getParameter<std::string>("folder")),
      hitMinEnergy_(iConfig.getParameter<double>("HitMinimumEnergy")),
      optionalPlots_(iConfig.getParameter<bool>("optionalPlots")),
      uncalibRecHitsPlots_(iConfig.getParameter<bool>("UncalibRecHitsPlots")),
      hitMinAmplitude_(iConfig.getParameter<double>("HitMinimumAmplitude")),
      mtdgeoToken_(esConsumes<MTDGeometry, MTDDigiGeometryRecord>()),
      mtdtopoToken_(esConsumes<MTDTopology, MTDTopologyRcd>()){
      //cpeToken_(esConsumes<MTDClusterParameterEstimator, MTDCPERecord>(edm::ESInputTag("", "MTDCPEBase"))) {
  btlRecHitsToken_ = consumes<FTLRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitsTag"));
  btlUncalibRecHitsToken_ = consumes<FTLUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>("uncalibRecHitsTag"));
  btlSimHitsToken_ = consumes<CrossingFrame<PSimHit>>(iConfig.getParameter<edm::InputTag>("simHitsTag"));
  btlRecCluToken_ = consumes<FTLClusterCollection>(iConfig.getParameter<edm::InputTag>("recCluTag"));
  mtdTrackingHitToken_ = consumes<MTDTrackingDetSetVector>(iConfig.getParameter<edm::InputTag>("trkHitTag"));
  //r2sAssociationMapToken_ = consumes<MtdRecoClusterToSimLayerClusterAssociationMap>(
      //iConfig.getParameter<edm::InputTag>("r2sAssociationMapTag"));
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

  //auto const& cpe = iSetup.getData(cpeToken_);

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


//-------------------------------------------------------------------------------



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
      double hit_ene = 0.;

      double t_corr_L = 0.;  //---> Time L/R side with internal
      double t_corr_R = 0.;  //     light propagation correction 
                             
      double hit_amplitude_L = 0.; //---> Separate L/R side signal amplitude
      double hit_amplitude_R = 0.; 

      double hit_time_noTW = 0.;
      double t_L_noTW = 0.; //---> Time L/R side without TW correction
      double t_R_noTW = 0.; 

      double t_corr_L_noTW = 0.; //--->Time L/R side with internal light
      double t_corr_R_noTW = 0.; //     propagation correction and without TW correction

      double delta_time_L = 0.; //--->t_corr_L_noTW - t_corr_L
      double delta_time_R = 0.; // differece bwt without and with TW correction


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
      hit_ene = hit_amplitude*0.03125;
      hit_time_noTW /= nHits;


       if (hit_amplitude < hitMinAmplitude_)
        continue;


      //--- Global Test on Amplitude and ToA before the selection:
      meUncAmpl_global_noSelection_->Fill(hit_amplitude);
      meUncTimeMean_noSelection_->Fill(hit_time);
      meUncEne_global_noSelection_->Fill(hit_ene);
      
      //--- Hit selection: on amplitude and ToA (focus on MIP peak and exclude the saturation peak)
      if (hit_amplitude > hitMaxAmplitude_)
	      continue;
      if(hit_amplitude < hitMinimumAmplitude_)
	      continue;
      if (hit_time>hitMaxTime_)
	      continue;

      //--- Geometriacal information 
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
            
      
      //--- Distance beam spot position <---> central point of the hitted crystal:
      double DistUnc_BS = std::sqrt(std::pow(global_point.x() - beamSpot.x0(), 2) + 
		                   std::pow(global_point.y() - beamSpot.y0(), 2) + 
		                   std::pow(global_point.z() - beamSpot.z0(), 2));
      
      //--- TOF photon-like:
      double TOFUnc = DistUnc_BS / c_cm_ns;

      //--- Hit Time-Stamp correction:
      double hit_time_corr = hit_time - TOFUnc;


      meUncTimeMean_corr_no_selection_->Fill(hit_time_corr);


      //--- Selection on the corrected time (to exclude the residual part if the prev bunch crossing)
      if(hit_time_corr < MinRecoHitTime_)
            continue;


      //--- Usefull BTL geometrical index manipulations:
      auto index = topology->btlIndex(geoId.rawId());
      //uint32_t SMphi_index = index.first; //--> range [1,108] 
      uint32_t SMeta_index = index.second; //-->range [1,98]

      int RU_index = (detId.mtdSide() == 0) ? (7 - detId.runit()) : (6 + detId.runit()); //--> Ru_index [1,12] from lx to rx
      int SMinRU_index = detId.dmodule() + (detId.dmodule() + detId.smodule()); //--> Run [0,23] inside the RU
      int crystal_index = detId.crystal() + (SMeta_index-1)*16; //--> detId.crystal() [0,15] for every SM
                                                                //    crystal_index run over all BTL [0,]




	////////////////////////////////
	//                            //
        //         Histograms         // 
        //                            //
        ////////////////////////////////

      //--- Global BTL histograms
      meUncTimeMean_->Fill(hit_time);
      meUncTimeMean_corr_->Fill(hit_time_corr);
      meUncTime_Left_->Fill(t_corr_L);
      meUncTime_Right_->Fill(t_corr_R);
      meUncAmpl_global_->Fill(hit_amplitude);
      meUncEne_global_->Fill(hit_ene);
      meUncHitAmpl_vs_TimeCorr_->Fill(hit_amplitude,hit_time_corr);
      

      //--- Histograms per fixed RU ring w/o and w/ TOF correction
      meUncTimeRUSlice_[RU_index-1]->Fill(hit_time);
      meUncTimeRUSlice_corr_[RU_index-1]->Fill(hit_time_corr);

      //--- Histograms per each single RU [RU ring index, Tray index]
      meUncTime_SingleRU_[RU_index-1][detId.mtdRR()-1]->Fill(hit_time_corr);		

      //--- Histograms for each SM ring
      meUncTimePhiSlice_[SMeta_index-1]->Fill(hit_time_corr);      
      
      //--- Histograms for each SM inside a single RU (selected only a single Tray (tray index == 1)
      if (detId.mtdRR() == 1){
              meUncTimeSingleSmInRu_[RU_index-1][SMinRU_index]->Fill(hit_time_corr);
      }

      //--- Histograms per Crystal ring (central ring per each RU)
      for (size_t i = 0; i < ringStartIndices_.size(); ++i) {
	      if (crystal_index == ringStartIndices_[i]) {
		      meHitTimeCrystalRing_[i]->Fill(hit_time_corr);
		      break;
	      }
      }





           ////////////////////////////////
           //                            //
           //      SimHit Id studies     // 
           //                            //
           ////////////////////////////////


      //--- UncalibratedRecHits matched to SimHits
      if (m_btlSimHits.count(detId.rawId()) != 1)
        continue;

      if(m_btlSimTrackId[detId.rawId()] == 0) meUncTimeMatched_Id_0_->Fill(hit_time_corr);
      if(m_btlSimTrackId[detId.rawId()] == 1) meUncTimeMatched_Id_1_->Fill(hit_time_corr);
      if(m_btlSimTrackId[detId.rawId()] == 2) meUncTimeMatched_Id_2_->Fill(hit_time_corr);
      if(m_btlSimTrackId[detId.rawId()] == 3) meUncTimeMatched_Id_3_->Fill(hit_time_corr);      

      if(m_btlSimTrackId[detId.rawId()] == 0) {
	      meUncHitAmpl_Matched_Id_0_->Fill(hit_amplitude);
              meUncTimeCorrRUSlice_Matched_Id_0_[RU_index-1]->Fill(hit_time_corr);
      }

      if(m_btlSimTrackId[detId.rawId()] == 1) {
              meUncHitAmpl_Matched_Id_1_->Fill(hit_amplitude);
              meUncTimeCorrRUSlice_Matched_Id_1_[RU_index-1]->Fill(hit_time_corr);
      }

      if(m_btlSimTrackId[detId.rawId()] == 2) {
              meUncHitAmpl_Matched_Id_2_->Fill(hit_amplitude);
              meUncTimeCorrRUSlice_Matched_Id_2_[RU_index-1]->Fill(hit_time_corr);
      }

      if(m_btlSimTrackId[detId.rawId()] == 3) {
              meUncHitAmpl_Matched_Id_3_->Fill(hit_amplitude);
              meUncTimeCorrRUSlice_Matched_Id_3_[RU_index-1]->Fill(hit_time_corr);
	      meUncHitAmpl_vs_TimeCorr_Matched_Id_3_->Fill(hit_amplitude,hit_time_corr);
      }



/*
      if(m_btlSimTrackId[detId.rawId()] == 3){
	      const auto& simHitInfo = m_btlSimHits[detId.rawId()];
	      if(detId.runit()==6 || detId.runit()==1){

		      std::cout << "|   SIDE BTL: " << detId.mtdSide() << std::endl;
                      std::cout << "|   RU NUMBER: " << detId.runit() << std::endl;
                      std::cout << "|   RU_index: " << RU_index << std::endl;
                      std::cout << "|   TRAY NUMBER: " << detId.mtdRR() << std::endl;
                      std::cout << "|   DM NUMBER: " << detId.dmodule() << std::endl;
                      std::cout << "|   SM NUMBER: " << detId.smodule() << std::endl;
		      std::cout << "|   RecHit x: " << global_point.x() << std::endl;
		      std::cout << "|   RecHit y: " << global_point.y() << std::endl;
		      std::cout << "|   RecHit z: " << global_point.z() << std::endl;

		      // info sull'evento  
		      std::cout << "|   RUN: " << iEvent.id().run()
			      << " LUMI: " << iEvent.id().luminosityBlock()
			      << " EVENT: " << iEvent.id().event() << std::endl;

		      // info sul DetId e sulla simHit
		      std::cout << "|   DetId raw: " << detId.rawId() << std::endl;
		      std::cout << "|   SimTrackId: " << m_btlSimTrackId[detId.rawId()] << std::endl;
		      std::cout << "|   SimHit TOF: " << simHitInfo.time << " ns" << std::endl;
		      std::cout << "|   CorrTime: " << hit_time_corr << " ns" << std::endl;
		      std::cout << "|   SimHit Edep: " << simHitInfo.energy << " MeV" << std::endl;
		      std::cout << "|   SimHit local (x,y,z): ("
			      << simHitInfo.x << ", "
			      << simHitInfo.y << ", "
			      << simHitInfo.z << ")" << std::endl;
  
		      std::cout << "|=============================================|" << std::endl;

	      }	
      }
*/      
//--------------------------------------------------------------------------------------------

           ////////////////////////////////
           //                            //
           //    Walk Time Monitoring    // 
           //                            //
           ////////////////////////////////

      double time_L_TOF = t_corr_L - TOFUnc;
      double time_R_TOF = t_corr_R - TOFUnc;

      double hit_time_noTW_corr = hit_time_noTW - TOFUnc;
      double t_L_noTW_TOF = t_corr_L_noTW - TOFUnc;
      double t_R_noTW_TOF = t_corr_R_noTW - TOFUnc;


      meUncTimeCorr_Left_->Fill(time_L_TOF);
      meUncTimeCorr_Right_->Fill(time_R_TOF);

      //--- Crystal rings in the cetral zone (RU=6, 7):
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
//--- Test space:

                std::cout << "|   SIDE BTL: " << detId.mtdSide() << std::endl;
		std::cout << "|   RU NUMBER: " << detId.runit() << std::endl;
                std::cout << "|   TRAY NUMBER: " << detId.mtdRR() << std::endl;
                std::cout << "|   DM NUMBER: " << detId.dmodule() << std::endl;
                std::cout << "|   SM NUMNBER: " <<detId.smodule() << std::endl;
		std::cout << "|   SM ETA: " << SMeta_index << std::endl;
		std::cout << "|   SM PHI: " << SMphi_index << std::endl;
                std::cout << "|   Global z: " << global_point.z() << std::endl;
                std::cout << "|   CRYSTAL NUMBER: " << detId.crystal() << std::endl;
                std::cout << "|=============================================|"<< std::endl;


      if(detId.mtdRR() == 1 && detId.runit() ==6 && detId.mtdSide() == 0){
		
			std::cout << "|   Global z: " << global_point.z() << std::endl;
			std::cout << "|   Global x: " << global_point.x() << std::endl;
			std::cout << "|   Global y: " << global_point.y() << std::endl;
			std::cout << "|   SM ETA: " << SMeta_index << std::endl;
			std::cout << "|   SM PHI: " << SMphi_index << std::endl;
			std::cout << "|=============================================|"<< std::endl;
		}
*/
      



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


  // --- Gen Particle histograms
 
  meGenPt_ = ibook.book1D("BtlGenPt", "Pt at generator level particles;P_{t} [GeV];Entries", 1000, 0., 500.);
  meGenEne_ = ibook.book1D("BtlGenEne", "Energy at generator level particles;Energy [GeV];Entries", 800, 0., 500.);
  meGenEta_ = ibook.book1D("BtlGenEta", "#eta at generator level particles; #eta;Entries", 100, -4., 4.);
  meGenTheta_ = ibook.book1D("BtlGenTheta", "#theta at generator level particles; #theta [rad];Entries", 100, -3.14159, 3.14159);
  meGenPhi_ = ibook.book1D("BtlGenPhi", "#phi at generator level particles; #phi [rad];Entries", 100, -3.14159, 3.14159);
  meGenPdg_ = ibook.book1D("BtlGenPdg", "Pdg number at generator level particles; Pdg number;Entries", 2000, -1000, 1000);



  //--- UncalibratedRecHits histograms

  meUncTimeMean_ = ibook.book1D("BtlUncTimeMean","Btl Uncalibrated RecHits ToA;ToA_{UNC RECO} [ns]", 1000, -5., 25.);
  meUncTimeMean_corr_ = ibook.book1D("BtlUncTimeMean_corr", "Mean Time of Uncalibrated RECO Hits with Photon-like TOF correction;Time [ns];Entries", 1000, -5., 25.);
  meUncAmpl_global_ = ibook.book1D("BtlUncAmpl_global", "Hit Amplitude;Hit Amplitude [pC];Entries", 300, 0., 700.);
  meUncEne_global_ = ibook.book1D("BtlUncEne_global", "Hit Energy;Hit Energy [MeV];Entries", 100, 0., 30.);

  meUncTime_Left_ = ibook.book1D("BtlUncTime_Left_","Btl Uncalibrated RecHits ToA Left side;Time (Left) [ns];Entries", 500, 0., 25.);
  meUncTime_Right_ = ibook.book1D("BtlUncTime_Right_","Btl Uncalibrated RecHits ToA right side;Time (Right) [ns];Entries", 500, 0., 25.);
  
  meUncTimeCorr_Left_ = ibook.book1D("BtlUncTimeCorr_Left_","Btl Uncalibrated RecHits Time corrected with TOF  - Left side;Time (Left) [ns];Entries", 1000, -5., 25.);    
  meUncTimeCorr_Right_ =  ibook.book1D("BtlUncTimeCorr_Right_","Btl Uncalibrated RecHits Time corrected with TOF - right side;Time (Right) [ns];Entries", 1000, -5., 25.);

  meUncTimeMean_noSelection_ = ibook.book1D("BtlUncTimeMean_noSelection","Btl Uncalibrated RecHits mean ToA -- NO HIT SELECTION;ToA_{UNC RECO} [ns]", 1000, -5., 25.);
  meUncAmpl_global_noSelection_ = ibook.book1D("BtlUncAmpl_global_noSelection", "Btl Uncalibrated RecHits amplitude -- NO HIT SELECTION;Hit Amplitude [pC];Entries", 300, 0., 700.);
  meUncEne_global_noSelection_ = ibook.book1D("BtlUncEne_global_noSelection", "Btl Uncalibrated RecHits Energy -- NO HIT SELECTION;Hit Energy [MeV];Entries", 100, 0., 30.);
  meUncTimeMean_corr_no_selection_ = ibook.book1D("BtlUncTimeMean_corr_no_selection","Btl Uncalibrated RecHits corrected time -- NO HIT SELECTION;Time [ns]", 1000, -5., 25.);

  
  meUncHitAmpl_vs_TimeCorr_ = ibook.bookProfile("BTLUncHitAmpl_vs_TimeCorr",
                                      "BTL uncalibrated hit amplitude (avergaged) vs Corrected Time; <Ampl> [pC]; Corrected Time [ns]",
                                      50,
                                      0.,
                                      700.,
                                      -5.,
                                      25.
                                       );

  //--- Time corr per granularity levels:



  for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeRUSlice_" + std::to_string(ihistoRU + 1);
            std::string title = "Mean Time of Uncalibrated RecHits: (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meUncTimeRUSlice_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }

   for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeRUSlice_corr_" + std::to_string(ihistoRU + 1);
            std::string title = "Mean Time of Uncalibrated RecHits with TOF correction: (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries/30ps";
            meUncTimeRUSlice_corr_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }



   for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
    for (unsigned int ihistoTR = 0; ihistoTR < nTR_; ++ihistoTR) {

        std::string name_corr = "BtlUncTime_SingleRU_" + std::to_string(ihistoRU + 1) + "_TR" + std::to_string(ihistoTR + 1);
        std::string title_corr = "Mean Time of Uncalibrated RecHits with TOF correction: RU " + std::to_string(ihistoRU + 1) + " TR " + std::to_string(ihistoTR + 1);

        meUncTime_SingleRU_[ihistoRU][ihistoTR] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);

        meUncTime_SingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Time [ns]", 1);
        meUncTime_SingleRU_[ihistoRU][ihistoTR]->setAxisTitle("Counts", 2);

    }

   }


   for (uint32_t i = 0; i < nSMphi_; ++i) {

        meUncTimePhiSlice_[i] = ibook.book1D(
                        Form("BtlUncTimePhiSlice_%d", i),
                        Form("Mean Time of Uncalibrated RecHits with TOF correction for #phi ring: %d", i),
                        1000, -5., 25.);
        meUncTimePhiSlice_[i]->setAxisTitle("Time [ns]", 1);
        meUncTimePhiSlice_[i]->setAxisTitle("Entries", 2);
   }


   for (unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
          for (unsigned int iSM = 0; iSM < nSMinRU_; ++iSM) {
                  std::string name_corr = "UncBtlTimeSingleSmInRu_" + std::to_string(ihistoRU + 1) + "_SM" + std::to_string(iSM + 1);
                  std::string title_corr = "Mean Time of Uncalibrated RecHits with TOF correction - RU ring: " + std::to_string(ihistoRU + 1) + " SM: " + std::to_string(iSM + 1);
                  meUncTimeSingleSmInRu_[ihistoRU][iSM] = ibook.book1D(name_corr, title_corr, 1000, -5., 25.);
                  meUncTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Time_{RECO} [ns]", 1);
                  meUncTimeSingleSmInRu_[ihistoRU][iSM]->setAxisTitle("Entries/30ps", 2);
          }
  }


  for (size_t i = 0; i < meHitTimeCrystalRing_.size(); ++i) {
  std::string name = "BtlHitTimeCrystalRing_RU" + std::to_string(i + 1);
  std::string title = "Mean Time of Uncalibrated RecHits with TOF correction - Crystal ring: " + std::to_string(i + 1) +
                      ";Time [ns];Entries";
  meHitTimeCrystalRing_[i] = ibook.book1D(name, title, 1000, -5., 25.);
}






//--- Profile Uncalibrated RecoHit:


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







// --- SimHit Matching studies:
 
 meUncTimeMatched_Id_0_ = ibook.book1D("BtlUncTimeMatched_Id_0", "Mean Time of Unc RecHits Matched with a SimHit with offSetId = 0;Time [ns];Entries", 1000, -5.,25.);
 meUncTimeMatched_Id_1_ = ibook.book1D("BtlUncTimeMatched_Id_1", "Mean Time of Unc RecHits Matched with a SimHit with offSetId = 1;Time [ns];Entries", 1000, -5.,25.);
 meUncTimeMatched_Id_2_ = ibook.book1D("BtlUncTimeMatched_Id_2", "Mean Time of Unc RecHits Matched with a SimHit with offSetId = 2;Time [ns];Entries", 1000, -5.,25.);
 meUncTimeMatched_Id_3_ = ibook.book1D("BtlUncTimeMatched_Id_3", "Mean Time of Unc RecHits Matched with a SimHit with offSetId = 3;Time [ns];Entries", 1000, -5.,25.);



 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeCorrRUSlice_Matched_Id_0_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Mean Time of Unc RecHits (Matched w/ SimHit OffSetId = 0): (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries";
            meUncTimeCorrRUSlice_Matched_Id_0_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }


 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeCorrRUSlice_Matched_Id_1_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Mean Time of Unc RecHits (Matched w/ SimHit OffSetId = 1): (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries";
            meUncTimeCorrRUSlice_Matched_Id_1_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }


 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeCorrRUSlice_Matched_Id_2_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Mean Time of Unc RecHits (Matched w/ SimHit OffSetId = 2): (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries";
            meUncTimeCorrRUSlice_Matched_Id_2_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }


 for(unsigned int ihistoRU = 0; ihistoRU < nRU_tot_; ++ihistoRU) {
            std::string name = "BtlUncTimeCorrRUSlice_Matched_Id_3_" + std::to_string(ihistoRU + 1);
            std::string title = "Corrected Mean Time of Unc RecHits (Matched w/ SimHit OffSetId = 3): (RU " + std::to_string(ihistoRU + 1) + ");Time [ns];Entries";
            meUncTimeCorrRUSlice_Matched_Id_3_[ihistoRU] = ibook.book1D(name, title, 1000, -5., 25.);
    }



 meUncHitAmpl_Matched_Id_0_ = ibook.book1D("BtlUncHitAmpl_Matched_Id_0", "BTL UncRecHit energy - SimHit Matched w/ OffSetId = 0;Energy [MeV];Entries", 300, 0., 700.);
 meUncHitAmpl_Matched_Id_1_ = ibook.book1D("BtlUncHitAmpl_Matched_Id_1", "BTL UncRecHit energy - SimHit Matched w/ OffSetId = 1;Energy [MeV];Entries", 300, 0., 700.);
 meUncHitAmpl_Matched_Id_2_ = ibook.book1D("BtlUncHitAmpl_Matched_Id_2", "BTL UncRecHit energy - SimHit Matched w/ OffSetId = 2;Energy [MeV];Entries", 300, 0., 700.);
 meUncHitAmpl_Matched_Id_3_ = ibook.book1D("BtlUncHitAmpl_Matched_Id_3", "BTL UncRecHit energy - SimHit Matched w/ OffSetId = 3;Energy [MeV];Entries", 300, 0., 700.);

meUncHitAmpl_vs_TimeCorr_Matched_Id_3_ = ibook.bookProfile("BTLUncHitAmpl_vs_TimeCorr_Matched_Id_3",
                                      "BTL uncalibrated hit amplitude (avergaged) vs Corrected Time; <Ampl> [pC]; Corrected Time [ns]",
                                      70,
                                      40.,
                                      500.,
                                      -5.,
                                      25.
                                       );

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
