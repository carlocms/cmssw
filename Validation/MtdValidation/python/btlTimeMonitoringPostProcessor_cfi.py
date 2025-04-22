import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

btlTimeMonitoringPostProcessor = DQMEDHarvester("DQMGenericClient",
    subDirs = cms.untracked.vstring("MTD/BTL/RecoTiming"),
    efficiency = cms.vstring(
        # Inserisci qui eventuali efficiency plots, es:
        # "eff_vs_eta 'Efficiency vs eta;#eta;Efficiency' h_den_eta h_num_eta"
    ),
    resolution = cms.vstring(
        # Inserisci qui eventuali risoluzioni, es:
        # "timeRes_vs_eta 'Time resolution vs eta;#eta;#sigma_{t} [ps]' h_sigma_time_eta"
    ),
    verbose = cms.untracked.uint32(0)
)

