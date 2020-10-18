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


range_data = {
    'size': [0, 0.012, 0.049, 0.111, 0.198, 0.310, 0.446, 0.607, 0.792, 0.997],
    'R-Tree': [0.004, 0.337, 0.801, 1.198, 1.775, 2.277, 2.967, 3.509, 4.062, 4.15],
    'LSPH': [0.004, 0.849, 1.533, 2.118, 2.812,  3.614, 4.253, 4.606, 5.197, 5.236],
    # 'number': [0, 8034, 21305, 35374, 50347, 66121, 78701, 92934, 107705, 119560]
}
range_df = pd.DataFrame(
    range_data, columns=['size', 'R-Tree', 'LSPH'])
range_df = range_df.set_index('size', inplace=True)


def plot():

    # figure(num=None, figsize=set_size(width), dpi=80, facecolor='w', edgecolor='k')

    fig, ax = plt.subplots(1, 1, figsize=set_size(width))
    ax = sns.lineplot(data=range_df,
                      err_style='bars', marker="o")
    # ax = sns.heatmap(range_df)
    # ax.set(yscale="log")
    # ax.axes.get_xaxis().get_label().set_visible(False)
    plt.xlabel("Range Window Size (\%)")
    plt.ylabel("Range Query Time (ms)")
    plt.show()
    # fig = sns_plot.get_figure()
    fig.savefig('range_result.pdf', format='pdf', bbox_inches='tight')


if __name__ == '__main__':
    # filename = sys.argv[1]
    plot()
