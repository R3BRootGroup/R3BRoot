#ifndef _R3BConstantFraction_
#define _R3BConstantFraction_

#include "FairTask.h"

struct pulse
{
  double time;
  double segmentTime;
  double amplitude;
  int segment;
  pulse* nextActivePulse;
  pulse* prevoiusActivePulse;
};

struct pulseShapeParamaterStruct
{
  double* offset;
  double* a;
  double* c;
};

struct cfdPulseDefiningParameterStruct
{
  double x0, x1, x2, x3, x4;
};

class R3BConstantFraction
{
  private:
    double delay;        //Timeshift for negative pulses
    double fraction;     //Decrease of amplitude of negative pulses
    double threshold;    //Threshold for the trigger (leading edge)

    double* segmentTimePosition;
    pulseShapeParamaterStruct pulseShapeParameters;
    int nrOfPulseSegments;

    pulse* pulses;
    pulse** pulsePointer;
    int arrayCapacity;

    bool useLeadingEdge;
    
    double calibrationTimeShift;

    static bool comp (pulse* hit_a, pulse* hit_b);
    double IterateThroughTime(int totalNumOfPulses);
    void PulseParameterGenerator (double amplitude, double time, int segment, double* pulseParameters);
    double PrimitiveParabel(double x, double xCenter,double a, double c);

    double FindZero(double A, double B, double C, double time, double nextTime);

  public:
    void Init(cfdPulseDefiningParameterStruct* parameters);
    double Calculate(int nrOfPaddleHits, double* pmHitTimes, double* pmHitEnergies);
    void SetParameters(double _threshold, double _delay, double _fraction);
    void SetParameters(double _threshold);
    ~R3BConstantFraction();
};

#endif
