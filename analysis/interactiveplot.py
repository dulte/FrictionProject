import matplotlib.pyplot as plt


class InteractivePlot:
    targets = {}
    def __init__(self, hook, event):
        self.hook = hook
        self.event = event
        print("In init")

    def __call__(self, func):
        print("In call")
        self.targets[self.hook] = func
        def wrappedFunc(*args, **kwargs):
            print("In wrap", self.targets)
            print(self.hook, self.event)
            returnVal = func(*args, **kwargs)
            return returnVal
        return wrappedFunc

