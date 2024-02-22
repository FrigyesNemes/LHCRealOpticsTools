import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras
process = cms.Process('CTPPSTest', eras.Run3)

from conditions import *

def SetConditions(process):
    #UseOpticsGT(process)
    UseLHCInfoGT(process)
    UseAlignmentGT(process)
    UseOpticsFile(process, "sqlite_file:/afs/cern.ch/user/w/wcarvalh/public/CTPPS/optical_functions/PPSOpticalFunctions_2023_calib_v1.db", "PPSOpticalFunctions_test")

process.load('Validation.CTPPS.ctppsLHCInfoPlotter_cfi')

# minimal logger settings
process.MessageLogger = cms.Service("MessageLogger",
    statistics = cms.untracked.vstring(),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    )
)

#process.load('CalibPPS.ESProducers.CTPPSOpticalFunctionsESSource_cfi')

# default source
process.source = cms.Source("EmptySource",
    firstRun = cms.untracked.uint32(1),
)

#process.CondDB.connect = 'frontier://FrontierProd/CMS_CONDITIONS'
#process.PoolDBESSource = cms.ESSource("PoolDBESSource",
#    process.CondDB,
#    toGet = cms.VPSet(cms.PSet(
#        record = cms.string('CTPPSPixelAnalysisMaskRcd'),
#        tag = cms.string("CTPPSPixelAnalysisMask_Run3_v1_hlt")
#    )),
#)

# number of events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(int(1E4))
)

SetConditions(process)
CheckConditions()

# LHCInfo plotter
process.ctppsLHCInfoPlotter.outputFile = "simu_2023_lhcInfo.root"

process.CTPPSOpticsPlotter = cms.EDAnalyzer("CTPPSOpticsPlotter",
    opticsLabel = cms.string(""),
    rpId_45_N = cms.uint32(3),
    rpId_45_F = cms.uint32(23),
    rpId_56_N = cms.uint32(103),
    rpId_56_F = cms.uint32(123),
    outputFile = cms.string("simu_2023_optics.root")
)

process.validation = cms.Path(
    process.ctppsLHCInfoPlotter
    * process.CTPPSOpticsPlotter
)

# processing path
process.schedule = cms.Schedule(
    process.validation
)

from SimPPS.Configuration.Utils import setupPPSDirectSim
setupPPSDirectSim(process)

del process.es_prefer_composrc

process.ctppsBeamParametersFromLHCInfoESSource.vtxOffsetX45 = 0.
process.ctppsBeamParametersFromLHCInfoESSource.vtxOffsetY45 = 0.
process.ctppsBeamParametersFromLHCInfoESSource.vtxOffsetZ45 = 0.
process.source.numberEventsInLuminosityBlock = process.ctppsCompositeESSource.generateEveryNEvents

