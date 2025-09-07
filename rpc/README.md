# Overview of the Resistive Plate Chamber (RPC) classes

The **RPC detector** consists of **two multigap RPC modules**, each enclosed in a permanently sealed plastic gas-tight box, equipped with feed-throughs for **gas** and **High Voltage (HV)** connections.

## Structure

- Each RPC module has **six gas gaps**, defined by **seven 1 mm thick float glass electrodes** (~1550 × 1250 mm²), separated by **0.3 mm nylon monofilaments**.  
- The HV electrodes are formed by a **semi-conductive layer** applied with airbrush techniques on the outer surfaces of the outermost glasses.  
- Both modules are read out in parallel by a **readout strip plane** consisting of:
  - **41 copper strips** (29 mm width, 30 mm pitch, 1600 mm long),  
  - Located between the two modules.  
- Two ground planes (top and bottom of the stack) complete the readout system.  
- The whole structure is enclosed in an **aluminum box** providing:
  - Electromagnetic insulation  
  - Mechanical rigidity  

## Readout Electronics

- Strips are read from both sides by **fast Front End Electronics (FEE)** capable of encoding:
  - **Time** (leading edge) with precision **<30 ps**  
  - **Charge** (pulse width), obtained via **Time over Threshold (TToT)**  
    - Integration constant ≈ **100 ns**  
- Output is handled by a **TDC-and-Readout Board (TRB, version 3)**:  
  - Equipped with **128 multihit TDC channels** (TDC-in-FPGA technology)  
  - Time precision better than **20 ps**  
- Together with a **TRBv3sc** (logic unit and trigger distribution), they form an **autonomous DAQ system** that:  
  - Exports data  
  - Synchronizes via **White Rabbit protocol** with the **R3B DAQ system**  

## Operation

- The RPC operates in an **open gas loop** with a mixture of:
  - **98% C₂H₂F₄**  
  - **2% SF₆**  
- Gas pressure: a few millibars below atmospheric pressure  
  - Atmospheric compression defines the gap width (no mechanical compression required)  
- Detector working point: ~**3000 kV/gap**  

## Data Structures

Data structures for the **mapped, precal, cal and hit levels** can be found at:

[`../r3bdata/rpcData`](../r3bdata/rpcData)

---

