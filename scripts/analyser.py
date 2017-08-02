#!/bin/python3
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import argparse
import os
import sys
import re
import glob as _glob
from string import Formatter
from operator import attrgetter
from mpl_toolkits.mplot3d import Axes3D
from scipy.signal import argrelmax
from scipy.stats import linregress

try:
    import seaborn as sns
except ImportError:
    pass

    # Global variables
join = os.path.join


class DataReader:
    """ Reads and converts the binary data into numpy arrays.

    Attributes:
        wd: Path to the working directory.
        params: A dict mapping parameter keys to their values.
            Should come from Parser.parse.
    """

    def __init__(self, parameters, wd=os.getcwd()):
        self.wd = os.path.abspath(wd)
        self.params = parameters

    def read(self, path):
        """Wrapper around numpy.fromfile. """
        return np.fromfile(join(self.wd, path))

    def resize(self, array, start=0, step=1, stop=None):
        """Resizes an array

        Optionally shrinks the array by selecting a subarray before
        resizing.
        """
        if stop is None: stop = len(array)
        # array = array[start:stop:step]
        x = len(array) / self.params['numBottomNodes']
        y = self.params['numBottomNodes']

        assert(int(x) == x)
        x = int(x)
        return np.reshape(array, (x, y))


class File:
    def __init__(self, filename, parameters, src):
        self.filename = filename
        self.parameters = parameters
        self.reader = DataReader(parameters, src)
        if 'bin' in self.filename:
            self.frequency = self.parameters['freq' +
                                             self.filename[0].upper() +
                                             self.filename[1:-4]]
        elif 'xyz' in self.filename:
            self.frequency = self.parameters['freqXYZ']
        else:
            raise RuntimeError("Don't know how to read {}".format(filename))

    def get(self, persistent=True):
        if persistent is True and hasattr(self, 'data'):
            return self.data

        sys.stdout.write("Reading {}...".format(self.filename))
        data = self.reader.read(self.filename)
        sys.stdout.write("done.\n")
        if 'interface' in self.filename.lower():
            y = self.parameters['numBottomNodes']
            x = len(data)/y
            if int(x) != x:
                raise RuntimeError(("Could not resize {}. len={}, "
                                    "x={} and y={}".format(self.filename,
                                                           len(data),
                                                           x, y)))
            x = int(x)
            data.resize((x, y))
        elif 'beam' in self.filename.lower():
            y = self.parameters['numTopNodes']
            x = len(data)/y
            if int(x) != x:
                raise RuntimeError(("Could not resize {}. len={}, "
                                    "x={} and y={}".format(self.filename,
                                                           len(data),
                                                           x, y)))
            x = int(x)
            data.resize((x, y))
        elif 'all' in self.filename.lower():
            pass
        self.xlength = data.shape[0]

        if persistent:
            self.data = data

        return data

    def delete(self):
        del self.data

    def time(self):
        assert self.xlength is not None, "Read the file before requesting time"
        dt = self.frequency*self.parameters['step']
        return dt*np.arange(0, self.xlength, 1)


class Lattice:
    """ Wrapper around useful parameters """
    def __init__(self, parameters):
        self.nx = parameters['nx']
        self.ny = parameters['ny']
        self.numBottom = parameters['numBottomNodes']
        self.numTop = parameters['numTopNodes']
        self.numNodes = parameters['numNodes']
        self.height = parameters['grooveHeight']
        self.size = parameters['grooveSize']
        self.speed = parameters['vD']
        self.angle = parameters['beamAngle']


class Parser:
    @staticmethod
    def parse(path, delimiter=r'\s+', comment='#'):
        """ Parses the file into a dict of keywords and their value.

        Reads all lines into a list, stripping each line for whitespace.
        Empty lines and lines starting with the comment character are
        removed. Next the on-line comments are removed, and the cleaned
        lines are sent to Parser.parseLines along with the delimiter.

        Args:
            path: Path to the parameter file to be read.
            delimiter: A regex-expression used to delimit a
                    parameter from its value. Default is any
                    whitespace.
           comment: A character denoting comments
        Returns:
            A dict mapping parameters to their values of correct type.
        """
        with open(path, 'r') as infile:
            lines = [line.rstrip() for line in infile.readlines()]

        # Filter out empty lines and comments
        lines = filter(lambda l: len(l) > 0 and
                       not l.startswith(comment),
                       lines)
        # Remove inline comments
        temp = []
        for line in lines:
            if comment in line:
                line = line[:line.find(comment)].rstrip()
            temp.append(line)
        lines = temp

        parameters = {key: value for key, value
                      in Parser.parseLines(lines, delimiter)}
        return parameters

    @staticmethod
    def parseLines(lines, delimiter):
        """ Parses parseable lines from the configuration file.

        Splits each line according to the delimiter into a head
        and a tail. If the tail is greater than two, the line
        is not parseable, and is ignored. Otherwise, the tail's
        value is attempted to be converted to a float or int using
        eval. If it fails, it is assumed to be a string.

        Args:
            lines: A list of lines to parse.
            delimiter: A regex expression used to split
                     each line into a head and tail.
        Yields:
            A tuple of a key and its value
        """
        for line in lines:
            key, *value = re.split(delimiter, line)
            if len(value) < 2:
                try:
                    # Handle all floats and ints
                    value = eval(value[0])
                except:
                    # Strings
                    value = value[0]
            yield key, value


class Analyzer:
    def __init__(self, workDirectory, outputPath, inputPath):
        """Sets the self arguments.

        Args:
             workDirectory: The root of the project where the
                          analyzer will be working
             outputPath: Path to the output files produced by the
                       executable which will be analyzed
             inputPath: Path to the configuration files and such
                     used by the executable to produce the output
        """
        self.wd = os.path.abspath(workDirectory)
        self.input = inputPath
        self.output = outputPath

    def readParameters(self, filename):
        """Reads parameters from file.

        Uses the attribute inputpath and argument filename
        to create the path to the file to be read.
        The static method Parser.parse reads the file and
        returns a dict of the parameters and their values.
        This is used to initialize a DataReader.

        Args:
             filename: Name of the parameter file
        """
        path = os.path.join(self.input, filename)
        self.parameters = Parser.parse(path)
        assert(len(self.parameters) > 0)
        self.datareader = DataReader(self.parameters, self.output)

    def read(self, path):
        """A wrapper around DataReader.read.

        Args:
             path: Path to be read by DataReader
        Returns:
             A numpy array of loaded data
        """
        sys.stdout.write("Reading {}...".format(path))
        data = self.datareader.read(path)
        sys.stdout.write("done.\n")
        return data

    def readAndResize(self, path, start=0, step=2, stop=None):
        """Wrapper around Datareader.resize."""
        data = self.read(path)
        return self.datareader.resize(data, start, step, stop)

    def setPlotPath(self, path):
        self.plotpath = path

    def plotable(fn):
        decoratorKeywords = ('show', 'save')
        def decorator(*args, **kwargs):
            fnKwargs = {k: v for k, v in kwargs.items()
                        if k not in decoratorKeywords}
            dcKwargs = {k: v for k, v in kwargs.items()
                        if k in decoratorKeywords}
            res = fn(*args, **fnKwargs)
            if ('show' in dcKwargs and dcKwargs['show'] is True):
                plt.show()
            if ('save' in dcKwargs and dcKwargs['save'] is True):
                if 'savepath' not in fn.__code__.co_varnames:
                    raise RuntimeError("Expected variable "
                                       "'savepath' in {}".format(fn.__name__))
                name = fn.__code__.co_consts[
                    fn.__code__.co_varnames.index('savepath')]
                self = args[0]
                plt.savefig(join(self.plotpath, name), dpi=1200)
            return res
        return decorator

    def restrict(fn):
        def decorator(*args, **kwargs):
            cls = args[0]
            result = None
            dcKwargs = {}
            if 'restrict' in kwargs:
                for k, v in kwargs['restrict'].items():
                    dcKwargs[k] = v if isinstance(v, (list, set, tuple)) else [v]

            flag = True
            for key, vals in dcKwargs.items():
                if key in cls.parameters and cls.parameters[key] not in vals:
                    flag = False
                    break

            fnKwargs = {k: v for k, v in kwargs.items()
                        if k != 'restrict'}

            if flag:
                result = fn(*args, **fnKwargs)
            return result
        return decorator


class AnalyzerManager:
    def __init__(self):
        self.analyzers = []

    def __getattr__(self, name):
        def func(*args, **kwargs):
            returnVals = []
            for analyzer in self.analyzers:
                try:
                    returnVals.append(getattr(analyzer, name)(*args, **kwargs))
                except AttributeError as attrErr:
                    raise attrErr
            return returnVals
        return func

    def setUp(self, args):
        globbler = Globbler(args.search_path)
        self.analyzers = globbler.glob(className=FrictionAnalyzer,
                                       pattern=args.pattern,
                                       outputPattern=args.output,
                                       inputPattern=args.input)
        for analyzer in self.analyzers:
            analyzer.setPlotPath(args.plotpath)
            analyzer.readParameters(args.paramname)
            analyzer.readNumberOfNodes()
            analyzer.addAlias()
            analyzer.lattice = Lattice(analyzer.parameters)
            analyzer.setUpFiles()


class FrictionAnalyzer(Analyzer):
    def readNumberOfNodes(self):
        filename = 'lattice.xyz'
        blockSize = 10000  # Each xyz-block is atleast larger than 0
        numberOfBottomNodes = 0
        numberofTopNodes = 0
        with open(join(self.input, filename)) as xyz:
            for i, line in enumerate(xyz):
                if i == 0:
                    blockSize = int(line)
                elif 1 < i < blockSize+2:  # Second line is the comment
                    if 'B' in line:
                        numberOfBottomNodes += 1
                    if 'T' in line:
                        numberofTopNodes += 1
                elif i > blockSize+2:
                    break
        # Since dicts are muteable, self.datareader.params is
        # simulanously updated.
        self.parameters['numBottomNodes'] = numberOfBottomNodes
        self.parameters['numTopNodes'] = numberofTopNodes
        self.parameters['numNodes'] = blockSize

    def setUpFiles(self):
        self.interfacePosition = File('interfacePosition.bin',
                                      self.parameters, self.output)
        self.interfaceVelocity = File('interfaceVelocity.bin',
                                      self.parameters, self.output)
        self.interfaceAttachedSprings = File('interfaceAttachedSprings.bin',
                                             self.parameters, self.output)
        self.allPosition = File('allPosition.bin',
                                self.parameters, self.output)
        self.allVelocity = File('allVelocity.bin',
                                self.parameters, self.output)
        self.allEnergy = File('allEnergy.bin',
                              self.parameters, self.output)
        self.allForce = File('allForce.bin',
                             self.parameters, self.output)
        self.pusherForce = File('pusherForce.bin',
                                self.parameters, self.output)
        self.xyz = File('model.xyz',
                        self.parameters, self.output)
        self.beamTorque = File('beamTorque.bin',
                               self.parameters, self.output)
        self.beamShearForce = File('beamShearForce.bin',
                                   self.parameters, self.output)
        self.activeFiles = []
        self.files = [self.interfacePosition, self.interfaceVelocity,
                      self.interfaceAttachedSprings, self.allPosition,
                      self.allVelocity, self.allEnergy, self.allForce,
                      self.pusherForce, self.xyz, self.beamTorque,
                      self.beamShearForce]

    def readAll(self):
        """ Reads all binary files available as described in parameters.

        Reads the file writing flags in the parameter file to
        determine which binary files to load.
        """
        writtenOutput = {param for param, flag in
                         self.parameters.items()
                         if param.startswith('write')
                         and flag == 1}

    def addAlias(self):
        self.parameters['size'] = self.parameters['grooveSize']
        self.parameters['height'] = self.parameters['grooveHeight']
        self.parameters['angle'] = self.parameters['beamAngle']
        self.parameters['speed'] = self.parameters['vD']

    @Analyzer.plotable
    def plotPositionInterface(self):
        pos = self.interfacePosition.get() - self.interfacePosition.get()[0, :]
        print(pos.shape)
        plt.pcolormesh(pos[:, :, 1], cmap=plt.get_cmap('jet'))
        plt.colorbar()
        plt.title("Relative position of the interface")

    @Analyzer.restrict
    @Analyzer.plotable
    def plotAttachedSprings(self):
        savepath = 'attachedSprings.png'
        data = self.interfaceAttachedSprings.get()
        time = self.interfaceAttachedSprings.time()
        plt.pcolormesh(data, cmap=plt.get_cmap('jet'))
        ticks = [int(t) for t in np.linspace(0, data.shape[0]-1, 10)]
        print(ticks, time.shape)
        plt.yticks(ticks, time[ticks])
        plt.colorbar()
        plt.title(("Percent of attached springs for "
                   "size {} and height {}".format(self.lattice.size,
                                                  self.lattice.height)))
        plt.xlabel("Block index")
        plt.ylabel("Time [s]")


    def getTimeArray(self, data, freqName):
        """Method for quickly getting a time array for a spesific type of data.

        Args:
            freqName: The name found in the parameters file
                corresponding to the given data.
        """
        return np.arange(len(data))*(self.parameters['step'] *
                                     self.parameters[freqName])

    def getReducedSpringAttachments(self, cutoffPoint):
        """

        If the number of springs attached is less then the
        cutoffPoint (percentage) times the total number
        of springs belonging to that node, the node is treated
        as dislodged, otherwise it is treated as attached.
        """
        return np.sum(np.where(self.readAndResize('node_springs_attached_interface.bin') >= self.parameters['ns']*cutoffPoint,0,1))

    @Analyzer.restrict
    @Analyzer.plotable
    def plotReducedSpringAttachments(self, cutoffPoint=0.1):
        """Plots the number of attached nodes

        Show desides if the plot will be shown, or used in an other figure.
        """
        data = self.getReducedSpringAttachments(cutoffPoint)
        time = self.getTimeArray(data, 'freqNodeSpringsAttachedInterface')
        plt.plot(time, data)
        plt.title("Number of Nodes Attached to the Ground")
        plt.xlabel("Time [s]")
        plt.ylabel("Number")

    def getFrontVelocities(self, cutoffPoint=0.1):
        """Finds the front velocity of the rapture of the springs.

        Symmetric differentiation is used to find the velocity.
        """
        data = self.getReducedSpringAttachments(cutoffPoint)
        v = np.zeros(len(data) - 2)
        h = self.parameters["step"]*self.parameters['freqNodeSpringsAttachedInterface']
        for i in range(1, len(data)-1):
            v[i-1] = (data[i+1] - data[i-1])/(2.0*h)

        del data
        return np.abs(v)*self.parameters['d']

    @Analyzer.plotable
    def plotFrontVelocities(self, cutoffPoint=0.1):
        """ Plots the Front Velocities.
        """
        data = self.getFrontVelocities(cutoffPoint)
        time = np.arange(len(data))*self.parameters['step']*self.parameters['freqNodeSpringsAttachedInterface']
        plt.plot(time, data)
        plt.title("Front Velocity for size %g and height %g" % (self.getGrooveDim()[1],
                                                                self.getGrooveDim()[0]))
        plt.xlabel("Time [s]")
        plt.ylabel("Velocity [m/s]")

    def getStaticFrictionCoefficient(self, mean=False):
        """Reads the shear forces on the top rod.

        The array is then resized to a matrix with the shape [time,block].
        If the parameter mean == True, mean forces is calculated,
        otherwise the total force is calculated.
        The static friction coefficent is returned as the
        maximum value of the mean or total shear force  on the rod
        divided by the normal force.

        TODO: Get the friction force from the flat case,
        and use that to normalize the static friction for grooves.
        """
        shearForce_on_rod = self.read('beam_shear_force.bin')
        shearForce_on_rod = np.reshape(shearForce_on_rod,
                                       (len(shearForce_on_rod)/self.parameters['nx'],
                                       self.parameters['nx']))
        if mean:
            shearForce_on_rod = np.mean(shearForce_on_rod, axis=1)
        else:
            shearForce_on_rod = np.sum(shearForce_on_rod, axis=1)

        staticCoefficient = np.max(-shearForce_on_rod)/np.float(self.parameters['fn'])
        del shearForce_on_rod
        return staticCoefficient

    @Analyzer.plotable
    def plotRodShearForceTimeSeries(self, mean=False, useAngleGreaterThanZero = True, certainHeight = None,certainSize = None):
        if useAngleGreaterThanZero or self.parameters['beamAngle'] == 0:
            if (certainHeight is not None) and self.parameters['grooveHeight'] == certainHeight:
                if (certainSize is not None) and self.parameters['grooveSize'] == certainSize:
                    data = self.getRodShearForceTimeSeries(mean)
                    time = np.arange(len(data))*self.parameters['step']*self.parameters['freqBeamShearForce']
                    plt.plot(time,data,label = "size %g height %g"%(self.getGrooveDim()[1],self.getGrooveDim()[0]))
                    plt.title("Shear force of rod for size %g and height %g"%(self.getGrooveDim()[1],self.getGrooveDim()[0]))
                    plt.xlabel("Time [s]")
                    if mean:
                        plt.ylabel(r"Mean $F_T/F_N$")
                    else:
                        plt.ylabel(r"Total $F_T/F_N$")

    def getYForceBeam(self):
        force = self.read('node_force_all.bin')
        readStep = 2*(self.parameters['numNodes'] - self.parameters['nx'] + 1)
        beamForceY = force[::readStep]
        del force
        return beamForceY


    def getRodShearForceTimeSeries(self, mean=False):
        """Returns a time serier of the shear force on the rod.

        Much of the processing is the same as for the method above.
        """

        shearForce_on_rod = self.read('beam_shear_force.bin')
        shearForce_on_rod = -1*np.reshape(shearForce_on_rod,
                                          (len(shearForce_on_rod)/self.parameters['nx'],
                                           self.parameters['nx']))
        if mean:
            shearForce_on_rod = np.mean(shearForce_on_rod, axis=1)
        else:
            shearForce_on_rod = np.sum(shearForce_on_rod, axis=1)

        return shearForce_on_rod/np.float(self.parameters['fn'])

    def getLocalMax(self):
        shearForce = -1*self.beamShearForce.get()
        force = np.sum(shearForce, axis=1)/self.parameters['fn']
        locMaxIndex = argrelmax(force)[0]
        # timeArray = self.getTimeArray(force, 'freqBeamShearForce')[argrelmax(force)[0]]
        time = self.beamShearForce.time()[locMaxIndex]
        locMax = force[locMaxIndex]
        # return time, locMax
        return self.beamShearForce.time(), force

    @Analyzer.plotable
    @Analyzer.restrict
    def plotLocalMax(self, i=[0]):
        lines = ['-', '--', '-.']
        time, data = self.getLocalMax()
        plt.plot(time, data,
                 label='Size {}, height {}'.format(self.lattice.size,
                                                   self.lattice.height),
                 linestyle=lines[i[0]])
        self.plotReleaseTime()
        plt.xlabel("Time [s]")
        plt.ylabel(r"$F_S/F_N$")
        plt.title("Local Max for Beam Shear Force for size %g height %g"%(self.lattice.size,
                                                                          self.lattice.height))
        i[0] += 1
        i[0] %= len(lines)

    @Analyzer.restrict
    def getRigressionLine(self, endTimes=[0, 0]):
        data, time = self.getLocalMax()
        startIndex = np.argwhere(time >= endTimes[0])[0]
        endIndex = np.argwhere(time >= endTimes[1])[0]
        data = np.copy(data[startIndex:endIndex])
        timeReduced = np.copy(time[startIndex:endIndex])
        slope, intercept, r_value, p_value, std_err = linregress(timeReduced,data)
        return time*slope + intercept, time

    @Analyzer.restrict
    @Analyzer.plotable
    def plotRegressionLine(self, endTimes=[0, 0]):
        data, time = self.getRigressionLine(endTimes)
        plt.plot(time, data)

    @Analyzer.restrict
    def findCoeffFromSquareError(self, endTimes=[0, 0], maxErrorAllowed=0.001):
        """
        This function uses a regresssion line to find the
        first major slip event by using the square error
        """
        dataRegression, timeRegression = self.getRigressionLine(endTimes)
        data, time = self.getLocalMax()
        error = (data - dataRegression)**2
        coeff = data[np.argwhere(error <= maxErrorAllowed)[-1]][0]
        return coeff

    def plotReleaseTime(self):
        plt.axvline(x=self.parameters['releaseTime']*self.parameters['step'],
                    linestyle='--', color='k')

    def makePlots(self):
        self.normalForce = self.readAndResize('normal_force.bin')
        self.plotNormalForce()
        del self.normalForce
        self.pusherForce = self.read('pusher_force.bin')
        self.plotPusherForce()
        del self.pusherForce
        self.positionInterface = self.readAndResize(
            'node_position_interface.bin')
        self.plotPositionInterface()
        del self.positionInterface
        self.attachedSprings = self.readAndResize(
            'node_springs_attached_interface.bin')
        self.plotAttachedSprings()
        del self.attachedSprings
        self.shearForce = self.readAndResize('shear_force.bin')
        self.plotShearForce()
        del self.shearForce


class SortingHelpFormatter(argparse.RawTextHelpFormatter):
    """ Custom formatter for argparse help """

    def add_arguments(self, actions):
        actions = sorted(actions, key=attrgetter('option_strings'))
        super(SortingHelpFormatter, self).add_arguments(actions)


class Globbler:
    def __init__(self, searchPath):
        self.searchPath = searchPath

    def glob(self, className, pattern,
             inputPattern, outputPattern):
        searchPattern = join(self.searchPath, pattern)
        print("Searching with {}".format(searchPattern))
        matches = _glob.glob(searchPattern)
        instanceList = []
        for match in matches:
            input = _glob.glob(join(match, inputPattern))
            output = _glob.glob(join(match, outputPattern))
            if input == []:
                print("Could not find input in {}".format(match))
                continue
            if output == []:
                print("Could not find output in {}".format(match))
                continue
            if len(input) > 1:
                raise RuntimeError("Input pattern too ambiguous")
            if len(output) > 1:
                raise RuntimeError("Output pattern too ambiguous")

            analyzer = className(workDirectory=match,
                                 outputPath=output[0],
                                 inputPath=input[0])
            instanceList.append(analyzer)

        return instanceList


class Compare:
    def __init__(self, instanceList):
        self.instanceList = instanceList

    def makeStaticCoeffArray(self, useAngleGreaterThanZero=False):
        instanceLength = len(self.instanceList)
        instanceIndex = range(instanceLength)
        if(not useAngleGreaterThanZero):
            instanceLength = 0
            instanceIndex = []
            for i in range(len(self.instanceList)):
                if self.instanceList[i].parameters['beamAngle'] == 0:
                    instanceLength += 1
                    instanceIndex.append(i)

        self.staticCoefficiantArray = np.zeros([len(self.instanceList), 3])
        for i, j in zip(range(instanceLength), instanceIndex):
            self.staticCoefficiantArray[i, 0] = self.instanceList[j].getStaticFrictionCoefficient()
            self.staticCoefficiantArray[i, 1] = self.instanceList[j].getGrooveDim()[0]
            self.staticCoefficiantArray[i, 2] = self.instanceList[j].getGrooveDim()[1]

    def makeStaticCoeffArrayFromLinReg(self, endTimes, maxErrorAllowed,
                                       useAngleGreaterThanZero=False):
        instanceLength = len(self.instanceList)
        instanceIndex = range(instanceLength)
        if(not useAngleGreaterThanZero):
            instanceLength = 0
            instanceIndex = []
            for i in range(len(self.instanceList)):
                if self.instanceList[i].parameters['beamAngle'] == 0:
                    instanceLength += 1
                    instanceIndex.append(i)

        self.staticCoefficiantArray = np.zeros([len(self.instanceList), 3])
        for i, j in zip(range(instanceLength), instanceIndex):
            self.staticCoefficiantArray[i, 0] = self.instanceList[j].getStaticFrictionCoefficient()#findCoeffFromSquareError(endTimes=endTimes, maxErrorAllowed=maxErrorAllowed)
            """
            try:
                self.staticCoefficiantArray[i, 0] = self.instanceList[j].getStaticFrictionCoefficient()#findCoeffFromSquareError(endTimes=endTimes, maxErrorAllowed=maxErrorAllowed)
            except:
                self.staticCoefficiantArray[i, 0] = np.nan
            """
            self.staticCoefficiantArray[i, 1] = self.instanceList[j].getGrooveDim()[0]
            self.staticCoefficiantArray[i, 2] = self.instanceList[j].getGrooveDim()[1]

    def plotCoeffHeight(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()

        plt.plot(self.staticCoefficiantArray[:, 1],
                 self.staticCoefficiantArray[:, 0])
        plt.show()

    @Analyzer.restrict
    @Analyzer.plotable
    def plotCoeffLinRegSize(self, endTimes = [0.05,0.2], maxErrorAllowed = 0.001,certainHeight = None):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArrayFromLinReg(endTimes, maxErrorAllowed)
        if certainHeight is not None:
            plotIndex = np.where(self.staticCoefficiantArray[:,1] == certainHeight)
            plt.scatter(self.staticCoefficiantArray[plotIndex,2][0],self.staticCoefficiantArray[plotIndex,0][0])
            plt.title("Static Coefficent for Height %g" %certainHeight)
        else:
            plottedHeights = []
            for h in self.staticCoefficiantArray[:,1]:
                if h not in plottedHeights:
                    plottedHeights.append(h)
            colors = iter(cm.jet(np.linspace(0, 1, len(plottedHeights))))
            for h in sorted(plottedHeights):
                HIndex = np.where(self.staticCoefficiantArray[:,1] == h)
                plt.scatter(self.staticCoefficiantArray[HIndex,2][0],self.staticCoefficiantArray[HIndex,0][0], label = "height %g" %h, color = next(colors))


        plt.xlabel("Groove Size")
        plt.ylabel(r"$F_s/F_N$")
        plt.legend()

    @Analyzer.restrict
    @Analyzer.plotable
    def plotCoeffLinRegHeight(self, endTimes=[0.05,0.2], maxErrorAllowed=0.001,certainSize=None):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArrayFromLinReg(endTimes, maxErrorAllowed)
        if certainSize is not None:
            plotIndex = np.where(self.staticCoefficiantArray[: ,2] == certainSize)
            plt.scatter(self.staticCoefficiantArray[plotIndex, 1][0],self.staticCoefficiantArray[plotIndex,0][0])
            plt.title("Static Coefficent for Size %g" %certainSize)
        else:
            plottedSizes = []
            for s in self.staticCoefficiantArray[: ,2]:
                if s not in plottedSizes:
                    plottedSizes.append(s)


            colors = iter(cm.jet(np.linspace(0, 1, len(plottedSizes))))
            for s in sorted(plottedSizes):
                SIndex = np.where(self.staticCoefficiantArray[:, 2] == s)
                plt.scatter(self.staticCoefficiantArray[SIndex, 1][0],self.staticCoefficiantArray[SIndex, 0][0], label = "size %g" %s, color = next(colors))

        plt.xlabel("Groove Height")
        plt.ylabel(r"$F_s/F_N$")
        plt.legend()


    def degDist(self,endTimes=[0.05,0.2], maxErrorAllowed=0.001):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArrayFromLinReg(endTimes,maxErrorAllowed)

        differentCoeff = np.trim_zeros(self.staticCoefficiantArray[:,0])
        print(len(differentCoeff))
        plt.hist(differentCoeff)
        plt.show()

    def plotCoeffSize(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()

        plt.plot(self.staticCoefficiantArray[:, 2],
                 self.staticCoefficiantArray[:, 0])
        plt.show()

    def plotCoeff3D(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')

        ax.scatter(self.staticCoefficiantArray[:, 2],
                   self.staticCoefficiantArray[:, 1],
                   self.staticCoefficiantArray[:, 0])
        ax.set_xlabel("Size")
        ax.set_ylabel("Height")
        ax.set_zlabel(r"$\mu_s$")

        plt.show()

    def printStaticCoeffArray(self):
        if not hasattr(self, "staticCoefficiantArray"):
            self.makeStaticCoeffArray()
        print(self.staticCoefficiantArray)

    def plotPushers(self):

        for i in self.instanceList:
            plt.plot(i.read('pusher_force.bin'),
                     label="size: %g, height: %g" % (i.getGrooveDim()[1],
                                                     i.getGrooveDim()[0]))

        plt.legend()
        plt.show()

    def plotRodShearForceAgainsRodVelocity(self):
        """Plots the different times series for the shear force on the rod.

        As of now, this does nothing. Change freq... to the
        right name for the freq of the shear force of the rod.
        """
        for i in self.instanceList:
            timeSeries = i.getRodShearForceTimeSeries
            time = np.arange(0, len(timeSeries))*i.parameters['dt']*i.parameters['freq...']
            plt.plot(timeSeries, label="Rod Velocity: %g m/s" % i.parameters['driverVD'])
        plt.legend()
        plt.show()

######################
#  GLOBAL FUNCTIONS  #
######################


def getArgs():
    parser = argparse.ArgumentParser(
        description=("Creates plots from data\n\n"
                     "A directory path (search_path) is specified (defaults to current) where its\n"
                     "directories are matched to a given pattern (--pattern, defaults to\n"
                     "any directory). Within these matched directories, input directory\n"
                     "(--inputpath) and output directory (--output) paths are sent to an analyzer\n"
                     "which subsequently creates the plots.\n"),
        formatter_class=SortingHelpFormatter)
    parser.add_argument('search_path', type=os.path.abspath,
                        help="Directory to search for output files to analyze.")
    parser.add_argument('--plotpath', default=os.path.abspath('./plots'),
                        type=os.path.abspath,
                        help="Path to place the plots.")
    parser.add_argument('--paramname', default='parameters.txt',
                        help="Name of the parameter file.")
    parser.add_argument('--input', default='input',
                        help='Directory to search for input files.')
    parser.add_argument('--pattern', default="*",
                        help="Pattern of directories to match.")
    parser.add_argument('--output', default='output',
                        help="Directory to search for output files.")
    args = parser.parse_args()
    return args


if __name__ == '__main__':
    args = getArgs()
    manager = AnalyzerManager()
    manager.setUp(args)
    #manager.readAll()
    #manager.plotAttachedSprings(show=True, restrict = {'size':[2,8], 'height':[1,2]})
    #manager.plotRodShearForceTimeSeries(show = True,certainSize = 1)
    #manager.plotYForceBeam(show = True,certainSize = 1)
    # manager.findCoeffFromSquareError(endTimes = [0.05,0.2], certainSize = 1,certainHeight = 1,certainAngle = 0)
    # manager.plotRegressionLine(endTimes = [0,0.2], certainSize = 1,certainHeight = 1,certainAngle = 0)
    # manager.plotLocalMax(show=False, restrict={'size':[i for i in range(11)],'height':4	,'angle':0})
    # manager.plotPositionInterface(show=True)
    manager.plotAttachedSprings(show=True)
    manager.plotLocalMax(show=True)
    # plt.legend()
    # plt.show()

    # manager.getRigressionLine(endTimes = [0,0.2], certainSize = 1,certainHeight = 1,certainAngle = 0)
    # comp = Compare(manager.analyzers)
    #comp.degDist()
    #comp.plotCoeffLinRegHeight(show = True,certainSize = 1, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegHeight(show = True,certainSize = 2, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegHeight(show = True,certainSize = 3, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegHeight(show = True,certainSize = 4, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegHeight(show = True,certainSize = 5, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegSize(show = True, certainHeight=1, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegSize(show = True, certainHeight=2, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegSize(show = True, certainHeight=3, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegSize(show = True, certainHeight=4, endTimes = [0.03,0.06])
    #comp.plotCoeffLinRegSize(show = True, certainHeight=5, endTimes = [0.03,0.06])
    # comp.plotCoeffLinRegHeight(show = True, endTimes = [0.03,0.06])
    #comp.makeStaticCoeffArray()
    #comp.printStaticCoeffArray()
    #comp.plotCoeff3D()
    #comp.plotPushers()
    #analyser = Analyzer(args.dir)
