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
  std::tuple<double, double, double> computeMedianErrorBootstrap(MonitorElement* me, unsigned int nResamples = 1000) const;
  std::tuple<double, double, double> computeMean_MeanError(MonitorElement* me) const;
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

std::tuple<double, double, double> BtlTimeMonitoringHarvester::computeMedianErrorBootstrap(MonitorElement* me, unsigned int nResamples) const {
    if (!me)
        return {0., 0., 0.};

    const TH1* h = me->getTH1();
    if (!h)
        return {0., 0., 0.};

    std::vector<double> values;
    const int nbins = h->GetNbinsX();

    
    int totalCounts = 0;
    for (int i = 1; i <= nbins; ++i)
        totalCounts += static_cast<int>(h->GetBinContent(i));

    if (totalCounts < 2)
        return {0., 0., 0.};

    const double threshold = 0.83 * totalCounts;//prima 0.88
    int cumulative = 0;
    int cutoffBin = nbins;

    for (int i = 1; i <= nbins; ++i) {
        cumulative += static_cast<int>(h->GetBinContent(i));
        if (cumulative >= threshold) {
            cutoffBin = i;
            break;
        }
    }

    for (int i = 1; i <= cutoffBin; ++i) {
        const int binContent = static_cast<int>(h->GetBinContent(i));
        const double binCenter = h->GetBinCenter(i);
        values.insert(values.end(), binContent, binCenter);
    }

    if (values.size() < 2)
        return {0., 0., 0.};

    // Calcolo della mediana dell'istogramma originale
    const double medianHist = TMath::Median(static_cast<int>(values.size()), values.data());

    // Bootstrap
    std::vector<double> resampledMedians;
    resampledMedians.reserve(nResamples);

    std::mt19937 rng(12345);
    std::uniform_int_distribution<size_t> dist(0, values.size() - 1);

    for (unsigned int resample = 0; resample < nResamples; ++resample) {
        std::vector<double> sample;
        sample.reserve(values.size());

        for (size_t i = 0; i < values.size(); ++i)
            sample.push_back(values[dist(rng)]);

        resampledMedians.push_back(TMath::Median(static_cast<int>(sample.size()), sample.data()));
    }

    // Deviazione standard delle mediane bootstrap
    const double mean = std::accumulate(resampledMedians.begin(), resampledMedians.end(), 0.0) / resampledMedians.size();

    double sum_sq_diff = 0.;
    for (const double m : resampledMedians)
        sum_sq_diff += (m - mean) * (m - mean);

    const double stddev = std::sqrt(sum_sq_diff / (resampledMedians.size() - 1));
    const double stddevError = stddev / std::sqrt(2. * (resampledMedians.size() - 1));

    return {stddev, stddevError, medianHist};
}

//------------------------------------------------------------------------------

// Function to calculate mean, standard deviation and mean error (using TH1 methods)
std::tuple<double, double, double> BtlTimeMonitoringHarvester::computeMean_MeanError(MonitorElement* me) const {
    if (!me)
        return {0., 0., 0.};

    const TH1* h = me->getTH1();
    if (!h)
        return {0., 0., 0.};

    const int nbins = h->GetNbinsX();

    int totalCounts = 0;
    for (int i = 1; i <= nbins; ++i)
        totalCounts += static_cast<int>(h->GetBinContent(i));

    if (totalCounts < 2)
        return {0., 0., 0.};

    const double threshold = 1.0 * totalCounts;
    int cumulative = 0;
    int cutoffBin = nbins;

    for (int i = 1; i <= nbins; ++i) {
        cumulative += static_cast<int>(h->GetBinContent(i));
        if (cumulative >= threshold) {
            cutoffBin = i;
            break;
        }
    }

    // Calcola media e stddev sui dati troncati
    double weightedSum = 0.0;
    double weightedSumSq = 0.0;
    int truncatedCounts = 0;

    for (int i = 1; i <= cutoffBin; ++i) {
        const int count = static_cast<int>(h->GetBinContent(i));
        const double x = h->GetBinCenter(i);
        weightedSum += count * x;
        weightedSumSq += count * x * x;
        truncatedCounts += count;
    }

    if (truncatedCounts < 2)
        return {0., 0., 0.};

    const double mean = weightedSum / truncatedCounts;
    const double variance = (weightedSumSq / truncatedCounts) - (mean * mean);
    const double stddev = (variance > 0.0) ? std::sqrt(variance) : 0.0;
    const double meanError = stddev / std::sqrt(truncatedCounts);
    const double meanErrorUncertainty = stddev / std::sqrt(2. * (truncatedCounts - 1) * truncatedCounts);

    return {mean, meanError, meanErrorUncertainty};
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


//-------------------------------------------------------------------
const unsigned int nRU = 12;
const unsigned int nTray = 36;
std::vector<double> RU_phi_meanOfMedianErrors;
std::vector<double> RU_phi_meanOfMedianErrorsUncertainties;
std::vector<double> RU_phi_meanOfMedians;
std::vector<double> RU_phi_meanOfMedianUncertainties;

std::vector<double> RU_phi_meanOfMeans;
std::vector<double> RU_phi_meanOfMeanUncertainties;
std::vector<double> RU_phi_meanOfMeanErrors;
std::vector<double> RU_phi_meanOfMeanErrorsUncertainties;



for (unsigned int ru = 1; ru <= nRU; ++ru) {
    std::vector<double> medianErrors;
    std::vector<double> medianErrorUncertainties;
    std::vector<double> medians;
    std::vector<double> RuMeans;
    std::vector<double> RuMeanErrors;
    std::vector<double> RuMeanErrorsUncertainties;

    std::ostringstream out_RU_Slice_Tray;
    out_RU_Slice_Tray << "RU SLICE " << ru << ":  TRAY Index --- Median --- Median Error --- Median Err Unc --- Mean --- Mean Error --- Mean Err Unc \n";

    for (unsigned int t = 1; t <= nTray; ++t) {
        std::string histoname = folder_ + "BtlUncTime_SingleRU_" + std::to_string(ru) + "_TR" + std::to_string(t);
        MonitorElement* me = igetter.get(histoname);

        if (!me) {
            edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
            medianErrors.push_back(0.);
            medianErrorUncertainties.push_back(0.);
            continue;
        }

        auto [medianError, medianErrorUncertainty, median] = computeMedianErrorBootstrap(me);
	auto [mean, meanError, meanErrorUncertainty] = computeMean_MeanError(me);

        medianErrors.push_back(medianError);
        medianErrorUncertainties.push_back(medianErrorUncertainty);
	medians.push_back(median);
	
	RuMeans.push_back(mean);
	RuMeanErrors.push_back(meanError);
	RuMeanErrorsUncertainties.push_back(meanErrorUncertainty);
    }
//-----------------------------------------


    
    //Mean of the Medians
    auto [mean_medians, err_medians] = vector_Mean_MeanError(medians);
    RU_phi_meanOfMedians.push_back(mean_medians);
    RU_phi_meanOfMedianUncertainties.push_back(err_medians);

    // Mean of the Median Errors
    auto [mean_medianErrs, err_medianErrs] = vector_Mean_MeanError(medianErrors);
    RU_phi_meanOfMedianErrors.push_back(mean_medianErrs);
    RU_phi_meanOfMedianErrorsUncertainties.push_back(err_medianErrs);

    // Mean of the Means
    auto [mean_means, err_means] = vector_Mean_MeanError(RuMeans);
    RU_phi_meanOfMeans.push_back(mean_means);
    RU_phi_meanOfMeanUncertainties.push_back(err_means);

    // Mean of the Mean Errors
    auto [mean_meanErrs, err_meanErrs] = vector_Mean_MeanError(RuMeanErrors);
    RU_phi_meanOfMeanErrors.push_back(mean_meanErrs);
    RU_phi_meanOfMeanErrorsUncertainties.push_back(err_meanErrs);  


    for (unsigned int t = 0; t < nTray; ++t) {
        out_RU_Slice_Tray << std::setw(10) << t + 1
	    << std::setw(15) << medians[t]
            << std::setw(20) << medianErrors[t]
            << std::setw(25) << medianErrorUncertainties[t]
	    << std::setw(30) << RuMeans[t]
	    << std::setw(35) << RuMeanErrors[t]
	    << std::setw(40) << RuMeanErrorsUncertainties[t]
            << "\n";
    }
    out_RU_Slice_Tray << "-----------------------------------------------------------------------";
    edm::LogPrint("BtlTimeMonitoringHarvester") << out_RU_Slice_Tray.str();
}

std::ostringstream out_RU_Slice_Mean;
out_RU_Slice_Mean << "--- MEAN VALUES RU ---\n";

for (unsigned int ru = 0; ru < nRU; ++ru) {
        out_RU_Slice_Mean << std::setw(10) << ru + 1
            << std::setw(15) << RU_phi_meanOfMedians[ru]
            << std::setw(20) << RU_phi_meanOfMedianUncertainties[ru]	    
            << std::setw(25) << RU_phi_meanOfMedianErrors[ru]
            << std::setw(30) << RU_phi_meanOfMedianErrorsUncertainties[ru]
	    << std::setw(35) << RU_phi_meanOfMeans[ru]
	    << std::setw(40) << RU_phi_meanOfMeanUncertainties[ru]
	    << std::setw(45) << RU_phi_meanOfMeanErrors[ru]
            << std::setw(50) << RU_phi_meanOfMeanErrorsUncertainties[ru]
            << "\n";
}
out_RU_Slice_Mean << "-----------------------------------------------------------------------";    
edm::LogPrint("BtlTimeMonitoringHarvester") << out_RU_Slice_Mean.str();

//-----------------------------------------------------------------

//---- SM Slice VS eta -------------
std::vector<double> SMmedians;
std::vector<double> SMmedianErrors;
std::vector<double> SMmedianErrorUncertainties;

const unsigned int nSM = 96;
for (unsigned int i = 1; i <= nSM; ++i) {
  std::string histoname = folder_ + "BtlUncTimePhiSlice_" + std::to_string(i);

  MonitorElement* me = igetter.get(histoname);
  if (!me) {
    edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
    continue;
  }

  auto [medianError, medianErrorUncertainty, median] = computeMedianErrorBootstrap(me);
        SMmedianErrors.push_back(medianError);
        SMmedianErrorUncertainties.push_back(medianErrorUncertainty);
        SMmedians.push_back(median);

}

std::ostringstream out_SMslice;
out_SMslice << "------ SM Slice Index --- Median --- Median Error --- Uncertainty Median Error---\n";

for (unsigned int i = 0; i < nSM; ++i) {
    out_SMslice << std::setw(10) << i+1
	        << std::setw(15) << SMmedians[i]
                << std::setw(25) << SMmedianErrors[i]
                << std::setw(20) << SMmedianErrorUncertainties[i]
                << "\n";
}

out_SMslice << "-----------------------------------------------------------------------";
edm::LogPrint("BtlTimeMonitoringHarvester") << out_SMslice.str();

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------



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
