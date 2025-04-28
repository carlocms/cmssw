import FWCore.ParameterSet.Config as cms

btlTimeMonitoringPostProcessor = cms.EDProducer("BtlTimeMonitoringHarvester",
    folder = cms.string("MTD/BTL/RecoTiming/")
)
