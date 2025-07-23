// -*- C++ -*-
//
// Package:    Validation/MtdValidation
// Class:      BtlLocalRecoHarvester
//
/**\class BtlLocalRecoHarvester BtlLocalRecoHarvester.cc Validation/MtdValidation/plugins/BtlLocalRecoHarvester.cc

 Description: BTL SIM hits validation harvester

 Implementation:
     [Notes on implementation]
*/

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include "TMath.h"
#include <iomanip>
#include <sstream>
#include <tuple>
#include <utility>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/ForwardDetId/interface/BTLDetId.h"

class BtlTimeMonitoringHarvester : public DQMEDHarvester {
public:
  explicit BtlTimeMonitoringHarvester(const edm::ParameterSet& iConfig);
  ~BtlTimeMonitoringHarvester() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

protected:
  void dqmEndJob(DQMStore::IBooker&, DQMStore::IGetter&) override;

private:
  const std::string folder_;
  std::pair<double, double> vector_Mean_MeanError(const std::vector<double>& data) const;
  std::pair<double, double> computeMean_MeanError(MonitorElement* me) const;
  
  // --- Histograms
  MonitorElement* meHitOccupancy_;
};

//-------------------------------------------------------------------------------------------
//Funzione che mi calcola media e errore della media di un vettore dato in input

std::pair<double, double> BtlTimeMonitoringHarvester::vector_Mean_MeanError(const std::vector<double>& data) const {
    const std::size_t N = data.size();
    if (N < 2) {
        throw std::invalid_argument("The vector size must be at least 2");
    }

    //Mean
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    double mean_vec = sum / N;

    //Std Dev
    double sumSqr = 0.0;
    for (double val : data) {
        sumSqr += (val - mean_vec) * (val - mean_vec);
    }
    double variance = sumSqr / (N - 1);
    double stddev = std::sqrt(variance);

    //Mean std Error
    double meanErr_vec = stddev / std::sqrt(N);

    return {mean_vec, meanErr_vec};
}

//-------------------------------------------------------------------------------------------

std::pair<double, double> BtlTimeMonitoringHarvester::computeMean_MeanError(MonitorElement* me) const {
    if (!me)
        return {0., 0.};

    const TH1* h = me->getTH1();
    if (!h)
        return {0., 0.};

    const int nbins = h->GetNbinsX();
    const double leftCut = -1.0;
    int totalCounts = 0;

    // Conteggio totale dei contenuti sopra il taglio sinistro
    for (int i = 1; i <= nbins; ++i) {
        if (h->GetBinCenter(i) >= leftCut) {
            totalCounts += static_cast<int>(h->GetBinContent(i));
        }
    }

    if (totalCounts < 2)
        return {0., 0.};

    const double threshold = 0.9 * totalCounts; //VALORE TRONCAMENTO CODA
    double cumulative = 0.0;
    double weightedSum = 0.0;
    // Calcolo cumulativo per conteggi da sinistra verso destra
    int truncatedCounts = 0;

    for (int i = 1; i <= nbins; ++i) {
        const double x = h->GetBinCenter(i);
        if (x < leftCut)
            continue; // Applica taglio fisso a sinistra

        const double count = h->GetBinContent(i);
        if (cumulative + count > threshold)
            break; // Taglio a destra: si ferma una volta raggiunto l'88%

        cumulative += count;
	weightedSum += count * x;
        truncatedCounts += static_cast<int>(count);
    }

    if (truncatedCounts < 2)
        return {0., 0.};

    const double mean = weightedSum / truncatedCounts;
    

    cumulative = 0.0;
    double sumSqDiff = 0.0;

    for (int i = 1; i <= nbins; ++i) {
        const double x = h->GetBinCenter(i);
        const double count = h->GetBinContent(i);
        if (x < leftCut) continue;
        if (cumulative + count > threshold) break;

        const double diff = x - mean;
        sumSqDiff += count * diff * diff;
        cumulative += count;
    }

    const double variance = sumSqDiff / (truncatedCounts - 1);
    const double stddev = std::sqrt(variance);
    const double meanError = stddev / std::sqrt(truncatedCounts);

    return {mean, meanError};
}
//------------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////|
//------------------------------------------------------------------------------------------


// ------------ constructor and destructor --------------
BtlTimeMonitoringHarvester::BtlTimeMonitoringHarvester(const edm::ParameterSet& iConfig)
    : folder_(iConfig.getParameter<std::string>("folder")) {}

BtlTimeMonitoringHarvester::~BtlTimeMonitoringHarvester() {}

// ------------ endjob tasks ----------------------------
void BtlTimeMonitoringHarvester::dqmEndJob(DQMStore::IBooker& ibook, DQMStore::IGetter& igetter) {
  // --- Get the monitoring histograms
  MonitorElement* meBtlHitLogEnergy = igetter.get(folder_ + "BtlHitLogEnergy");
  MonitorElement* meNevents = igetter.get(folder_ + "BtlNevents");

  if (!meBtlHitLogEnergy || !meNevents) {
    edm::LogError("BtlTimeMonitoringHarvester") << "Monitoring histograms not found!" << std::endl;
    return;
  }


const unsigned int nRU = 12;
const unsigned int nTray = 36;
const unsigned int nSM = 96;
const unsigned int nSMinRU = 24;
const unsigned int nCRY = 12;


//----------------UNCALIBRATED RECOHIT MONITORING-------------------------------
//-------------------------------------------------------------------
std::vector<double> RU_phi_meanOfMeans;
std::vector<double> RU_phi_meanOfMeanUncertainties;
std::vector<double> RU_phi_meanOfMeanErrors;
std::vector<double> RU_phi_meanOfMeanErrorsUncertainties;

for (unsigned int ru = 1; ru <= nRU; ++ru) {
    std::vector<double> RuMeans;
    std::vector<double> RuMeanErrors;

    std::ostringstream out_RU_Slice_Tray;
    out_RU_Slice_Tray << "RU SLICE " << ru << "--------------------------------------------------- \n";

    for (unsigned int t = 1; t <= nTray; ++t) {
        std::string histoname = folder_ + "BtlUncTime_SingleRU_" + std::to_string(ru) + "_TR" + std::to_string(t);
        MonitorElement* me = igetter.get(histoname);

        if (!me) {
            edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
            continue;
        }

	auto [mean, meanError] = computeMean_MeanError(me);
	
	RuMeans.push_back(mean);
	RuMeanErrors.push_back(meanError);
    }
//-----------------------------------------

    // Mean of the Means
    auto [mean_means, err_means] = vector_Mean_MeanError(RuMeans);
    RU_phi_meanOfMeans.push_back(mean_means);
    RU_phi_meanOfMeanUncertainties.push_back(err_means);

    // Mean of the Mean Errors
    auto [mean_meanErrs, err_meanErrs] = vector_Mean_MeanError(RuMeanErrors);
    RU_phi_meanOfMeanErrors.push_back(mean_meanErrs);
    RU_phi_meanOfMeanErrorsUncertainties.push_back(err_meanErrs);  

    out_RU_Slice_Tray
    << std::setw(10) << "nTray"
    << std::setw(15) << "Mean"
    << std::setw(15) << "Er Mean"
    << std::setw(15) << "Unc Er-Mean"
    << "\n";

    for (unsigned int t = 0; t < nTray; ++t) {
        out_RU_Slice_Tray << std::setw(10) << t + 1
	    << std::setw(15) << RuMeans[t]
	    << std::setw(15) << RuMeanErrors[t]
            << "\n";
    }
    out_RU_Slice_Tray << "-----------------------------------------------------------------------";
    edm::LogPrint("BtlTimeMonitoringHarvester") << out_RU_Slice_Tray.str();
}

std::ostringstream out_RU_Slice_Mean;
out_RU_Slice_Mean << "--- MEAN VALUES RU ---\n";

out_RU_Slice_Mean
    << std::setw(10) << "RU"
    << std::setw(15) << "<Mean>"
    << std::setw(15) << "Er <Mean>"
    << std::setw(15) << "<Er Mean>"
    << std::setw(15) << "Er <Er Mean>"
    << "\n";

for (unsigned int ru = 0; ru < nRU; ++ru) {
        out_RU_Slice_Mean << std::setw(10) << ru + 1
	    << std::setw(15) << RU_phi_meanOfMeans[ru]
	    << std::setw(15) << RU_phi_meanOfMeanUncertainties[ru]
	    << std::setw(15) << RU_phi_meanOfMeanErrors[ru]
            << std::setw(15) << RU_phi_meanOfMeanErrorsUncertainties[ru]
            << "\n";
}
out_RU_Slice_Mean << "-----------------------------------------------------------------------";    
edm::LogPrint("BtlTimeMonitoringHarvester") << out_RU_Slice_Mean.str();

//-----------------------------------------------------------------

//---- SM Slice VS eta -------------
std::vector<double> SMmean;
std::vector<double> SMmeanErrors;
std::vector<double> SMmeanErrorUncertainties;


for (unsigned int i = 1; i <= nSM; ++i) {
  std::string histoname = folder_ + "BtlUncTimePhiSlice_" + std::to_string(i);

  MonitorElement* me = igetter.get(histoname);
  if (!me) {
    edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
    continue;
  }

  auto [mean, meanError] = computeMean_MeanError(me);
        
  SMmean.push_back(mean);
  SMmeanErrors.push_back(meanError);
}

std::ostringstream out_SMslice;
out_SMslice << "------ SM Slice Index ------\n";

out_SMslice
    << std::setw(10) << "SM Index"
    << std::setw(15) << "Mean"
    << std::setw(15) << "Er Mean"
    << "\n";


for (unsigned int i = 0; i < nSM; ++i) {
    out_SMslice << std::setw(10) << i+1
		<< std::setw(15) << SMmean[i]
                << std::setw(15) << SMmeanErrors[i]
                << "\n";
}

out_SMslice << "-----------------------------------------------------------------------";
edm::LogPrint("BtlTimeMonitoringHarvester") << out_SMslice.str();

//---------------------------------------------------------------------------------------
//-----Single SM selected in Tray index ==1 ---------------------------------------------

for (unsigned int ru = 1; ru <= nRU; ++ru) {
    std::vector<double> RuMeans;
    std::vector<double> RuMeanErrors;

    std::ostringstream out_Cal_SMinRU;
    out_Cal_SMinRU << "CALIBRATED RECO HIT --- RU INDEX " << ru << "------------------------------------------------ \n";

    for (unsigned int t = 1; t <= nSMinRU; ++t) {
        std::string histoname = folder_ + "UncBtlTimeSingleSmInRu_" + std::to_string(ru) + "_SM" + std::to_string(t);
        MonitorElement* me = igetter.get(histoname);

        if (!me) {
            edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
            continue;
        }

        auto [mean, meanError] = computeMean_MeanError(me);

        RuMeans.push_back(mean);
        RuMeanErrors.push_back(meanError);
    }


    out_Cal_SMinRU
    << std::setw(10) << "SM"
    << std::setw(15) << "Mean"
    << std::setw(15) << "Er Mean"
    << "\n";

    for (unsigned int t = 0; t < nSMinRU; ++t) {
        out_Cal_SMinRU << std::setw(10) << t + 1
            << std::setw(15) << RuMeans[t]
            << std::setw(15) << RuMeanErrors[t]
            << "\n";
    }
    out_Cal_SMinRU << "-----------------------------------------------------------------------";
    edm::LogPrint("BtlTimeMonitoringHarvester") << out_Cal_SMinRU.str();
}

//---------------------------------------------------------------------------------------
//SINGLE CRYSTAL:

//---- SM Slice VS eta -------------
std::vector<double> Cry_SMmean;
std::vector<double> Cry_SMmeanErrors;
//std::vector<double> Cry_SMmeanErrorUncertainties;


for (unsigned int i = 1; i <= nCRY; ++i) {
  std::string histoname = folder_ + "BtlUncTimeCrystalRing_RU" + std::to_string(i) + "_";

  MonitorElement* me = igetter.get(histoname);
  if (!me) {
    edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
    continue;
  }

  auto [mean_cry, meanError_cry] = computeMean_MeanError(me);


  Cry_SMmean.push_back(mean_cry);
  Cry_SMmeanErrors.push_back(meanError_cry);
  //Cry_SMmeanErrorUncertainties.push_back(meanErrorUncertainty_cry);

}

std::ostringstream out_CRYslice;
out_CRYslice << "------CALIBRATED HIT --- CRYSTAL Slice Index ------\n";
out_CRYslice
    << std::setw(10) << "CRY Index"
    << std::setw(15) << "Mean"
    << std::setw(15) << "Er Mean"
    //<< std::setw(15) << "Un Er Mean"
    << "\n";

    for (unsigned int i = 0; i < nCRY; ++i) {
    out_CRYslice << std::setw(10) << i+1
                << std::setw(15) << Cry_SMmean[i]
                << std::setw(15) << Cry_SMmeanErrors[i]
                //<< std::setw(15) << Cry_SMmeanErrorUncertainties[i]
                << "\n";
}

out_CRYslice << "-----------------------------------------------------------------------";
edm::LogPrint("BtlTimeMonitoringHarvester") << out_CRYslice.str();

//-------------------------------------------------------------------
  // --- Get the number of BTL crystals and the number of processed events
  const float NBtlCrystals = BTLDetId::kCrystalsBTL;
  const float Nevents = meNevents->getEntries();
  const float scale = (Nevents > 0 ? 1. / (Nevents * NBtlCrystals) : 1.);

  // --- Book the cumulative histogram
  ibook.cd(folder_);
  meHitOccupancy_ = ibook.book1D("BtlHitOccupancy",
                                 "BTL cell occupancy vs RECO hit energy;log_{10}(E_{RECO} [MeV]); Occupancy per event",
                                 meBtlHitLogEnergy->getNbinsX(),
                                 meBtlHitLogEnergy->getTH1()->GetXaxis()->GetXmin(),
                                 meBtlHitLogEnergy->getTH1()->GetXaxis()->GetXmax());

  // --- Calculate the cumulative histogram
  double bin_sum = meBtlHitLogEnergy->getBinContent(meBtlHitLogEnergy->getNbinsX() + 1);
  for (int ibin = meBtlHitLogEnergy->getNbinsX(); ibin >= 1; --ibin) {
    bin_sum += meBtlHitLogEnergy->getBinContent(ibin);
    meHitOccupancy_->setBinContent(ibin, scale * bin_sum);
  }
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void BtlTimeMonitoringHarvester::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<std::string>("folder", "MTD/BTL/RecoTiming/");

  descriptions.add("btlTimeMonitoringPostProcessor", desc);
}



DEFINE_FWK_MODULE(BtlTimeMonitoringHarvester);
