import ovito
import os
import re
import shutil
from itertools import product
from subprocess import call, PIPE
from ovito.io import import_file
from ovito.vis import Viewport, RenderSettings


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


class Tester:
    def __init__(self, parametersPath, outputPath):
        self.parameters = Parser.parse(parametersPath)
        self.sizes = self.parameters['grooveSize']
        self.heights = self.parameters['grooveHeight']
        self.outputPath = outputPath

    def run(self):
        self.makeOutput()
        total = len(list(product(self.sizes, self.heights)))
        for i, (size, height) in enumerate(product(self.sizes, self.heights)):
            # print("Constructing size {} and height {}".format(size, height))
            lattice = self.constructLattice(size, height)
            self.render(lattice)
            print("{:04.2f}% complete".format((i+1)/total*100))
        os.chdir(self.pwd)

    def makeOutput(self):
        self.pwd = os.getcwd()
        try:
            os.mkdir(self.outputPath)
        except Exception:
            pass
        shutil.copy('constructLattice.py', self.outputPath)
        os.chdir(self.outputPath)

    def constructLattice(self, size, height):
        path = 'parameters{:0>2}-{:0>2}.txt'.format(size, height)
        with open(path, 'w') as paramfile:
            lines = ['{}  {}\n'.format(k, v) for k, v in self.parameters.items()
                     if k not in {'grooveSize', 'grooveHeight'}]
            lines += ['grooveSize {}\n'.format(size),
                      'grooveHeight {}\n'.format(height)]
            paramfile.writelines(lines)

        latticepath = 'lattice{:0>2}-{:0>2}.xyz'.format(size, height)
        call(['python', 'constructLattice.py', path, latticepath], stdout=PIPE)
        return latticepath

    def render(self, latticePath):
        name = latticePath
        cols = ['Particle Type', 'Position.X', 'Position.Y']
        node = import_file(name, columns=cols)
        node.add_to_scene()

        rs = RenderSettings(
            filename=name[:name.find('.')] + '.png',
            size=(1024, 768),
            background_color=(0.4, 0.4, 0.4)
            # generate_alpha=True
        )
        vp = ovito.dataset.viewports.active_vp
        vp.type = Viewport.Type.TOP
        vp.zoom_all()
        vp.render(rs)
        node.remove_from_scene()


if __name__ == "__main__":
    tester = Tester('parameters.txt', 'output')
    tester.run()
