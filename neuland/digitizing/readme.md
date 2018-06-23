# Neuland Digitizing

Digitizing is the process of converting the "raw" Monte Carlo energy depositions to experimental look-alike hits.

## Overview

The Digitizing task `R3BNeulandDigitizer` handles the Input and Output of data. The task takes the `R3BNeulandPoints` from the Simulation, runs them through a `DigitizingEngine` and fills `R3BNeulandHits`. It also requires the configuration storage `NeulandGeoPar` to convert position coordinates.

A `DigitizingEngine` handles the actual data processing. The `DigitizingEngine` is an abstract class (not to be instantiated) and includes all common, electronics-independent logic. The derived `DigitizingTacQuila` and `DigitizingTamex` classes implement the logic specific to these signal processing devices.
These classes are independent of any (Fair-/R3B-)Root class, except for the random number generator, and can be reused in other projects if necessary. 

Control histograms are created by the `R3BNeulandHitMon` task to keep the `R3BNeulandDigitizer` clean. 



## Digitizing Procedure

The Neuland Digitizing Engine handles the detector physics and data acquisition
part of the conversion from raw deposited energy to experiment-like detector hits.

The deposited energy has already been converted to a light amount in the simulation class using Birk's law (`simulation\R3BNeuland.cxx`-`GetLightYield()`). This light amount enters the digitizer via 
```C++
void DepositLight(Int_t paddle_id, Double_t time, Double_t light, Double_t dist);
```

Once every light point has been deposited in the digitizing engine, the results can be obtained with 
```C++
std::map<Int_t, std::unique_ptr<Digitizing::Paddle>> ExtractPaddles();
```
The digitizer is ready for the next event immediately and does not require an explicit reset.  


### Channel

The photomultiplier tube (PMT) receives the individual light impulses (`PMTHits`).
Together with its associated signal processing electronics a `Channel` is formed. The response of the channel to the `PMTHits` depends on the specific electronics. The digitizers derive from `Digitizing::Channel` and have to implement this response:
```C++
class Channel
{
    virtual void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) = 0;
    virtual bool HasFired() const = 0;
    virtual Double_t GetQDC() const = 0;
    virtual Double_t GetTDC() const = 0;
    virtual Double_t GetEnergy() const = 0;

  protected:
    std::vector<PMTHit> fPMTHits;
};
``` 


### Paddle

A paddle has a channel at the left and right side. The behavior is electronics independent:   
```C++
class Paddle
{
  public:
    Paddle(std::unique_ptr<Channel> l, std::unique_ptr<Channel> r);
    void DepositLight(Double_t time, Double_t light, Double_t dist);

    bool HasFired() const;
    bool HasHalfFired() const;
    Double_t GetEnergy() const;
    Double_t GetTime() const;
    Double_t GetPosition() const;

    const Channel* GetLeftChannel() const { return fLeftChannel.get(); }
    const Channel* GetRightChannel() const { return fRightChannel.get(); }

  protected:
    std::unique_ptr<Channel> fLeftChannel;
    std::unique_ptr<Channel> fRightChannel;
}
```

The class derived from `DigitizingEngine` implements the factory for the correct class derived from `Channel`:
```C++
virtual std::unique_ptr<Digitizing::Channel> BuildChannel() = 0;
```
