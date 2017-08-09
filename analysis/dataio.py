import numpy as np
import os
import sys
import re
import glob


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
        return np.fromfile(os.path.join(self.wd, path))

    def resize(self, array, start=0, step=1, stop=None):
        """Resizes an array

        Optionally shrinks the array by selecting a subarray before
        resizing.
        """
        if stop is None:
            stop = len(array)
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
        if 'catnipsprings' in self.filename.lower():
            y = self.parameters['numBottomNodes']
            x = len(data)/y
            if int(x) != x:
                raise RuntimeError(("Could not resize {}. len={}, "
                                    "x={} and y={}".format(self.filename,
                                                           len(data),
                                                           x, y)))
            x = int(x)
            data.resize((x, y))

            interface = self.parameters["interfaceStructure"]
            print(len(interface), self.parameters['nx'], self.parameters['numBottomNodes'], sum(interface))
            wholeLattice = np.zeros(shape=(x, len(interface)))
            mask = np.array(interface, dtype=bool)
            wholeLattice[:, mask] = data
            data = wholeLattice
        elif 'interface' in self.filename.lower():
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


class Globbler:
    def __init__(self, searchPath):
        self.searchPath = searchPath

    def glob(self, className, pattern,
             inputPattern, outputPattern):
        searchPattern = os.path.join(self.searchPath, pattern)
        print("Searching with {}".format(searchPattern))
        matches = glob.glob(searchPattern)
        instanceList = []
        for match in matches:
            input = glob.glob(os.path.join(match, inputPattern))
            output = glob.glob(os.path.join(match, outputPattern))
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

        print("Globbler found {} complete matches".format(len(instanceList)))
        return instanceList
