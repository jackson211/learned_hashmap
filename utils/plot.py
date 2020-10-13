import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import sys

matplotlib.use("pgf")
matplotlib.rcParams.update({
    "pgf.texsystem": "pdflatex",
    'font.family': 'serif',
    'text.usetex': True,
    'pgf.rcfonts': False,
})


def plot(filename):
    x, y = np.loadtxt(filename, delimiter=' ', unpack=True)
    # print(x)
    # print(y)
    # x = np.array([1, 5, 15, 25, 31, 35, 45, 51])
    # y = np.arange(0, 8)

    slope, intercept = np.polyfit(x, y, 1)
    print(f"Slope: {slope}, Intercept: {intercept}")

    line = [slope * i + intercept for i in x]
    # print([int(i + 0.5) for i in line])

    f = plt.figure()
    plt.scatter(x, y, s=5)
    plt.plot(x, line, 'b')

    plt.title('Sample Linear Regression CDF mapping on data', fontsize=11)
    plt.xlabel('$x$', fontsize=11)
    plt.ylabel('$y$', fontsize=11)
    plt.show()

    f.savefig("test.pgf", bbox_inches='tight')


if __name__ == '__main__':
    filename = sys.argv[1]
    plot(filename)
