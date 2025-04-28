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

  std::pair<double, double> computeMedianAndMAD(MonitorElement* me) const;
  double computeMedianErrorBootstrap(MonitorElement* me, unsigned int nResamples = 1000) const;
  // --- Histograms
  MonitorElement* meHitOccupancy_;
};


//-----------------------------------------
std::pair<double, double> BtlTimeMonitoringHarvester::computeMedianAndMAD(MonitorElement* me) const {
    if (!me)
        return {0., 0.};

    const TH1* h = me->getTH1();
    if (!h)
        return {0., 0.};

    std::vector<double> values;
    int nbins = h->GetNbinsX();

    for (int i = 1; i <= nbins; ++i) {
        double binContent = h->GetBinContent(i);
        for (int j = 0; j < static_cast<int>(binContent); ++j) {
            values.push_back(h->GetBinCenter(i));
        }
    }

    if (values.empty())
        return {0., 0.};

    std::sort(values.begin(), values.end());

    // Median
    double median = 0.;
    size_t n = values.size();
    if (n % 2 == 0) {
        median = 0.5 * (values[n/2 - 1] + values[n/2]);
    } else {
        median = values[n/2];
    }

    // MAD (Median Absolute Deviation)
    std::vector<double> deviations;
    for (const auto& val : values) {
        deviations.push_back(std::abs(val - median));
    }
    std::sort(deviations.begin(), deviations.end());

    double mad = 0.;
    if (n % 2 == 0) {
        mad = 0.5 * (deviations[n/2 - 1] + deviations[n/2]);
    } else {
        mad = deviations[n/2];
    }

    return {median, mad};
}


//-------------------------------------


// Functin to calculate the median uncertainty via bootstrap
double BtlTimeMonitoringHarvester::computeMedianErrorBootstrap(MonitorElement* me, unsigned int nResamples) const {
    if (!me)
        return 0.;

    const TH1* h = me->getTH1();
    if (!h)
        return 0.;

    std::vector<double> values;
    int nbins = h->GetNbinsX();

    for (int i = 1; i <= nbins; ++i) {
        double binContent = h->GetBinContent(i);
        for (int j = 0; j < static_cast<int>(binContent); ++j) {
            values.push_back(h->GetBinCenter(i));
        }
    }

    if (values.size() < 2)
        return 0.;

    std::vector<double> resampledMedians;
    std::mt19937 rng(12345); 
    std::uniform_int_distribution<size_t> dist(0, values.size() - 1);

    for (unsigned int resample = 0; resample < nResamples; ++resample) {
        std::vector<double> sample;
        sample.reserve(values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            sample.push_back(values[dist(rng)]);
        }
        std::sort(sample.begin(), sample.end());
        double med = (sample.size() % 2 == 0)
                         ? 0.5 * (sample[sample.size()/2 - 1] + sample[sample.size()/2])
                         : sample[sample.size()/2];
        resampledMedians.push_back(med);
    }

    // Std of the resampled median
    double mean = std::accumulate(resampledMedians.begin(), resampledMedians.end(), 0.0) / resampledMedians.size();
    double sum_sq_diff = 0.;
    for (const auto& med : resampledMedians) {
        sum_sq_diff += (med - mean) * (med - mean);
    }

    return std::sqrt(sum_sq_diff / (resampledMedians.size() - 1)); // standard deviation
}




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

std::vector<double> medians;
std::vector<double> mads;
std::vector<double> medianErrors;

const unsigned int nRU = 12; 
for (unsigned int i = 1; i <= nRU; ++i) { 
  std::string histoname = folder_ + "BtlUncTimeRUSlice_corr_" + std::to_string(i);

  MonitorElement* me = igetter.get(histoname);
  if (!me) {
    edm::LogWarning("BtlTimeMonitoringHarvester") << "Histogram not found: " << histoname;
    medians.push_back(0.);
    mads.push_back(0.);
    continue;
  }

  auto [median, mad] = computeMedianAndMAD(me);
  double medianError = computeMedianErrorBootstrap(me);
  
  medians.push_back(median);
  mads.push_back(mad);
  medianErrors.push_back(medianError);
}


edm::LogPrint("BtlTimeMonitoringHarvester") << "------ RU Median, MAD and Median Error (bootstrap) ------";
for (unsigned int i = 0; i < nRU; ++i) {
  edm::LogPrint("BtlTimeMonitoringHarvester")
    << "RU index " << i+1
    << " : Median = " << medians[i] << " ns, "
    << " MAD = " << mads[i] << " ns, "
    << " Median error = " << medianErrors[i] << " ns";
}
edm::LogPrint("BtlTimeMonitoringHarvester") << "----------------------------------------------------------";

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
