import pandas as pd
import random
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

import numpy as np
import sys

import seaborn as sns


def set_size(width, fraction=1):
    """Set figure dimensions to avoid scaling in LaTeX.

    Parameters
    ----------
    width: float
            Document textwidth or columnwidth in pts
    fraction: float, optional
            Fraction of the width which you wish the figure to occupy

    Returns
    -------
    fig_dim: tuple
            Dimensions of figure in inches
    """
    # Width of figure (in pts)
    fig_width_pt = width * fraction

    # Convert from pt to inches
    inches_per_pt = 1 / 72.27

    # Golden ratio to set aesthetic figure height
    # https://disq.us/p/2940ij3
    golden_ratio = (5**.5 - 1) / 2

    # Figure width in inches
    fig_width_in = fig_width_pt * inches_per_pt
    # Figure height in inches
    fig_height_in = fig_width_in * golden_ratio

    fig_dim = (fig_width_in, fig_height_in)

    return fig_dim


# Using seaborn's style
plt.style.use('seaborn')
width = 417.47307

tex_fonts = {
    # Use LaTeX to write all text
    "text.usetex": True,
    "font.family": "serif",
    # Use 10pt font in plots, to match 10pt font in document
    "axes.labelsize": 10,
    "font.size": 10,
    # Make the legend/label fonts a little smaller
    "legend.fontsize": 8,
    "xtick.labelsize": 8,
    "ytick.labelsize": 8
}

plt.rcParams.update(tex_fonts)


sns.set_theme(style="darkgrid", color_codes=True)


def plot(filename):
    n = 114932854
    s = 10000  # desired sample size
    skip = sorted(random.sample(range(n), n-s))
    df = pd.read_csv(filename, names=[
                     'x', 'y'], delimiter=' ', skiprows=skip, keep_default_na=False)

    # x, y = np.loadtxt(filename, delimiter=' ', unpack=True)
    # print(x)
    # print(y)
    # x = np.array([1, 5, 15, 25, 31, 35, 45, 51])
    # y = np.arange(0, 8)

    # slope, intercept = np.polyfit(x, y, 1)
    # print(f"Slope: {slope}, Intercept: {intercept}")

    # line = [slope * i + intercept for i in x]
    # print([int(i + 0.5) for i in line])

    # data = {'x': x, 'y': y}
    # df = pd.DataFrame(data, columns=['x', 'y'])
    print(df)

    figure(num=None, figsize=set_size(width),
           dpi=80, facecolor='w', edgecolor='k')

    sns_plot = sns.lmplot(x="x", y="y", data=df, fit_reg=False,
                          scatter_kws={"s": 5, "alpha": .5})

    # ax = sns.barplot(x="data", y="memory", hue="model", data=memory_df)
    # ax.set(yscale="log")
    # ax.axes.get_xaxis().get_label().set_visible(False)
    # plt.ylabel("Memory [MB]")
    plt.show()
    sns_plot.savefig('aus_sample.pdf', format='pdf', bbox_inches='tight')

    # f = plt.figure()
    # plt.scatter(x, y, s=5)
    # plt.plot(x, line, 'b')
    # plt.title('Sample Linear Regression CDF mapping on data', fontsize=11)
    # plt.xlabel('$x$', fontsize=11)
    # plt.ylabel('$y$', fontsize=11)
    # plt.show()

    # f.savefig("test.pgf", bbox_inches='tight')


if __name__ == '__main__':
    filename = sys.argv[1]
    plot(filename)
