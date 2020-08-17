import matplotlib.pyplot as plt
import numpy as np
import sys


def plot(filename):
    x, y = np.loadtxt(filename, delimiter=' ', unpack=True)
    print(x)
    print(y)

    slope, intercept = np.polyfit(x, y, 1)

    line = [slope * i + intercept for i in x]

    plt.scatter(x, y, s=5)
    plt.plot(x, line, 'b')

    plt.show()


if __name__ == '__main__':
    filename = sys.argv[1]
    plot(filename)
