import os
import sys
import traceback
from dataio import Parser, DataReader, Globbler
import matplotlib.pyplot as plt
from inspect import ismethod, getmembers


class CorruptAnalyzer(RuntimeError):
    def __init__(self, *args, **kwargs):
        RuntimeError.__init__(self, *args, **kwargs)


class Analyzer:
    _index = [0]

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
        self.info = os.path.join(self.output, 'info')
        self.id = self._index[0]
        self._index[0] += 1

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
        path = os.path.join(self.info, filename)
        try:
            self.parameters = Parser.parse(path)
        except Exception as parent:
            raise CorruptAnalyzer("Could not read parameters")
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

    def restrict(fn):
        def decorator(*args, **kwargs):
            cls = args[0]
            result = None
            dcKwargs = {}
            if 'restrict' in kwargs:
                for k, v in kwargs['restrict'].items():
                    # Make sure the input is an iterable
                    dcKwargs[k] = (v if isinstance(v, (list, set, tuple))
                                   else [v])

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
        self.methods = None

    def inspect(self):
        assert len(self.analyzers) > 0, "Found nothing to analyze"
        self.methods = [name for name, value in getmembers(self.analyzers[0])
                        if ismethod(value)]

    def __getattr__(self, name):
        assert self.methods is not None, "inspect() must be called"

        def func(*args, **kwargs):
            returnVals = []
            for analyzer in self.analyzers:
                try:
                    returnVals.append(getattr(analyzer, name)(*args, **kwargs))
                except CorruptAnalyzer as exc:
                    self.handleException(analyzer, exc)
            return returnVals

        if name in self.methods:
            return func
        else:
            return [getattr(a, name) for a in self.analyzers]

    def handleException(self, analyzer, exc):
        print("Analyzer {} experienced an exception and is removed from further usage".format(analyzer.id))
        exc_type, exc_value, exc_traceback = sys.exc_info()
        traceback.print_exception(exc_type, exc_value, exc_traceback,
                                  file=sys.stdout)
        del self.analyzers[self.analyzers.index(analyzer)]

    def get(self, id):
        for a in self.analyzers:
            if a.id == id:
                return a
        raise IndexError("No analyzers with id {}".format(id))

    def setUp(self, args):
        raise NotImplementedError


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
            plt.savefig(os.path.join(self.plotpath, name), dpi=1200)
        return res

    return decorator
