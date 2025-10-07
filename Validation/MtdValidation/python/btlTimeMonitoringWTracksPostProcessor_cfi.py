import FWCore.ParameterSet.Config as cms

btlTimeMonitoringWTracksPostProcessor = cms.EDProducer("BtlTimeMonitoringWTracksHarvester",
    folder = cms.string("MTD/BTL/RecoTimingWTracks/")
)
