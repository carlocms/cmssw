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


#file_list = [
#        f'file:/eos/infnts/cms/store/user/cgiraldi/SingleGamma_FlatEne_low_energy/step3_{i}.root'
#        for i in range (1, 300)
#]

# --- in case of condor: sys.argv[1]
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(

        '/store/relval/CMSSW_15_1_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/150X_mcRun4_realistic_v1_STD_RegeneratedGS_Run4D110_noPU-v1/2580000/01fb673b-a782-4b27-83ab-c4bb1eecb508.root',
       # '/store/relval/CMSSW_15_1_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/150X_mcRun4_realistic_v1_STD_RegeneratedGS_Run4D110_noPU-v1/2580000/026b5b0e-e380-4b12-8859-b1e7002111c6.root',
        #'/store/relval/CMSSW_15_1_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/150X_mcRun4_realistic_v1_STD_RegeneratedGS_Run4D110_noPU-v1/2580000/02ba004c-a747-4891-a3d2-938620b2a920.root',
        #'/store/relval/CMSSW_15_1_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/150X_mcRun4_realistic_v1_STD_RegeneratedGS_Run4D110_noPU-v1/2580000/082ff273-609a-41e3-8e15-b0cfb3f5bdd3.root',
        #'/store/relval/CMSSW_15_1_0_pre2/RelValMinBias_14TeV/GEN-SIM-RECO/150X_mcRun4_realistic_v1_STD_RegeneratedGS_Run4D110_noPU-v1/2580000/08a4bdb8-887a-4fca-91b5-c662da4ac7f2.root'
        
        )

#     fileNames = cms.untracked.vstring (*file_list)
)

process.mix.digitizers = cms.PSet()
for a in process.aliases: delattr(process, a)

# --- BTL Validation
process.load("Validation.MtdValidation.btlSimHitsValid_cfi")
process.load("Validation.MtdValidation.btlDigiHitsValid_cfi")
process.load("Validation.MtdValidation.btlLocalRecoValid_cfi")
process.load("Validation.MtdValidation.btlTimeMonitoring_cfi")
btlValidation = cms.Sequence(process.btlSimHitsValid + process.btlDigiHitsValid + process.btlLocalRecoValid + process.btlTimeMonitoring)

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
process.btlLocalRecoValid.optionalPlots = True
# process.etlLocalRecoValid.optionalPlots = True
# process.mtdTracksValid.optionalPlots = True
# process.vertices4DValid.optionalPlots = True
process.btlTimeMonitoring.optionalPlots = True

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

process.schedule = cms.Schedule( process.p , process.endjob_step , process.DQMoutput_step )
