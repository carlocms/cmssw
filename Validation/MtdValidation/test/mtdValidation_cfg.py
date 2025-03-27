import FWCore.ParameterSet.Config as cms
import sys

from Configuration.Eras.Era_Phase2C17I13M9_cff import Phase2C17I13M9
process = cms.Process('mtdValidation',Phase2C17I13M9)

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

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

#Setup FWK for multithreaded
process.options.numberOfThreads = 4
process.options.numberOfStreams = 0
process.options.numberOfConcurrentLuminosityBlocks = 0
process.options.eventSetup.numberOfConcurrentIOVs = 1

process.MessageLogger.cerr.FwkReport  = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100),
)


file_list = [
        f'file:/eos/infnts/cms/store/user/cgiraldi/29703.0_SingleMuFlatPt0p7To10+Run4D110/step3_{i}.root'
        for i in range (1, 10)
]

# --- in case of condor: sys.argv[1]
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring(
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/0496e9e7-4540-4673-911f-0c6c811fa6f2.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/065c1d5f-46b6-4063-b985-810eff026d43.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/07b4c19e-e29e-4cab-bdb2-f1557e9bd9f1.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/0da94c27-0173-46f4-988b-c52f1febd6df.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/0dd8ca91-80bd-4f85-8797-451d7583282b.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/16f18951-be93-4bcf-9975-9c2b70677a88.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/1cdcc9fe-c6cc-4ac8-ae18-376d1d308932.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/1df4f4ae-3697-40f3-a28c-0ad8382e1c17.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/1fbe7afc-aa42-475c-940d-2c2bbf47551e.root',
        #'/store/relval/CMSSW_15_0_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/141X_mcRun4_realistic_v3_STD_RegeneratedGS_Run4D110_noPU-v1/2590000/1fc7f3d7-0024-4a4d-a455-37924b68b750.root'
       #)

     fileNames = cms.untracked.vstring (*file_list)
)

process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)

# --- BTL Validation
process.load("Validation.MtdValidation.btlSimHitsValid_cfi")
process.load("Validation.MtdValidation.btlDigiHitsValid_cfi")
process.load("Validation.MtdValidation.btlLocalRecoValid_cfi")
btlValidation = cms.Sequence(process.btlSimHitsValid + process.btlDigiHitsValid + process.btlLocalRecoValid)

# --- ETL Validation
process.load("Validation.MtdValidation.etlSimHitsValid_cfi")
process.load("Validation.MtdValidation.etlDigiHitsValid_cfi")
process.load("Validation.MtdValidation.etlLocalRecoValid_cfi")
etlValidation = cms.Sequence(process.etlSimHitsValid + process.etlDigiHitsValid + process.etlLocalRecoValid)

# --- Global Validation
process.load("Validation.MtdValidation.mtdTracksValid_cfi")
process.load("Validation.MtdValidation.mtdEleIsoValid_cfi")
process.load("Validation.MtdValidation.vertices4DValid_cfi")

# process.btlDigiHitsValid.optionalPlots = True
# process.etlDigiHitsValid.optionalPlots = True
process.btlLocalRecoValid.optionalPlots = True
# process.etlLocalRecoValid.optionalPlots = True
# process.mtdTracksValid.optionalPlots = True
# process.vertices4DValid.optionalPlots = True

process.validation = cms.Sequence(btlValidation + etlValidation + process.mtdTracksValid + process.mtdEleIsoValid + process.vertices4DValid)

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

process.schedule = cms.Schedule( process.p , process.endjob_step , process.DQMoutput_step )
