import FWCore.ParameterSet.Config as cms
from DQMServices.Core.DQMEDHarvester import DQMEDHarvester

btlTimeMonitoringPostProcessor = DQMEDHarvester("DQMGenericClient",
    subDirs = cms.untracked.vstring("MTD/BTL/RecoTiming"),
    efficiency = cms.vstring(
    ),
    resolution = cms.vstring(
    ),
    verbose = cms.untracked.uint32(0)
)

