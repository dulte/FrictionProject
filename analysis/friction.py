from analyzer import Analyzer, AnalyzerManager, plotable, CorruptAnalyzer
from dataio import File, Globbler
import os
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import argrelmax
import pandas as pd


try:
    import seaborn as sns
    sns.set()
except ImportError:
    pass


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
        self.beamMass = parameters['beamMass']


class FrictionAnalyzer(Analyzer):
    def readNumberOfNodes(self):
        filename = 'lattice.xyz'
        blockSize = 10000  # Each xyz-block is atleast larger than 0
        numberOfBottomNodes = 0
        numberofTopNodes = 0
        with open(os.path.join(self.info, filename)) as xyz:
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
        nFile = lambda s: File(s, self.parameters, self.output)
        self.interfacePosition = nFile('interfacePosition.bin')
        self.interfaceVelocity = nFile('interfaceVelocity.bin')
        self.interfaceAttachedSprings = nFile('interfaceAttachedSprings.bin')
        self.allPosition = nFile('allPosition.bin')
        self.allVelocity = nFile('allVelocity.bin')
        self.allEnergy = nFile('allEnergy.bin')
        self.allForce = nFile('allForce.bin')
        self.pusherForce = nFile('pusherForce.bin')
        self.xyz = nFile('model.xyz')
        self.beamTorque = nFile('beamTorque.bin')
        self.beamShearForce = nFile('beamShearForce.bin')
        self.activeFiles = []
        self.files = [self.interfacePosition, self.interfaceVelocity,
                      self.interfaceAttachedSprings, self.allPosition,
                      self.allVelocity, self.allEnergy, self.allForce,
                      self.pusherForce, self.xyz, self.beamTorque,
                      self.beamShearForce]

    def getInterfaceStructure(self):
        print(self.input)
        filename = 'interfaceStructure.txt'
        return
        try:
            with open(os.path.join(self.input, filename)) as xyz:
                self.parameters['interfaceStructure'] = eval(xyz.readline())
        except Exception as exc:
            raise CorruptAnalyzer("Could not read interface structure")

    def readAll(self):
        """ Reads all binary files available as described in parameters.

        Reads the file writing flags in the parameter file to
        determine which binary files to load.
        """
        writtenOutput = {param for param, flag in
                         self.parameters.items()
                         if param.startswith('write')
                         and flag == 1}

    @plotable
    def plotPositionInterface(self):
        pos = self.interfacePosition.get() - self.interfacePosition.get()[0, :]
        plt.pcolormesh(pos[:, :, 1], cmap=plt.get_cmap('jet'))
        plt.colorbar()
        plt.title("Relative position of the interface")

    @Analyzer.restrict
    @plotable
    def plotAttachedSprings(self):
        savepath = 'attachedSprings.png'
        data = self.interfaceAttachedSprings.get()
        time = self.interfaceAttachedSprings.time()
        plt.pcolormesh(data, cmap=plt.get_cmap('jet'))
        ticks = [int(t) for t in np.linspace(0, data.shape[0]-1, 10)]
        plt.yticks(ticks, time[ticks])
        cb = plt.colorbar()
        cb.set_label("Fraction of pinned springs")
        cb.ax.get_yaxis().set_ticks([])
        for i, lab in enumerate(['0%', '100%']):
            cb.ax.text(1, 0.95*i, lab)
        plt.title(("Percent of pinned springs for "
                   "size {} and height {}".format(self.lattice.size,
                                                  self.lattice.height)))
        plt.xlabel("Block index")
        plt.ylabel("Time [s]")

    def getFrontVelocities(self, cutoffPoint=0.1):
        """Finds the front velocity of the rapture of the springs.

        Symmetric differentiation is used to find the velocity.
        """
        data = self.getReducedSpringAttachments(cutoffPoint)
        v = np.zeros(len(data) - 2)
        h = self.interfaceAttachedSprings.time()
        for i in range(1, len(data)-1):
            v[i-1] = (data[i+1] - data[i-1])/(2.0*h)

        return np.abs(v)*self.parameters['d']

    @plotable
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
        shearForce_on_rod = self.beamShearForce.get()

        if mean:
            shearForce_on_rod = np.mean(shearForce_on_rod, axis=1)
        else:
            shearForce_on_rod = np.sum(shearForce_on_rod, axis=1)

        staticCoefficient = np.max(-shearForce_on_rod)/np.float(self.parameters['fn'])
        return staticCoefficient

    def getLocalMax(self):
        shearForce = -1*self.beamShearForce.get()
        force = np.sum(shearForce, axis=1)/self.parameters['fn']
        locMaxIndex = argrelmax(force)[0]
        # timeArray = self.getTimeArray(force, 'freqBeamShearForce')[argrelmax(force)[0]]
        time = self.beamShearForce.time()[locMaxIndex]
        locMax = force[locMaxIndex]
        # return time, locMax
        return self.beamShearForce.time(), force

    @plotable
    @Analyzer.restrict
    def plotLocalMax(self, i=[0]):
        lines = ['-', '--', '-.']
        time, data = self.getLocalMax()
        plt.plot(time, data,
                 label='Size {}, height {}'.format(self.lattice.size,
                                                   self.lattice.height))
                 # linestyle=lines[i[0]])
        self.plotReleaseTime()
        self.plotDrivingTime()
        plt.xlabel("Time [s]")
        plt.ylabel(r"$F_S/F_N$")
        plt.title(("Local Max for Beam Shear Force for "
                   "size {} height {}".format(self.lattice.size,
                                              self.lattice.height)))
        i[0] += 1
        i[0] %= len(lines)

    def plotReleaseTime(self):
        plt.axvline(x=self.parameters['releaseTime']*self.parameters['step'],
                    linestyle='--', color='k')

    def plotDrivingTime(self):
        plt.axvline(x=self.parameters['drivingTime']*self.parameters['step'],
                    linestyle='-.', color='k')


class FrictionManager(AnalyzerManager):
    def setUp(self, args):
        globbler = Globbler(args.search_path)
        self.analyzers = globbler.glob(className=FrictionAnalyzer,
                                       pattern=args.pattern,
                                       outputPattern=args.output,
                                       inputPattern=args.input)
        for analyzer in self.analyzers:
            try:
                analyzer.setPlotPath(args.plotpath)
                analyzer.readParameters(args.paramname)
                analyzer.readNumberOfNodes()
                analyzer.getInterfaceStructure()
                analyzer.lattice = Lattice(analyzer.parameters)
                analyzer.setUpFiles()
            except CorruptAnalyzer as exc:
                self.handleException(analyzer, exc)

        self.inspect()


class GroupAnalyzer():
    """

    Since numpy arrays are several order of magnitudes faster than
    panda dataframes, numpy arrays are used for large datasets while
    dataframes are used for small datasets.
    """
    def __init__(self, manager):
        self.manager = manager
        self.df = self.createDataFrame()
        self.cids = []
        self.scatterData = {}

    def createDataFrame(self):
        manager = self.manager
        lattices = manager.lattice
        return pd.DataFrame({'nx': [l.nx for l in lattices],
                             'ny': [l.ny for l in lattices],
                             'nodes': [l.numNodes for l in lattices],
                             'top nodes': [l.numTop for l in lattices],
                             'bottom nodes': [l.numBottom for l in lattices],
                             'height': [l.height for l in lattices],
                             'size': [l.size for l in lattices],
                             'angle': [l.angle for l in lattices],
                             'driver speed': [l.speed for l in lattices],
                             'static coefficient': manager.getStaticFrictionCoefficient()},
                            index=manager.id)

    def entryFromEvent(self, event):
        x, y = self.scatterData.columns
        dist = ((self.scatterData[x]-event.mouseevent.xdata)**2 +
                (self.scatterData[y]-event.mouseevent.ydata)**2)
        nearest = np.argmin(dist)
        return nearest, self.df.iloc[nearest]

    def plot(self):
        self.fig = plt.figure(1)
        self.scatterData = self.df[['size', 'static coefficient']]
        plt.scatter(self.scatterData['size'],
                    self.scatterData['static coefficient'], picker=5)
        self.cids.append(self.fig.canvas.mpl_connect('pick_event',
                                                     self.plotAnalyzer))
        plt.show()
        # self.fig.canvas.mpl_disconnect(self.cids.pop())

    def plotAnalyzer(self, event):
        id, entry = self.entryFromEvent(event)
        analyzer = self.manager.get(id)
        plt.subplots()
        plt.subplot(2, 1, 1)
        analyzer.plotLocalMax()
        plt.subplot(2, 1, 2)
        analyzer.plotAttachedSprings()
        plt.show()
