import itertools

import numpy as np
import pandas as pd


def th2d_to_arrays(histgram):
    nbinsy = histgram.GetNbinsY()
    nbinsx = histgram.GetNbinsX()
    x_data = np.zeros(nbinsx)
    y_data = np.zeros(nbinsy)
    z_data = np.zeros((nbinsy, nbinsx))
    for index in range(nbinsx):
        x_data[index] = histgram.GetXaxis().GetBinCenter(index + 1)
    for index in range(nbinsy):
        y_data[index] = histgram.GetYaxis().GetBinCenter(index + 1)
    for idx_x, idx_y in itertools.product(range(1, nbinsx), range(1, nbinsy)):
        z_data[idx_y, idx_x] = histgram.GetBinContent(idx_x + 1, idx_y + 1)
    return x_data, y_data, z_data


def th1d_to_df(histgram):
    nbinsx = histgram.GetNbinsX()
    x_data = np.zeros(nbinsx)
    y_data = np.zeros(nbinsx)

    for index in range(0, nbinsx):
        x_data[index] = histgram.GetBinCenter(index + 1)
        y_data[index] = histgram.GetBinContent(index + 1)

    return pd.DataFrame({"x": x_data, "y": y_data})
