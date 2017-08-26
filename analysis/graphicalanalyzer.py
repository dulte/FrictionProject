# -*- coding: utf-8 -*-

import numpy as np
# import matplotlib
# matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.figure import Figure
from matplotlib.colors import ListedColormap
from matplotlib.backend_bases import key_press_handler
from matplotlib.backends.backend_qt5agg import (
    FigureCanvasQTAgg as FigureCanvas,
    NavigationToolbar2QT as NavigationToolbar)
from PyQt5 import QtWidgets, QtCore

try:
    import seaborn as sns
    sns.set_style("dark")
    sns.despine()
except ImportError:
    pass


class GraphicalAnalyzer(QtWidgets.QMainWindow):
    def __init__(self, manager, parent=None):
        QtWidgets.QMainWindow.__init__(self, parent)
        self.manager = manager
        self.df = self.createDataFrame()
        self.scatterData = {}
        self.xkey = 'size'
        self.ykey = 'static coefficient'
        self.colorkey = 'height'
        self.pickerEpsilon = 5
        self.labels = {'size': 'Size of grooves [nr. blocks wide]',
                       'height': 'height of grooves',
                       'static coefficient': r'Static coefficient   $F_S/F_N$',
                       'driver speed': 'Driver speed [mm/s]',
                       'ny': 'Number of blocks in y-direction',
                       'nx': 'Number of nodes in x-direction',
                       'nodes': 'number of nodes',
                       'angle': 'Angle of driver beam [degrees]',
                       'beam mass': 'Mass of driver beam [kg]',
                       'top nodes': 'Number of top nodes',
                       'bottom nodes': 'Number of bottom nodes'}

        self.createMainFrame()
        self.onDraw()

    def createMainFrame(self):
        self.mainFrame = QtWidgets.QWidget()
        self.fig = Figure((5.0, 4.0), dpi=100)
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self.mainFrame)
        self.canvas.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.canvas.setFocus()

        self.mpl_toolbar = NavigationToolbar(self.canvas, self.mainFrame)
        self.canvas.mpl_connect('key_press_event', self.onKeyPress)
        self.canvas.mpl_connect('pick_event', self.plotAnalyzer)

        # Other GUI widgets
        self.button = QtWidgets.QPushButton('&Draw')
        self.button.clicked.connect(self.onDraw)

        xlabel = QtWidgets.QLabel("x-axis")
        self.xaxcb = QtWidgets.QComboBox()
        self.xaxcb.addItems(self.getChangingKeys())
        self.xaxcb.setCurrentText(self.xkey)
        self.xaxcb.currentIndexChanged.connect(self.xaxSelectionChange)

        ylabel = QtWidgets.QLabel("y-axis")
        self.yaxcb = QtWidgets.QComboBox()
        self.yaxcb.addItems(sorted(self.df.columns))
        self.yaxcb.setCurrentText(self.ykey)
        self.yaxcb.currentIndexChanged.connect(self.yaxSelectionChange)

        colorLabel = QtWidgets.QLabel("color by")
        self.colorcb = QtWidgets.QComboBox()
        self.colorcb.addItems(sorted(self.df.columns))
        self.colorcb.setCurrentText(self.colorkey)
        self.colorcb.currentIndexChanged.connect(self.colorSelectionChange)

        # Add all widgets to layout
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(self.canvas)
        layout.addWidget(self.mpl_toolbar)
        hbox = QtWidgets.QHBoxLayout()
        hbox.addWidget(xlabel)
        hbox.addWidget(self.xaxcb)
        hbox.addWidget(ylabel)
        hbox.addWidget(self.yaxcb)
        hbox.addWidget(colorLabel)
        hbox.addWidget(self.colorcb)
        layout.addLayout(hbox)
        self.mainFrame.setLayout(layout)
        self.setCentralWidget(self.mainFrame)

    def xaxSelectionChange(self, i):
        self.xkey = self.xaxcb.currentText()
        self.onDraw()

    def yaxSelectionChange(self, i):
        self.ykey = self.yaxcb.currentText()
        self.onDraw()

    def colorSelectionChange(self, i):
        self.colorkey = self.colorcb.currentText()
        self.onDraw()

    def onDraw(self):
        self.fig.clear()
        if self.xkey == self.colorkey or self.ykey == self.colorkey:
            self.scatterData = self.df[[self.xkey, self.ykey]]
        else:
            self.scatterData = self.df[[self.xkey, self.ykey, self.colorkey]]

        cmap = ListedColormap(sns.color_palette("coolwarm").as_hex() )
        # cmap = ListedColormap(['r', 'b'])
        self.axes = self.fig.add_subplot(111)
        points = self.axes.scatter(self.scatterData[self.xkey],
                                   self.scatterData[self.ykey],
                                   c=self.scatterData[self.colorkey],
                                   picker=self.pickerEpsilon,
                                   cmap=cmap)
        self.cb = self.fig.colorbar(points, ax=self.axes)
        self.cb.set_label(self.labels[self.colorkey])
        self.setAxisTicks()
        self.axes.set_xlabel(self.labels[self.xkey])
        self.axes.set_ylabel(self.labels[self.ykey])
        self.canvas.draw()

    def onKeyPress(self, event):
        print(event.key)
        key_press_handler(event, self.canvas)

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
                             'driver speed': [l.speed/1e-3 for l in lattices],
                             'beam mass': [l.beamMass for l in lattices],
                             'static coefficient': manager.getStaticFrictionCoefficient()},
                            index=manager.id)

    def getChangingKeys(self):
        return sorted([key for key in self.df.columns
                       if len(self.df[key].unique()) > 1])

    def entryFromEvent(self, event):
        x, y = self.scatterData.columns[:2]
        dist = ((self.scatterData[x]-event.mouseevent.xdata)**2 +
                (self.scatterData[y]-event.mouseevent.ydata)**2)
        nearest = np.argmin(dist)
        return nearest, self.df.iloc[nearest]

    def setAxisTicks(self):
        # Make the ticks discerete if the values are discrete
        discreteValues = {'nx', 'ny', 'nodes', 'top nodes',
                          'bottom nodes', 'height', 'size'}
        if self.xkey in discreteValues:
            minVal = min(self.scatterData[self.xkey])
            maxVal = max(self.scatterData[self.xkey])
            ticks = np.arange(minVal, maxVal+1, 1)
            self.axes.xaxis.set(ticks=ticks)
        if self.ykey in discreteValues:
            minVal = min(self.scatterData[self.ykey])
            maxVal = max(self.scatterData[self.ykey])
            ticks = np.arange(minVal, maxVal+1, 1)
            self.axes.yaxis.set(ticks=ticks)
        if self.colorkey in discreteValues:
            minVal = min(self.scatterData[self.colorkey])
            maxVal = max(self.scatterData[self.colorkey])
            ticks = np.arange(minVal, maxVal+1, 1)
            # self.cb.ax.get_yaxis().set_ticks(ticks)

    def plotAnalyzer(self, event):
        id, entry = self.entryFromEvent(event)
        analyzer = self.manager.get(id)
        plot = GraphicalSingle(analyzer, parent=self)
        plot.show()


class GraphicalSingle(QtWidgets.QMainWindow):
    def __init__(self, analyzer, parent=None):
        QtWidgets.QMainWindow.__init__(self, parent)
        self.analyzer = analyzer
        self.createMainFrame()
        self.onDraw()

    def createMainFrame(self):
        self.mainFrame = QtWidgets.QWidget()
        self.fig = Figure((5.0, 4.0), dpi=100)
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self.mainFrame)
        self.canvas.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.canvas.setFocus()

        self.mpl_toolbar = NavigationToolbar(self.canvas, self.mainFrame)
        self.canvas.mpl_connect('key_press_event', self.onKeyPress)

        # Other GUI widgets
        self.xyzButton = QtWidgets.QPushButton("View &lattice")
        self.xyzButton.clicked.connect(self.showLattice)

        # Add all widgets to layout
        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(self.canvas)
        layout.addWidget(self.mpl_toolbar)

        hbox = QtWidgets.QHBoxLayout()
        hbox.addWidget(self.xyzButton)
        layout.addLayout(hbox)

        self.mainFrame.setLayout(layout)
        self.setCentralWidget(self.mainFrame)

    def onDraw(self):
        self.fig.clear()
        ax1 = self.fig.add_subplot(211)
        self.analyzer.plotLocalMax(ax1)
        ax2 = self.fig.add_subplot(212)
        self.analyzer.plotAttachedSprings(ax2, self.fig)
        self.canvas.draw()

    def onKeyPress(self, event):
        key_press_handler(event, self.canvas)

    def showLattice(self):
        fig, ax = plt.subplots()
        self.analyzer.plotLattice(ax, fig)
        fig.show()
