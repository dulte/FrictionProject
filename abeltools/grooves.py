#!/usr/bin/env python
# -*- coding: utf-8 -*-

from datetime import datetime
from SystematicStudy import SystematicStudy, RunSettings

# The general cases
analysisParameters = {'grooveHeight': [1, 2, 3, 4],
                      'grooveSize': [6, 8]}

runSettings = RunSettings(executable='FrictionProject', nodes=1,
                           tasksPerNode=16, slurm=True, jobName='grooves')
systematicStudy = SystematicStudy(parameterDict=analysisParameters,
                                  configFilename='config.txt',
                                  runsettings=runSettings)
date = datetime.now().strftime('%d-%m-%H')
systematicStudy.generateSimulationFolders(path="Simulation-{}".format(date))
systematicStudy.launchSimulations(runSettings)
# For the case height = 1 and grooveSize = 0
analysisParameters = {'grooveHeight': [1],
                      'grooveSize': [0]}

systematicStudy = SystematicStudy(parameterDict=analysisParameters,
                                  configFilename='config.txt',
                                  runsettings=runSettings)

systematicStudy.generateSimulationFolders(path="Simulation-{}".format(date))
systematicStudy.launchSimulations(runSettings)
