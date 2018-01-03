#!bin/bash/python3
# -*- coding: utf-8 -*-

"""
Overall design:
Lattice reads parameters and writes its nodes to a xyz-file
specifying which nodes are Top, Bottom, etc.
In order to make the Lattice, a Geometry is needed.
Geometry is an abstract class from which practical geometries
must be derived. Two examples are SquareGeometry and
TriangleGeometry. These simply create nx×ny blocks

To extend the geometries, one must add a GeometryExtension. Examples of
these are SymmetricGrooves, which adds symmetric groves, and SymmetricLegs,
which makes the grooves' legs symmetric.

These form a sort of algebra, as one can add GeometryExtensions to
eachother and to an uninitialized Geometry. These rules are followed:
GeometryExtension() + GeometryExtension() -> GeometryExtension()
GeometryExtension() + Geometry            -> Geometry
Geometry            + GeometryExtension() -> Geometry
    Where () denotes an initialized object
A + (B + C) = (A + B) + C                (Commutativity)
    Where, A, B, C are any object

It is a goal to makes every GeometricExtension idempotent,
i.e that addition of a GeometricExtension leaves the geometry unchanged if
there is nothing to change.

This makes it easy to make different Geometries; simply make an extension
and add it to an already existing geometry.
"""

import os
import argparse
import inspect
from itertools import product
from math import sin, cos, pi, floor,ceil

BOTTOM_NODE_CHAR = 'B'
TOP_NODE_CHAR = 'T'
NORMAL_NODE_CHAR = 'N'
LEFT_NODE_CHAR = 'L'


class Node:
    def __init__(self, x=None, y=None):
        "docstring"
        self.x = x
        self.y = y
        self.isBottom = False
        self.isTop = False
        self.isLeft = False

    def __str__(self):
        char = ''
        if self.isBottom:
            char += BOTTOM_NODE_CHAR
        if self.isTop:
            char += TOP_NODE_CHAR
        if self.isLeft:
            char += LEFT_NODE_CHAR
        if not (self.isBottom or self.isTop or self.isLeft):
            char = NORMAL_NODE_CHAR
        self.char = char
        return "{char} {x} {y}".format(**self.__dict__)


class Lattice:
    def __init__(self, parametersPath, xyzPath=None):
        "docstring"
        self.parametersPath = parametersPath
        self.parameters = {}
        self.nodes = []
        self.readParameters()
        if xyzPath is None:
            # Write to the latticefile specified in the parameters
            raise NotImplemented()
        else:
            self.xyzPath = xyzPath

    def readParameters(self):
        """ Read the parameters from the parameter file

            Each line is split into tokens (words). If the first token
            is not a comment and there are two tokens, the first token
            is used as a key to the value of the second token

            Can be written neatly as a single dict comprehension, but
            this might be difficult to read for some, and is thus avoided.
        """
        if not os.path.exists(self.parametersPath):
            raise RuntimeError("{} does not exist".format(self.parametersPath))

        with open(self.parametersPath, 'r') as infile:
            for line in infile:
                tokens = line.split()
                if len(tokens) < 2 or tokens[0][0] == '#':
                    continue

                # Strings can't be evaluated.
                # All (current) strings are something-filename
                # This is an incredible stupid solution
                if 'filename' in tokens[0] or 'path' in tokens[0]:
                    self.parameters[tokens[0]] = tokens[1]
                else:
                    # Otherwise, evaluate the expression to get
                    # the correct int or float value
                    self.parameters[tokens[0]] = eval(tokens[1])
        self.checkParameters()

    def checkParameters(self):
        """ Runs tests on each parameter to determine if its value is valid

        """
        try:
            assert self.parameters["grooveSize"] >= 0
            assert self.parameters["grooveHeight"] >= 0
            assert self.parameters["nx"] >= 0
            assert self.parameters["ny"] >= 0
            assert 'latticefilename' in self.parameters
        except KeyError as keyErr:
            print("Missing Parameter: ", keyErr)

    def writeXYZ(self):
        with open(self.xyzPath, 'w') as outfile:
            print("Writing lattice to ", self.xyzPath)
            # Make filewriting a bit nicer by converting the argument
            # to a string and adding newline
            outfile.print = lambda x: outfile.write('{}\n'.format(x))

            # First line specifies how many lines there are,
            # excluding the first and second line
            outfile.print(len(self.nodes))

            # The second line is the comment
            outfile.print("{nx} {ny} {d} <- nx ny d".format(**self.parameters))

            # The rest are the nodes in the format
            # T/B/L/N x y
            # where T is for top, B is for bottom, L is for left and N is for normal
            # and x y its position
            for node in self.nodes:
                outfile.print(node)

    def writeInterfaceStructure(self):
        path = os.path.split(self.xyzPath)[0] + '/interfaceStructure.txt'
        with open(path, 'w') as outfile:
            print("Writing interface to ", path)
            outfile.write(str(self.geometry.getInterfaceStructure()))

    def makeLattice(self, geometry):
        self.geometry = geometry(self.parameters)
        self.nodes = self.geometry.makeGeometry()
        self.writeXYZ()
        self.writeInterfaceStructure()


class Meta(type):
    """ 'Metaclass is a solution looking for a problem', they say.

        That is probably true. This is just a proof of concept
        to make GeometryExtension's addition work with
        an uninitialized geometry
    """
    def __add__(self, other):
        return other.__add__(self)


class Geometry(metaclass=Meta):
    """An abstract class from which all other geometries are derived

       Should be initialized from within a Lattice
       A lattice is constructed by calling makeGeometry().
       This iterates of coordinated (i,j) in the cartesian
       product nx×ny.

       A call is made to doMakeNode(i, j) to
       determine whether or not a node is to be made at the
       coordinate (i, j). This in turn checks a boolean table
       doPlaceBottomNodeHere, indicating whether a bottom node
       is to be placed at (i). This table is by default always true,
       and so must be modified to add grooves.

       Next, the actual (x, y)-coordinates are made from
       a call to makeXYfromIJ(i, j). This function is modified
       to add different kind of geometries, such as square or triangular.

       The resulting nodes are returned to the calling Lattice(), which
       writes them to a file.
    """
    def __init__(self, parameters):
        self.nx = parameters['nx']
        self.ny = parameters['ny']
        self.d = parameters['d']
        self.grooveHeight = parameters['grooveHeight']
        self.grooveSize = parameters['grooveSize']
        # A 1D list of all nodes in the geometry
        self.nodes = []
        # A 1D bool list indicating whether or not a node is to be placed
        # at an i-index along nx when j < grooveHeight
        self.doPlaceBottomNodeHere = []
        if self.grooveSize > 0 and self.grooveHeight > 0:
            self.makeGrooveList()
        else:
            self.doPlaceBottomNodeHere = [True for i in range(self.nx)]

    def makeGeometry(self):
        # Iterates over the cartesian product of nx×ny
        for i, j in product(range(self.nx), range(self.ny)):
            if not self.doMakeNode(i, j):
                continue

            x, y = self.makeXYfromIJ(i, j)
            node = Node(x, y)
            self.markNode(node, i, j)
            self.nodes.append(node)

        return self.nodes

    def doMakeNode(self, i, j):
        """ Returns true or false whether a node should be made at (i, j) """
        # Always make a node when it is above the grooves
        if j > self.grooveHeight:
            return True
        # For groove nodes, check if there should be a node at the current i-index
        # by using the self.doPlaceBottomNodeHere as a lookup
        return self.doPlaceBottomNodeHere[i]

    def markNode(self, node, i, j):
        """ Marks the node as top, bottom or left based on i and j """
        if i == 0:
            node.isLeft = True
        if j == 0:
            node.isBottom = True
        if j == self.ny-1:
            node.isTop = True

    def makeGrooveList(self):
        """ Creates the list indicating where bottom nodes are located.

            Defaults to no grooves
        """
        self.doPlaceBottomNodeHere = [1]*self.nx

    def makeXYfromIJ(self):
        raise NotImplementedError("Geometry is an abstract class")

    def getInterfaceStructure(self):
        return self.doPlaceBottomNodeHere






class SquareGeometry(Geometry):
    """ Implements a nx×ny square geometry """
    def makeXYfromIJ(self, i, j):
        x = self.d * i
        y = self.d * j
        return x, y


class TriangleGeometry(Geometry):
    """ Implements a nx×ny triangular geometry """
    def makeXYfromIJ(self, i, j):
        x = i*self.d + (j % 2)*self.d*cos(pi/3)
        y = j*self.d*sin(pi/3)
        return x, y


# This might be bringing a sledgehammer to kill a bug
class GeometryExtension:
    """ Extends a geometry by adding functions """
    def __add__(self, other):
        """ Copies all methods from self to other and returns other.

            Is commutative
        """
        # Get all functions from self
        funcs = inspect.getmembers(self.__class__)
        # Remove dunder methods
        funcs = [(n, f) for n, f in funcs if '__' not in n]

        for name, func in funcs:
            # If other is a class, add the method to the class
            # If it is an object, add it to the object's class
            if inspect.isclass(other):
                setattr(other, name, func)
            else:
                setattr(other.__class__, name, func)

        return other


class SymmetricGroovesByReverseConstruction(GeometryExtension):
    """ Extends a geometry by adding symmetric grooves

       The grooves are made symmetric by adding a piece to the
       beginning of the lattice, reversing the lattice, adding
       a piece to the beginning (or "end", if you will) of the lattice
       and reversing the lattice again. Then two counters are increased
       and a new piece is added to previously added piece, the lattice
       is reversed, and, you got the picture.

       This is done until there is no more space.
       Illustration:
        ████████████████████
                 ↓
        ████████████████████
        ███
        ███
                 ↻
        ████████████████████
                         ███
                         ███
                 ↓
        ████████████████████
        ███              ███
        ███              ███
                 ↻
        ████████████████████
        ███              ███
        ███              ███
                 ↓
        ████████████████████
        ███    ███       ███
        ███    ███       ███
                osv
    """
    def makeGrooveList(self):
        """ A lattice looks like this:

                           Non-symmetric lattice
                          _______________________
                         |                       |
        'Non-grooves' ->  ███████████████████████
                          ███████████████████████
        'Grooves'     ->  ███   ███   ███   ███
                          ███   ███   ███   ███
                         |_____|___|__|       |___|
                            |    |   |       Leftover
                         A piece |   L___
                           Piece nodes  |
                                   Piece groove

                          Ideal Symmetric lattice
                          _______________________
                         |                       |
                          ███████████████████████
                          ███████████████████████
                          ███   ████   ████   ███
                          ███   ████   ████   ███
        """
        restLength = self.nx
        grooves = [0]*self.nx
        prev = 0
        next = 2*self.grooveSize
        piece = [1]*self.grooveSize+[0]*self.grooveSize;
        while restLength > len(piece):
            for i in range(2):
                grooves[prev:next] = piece
                grooves.reverse()
            prev = next
            next += 2*self.grooveSize
            restLength -= 4*self.grooveSize

        # Is it necessary to handle a failed case?
        # if restLength <= 0:
        #     nearMiddle = int(self.nx/2+self.grooveSize/2)
        #     pieces[nearMiddle:nearMiddle+self.grooveSize//2] = [1]*self.grooveSize

        self.doPlaceBottomNodeHere = grooves


class SymmetricGroovesEqual(GeometryExtension):
    def makeGrooveList(self):

        toothSize = self.grooveSize-1
        iterations = self.nx//(toothSize*4)

        restLength = self.nx
        grooves = [0]*self.nx
        prev = 0
        next = 2*toothSize
        piece = [1]*toothSize+[0]*toothSize;
        for j in range(iterations):
            for i in range(2):
                grooves[prev:next] = piece
                grooves.reverse()
            prev = next
            next += 2*toothSize
            restLength -= 4*toothSize

        halfGrooveSize = floor((toothSize+1)/2.)

        if restLength >= 0.5*(toothSize+1):
            grooves[-self.nx//2-(halfGrooveSize-1):-self.nx//2] = [1]*halfGrooveSize
            grooves.reverse()
            grooves[-self.nx//2-(halfGrooveSize):-self.nx//2] = [1]*halfGrooveSize
            grooves.reverse()

        self.doPlaceBottomNodeHere = grooves


class SymmetricGroovesByDaniel(GeometryExtension):
    """ Extends a geometry by adding symmetric grooves

        Written by Daniel H.
        I have no clue how this works
    """
    def makeGrooveList(self):
        import numpy as np
        d = self.d
        nx = self.nx
        ny = self.ny
        grooveSize = self.grooveSize
        grooveHeight = self.grooveHeight

        self.bottomNodes = []

        if not (grooveHeight == 0 or
                grooveSize == 0):
            bottomNodes = []
            nodes = 0
            for i in range(nx):
                batch = []

                for _ in range(grooveSize):
                    batch.append(1)
                    nodes += 1
                if nx - nodes < 2 * grooveSize:
                    bottomNodes.append(batch)
                    break

                for _ in range(grooveSize):
                    batch.append(0)
                    nodes += 1

                bottomNodes.append(batch)
                if nx - nodes < grooveSize:
                    break

            restLength = nx - nodes
            restBlanks = restLength // 2
            restNodes = restLength // 2 + restLength % 2
            batches = len(bottomNodes)

            midBatch = int(np.floor(batches / 2.0))
            batchesFromTheMid = 0
            placeOnSide = 1
            restsPlaced = 1

            while (restBlanks > 0 or restNodes > 0):
                if (midBatch - batchesFromTheMid) <= 0 or (
                        midBatch + batchesFromTheMid) >= batches - 1:
                    batchesFromTheMid = 0
                    placeOnSide = -1

                if restBlanks > 0:
                    bottomNodes[midBatch +
                                placeOnSide * batchesFromTheMid].insert(-1, 0)
                    restBlanks -= 1
                if restNodes > 0:
                    bottomNodes[midBatch +
                                placeOnSide * batchesFromTheMid].insert(0, 1)
                    restNodes -= 1

                restsPlaced += 1
                batchesFromTheMid = batchesFromTheMid + restsPlaced % 2
                placeOnSide *= -1

            bottomNodes = [l for sublist in bottomNodes for l in sublist]

        self.doPlaceBottomNodeHere = bottomNodes


class SymmetricLegs(GeometryExtension):
    """ Adds symmetric groove legs

    """
    def doMakeNode(self, i, j):
        """ Returns true or false whether a node should be made at (i, j) """
        # Always make a node when it is above the grooves
        if j > self.grooveHeight:
            return True
        # Add a node if the previous node is a node and j is even
        # This makes triangular legs symmetric while making
        # square legs unsymmetric. Hence, check if the geometry is
        # square/already symmetric and leave it if it is
        if (j % 2) and i < self.nx-1 and self.doPlaceBottomNodeHere[i+1]:
            # Leave already symmetric geometries symmetric by comparing the
            # x-coordinate for two nodes above each other
            if self.makeXYfromIJ(1, 1)[0] != self.makeXYfromIJ(1, 2)[0]:
                return True
        # For groove nodes, check if there should be a node at the current i-index
        # by using the self.doPlaceBottomNodeHere as a lookup
        return self.doPlaceBottomNodeHere[i]

class SingleTooth(GeometryExtension):
    def makeGeometry(self):
        height = self.ny + self.grooveHeight
        width = self.nx
        for i, j in product(range(width),range(height)):
            if not self.doMakeNode(i, j):
                continue

            x, y = self.makeXYfromIJ(i, j)
            node = Node(x, y)
            self.markNode(node, i, j)
            self.nodes.append(node)

        return self.nodes

    def markNode(self, node, i, j):
        """ Marks the node as top, bottom or left based on i and j """
        if i == 0:
            node.isLeft = True
        if j == 0:
            node.isBottom = True
        if j == self.ny + self.grooveHeight-1:
            node.isTop = True

class ConstructSingleTooth(GeometryExtension):
    def makeGrooveList(self):
        grooves = [0 for i in range(self.nx)]
        midpoint = ceil(self.nx/2.)
        nodesInFrontOfMidpoint = midpoint + floor(self.grooveSize/2.)
        nodesBackOfMidpoint = midpoint - ceil(self.grooveSize/2.)

        grooves[nodesBackOfMidpoint:nodesInFrontOfMidpoint] = \
                    [1 for i in range(self.grooveSize)]

        assert -nodesBackOfMidpoint+nodesInFrontOfMidpoint == self.grooveSize \
                    , "Something went wrong with the creation of bottomNodes"

        self.doPlaceBottomNodeHere = grooves

def getArgs():
    parser = argparse.ArgumentParser()
    parser.add_argument('parampath', type=os.path.realpath)
    parser.add_argument('outputpath', type=os.path.realpath)
    return parser.parse_args()


if __name__ == '__main__':
    args = getArgs()
    lattice = Lattice(args.parampath, args.outputpath)
    geometry = (TriangleGeometry
                + ConstructSingleTooth()
                + SingleTooth())
                # + SymmetricLegs()
                # + SymmetricGroovesEqual())
    lattice.makeLattice(geometry)
