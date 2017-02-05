
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shutil
import itertools
import subprocess
from random import randint

class SystematicStudy:
    def __init__(self, parameterDict={}, configFilename='', runsettings=None):
        # Know that dict 1 will contain structure filename and replication number. dict2 will contain various variation parameters that are (presumably) compatible with the template_string.
        self.configFilename = configFilename
        self.parameterDict = parameterDict
        self.simulationDirectories = {}
        self.runsettings = runsettings
        #print(self.lammpsTemplateString)

    def generateSimulationFolders(self, path=os.path.join('.')):
        keys = list(self.parameterDict.keys())
        permutations = list(itertools.product(*self.parameterDict.values()))

        ifile = open(self.configFilename).readlines()
        for permutation in permutations:
            folder_name = '-'.join([keys[i]+str(permutation[i]) for i in range(len(keys))])
            folder_path = os.path.join(path, folder_name)
            scriptContents = ""
            for line in ifile:
                for word in line.split():
                    lmp_append = word+" "
                    for i in range(len(keys)):
                        if "@"+keys[i] in word:
                            lmp_append = str(permutation[i])+" "
                    scriptContents += lmp_append
                scriptContents += "\n"

            # Create output folder
            os.makedirs(folder_path)
            os.makedirs(os.path.join(folder_path, 'Output'))
            shutil.copy(self.configFilename, os.path.join(folder_path, self.configFilename))
            shutil.copy(self.runsettings.executable, os.path.join(folder_path, self.runsettings.executable))
            with open(os.path.join(folder_path, self.configFilename), 'w') as ofile:
                ofile.write(scriptContents)
                ofile.close()
            self.simulationDirectories[folder_path] = self.configFilename
            #print(self.simulationDirectories)

    def launchSimulations(self, runSettings=None):
        curDir = os.getcwd()
        for folder, filename in self.simulationDirectories.items():
            if runSettings.slurm:
                print("Running Slurm")
                slurmJobScript = runSettings.createSlurmScriptString()
                os.chdir(curDir)
                os.chdir(folder)
                jobFile = open("job.sh", "w")
                jobFile.write(slurmJobScript)
                jobFile.close()
                if not runSettings.dryRun:
                    subprocess.call("sbatch job.sh", shell=True)
            else:
                print("Not running slurm")
                runSettings.arguments = "-in %s" % filename
                launchCommand = runSettings.createLaunchCommand()
                print(launchCommand)
                os.chdir(curDir)
                os.chdir(folder)
                if not runSettings.dryRun:
                    subprocess.call(launchCommand, shell=True)
        os.chdir(curDir)

class RunSettings():
    def __init__(self, account="uio", dryRun=False, wallTime=3600*48, nodes=1,
                 tasksPerNode=15, executable="lmp_mpi", jobName="job", slurm=False, arguments=""):
        self.slurm = slurm
        self.account = account
        self.jobName = jobName
        self.nodes = nodes
        self.dryRun = dryRun
        self.tasksPerNode = tasksPerNode
        self.memoryPerNode = "3600M"
        self.exclusive = False
        self.ntasksOnly = False
        self.executable = executable
        self.arguments = ""
        self.minutes, self.seconds = divmod(wallTime, 60)
        self.hours, self.minutes = divmod(self.minutes, 60)

    def createSlurmScriptString(self):
        script = ""
        script += "#!/bin/sh\n"
        script += "# Job name:\n"
        script += "#SBATCH --job-name={}\n".format(self.jobName)
        script += "#\n"
        script += "# Project:\n"
        script += "#SBATCH --account={}\n".format(self.account)
        script += "#\n"
        script += "# Wall clock limit:\n"
        script += "#SBATCH --mail-type=ALL\n"
        script += "#SBATCH --time='%02d:%02d:%02d'\n" % (self.hours, self.minutes, self.seconds)
        script += "#\n"
        script += "# Max memory usage per task:\n"
        script += "#SBATCH --mem-per-cpu={}\n".format(self.memoryPerNode)
        script += "#\n"
        script += "#SBATCH --cpus-per-task=16\n"
        if self.exclusive:
            script += "#SBATCH --exclusive\n"
        script += "\n"
        script += "## Set up job environment\n"
        script += "source /cluster/bin/jobsetup\n"
        script += "module load gcc\n"
        script += "\n"
        script += "## Run command\n"
        script += "./{} {}\n".format(self.executable, self.arguments)
        return script

    def createLaunchCommand(self):
        return "mpirun -n {} {} {}".format(self.tasksPerNode, self.executable, self.arguments)
