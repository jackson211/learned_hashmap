import matplotlib.pyplot as plt
import numpy as np
import sys


def plot(filename):
    x, y = np.loadtxt(filename, delimiter=' ', unpack=True)
    print(x)
    print(y)
    # x = np.array([1, 5, 15, 25, 31, 35, 45, 51])
    # y = np.arange(0, 8)

    slope, intercept = np.polyfit(x, y, 1)
    print(f"Slope: {slope}, Intercept: {intercept}")

    line = [slope * i + intercept for i in x]
    print([int(i + 0.5) for i in line])

    plt.scatter(x, y, s=5)
    plt.plot(x, line, 'b')

    plt.show()


if __name__ == '__main__':
    filename = sys.argv[1]
    plot(filename)
