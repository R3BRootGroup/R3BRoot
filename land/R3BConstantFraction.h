#ifndef _R3BConstantFraction_
#define _R3BConstantFraction_

#include "FairTask.h"

struct lightPulse
{
	double time;
	double segmentTime;
	double energy;
	int segment;
	lightPulse* nextActivePulse;
	lightPulse* prevoiusActivePulse;
};

struct timeEnergyStruct
{
	double energy;
	double time;
	int segment;
};

struct pulseShapeParamaterStruct
{
	double a1, c1;
	double a2, c2;
	double a3, c3;
	double a4, c4;
};

struct cfdPulseDefiningParameterStruct
{
	double x0, x1, x2, x3, x4;
};

class R3BConstantFraction
{
	private:
		double deltaTime; 					//Timeshift for negative pulses
		double amplitudeScaling; 		//Decrease of amplitude of negative pulses
		double triggerThreshold;		//Threshold for the trigger (leading edge)

		double* segmentTimePosition;
		pulseShapeParamaterStruct pulseShapeParameters;
		int nrOfPulseSegments;

		bool useLeadingEdge;
		
		double calibrationTimeShift;

		static bool comp (lightPulse * hit_a, lightPulse * hit_b);
		double IterateThroughTime(int totalNumOfPulses, lightPulse** pulsePointer);
		void PulseParameterGenerator (Double_t amplitude, Double_t time, Int_t segment, double* pulseParameters);
		double PrimitiveParabel(double x, double xCenter,double a, double c);

		double FindZero(double A, double B, double C, double time, double nextTime);

	public:
		void Init(cfdPulseDefiningParameterStruct* parameters);
		double Calculate(int nrOfPaddleHits, double* pmHitTimes, double* pmHitEnergies);
		void SetParameters(double _threshold, double _deltaTime, double _amplitudeScaling);
		void SetParameters(double _threshold);
		~R3BConstantFraction();
};

#endif
