import numpy as np


class nodeConfiguration:

    def __init__(self, configPath, configFileName):

        self.configPath = configPath
        self.fileName = configFileName




        self.parameters = {}
        self.readConfig()

        self.nodePositionX = np.zeros((self.parameters["ny"],self.parameters["nx"]))
        self.nodePositionY = np.zeros((self.parameters["ny"],self.parameters["nx"]))
        self.indicateBlankNode = -1



    def readConfig(self):

        with open(self.configPath + self.fileName) as f:
            for line in f:
                words = line.split()
                try:
                    if words[0] == "d":
                        self.parameters["d"] = float(words[1])
                    elif words[0] == "grooveSize":
                        self.parameters["grooveSize"] = int(words[1])
                    elif words[0] == "grooveHeight":
                        self.parameters["grooveHeight"] = int(words[1])
                    elif words[0] == "nx":
                        self.parameters["nx"] = int(words[1])
                    elif words[0] == "ny":
                        self.parameters["ny"] = int(words[1])
                except:
                    pass


    def configurateNodesEqual(self):
        d = self.parameters["d"]
        nx = self.parameters["nx"]
        ny = self.parameters["ny"]
        grooveSize = self.parameters["grooveSize"]
        grooveHeight = self.parameters["grooveHeight"]

        if not(self.parameters["grooveHeight"] == 0 or self.parameters["grooveSize"] == 0):
            bottomNodes = []
            nodes = 0
            for i in range(nx):
                batch = []

                for _ in range(grooveSize):
                    batch.append(1)
                    nodes += 1
                if nx - nodes < 2*grooveSize:
                    bottomNodes.append(batch)
                    break

                for _ in range(grooveSize):
                    batch.append(0)
                    nodes+=1

                bottomNodes.append(batch)
                if nx - nodes < grooveSize:
                    break

            restLength = nx - nodes
            restBlanks = restLength//2
            restNodes = restLength//2 + restLength%2
            batches = len(bottomNodes)


            print bottomNodes
            print nodes
            print batches
            print restBlanks,restNodes


            midBatch = int(np.floor(batches/2.0))
            batchesFromTheMid = 0
            placeOnSide = 1
            while (restBlanks > 0 or restNodes > 0):
                if(midBatch - batchesFromTheMid) <= 0 or (midBatch + batchesFromTheMid) >= len(bottomNodes) - 1:
                    batchesFromTheMid = 0
                    placeOnSide = 1

                if restBlanks > 0:
                    bottomNodes[midBatch + placeOnSide*batchesFromTheMid].insert(-1,0)
                    restBlanks -=1
                if restNodes > 0:
                    bottomNodes[midBatch + placeOnSide*batchesFromTheMid].insert(1,1)
                    restNodes -=1

                batchesFromTheMid += 1
                placeOnSide *= -1


            print bottomNodes
            bottomNodes = [l for sublist in bottomNodes for l in sublist]

            print bottomNodes
            print len(bottomNodes)
            print restBlanks,restNodes

            for i in range(int(nx)):
                for j in range(int(ny)):
                    if j < grooveHeight:
                        if bottomNodes[i] == 1:
                            self.nodePositionX[j,i] = i*d + (j%2)*d*np.cos(np.pi/3.)
                            self.nodePositionY[j,i] = j*d*np.sin(np.pi/3.)
                        else:
                            self.nodePositionX[j,i] = self.indicateBlankNode
                            self.nodePositionY[j,i] = self.indicateBlankNode
                    else:
                        self.nodePositionX[j,i] = i*d + (j%2)*d*np.cos(np.pi/3.)
                        self.nodePositionY[j,i] = j*d*np.sin(np.pi/3.)

        else:
            for i in range(int(nx)):
                for j in range(int(ny)):
                    self.nodePositionX[j,i] = i*d + (j%2)*d*np.cos(np.pi/3.)
                    self.nodePositionY[j,i] = j*d*np.sin(np.pi/3.)


    def makeXYZ(self):
        nx = int(self.parameters["nx"])
        ny = int(self.parameters["ny"])

        numNodes = np.sum(self.nodePositionX>=0)


        with open("nodes.xyz", "wr") as f:
            f.write(str(numNodes) + "\n")
            f.write("comment \n")
            for i in range(int(nx)):
                for j in range(int(ny)):
                    if not(self.nodePositionX[j,i] == self.indicateBlankNode or self.nodePositionY[j,i] == self.indicateBlankNode ):
                        f.write(str(self.nodePositionX[j,i]) + " " + str(self.nodePositionY[j,i]) + "\n")







if __name__ == "__main__":
    path = "/home/daniel/Dokumenter/Skole/FrictionProject/build-FrictionProject-Desktop_Qt_5_7_0_GCC_64bit-Release/"
    fileName = "config.txt"
    nodes = nodeConfiguration(path, fileName)
    nodes.configurateNodesEqual()
    nodes.makeXYZ()