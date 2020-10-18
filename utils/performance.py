import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd


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
# width = 345

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


width = 417.47307

sns.set_theme(style="darkgrid", color_codes=True)

memory = {
    'model': ['R-Tree', 'R-Tree', 'R-Tree', 'ZM', 'ZM', 'ZM', 'LSPH', 'LSPH', 'LSPH'],
    'data': ['post', 'g2', 'osm', 'post', 'g2', 'osm', 'post', 'g2', 'osm'],
    'memory': [35.78, 3.75, 11539.14, 92.31, 92.30, 966.676, 53.88, 2.53, 11656.16]
}
memory_df = pd.DataFrame(
    memory, columns=['model', 'data', 'memory'])

query_time = {
    'model': ['R-Tree', 'R-Tree', 'R-Tree', 'ZM', 'ZM', 'ZM', 'LSPH', 'LSPH', 'LSPH'],
    'data': ['post', 'g2', 'osm', 'post', 'g2', 'osm', 'post', 'g2', 'osm'],
    'query_time': [176.86, 2.81, 402471.05,  9.45, 0.148, 2787076.95, 6.78, 0.291, 46210.702]
}
query_time_df = pd.DataFrame(
    query_time, columns=['model', 'data', 'query_time'])

avg_query_time = {
    'model': ['R-Tree', 'R-Tree', 'R-Tree', 'ZM', 'ZM', 'ZM', 'LSPH', 'LSPH', 'LSPH'],
    'data': ['post', 'g2', 'osm', 'post', 'g2', 'osm', 'post', 'g2', 'osm'],
    'avg_query_time': [1453, 1372, 3516, 76, 72, 24249, 56, 145, 403]
}
avg_query_time_df = pd.DataFrame(
    avg_query_time, columns=['model', 'data', 'avg_query_time'])

build_time = {
    'model': ['R-Tree', 'R-Tree', 'R-Tree', 'ZM', 'ZM', 'ZM', 'LSPH', 'LSPH', 'LSPH'],
    'data': ['post', 'g2', 'osm', 'post', 'g2', 'osm', 'post', 'g2', 'osm'],
    'build_time': [519.2, 8.37, 868947.945, 300, 8.37, 15300, 69.78, 41.31, 54818.9]
}
build_time_df = pd.DataFrame(
    build_time, columns=['model', 'data', 'build_time'])


fig, ax = plt.subplots(1, 1, figsize=set_size(width))
ax = sns.barplot(x="data", y="memory", hue="model", data=memory_df)
ax.set(yscale="log")
ax.axes.get_xaxis().get_label().set_visible(False)
plt.ylabel("Memory [MB]")
plt.show()
fig.savefig('memory.pdf', format='pdf', bbox_inches='tight')


fig, ax = plt.subplots(1, 1, figsize=set_size(width))
ax = sns.barplot(x="data", y="query_time",
                 hue="model", data=query_time_df)
ax.set(yscale="log")
ax.axes.get_xaxis().get_label().set_visible(False)
plt.ylabel("Total Query Time [ms]")
plt.show()
fig.savefig('query_time.pdf', format='pdf', bbox_inches='tight')


fig, ax = plt.subplots(1, 1, figsize=set_size(width))
ax = sns.barplot(x="data", y="avg_query_time",
                 hue="model", data=avg_query_time_df)
ax.set(yscale="log")
ax.axes.get_xaxis().get_label().set_visible(False)
plt.ylabel("Avg Query Time [ns]")
plt.show()
fig.savefig('avg_query_time.pdf', format='pdf', bbox_inches='tight')


fig, ax = plt.subplots(1, 1, figsize=set_size(width))
ax = sns.barplot(x="data", y="build_time",
                 hue="model", data=build_time_df)
ax.set(yscale="log")
ax.axes.get_xaxis().get_label().set_visible(False)
plt.ylabel("Build Time [ms]")
plt.show()
fig.savefig('build_time.pdf', format='pdf', bbox_inches='tight')
