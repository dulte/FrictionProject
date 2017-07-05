#!/bin/python3
import numpy as np
import os
import argparse


class nodeConfiguration:
    def __init__(self, configPath):

        self.configPath = configPath
        self.parameters = {}
        self.readConfig()
        self.nodePositionX = np.zeros((self.parameters["ny"],
                                       self.parameters["nx"]))
        self.nodePositionY = np.zeros((self.parameters["ny"],
                                       self.parameters["nx"]))
        self.indicateBlankNode = -1

    def readConfig(self):
        """ Read the parameters from the config file

            Each line is split into tokens (words). If the first token
            is not a comment and there are two tokens, the first token
            is used as a key to the value of the second token

            Can be written neatly as a single dict comprehension, but
            this might be difficult to read for some, and is thus avoided.
        """
        with open(self.configPath) as infile:
            for line in infile:
                tokens = line.split()
                if len(tokens) < 2:
                    continue

                if tokens[0][0] != "#":
                    self.parameters[tokens[0]] = eval(tokens[1])
        self.checkConfig()

    def checkConfig(self):
        """ Runs tests on each parameter to determine if its value is valid

        """
        try:
            assert self.parameters["grooveSize"] >= 0
            assert self.parameters["grooveHeight"] >= 0
            assert self.parameters["nx"] >= 0
            assert self.parameters["ny"] >= 0
        except KeyError as keyErr:
            print("Missing Parameter: ", keyErr)
            raise keyErr

    def configurateNodesEqual(self):
        """ I am a magic elf doing magic stuff

        """
        d = self.parameters["d"]
        nx = self.parameters["nx"]
        ny = self.parameters["ny"]
        grooveSize = self.parameters["grooveSize"]
        grooveHeight = self.parameters["grooveHeight"]

        self.bottomNodes = []

        if not (self.parameters["grooveHeight"] == 0 or
                self.parameters["grooveSize"] == 0):
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

            # print(bottomNodes)
            # print(nodes)
            # print(batches)
            # print(restBlanks, restNodes)

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

            # print(bottomNodes)
            bottomNodes = [l for sublist in bottomNodes for l in sublist]

            self.bottomNodes = bottomNodes

            # print(bottomNodes)
            # print(len(bottomNodes))
            # print(restBlanks, restNodes)

            for i in range(nx):
                for j in range(ny):
                    if j < grooveHeight:
                        if bottomNodes[i] == 1:
                            self.nodePositionX[j, i] = i * d + (
                                j % 2) * d * np.cos(np.pi / 3.)
                            self.nodePositionY[j, i] = j * d * np.sin(
                                np.pi / 3.)
                        else:
                            self.nodePositionX[j, i] = self.indicateBlankNode
                            self.nodePositionY[j, i] = self.indicateBlankNode
                    else:
                        self.nodePositionX[
                            j, i] = i * d + (j % 2) * d * np.cos(np.pi / 3.)
                        self.nodePositionY[j, i] = j * d * np.sin(np.pi / 3.)
        else:
            for i in range(nx):
                for j in range(ny):
                    self.nodePositionX[
                        j, i] = i * d + (j % 2) * d * np.cos(np.pi / 3.)
                    self.nodePositionY[j, i] = j * d * np.sin(np.pi / 3.)

    def makeXYZ(self):
        nx = self.parameters["nx"]
        ny = self.parameters["ny"]
        d  = self.parameters["d"]

        numNodes = np.sum(self.nodePositionX >= 0)

        with open("nodes.xyz", "w") as f:
            f.write(str(numNodes) + "\n")
            f.write("{} {} {} \n".format(nx, ny, d))
            for i in range(nx):
                for j in range(ny):
                    if not (self.nodePositionX[j, i] == self.indicateBlankNode
                            or
                            self.nodePositionY[j, i] == self.indicateBlankNode):
                        f.write(
                            str(self.nodePositionX[j, i]) + " " +
                            str(self.nodePositionY[j, i]) + "\n")

    def makeBottomNodesText(self, outputPath):
        try:
            np.savetxt(outputPath,
                       np.array([len(self.bottomNodes)] + self.bottomNodes),
                       fmt="%i")
        except:
            print("something went wrong when writing to text file")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="string giving the path of the files")

    parser.add_argument('--config', type=os.path.realpath,
                        help="Path of the config file",
                        default="../Config/config.txt")
    parser.add_argument('--output', type=os.path.realpath,
                        help="Path of the output file",
                        default="../Config/bottomNodesConfig.txt")
    args = parser.parse_args()

    nodes = nodeConfiguration(args.config)
    nodes.configurateNodesEqual()
    nodes.makeXYZ()
    nodes.makeBottomNodesText(args.output)
