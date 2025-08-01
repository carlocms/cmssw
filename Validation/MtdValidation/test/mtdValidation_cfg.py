import FWCore.ParameterSet.Config as cms
import sys

from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9
process = cms.Process('mtdValidation',Phase2C17I13M9)

#process.Tracer = cms.Service('Tracer')

process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')

process.load("Configuration.Geometry.GeometryExtendedRun4D110Reco_cff")
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic_T33', '')
process.load('RecoLocalFastTime.FTLClusterizer.MTDCPEESProducer_cfi')
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

#Setup FWK for multithreaded
process.options.numberOfThreads = 1
process.options.numberOfStreams = 0
process.options.numberOfConcurrentLuminosityBlocks = 0
process.options.eventSetup.numberOfConcurrentIOVs = 1

process.MessageLogger.cerr.FwkReport  = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100),
)

file_list = [
        f'file:/eos/infnts/cms/store/user/cgiraldi/Report4/SingleMu_report4_BTL_default/step3_{i}.root'
        for i in range (1, 100)
]


# --- in case of condor: sys.argv[1]
process.source = cms.Source("PoolSource",
        #Con condor:
        #fileNames = cms.untracked.vstring(sys.argv[1])
    
    #Locale con file Singoli: 
    #fileNames = cms.untracked.vstring(
     #   '/store/relval/CMSSW_15_1_0_pre2/RelValSinglePiFlatPt0p7To10/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecycledGS_Run4D110_noPU-v1/2580000/2be217f1-2b2d-4a4f-b868-fa8acf50e36b.root',
     #   '/store/relval/CMSSW_15_1_0_pre2/RelValSinglePiFlatPt0p7To10/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecycledGS_Run4D110_noPU-v1/2580000/6ff65a42-1808-4be3-9f74-18ddf6b70fff.root',
     #   '/store/relval/CMSSW_15_1_0_pre2/RelValSinglePiFlatPt0p7To10/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecycledGS_Run4D110_noPU-v1/2580000/7b0313eb-1600-4686-91f4-71907c4d9b1e.root',
     #   '/store/relval/CMSSW_15_1_0_pre2/RelValSinglePiFlatPt0p7To10/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecycledGS_Run4D110_noPU-v1/2580000/868b3273-3371-4a3e-8984-bc9ed84218de.root',
     #   '/store/relval/CMSSW_15_1_0_pre2/RelValSinglePiFlatPt0p7To10/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RecycledGS_Run4D110_noPU-v1/2580000/8b1e684e-462a-4d65-81b8-906e01fdcff8.root'
     #   )

        #Con loop file EOS
    fileNames = cms.untracked.vstring (*file_list)
)

process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)

# --- BTL Validation
process.load("Validation.MtdValidation.btlSimHitsValid_cfi")
process.load("Validation.MtdValidation.btlDigiHitsValid_cfi")
process.load("Validation.MtdValidation.btlLocalRecoValid_cfi")
process.load("Validation.MtdValidation.btlTimeMonitoring_cfi")
process.load("Validation.MtdValidation.btlTimeMonitoringWTracks_cfi")
btlValidation = cms.Sequence(process.btlSimHitsValid + process.btlDigiHitsValid + process.btlLocalRecoValid + process.btlTimeMonitoring + process.btlTimeMonitoringWTracks)

# --- ETL Validation
process.load("Validation.MtdValidation.etlSimHitsValid_cfi")
process.load("Validation.MtdValidation.etlDigiHitsValid_cfi")
process.load("Validation.MtdValidation.etlLocalRecoValid_cfi")
etlValidation = cms.Sequence(process.etlSimHitsValid + process.etlDigiHitsValid + process.etlLocalRecoValid)

# --- Global Validation
process.load("Validation.MtdValidation.mtdTracksValid_cfi")
process.load("Validation.MtdValidation.mtdEleIsoValid_cfi")
process.load("Validation.MtdValidation.vertices4DValid_cfi")

# --- BTL Time Monitoring
#process.load("Validation.MtdValidation.btlTimeMonitoring_cfi")


# process.btlDigiHitsValid.optionalPlots = True
# process.etlDigiHitsValid.optionalPlots = True
#process.btlLocalRecoValid.optionalPlots = True
# process.etlLocalRecoValid.optionalPlots = True
# process.mtdTracksValid.optionalPlots = True
# process.vertices4DValid.optionalPlots = True
process.btlTimeMonitoring.optionalPlots = True
process.btlTimeMonitoringWTracks.optionalPlots = True

#process.validation = cms.Sequence(btlValidation + etlValidation + process.mtdTracksValid + process.mtdEleIsoValid + process.vertices4DValid)
process.validation = cms.Sequence(btlValidation + process.mtdTracksValid)
process.DQMoutput = cms.OutputModule("DQMRootOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('DQMIO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('file:step3_inDQM.root'),
    outputCommands = process.DQMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

process.p = cms.Path( process.mix + process.mtdTrackingRecHits + process.validation )
process.endjob_step = cms.EndPath(process.endOfProcess)
process.DQMoutput_step = cms.EndPath( process.DQMoutput )


#from Validation.Performace.python.TimeMemorySummary

process.schedule = cms.Schedule( process.p , process.endjob_step , process.DQMoutput_step )
