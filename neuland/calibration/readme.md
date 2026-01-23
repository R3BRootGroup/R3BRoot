# NeuLAND Calibration {#neuland_cal}

[TOC]

## General

NeuLAND calibration processes is to calculate the calibration relations and parameters that are used to convert the binary TDC values (Leading edge and time-over-threshold) to the physical data (`R3BNeulandHit`). The whole calibration processes can be divided into two parts:

- TDC calibration (see `R3B::Neuland::Map2CalParTask`)
  - input: mapped data (`std::vector<R3B::PaddleTamexMappedData>` and `std::vector<R3B::PaddleTamexTrigMappedData>`)
  - output: `R3B::Map2CalPar`
- Position, time and energy calibration (see `R3B::Neuland::Cal2HitParTask`)
  - input: cal data (`std::vector<R3B::Neuland::BarCalData>`)
  - output: `R3B::Neuland::Cal2HitPar`

The output parameters are used respectively for the following two data conversions:

- Mapped data to cal data (see `R3B::Neuland::Map2CalTask`)
  - input: mapped data (`std::vector<R3B::PaddleTamexMappedData>` and `std::vector<R3B::PaddleTamexTrigMappedData>`)
  - output: cal data (`std::vector<R3B::Neuland::BarCalData>`)
- Cal data to hit data (see `R3B::Neuland::Cal2HitTask`)
  - input: cal data (`std::vector<R3B::Neuland::BarCalData>`)
  - output: hit data (`std::vector<R3BNeulandHit>`)

Verification of the calibration processes can either be done via the real experimental data or through the simulations. The details of the verification through simulations can be found in the following documentation:

- \subpage neuland_sim_cal

The following sections introduce the details of data calibrations and data conversions.

## TDC calibration

The TDC calibrations converts various TDC values of a signal to the its real times, which include the time of the leading edge and the trailing edge. The time values has the unit of **nanoseconds**. The TDC values of a signal are incorporated into two different C++ structs:

- `R3B::PaddleTamexMappedData`
- `R3B::PaddleTamexTrigMappedData`

The first struct is used by data signals while the second struct is used by the trigger signal. The purpose of trigger signals is to eliminate spurious time jumps that could occur during the beam time. The trigger signals are emitted from the starter detector (LOS detector), which are synchronized through different detectors in R3B setup. By always using the difference between signal time and trigger signal time as the time value, the value will not be changed by a time jump as the time jump always (?) shifts the times of both data signals and trigger signals equally.

## Position and time calibration

After the time values from the PMTs are available from the TDC calibration, the position and time values of hits can be calculated using the times values recorded by PMTs. Suppose the time values from the left and right PMTs on the same bar are denoted by @f$t_l@f$ and @f$t_r@f$, the total length of the scintillation bar by @f$\mathcal{L}@f$, effective speed of the light (speed of the light inside the scintillation material) by @f$c_e@f$ and time and position (distance to the center point of the bar) value of hits as @f$t@f$ and @f$x_\text{bar}@f$, one can derive such relations between the time values measured by PMTs and the real position and time values of hits:

$$
\begin{align}
t_r &= t + \frac{\mathcal{L}/2 - x_\text{bar}}{c_e} \\\
t_l &= t + \frac{\mathcal{L}/2 + x_\text{bar}}{c_e}
\end{align}
$$

During the data acquisition, the time measurements from different PMTs are not synchronized due to different lengths of wires connected to the central electronic modules sitting on the top the detector. Suppose @f$D_r@f$ and @f$D_l@f$ denote the time corrections between the measured and the real time values:

$$
\begin{equation}
t_\text{real} = t_\text{measure} + D_{r/l} 
\end{equation}
$$

By redefining @f$D_{l/r}@f$ in terms of global time synchronization factor (@f$t_\text{sync}@f$) and local time offset parameter (@f$t_\text{offset}@f$) with:

$$
\begin{align}
t_\text{offset} &= D_r - D_l\\\
t_\text{sync} &= \left(D_r + D_l\right)/2
\end{align}
$$

The relations between real hit time value and the measured time values from the left and right PMTs can be expressed as:

$$
\begin{align}
\label{eq:tl}
t_l = t + (\mathcal{L}/2 + x_\text{bar})/c_e - t_\text{offset}/2 + t_\text{sync}\\\
\label{eq:tr}
t_r = t + (\mathcal{L}/2 - x_\text{bar})/c_e + t_\text{offset}/2 + t_\text{sync}
\end{align}
$$

By summing up equations @f$\ref{eq:tl}@f$ and @f$\ref{eq:tr}@f$, one can derive the calibration relation to calculate the real hit time value from the two PMT measured time values:

$$
\begin{equation}
\label{eq:t_rel}
t = \frac{t_l + t_r}{2} - t_\text{sync} - \frac{\mathcal{L}}{2 c_e}
\end{equation}
$$

By subtracting equation @f$\ref{eq:tl}@f$ from equation @f$\ref{eq:tr}@f$, one can derive the calibration relation to calculate the real hit position:

$$
\begin{equation}
\label{eq:x_rel}
x_\text{bar} = \frac{1}{2} c_e \cdot( t_l - t_r + t_\text{offset})
\end{equation}
$$

Seeing equations @f$\ref{eq:t_rel}@f$ and @f$\ref{eq:x_rel}@f$, one can clear see that there are three calibration parameters for each bar to calculate time and position values of hits: @f$c_e@f$, @f$t_\text{sync}@f$ and @f$t_\text{offset}@f$.

### Hits from cosmic muons

Cosmic muons are the major particle source used for the NeuLAND calibration. There are two assumptions from the cosmic muons used in the calibration process:

* The traveling speed of the cosmic muon inside the detector is always same, with no slowing down from the ionization.
* All cosmic muon tracks are straight lines in three-dimensional space, without being deflected by the scintillation material.

These two assumptions give rise to two further relations regarding @f$(x, y, z, t)@f$ coordinates of hits:

$$
\begin{align}
\label{eq:x_muon}
x &= a_{x} \cdot z + b_{x} \\\
y &= a_{y} \cdot z + b_{y} \\\
t &= a_{t} \cdot z + b_{t}
\end{align}
$$

The reason why the coordinates x, y, t are expressed by the coordinate z is because the z value, even though with an error of 5 cm, is available for all bars in the detector. By combining the three equations above with equations @f$\ref{eq:t_rel}@f$ and @f$\ref{eq:x_rel}@f$, one can determine the three calibration parameters @f$c_e@f$, @f$t_\text{sync}@f$ and @f$t_\text{offset}@f$ through a fitting process. The position to the bar center @f$x_\text{bar}@f$ is corresponding to x coordinate in the case of horizontal bars and y coordinate in the case of vertical bars.

Thus, for horizontal bars, by substituting the @f$x_\text{bar}@f$ in equation @f$\ref{eq:x_rel}@f$ with the @f$x@f$ in equation @f$\ref{eq:x_muon}@f$, one has the fitting equation:

$$
\begin{equation}
\label{eq:fitting}
b_x + z \cdot a_x + c_e \cdot t_\text{diff} /2 - t_\text{offset}/2 = 0
\end{equation}
$$
with @f$t_\text{diff} \equiv t_r - t_l@f$. Similarly, for vertical bars:

$$
\begin{equation}
b_y + z \cdot a_y + c_e \cdot t_\text{diff} /2 - t_\text{offset}/2 = 0
\end{equation}
$$

By combing the time-related equations, both horizontal and vertical bars have the following fitting equation:

$$
\begin{equation}
\label{eq:fitting_t}
b_t + z \cdot a_t + t_\text{sync} + \frac{\mathcal{L}}{2\cdot c_e} - t_\text{sum}/2 = 0
\end{equation}
$$
where @f$t_\text{sum} \equiv t_r + t_l@f$.

### Calibration with Millepede-II algorithm

Fitting the three equations above with available data of cosmic muons isn't an easy task. The major problem is that parameters relating to the muon tracks are only for one event. By including more and more muon tracks in the calibration process, the total number of calibration parameters grows significantly. For 13 double planes, it can be shown that @f$N_\text{par} = 3900 + 3 \cdot N_\text{events}@f$, where @f$N_\text{par}@f$ and @f$N_\text{events}@f$ are the total number of parameters and number of events respectively. Modern fitting algorithm typically involves with matrix inversion and the computation of the inversion would become impractically slow when the number of parameters exceed over 10k. The Millepede-II algorithm is one of the solutions to such a problem. By separating the parameters into global and local parameters, and updating only the global parameters in each iteration, the computation complexity is only dependent on the bar related parameters and independent of parameters related to the muon tracks.

For more information of Millepede-II algorithm, please visit the official manual page:

* [Millepede-II - Draft Manual](https://www.desy.de/~kleinwrt/MP2/doc/html/draftman_page.html)

#### Millepede-II program

The input data of the Millepede-II program contains the following 4 types of values:

* The coefficients of all global parameters
* The coefficients of all local parameters
* measurements (summation of terms that don't contain any global or local parameters)
* errors

A significant limitation of the algorithm is that only the linear fitting equation is allowed:

$$
\begin{equation}
\mathcal{m} = \sum{a_g \cdot p_g + a_l \cdot p_l}
\end{equation}
$$
where @f$\mathcal{m}@f$ is the measurement value, @f$p_g@f$ and @f$p_l@f$ the global and local parameters and @f$a_g@f$ and @f$a_l@f$ their coefficients.

From the first look, it seems that the fitting equations in NeuLAND calibration aren't linear equations, required by the Millepede algorithm. But through redefining parameters:

$$
\begin{align*}
g_{ct} &\equiv c_e \cdot t_\text{offset} \\\
g_c &\equiv c_e
\end{align*}
$$

, the equations @f$\ref{eq:fitting}@f$ becomes

$$
\begin{equation}
\label{eq:fitting_h}
b_x + z \cdot a_x +  t_\text{diff} /2 \cdot g_{c} - g_{ct}/2 = 0
\end{equation}
$$
which is a linear equation for parameters @f$b_x@f$, @f$a_x@f$, @f$g_c@f$ and @f$g_{ct}@f$. Similarly for the vertical bars:

$$
\begin{equation}
\label{eq:fitting_v}
b_y + z \cdot a_y +  t_\text{diff} /2 \cdot g_{c} - g_{ct}/2 = 0
\end{equation}
$$

The third fitting equation, equation @f$\ref{eq:fitting_t}@f$, is a linear function once the effective speed of light parameters are determined from the previous fitting equation:

$$
\begin{equation}
b_t + z \cdot a_t + t_\text{sync} + \frac{\mathcal{L}}{2\cdot C_e} - t_\text{sum}/2 = 0
\end{equation}
$$

where @f$C_e@f$ is the determined value for the effective speed of light @f$c_e@f$.

#### Issues of rank deficit

The existence of a rank deficit in a fitting equation means that some parameters cannot be calculated from the fitting process with the given data. A very simple example in this case would be:

$$
y = a \cdot x + b + c
$$
where a, b and c are fitting parameters. It's obvious that no matter how many data pairs of (x, y) are available for the fitting process, parameters b and c can never be determined. Suppose we have two data points @f$(x_1, y_1)@f$ and @f$(x_2, y_2)@f$, the two equations can be converted into a matrix multiplication:

$$
\begin{equation*}
\begin{bmatrix} x_1 & 1 & 1 \\\
x_2 & 1 & 1\end{bmatrix} \begin{bmatrix}
a \\\ b \\\ c
\end{bmatrix} = \begin{bmatrix}
y_1 \\\ y_2
\end{bmatrix}
\end{equation*}
$$

No matter how many data are available, the first matrix is always rank deficit because the linear combination between the second and third columns. By fixing either parameter b or c, the rank deficit can be eliminated. Another case with rank deficit can be illustrated with this fitting equation:

$$
a \cdot x + b \cdot y + c = 0
$$

similar with three data points, the matrix multiplication would be:

$$
\begin{equation*}
\begin{bmatrix} x_1 & y_1 & 1 \\\
x_2 & y_2 & 1 \\\
x_3 & y_3 & 1
\end{bmatrix} \begin{bmatrix}
a \\\ b \\\ c
\end{bmatrix} = \begin{bmatrix}
0 \\\ 0 \\\ 0
\end{bmatrix}
\end{equation*}
$$

Even though the matrix on the left doesn't have a rank deficit, the solution of such an equation always leads to all parameters equal zeros, which is an invalid fitting result. The solution for this, again, is to fix one parameter, such that the vector on the right side of the equation isn't a zero vector.

#### Elimination of rank deficit in NeuLAND calibration

For the equation @f$\ref{eq:fitting_h}@f$, there are **three rank deficits** in total from the linear combinations between the terms @f$g_{ct}/2@f$ and @f$b_x@f$, between terms @f$z \cdot a_x@f$ and @f$g_{ct}/2@f$ and the zero values on one side of the equation. However, additional equations can be added both in the case of horizontal and vertical bars.

For the horizontal bars, the y coordinate can be approximated by the bar position:

$$
a_y \cdot z + b_y = y_\text{bar}
$$

where @f$y_\text{bar}@f$ is the y position of a horizontal bar. Similarly for the vertical bars:

$$
a_x \cdot z + b_x = x_\text{bar}
$$

where @f$x_\text{bar}@f$ is the x position of a vertical bar. With these two constraits, all rank deficit from equation @f$\ref{eq:fitting_h}@f$ are eliminated.

By reformatting equation @f$\ref{eq:fitting_t}@f$ such that the measurement term is at one side and other parameter terms at the other side:

$$
b_t + z \cdot a_t + t_\text{sync} = t_\text{sum}/2 - \frac{\mathcal{L}}{2\cdot c_e}
$$

Here there are **two rank deficits** from linear combination between the terms @f$b_t@f$ and @f$t_\text{sync}@f$ and the terms between @f$z \cdot a_t@f$ and @f$t_\text{sync}@f$. The solutions would be:

* Fix two @f$t_\text{sync}@f$ parameters.
* Fix one @f$t_\text{sync}@f$ parameters and @f$a_t@f$ for all events.

In the current method, the second option is chosen as the @f$a_t@f$ can be approximated from the bar positions with:

$$
a_t = \text{sign}(a_y) \cdot \sqrt{1 + a^2_y + a^2_x}
$$

## Data conversions

### From mapped data to cal data

### From cal data to hit data
