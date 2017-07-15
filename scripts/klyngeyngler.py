#!/bin/python3
# -*- coding: utf-8 -*-
import argparse
from datetime import datetime
from itertools import permutations, product
from operator import attrgetter
import subprocess
from distutils.file_util import copy_file
import sys
import traceback
import os
import logging
import re
import collections


class Parser:
    """ Reads and parses a configuration file """
    def __init__(self, filename):
        self.filename = filename

    def parse(self, delimiter=r'\s+', comment='#', metachar='¤'):
        self.comment = comment
        self.delimiter = delimiter
        self.metachar = metachar
        """ Parses the file into a dict of keywords and their value """
        logger.info("Parsing file: %s", self.filename)
        with open(self.filename, 'r') as infile:
            lines = [line.rstrip() for line in infile.readlines()]

        # Extract metalines and execute
        self.metalines = [line[1:] for line in lines
                          if line.startswith(self.metachar)]
        logger.debug("Executing metalines: %s", self.metalines)
        [exec(line) for line in self.metalines]

        # Filter out empty lines and comments
        lines = filter(lambda l: len(l) > 0 and
                       not l.startswith(comment) and
                       not l.startswith(metachar),
                       lines)

        parameters = {key: value for key, value in self.parseLines(lines)}
        logger.info("Parsing was succesful")
        logger.debug("Parameters: %s", parameters)
        return parameters

    def parseLines(self, lines):
        """ Parses parseable lines from the configuration file """
        for line in lines:
            key, *value = re.split(self.delimiter, line)
            if len(value) < 2:
                try:
                    # Handle all floats and ints
                    value = eval(value[0])
                except:
                    # Strings
                    value = value[0]
            else:
                # Handle expression consisting of everything after the first delimiter
                value = self.handleExpression(line[line.find(value[0]):])
            yield key, value

    def handleExpression(self, exp):
        # If no comment is present, assume that the entire string is valid
        if self.comment in exp:
            # Extract the substring between the final character and the comment
            value = exp[:exp.find(self.comment)].rstrip()
        try:
            value = eval(value)
        finally:
            return value


class Jobfile:
    """ Creates a jobfile for running jobs on the cluster """
    def __init__(self, **kwargs):
        self.cluster = 'abel'
        self.jobname = 'bob'
        self.account = 'uio'
        self.wallTime = 24
        self.memPerCPU = '3600M'
        self.cpusPerTask = 16
        self.modules = []
        self.customCommands = []
        self.infiles = []
        self.outfiles = []
        # Overwrite any local variables with the ones given
        # as keyword arguments
        logger.debug("Updating Jobfile dictionary with: %s", kwargs)
        for key in kwargs:
            if key not in self.__dict__:
                logger.warning("Non-standard keyword: %s", key)
        self.__dict__.update(kwargs)
        self.minutes, self.seconds = divmod(self.wallTime, 60)
        self.hours, self.minutes = divmod(self.minutes, 60)
        self.__dict__.update(kwargs)

    def makeJobfile(self):
        script = (
            "#!/bin/sh\n"
            "# Job name:\n"
            "#SBATCH --job-name={jobname}\n"
            "#\n"
            "# Project: \n"
            "#SBATCH --account={account}\n"
            "# Wall time limit\n"
            "#SBATCH --time={hours:02d}:{minutes:02d}:{seconds:02d}\n"
            "#\n"
            "# Max memory usage per core:\n"
            "#SBATCH --mem-per-cpu={memPerCPU}\n"
            "#SBATCH --cpus-per-task={cpusPerTask}\n"
            "#\n\n"
            "# Set up the job environment\n"
        ).format(**self.__dict__)
        script += self.makeEnvironment()
        logger.debug("Creating jobfile: %s", script)
        return script

    def makeEnvironment(self):
        if self.cluster == 'abel':
            env = (
                'source /cluster/bin/jobsetup\n'
                '# Remove any previously load modules\n'
                'module purge\n'
                '# Exit on any errors after this point\n'
                'set -o errexit\n'
            )
            for module in self.modules:
                env += 'module load {module}\n'.format(module=module)
            env += '# Copy files to work directory\n'
            for f in self.infiles:
                env += 'cp -r $SUBMITDIR/{} $SCRATCH\n'.format(f)
            env += '# Mark outfiles for automatic copying to $SUBMITDIR\n'
            env += 'chkfile {}\n'.format(' '.join(self.outfiles))
            env += 'cd $SCRATCH\n'
            env += '# Run custom commands\n'
        else:
            raise RuntimeError('Cluster not recognized: {}'.format(self.cluster))
        for command in self.customCommands:
            env += "{}\n".format(command)
        return env

    def addModule(self, module):
        self.modules.append(module)

    def addCommand(self, command):
        self.customCommands.append(command)


class JobRunner:
    """ Manages and runs jobs on a cluster """
    def __init__(self, name, configpath, projectpaths, dryrun=False, **kwargs):
        logger.info("Attemping to parse {}".format(configpath))
        parser = Parser(configpath)
        self.configpath = configpath
        self.projectpaths = projectpaths
        self.name = name
        self.parameters = parser.parse()
        self.jobfilekwargs = kwargs
        self.jobdir = 'jobs'
        self.dryrun = dryrun

    def run(self):
        infiles = [self.configpath] + self.projectpaths
        jobfile = Jobfile(**self.jobfilekwargs, infiles=infiles)
        mkdir(self.jobdir)
        i = 0
        for configStr in self.makeConfigFile():
            jobname = self.name+str(i)
            jobfile.jobname = jobname
            currDir = self.handleFiles(jobname, configStr)
            self.writeJobfile(currDir, jobfile.makeJobfile())
            self.runJob(currDir)
            i += 1

    def runJob(self, src):
        logger.info("Running job")
        if self.dryrun:
            return

        wd = os.getcwd()
        os.chdir(src)
        subprocess.call(['sbatch', 'jobfile'])
        os.chdir(wd)

    def writeJobfile(self, path, jobfile):
        path = os.path.join(path, 'jobfile')
        with open(path, 'w') as jfile:
            jfile.write(jobfile)

    def handleFiles(self, jobname, configStr):
        currDir = os.path.join(self.jobdir, jobname)
        mkdir(currDir)
        configPath = os.path.join(currDir, self.configpath)
        for src in self.projectpaths:
            dst = os.path.join(currDir, src)
            logger.debug("Copying %s to %s", src, dst)
            cp(src, dst)

        logger.debug("Writing configuration %s to %s", self.configpath, configPath)
        mkdirRec(configPath)
        with open(configPath, 'w') as configFile:
            configFile.write(configStr)

        return currDir

    def makeConfigFile(self):
        nonIterables = ''.join(["{} {}\n".format(k, v)
                                for k, v in self.nonIterables()])
        for keys, prod in self.iterables():
            configStr = nonIterables
            configStr += ''.join(["{} {}\n".format(k, v)
                                  for k, v in zip(keys, prod)])
            yield configStr

    def iterables(self):
        """ Generator yielding the product of every iterable keyword """
        iterables = {}
        for key, value in self.parameters.items():
            if (not isinstance(value, str) and
                isinstance(value, collections.Iterable)):
                iterables[key] = value
        keys = list(iterables.keys())
        for prod in product(*iterables.values()):
            logger.debug("Yielding %s", str(prod))
            yield [keys, prod]

    def nonIterables(self):
        nonIterables = {}
        for key, value in self.parameters.items():
            if not isinstance(value, collections.Iterable) or isinstance(value, str):
                nonIterables[key] = value
        return nonIterables.items()


class SortingHelpFormatter(argparse.RawTextHelpFormatter):
    """ Sorts the arguments """
    def add_arguments(self, actions):
        actions = sorted(actions, key=attrgetter('option_strings'))
        super(SortingHelpFormatter, self).add_arguments(actions)

# GLOBAL FUNCTIONS


def getArgs():
    """
    Manages the argparse module.
    Any changes to the arguments from terminal are done here
    Parameters: none
    Returns: class instance of 'argparse.Namespace'
    Algorithm: Add arguments to argparse.ArgumentParser(), fix some arguments
               regarding logging, and return the parsed arguments.
    """
    parser = argparse.ArgumentParser(description=("Automates the process of setting up and "
                                                  "running jobs on clusters"),
                                     formatter_class=SortingHelpFormatter)
    parser.add_argument('-d', '--debug',
                        help="Print debugging information to stdout and file",
                        action='store_true')
    parser.add_argument('--dryrun',
                        help="Do actions as normally, but do not run jobs",
                        action='store_true')
    parser.add_argument('-c', '--configpath',
                        help="Path to the configuration file")
    parser.add_argument('-p', '--projectpaths',
                        help="Paths to all other files needed for the job",
                        nargs='+')

    return parser.parse_args()


def setUpLogger(debugmode=False):
    global logger
    logger = logging.getLogger()
    level = logging.DEBUG if debugmode else logging.INFO

    # File Handler - writes log messages to log file
    logHandler = logging.FileHandler('log.log')
    logHandler.setLevel(logging.DEBUG)

    # File Handler - writes error messages to error file
    errorHandler = logging.FileHandler('errors.log')
    errorHandler.setLevel(logging.ERROR)

    # Console handler - writes log messages to the console
    consoleHandler = logging.StreamHandler()
    consoleHandler.setLevel(level)

    formatter = logging.Formatter('%(asctime)s - %(levelname)-8s - %(message)s')
    logHandler.setFormatter(formatter)
    errorHandler.setFormatter(formatter)
    consoleHandler.setFormatter(formatter)

    # Connect the handlers to the logger
    logger.addHandler(logHandler)
    logger.addHandler(errorHandler)
    logger.addHandler(consoleHandler)


def excepthook(ex_cls, ex, tb):
    """ Replace the default excepthook

        The excepthook is called when the script experiences an exception.
        This replaces the default one so as to log exceptions
        Parameters: ex_cls: exception class
                    ex: exception instance
                    tb: the traceback
        Returns:    None
        Algorithm: Log the traceback, terminate any active children and exit
                   the program
        """
    # Log the traceback in a readable format
    logger.critical(''.join(traceback.format_tb(tb)))
    # Log a short summary of the exception
    logger.critical('{0}: {1}'.format(ex_cls.__name__, ex))
    sys.exit()


def mkdir(path):
    logger.debug("Making %s", path)
    if not os.path.exists(path):
        os.mkdir(path)


def mkdirRec(path):
    logger.debug("Making %s recursively.", path)
    if not os.path.exists(path):
        root = os.path.split(path)[0]
        if not os.path.exists(root):
            mkdirRec(root)
        else:
            os.mkdir(path)


def cp(src, dst):
    # Check if the source exists
    if not os.path.exists(src):
        raise IOError("{} does not exist".format(src))
    # If the src is a dir, make the dst as dir
    if os.path.isdir(src):
        if not os.path.exists(dst):
            mkdirRec(dst)
        else:
            if not os.path.isdir(dst):
                raise IOError("{} already exists as non-dir".format(dst))
        # And copy all files within src to dst
        for root, dirs, files in os.walk(src):
            for d in dirs:
                D = os.path.join(root, d)
                dstD = os.path.join(dst, d)
                logger.info('Internally cp dir %s to %s', D, dstD)
                cp(D, dstD)
            for f in files:
                F = os.path.join(root, f)
                dstF = os.path.join(dst, f)
                logger.debug('Internally cp file %s to %s', F, dstF)
                cp(F, dstF)
    else:
        # If src is file, copy the file to dst and make path if necessary
        try:
            copy_file(src, dst)
        except:
            mkdirRec(os.path.split(dst)[0])
        copy_file(src, dst)

if __name__ == '__main__':
    args = getArgs()

    # Set up logging
    logging.basicConfig(level=logging.DEBUG, filename='log.log',
                        filemode="w", format="%(asctime)s - %(levelname)-8s - %(message)s")
    logging.getLogger("__name__").addHandler(logging.NullHandler())
    setUpLogger(args.debug)
    sys.excepthook = excepthook
    jobr = JobRunner('friction', 'input/parameters.txt', ['simulate', 'input', 'constructLattice.py'], dryrun=args.dryrun,
                     customCommands=['mkdir output', 'python3 constructLattice.py input/parameters.txt input/lattice.xyz', './simulate'],
                     wallTime=86400, modules=['python3', 'gcc'], outfiles=['output/'])
    jobr.run()
