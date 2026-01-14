import itertools

import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt


def th2d_to_arrays(histogram):
    xaxis, yaxis = (histogram.GetXaxis(), histogram.GetYaxis())
    bin_xmin, bin_xmax = (xaxis.GetFirst(), xaxis.GetLast())
    bin_ymin, bin_ymax = (yaxis.GetFirst(), yaxis.GetLast())
    nbinsx, nbinsy = (bin_xmax - bin_xmin + 1, bin_ymax - bin_ymin + 1)

    x_data = np.zeros(nbinsx)
    y_data = np.zeros(nbinsy)
    z_data = np.zeros((nbinsy, nbinsx))
    for index, bin_x in enumerate(range(bin_xmin, bin_xmax + 1)):
        x_data[index] = histogram.GetXaxis().GetBinCenter(bin_x)
    for index, bin_y in enumerate(range(bin_ymin, bin_ymax + 1)):
        y_data[index] = histogram.GetYaxis().GetBinCenter(bin_y)
    for (idx_x, bin_x), (idx_y, bin_y) in itertools.product(
        enumerate(range(bin_xmin, bin_xmax + 1)),
        enumerate(range(bin_ymin, bin_ymax + 1)),
    ):
        z_data[idx_y, idx_x] = histogram.GetBinContent(bin_x, bin_y)
        # z_data[z_data == 0.] = np.nan
    return x_data, y_data, z_data


def th1d_to_df(histogram):
    xaxis = histogram.GetXaxis()
    bin_xmin, bin_xmax = (xaxis.GetFirst(), xaxis.GetLast())
    nbinsx = bin_xmax - bin_xmin + 1

    if bool(histogram.GetXaxis().GetLabels()):
        x_data = []
        y_data = []
        for idx, name in zip(
            range(0, nbinsx), histogram.GetXaxis().GetLabels()
        ):
            x_data.append(str(name))
            y_data.append(histogram.GetBinContent(idx + 1))
        return pd.DataFrame(
            {"x": pd.Series(x_data, dtype="string"), "y": y_data}
        )
    x_data = np.zeros(nbinsx)
    y_data = np.zeros(nbinsx)

    for index, bin_n in enumerate(range(bin_xmin, bin_xmax + 1)):
        x_data[index] = histogram.GetBinCenter(bin_n)
        y_data[index] = histogram.GetBinContent(bin_n)

    return pd.DataFrame({"x": x_data, "y": y_data})


class PlotHist:
    def __init__(self, root_folder, hist_names):
        self._root_folder = root_folder
        self._hist_names = hist_names
        self._fig = None
        self._axes = None

    def __call__(self, fig=None, axes=None):
        if fig is not None and axes is not None:
            self._fig = fig
            self._axes = axes
        else:
            self._fig, self._axes = plt.subplots(len(self._hist_names), 1)
        if not hasattr(self._axes, "__len__"):
            self._plot_hist(self._hist_names[0], self._axes)
            return self._fig, self._axes
        for hist_name, axis in zip(self._hist_names, self._axes.reshape(-1)):
            self._plot_hist(hist_name, axis)
        return self._fig, self._axes

    def _plot_hist(self, hist_name, axis):
        assert self._fig is not None
        hist_obj = self._root_folder.Get(hist_name)
        class_name = hist_obj.ClassName()
        if "TH1" in class_name:
            PlotHist.plot_th1(hist_obj, axis)
        elif "TH2" in class_name:
            pos = PlotHist.plot_th2(hist_obj, axis)
            self._fig.colorbar(pos, ax=axis)
        else:
            raise NameError(
                f"Cannot plot the object {hist_name} with class {class_name}"
            )

    @staticmethod
    def plot_th1(hist_obj, axis, **kwargs):
        data_df = th1d_to_df(hist_obj)

        if bool(hist_obj.GetXaxis().GetLabels()):
            sns.barplot(data=data_df, x="y", y="x", ax=axis, **kwargs)
            axis.set(xlabel=hist_obj.GetYaxis().GetTitle())
            axis.set(ylabel=hist_obj.GetXaxis().GetTitle())
        else:
            sns.lineplot(
                data=data_df,
                x="x",
                y="y",
                drawstyle="steps-mid",
                ax=axis,
                **kwargs,
            )
            x_bin_width = hist_obj.GetXaxis().GetBinWidth(0)
            x_bin_min = hist_obj.GetXaxis().GetXmin()
            x_bin_max = hist_obj.GetXaxis().GetXmax()
            axis.set(
                xlim=(x_bin_min + x_bin_width / 2, x_bin_max - x_bin_width / 2)
            )
            axis.set(xlabel=hist_obj.GetXaxis().GetTitle())
            axis.set(ylabel=hist_obj.GetYaxis().GetTitle())
        axis.set(title=hist_obj.GetTitle())

    @staticmethod
    def plot_th1s(hist_objs, axis, **kwargs):
        data_dfs = []
        for key, hist in hist_objs.items():
            data_df = th1d_to_df(hist)
            data_df["label"] = key
            data_dfs.append(data_df)
        total_data_df = pd.concat(data_dfs)

        first_hist = list(hist_objs.values())[0]
        if bool(first_hist.GetXaxis().GetLabels()):
            sns.barplot(
                data=total_data_df,
                x="y",
                y="x",
                ax=axis,
                hue="label",
                **kwargs,
            )
            axis.set(xlabel=first_hist.GetYaxis().GetTitle())
            axis.set(ylabel=first_hist.GetXaxis().GetTitle())
        else:
            sns.lineplot(
                data=total_data_df,
                x="x",
                y="y",
                drawstyle="steps-mid",
                ax=axis,
                hue="label",
            )
            x_bin_width = first_hist.GetXaxis().GetBinWidth(0)
            x_bin_min = first_hist.GetXaxis().GetXmin()
            x_bin_max = first_hist.GetXaxis().GetXmax()
            axis.set(
                xlim=(x_bin_min + x_bin_width / 2, x_bin_max - x_bin_width / 2)
            )
            axis.set(xlabel=first_hist.GetXaxis().GetTitle())
            axis.set(ylabel=first_hist.GetYaxis().GetTitle())
        # axis.set(title=hist_obj.GetTitle())

    @staticmethod
    def plot_th2(hist_obj, axis):
        x_data, y_data, z_data = th2d_to_arrays(hist_obj)
        x_bin_width = hist_obj.GetXaxis().GetBinWidth(0)
        y_bin_width = hist_obj.GetYaxis().GetBinWidth(0)
        axis.set(xlabel=hist_obj.GetXaxis().GetTitle())
        axis.set(ylabel=hist_obj.GetYaxis().GetTitle())
        axis.set(title=hist_obj.GetTitle())
        plot = axis.imshow(
            z_data,
            extent=[
                min(x_data) - x_bin_width / 2,
                max(x_data) + x_bin_width / 2,
                min(y_data) - y_bin_width / 2,
                max(y_data) + y_bin_width / 2,
            ],
            origin="lower",
            aspect="auto",
        )
        plot.set(cmap="gnuplot2_r")
        return plot
