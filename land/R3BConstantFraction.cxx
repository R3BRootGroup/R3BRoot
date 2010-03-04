#include "R3BConstantFraction.h"

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "TMath.h"

#include <algorithm>
using std::cout; using std::endl;

void R3BConstantFraction::Init(cfdPulseDefiningParameterStruct* parameters)
{
	nrOfPulseSegments=4;
	segmentTimePosition = new double[nrOfPulseSegments+1];

	double x0=parameters->x0, x1=parameters->x1, x2=parameters->x2, x3=parameters->x3, x4=parameters->x4;

	segmentTimePosition[0]=x0;
	segmentTimePosition[1]=x1;
	segmentTimePosition[2]=x2;
	segmentTimePosition[3]=x3;
	segmentTimePosition[4]=x4;

	double a1, c1, a2, c2, a3, c3, a4, c4;

	//Calculate the parameters
	a1 = 1;		// Does not matter, disapear when normalising
	c1 = 0;		// The curves begins at y=0;
	a2 =	a1*x1/(x1-x2);
	c2 = a1*x1*x1-a2*(x1-x2)*(x1-x2);
	a3 = -c2/((x3-x2)*(x3-x2)-(x3-x2)*(x3-x4));
	c3 = c2;
	a4 = a3*(x3-x2)/(x3-x4);
	c4 = 0;		// The curve ends at y=0;

	//Normalise Pulse
	double volume=PrimitiveParabel(x1,x0,a1,c1);
	volume+=PrimitiveParabel(x2,x2,a2,c2)-PrimitiveParabel(x1,x2,a2,c2);
	volume+=PrimitiveParabel(x3,x2,a3,c3)-PrimitiveParabel(x2,x2,a3,c3);
	volume+=PrimitiveParabel(x4,x4,a4,c4)-PrimitiveParabel(x3,x4,a4,c4);

	//Storing the values
	pulseShapeParameters.a1=a1/volume;
	pulseShapeParameters.c1=c1/volume;

	pulseShapeParameters.a2 = a2/volume;
	pulseShapeParameters.c2 = c2/volume; 

	pulseShapeParameters.a3 = a3/volume;
	pulseShapeParameters.c3 = c3/volume;

	pulseShapeParameters.a4 = a4/volume;
	pulseShapeParameters.c4 = c4/volume;

	calibrationTimeShift=0;

	if ( ! useLeadingEdge)
	{
		// Calibrating the time of the CFD
		double thresholdTemp=triggerThreshold;
		triggerThreshold=0.1;				//Setting a low trigger for our calibration pulse
		int nrOfCalibrateHits=1;
		double pmCalibrateHitTimes[] = {10};
		double pmCalibrateEnergies[] = {1};

		double cfdTime = Calculate(nrOfCalibrateHits, pmCalibrateHitTimes, pmCalibrateEnergies);

		calibrationTimeShift = pmCalibrateHitTimes[0] - cfdTime;

		//Resetting the real trigger threshold
		triggerThreshold=thresholdTemp;
	}


}

void R3BConstantFraction::SetParameters(double _threshold, double _deltaTime, double _amplitudeScaling)
{
	triggerThreshold=_threshold;
	deltaTime=_deltaTime;
	amplitudeScaling=_amplitudeScaling;
}

void R3BConstantFraction::SetParameters(double _threshold)
{
	triggerThreshold=_threshold;
	useLeadingEdge=true;
}

//The primitive function for the curves
double R3BConstantFraction::PrimitiveParabel(double x, double xCentre,double a, double c)
{
	return a/3*(x-xCentre)*(x-xCentre)*(x-xCentre)+c*x;
}

double R3BConstantFraction::Calculate(int nrOfPaddleHits, double* pmHitTimes, double* pmHitEnergies)
{
	int totalNumOfPulses=0;

	if(useLeadingEdge)
		totalNumOfPulses = nrOfPaddleHits;
	else
		totalNumOfPulses = nrOfPaddleHits*2;

	lightPulse* pulses = new lightPulse[totalNumOfPulses];
	lightPulse** pulsePointer = new lightPulse*[totalNumOfPulses];

	for (int index =0; index < nrOfPaddleHits; index++)
	{
		pulses[index].energy = pmHitEnergies[index];
		pulses[index].time = pmHitTimes[index];
		pulses[index].segmentTime = pmHitTimes[index];
		pulses[index].segment = 0;
		pulses[index].nextActivePulse = NULL;
		pulses[index].prevoiusActivePulse = NULL;
		pulsePointer[index]=&pulses[index];

		if(! useLeadingEdge)
		{
			pulses[index+nrOfPaddleHits].energy = -pmHitEnergies[index]*amplitudeScaling;
			pulses[index+nrOfPaddleHits].time = pmHitTimes[index] + deltaTime;
			pulses[index+nrOfPaddleHits].segmentTime = pmHitTimes[index] + deltaTime;
			pulses[index+nrOfPaddleHits].segment = 0;
			pulses[index+nrOfPaddleHits].nextActivePulse = NULL;
			pulses[index+nrOfPaddleHits].prevoiusActivePulse = NULL;
			pulsePointer[index+nrOfPaddleHits]=&pulses[index+nrOfPaddleHits];
		}
	}

	std::make_heap(pulsePointer, pulsePointer + totalNumOfPulses, comp);
	
	double cfdTime =	IterateThroughTime(totalNumOfPulses, pulsePointer);
	cfdTime += calibrationTimeShift;

	delete [] pulses;
	delete [] pulsePointer;

	return cfdTime;
}

double R3BConstantFraction::IterateThroughTime(int totalNumOfPulses, lightPulse** pulsePointer)
{
	int nrOfPulsesLeft = totalNumOfPulses - 1;
	double time = 0, nextTime = 0, valueOfExtremum=0;
	double pulseParameters[3];
	double A,B,C;
	bool triggered = false;
	lightPulse* active=NULL;
	lightPulse* negActive=NULL;
	lightPulse* tempPointer=active;

	//Check all the pulses
	std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft + 1, comp);
	while (true)
	{
		// Get start time from the next point in time
		time = pulsePointer[nrOfPulsesLeft]->segmentTime;
		pulsePointer[nrOfPulsesLeft]->segment++;

		// If new active pulse, add to list of active pulses
		if (pulsePointer[nrOfPulsesLeft]->segment == 1)
		{
			if (active == NULL)
				negActive = pulsePointer[nrOfPulsesLeft];
			else
				active->prevoiusActivePulse = pulsePointer[nrOfPulsesLeft];
			pulsePointer[nrOfPulsesLeft]->nextActivePulse = active;
			active = pulsePointer[nrOfPulsesLeft];
		}
		
		// If the pulse is to be removed from the list of active pulses
		if (pulsePointer[nrOfPulsesLeft]->segment > 4)
		{
			nrOfPulsesLeft--;
			negActive = negActive->prevoiusActivePulse;
			if (negActive == NULL)
				active = NULL;
			else
				negActive->nextActivePulse = NULL;
		}
		else	// Else, update the time nd push in to heap
		{
			pulsePointer[nrOfPulsesLeft]->segmentTime = pulsePointer[nrOfPulsesLeft]->time + segmentTimePosition[pulsePointer[nrOfPulsesLeft]->segment];
			std::push_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
		}

		// Pop from heap to get time of stop
		std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
		nextTime=pulsePointer[nrOfPulsesLeft]->segmentTime;

		//Calculate the curve parameters for the current time interval
		A=0; B=0; C=0;
		tempPointer=active;
		while (tempPointer != NULL)	// Loop the active pulses
		{
			if (tempPointer->energy > 0 || triggered)
			{
				PulseParameterGenerator(tempPointer->energy, tempPointer->time, tempPointer->segment-1, pulseParameters);

				A += pulseParameters[0];
				B += pulseParameters[1];
				C += pulseParameters[2];
			}
			tempPointer = tempPointer->nextActivePulse;
		}

		//Search for leading edge
		if (! triggered)
		{
			double xPosOfExtremum = -B/(2*A);			//Calculated with the derivative
			double valueOfEndPoint = A*nextTime*nextTime + B*nextTime + C;

			if (valueOfEndPoint > triggerThreshold)
			{
				if (useLeadingEdge)
					return FindZero(A, B, C-triggerThreshold, time, nextTime);
				else
					triggered = true;
			}
			else if (xPosOfExtremum > time && xPosOfExtremum < nextTime)
			{
				valueOfExtremum = A*xPosOfExtremum*xPosOfExtremum + B*xPosOfExtremum + C;
				if (valueOfExtremum > triggerThreshold )
				{
					if (useLeadingEdge)
						return FindZero(A, B, C-triggerThreshold, time, nextTime);
					else
					triggered = true;
				}
			}
		}
		else	//Search for zero
		{
			double valueOfStartPoint = A*time*time + B*time + C;
			double valueOfEndPoint = A*nextTime*nextTime + B*nextTime + C;

			if(valueOfStartPoint > 0 && valueOfEndPoint < 0)
				return FindZero(A, B, C, time, nextTime);
		}

		// If at the end of the last active pulse, remove
		if (pulsePointer[nrOfPulsesLeft]->segment == 4 && negActive == active)
		{
			if (nrOfPulsesLeft == 0)
				return TMath::Sqrt(-1);	// No more pulses to search, return NaN

			nrOfPulsesLeft--;
			negActive = negActive->prevoiusActivePulse;
			if (negActive == NULL)
				active = NULL;
			else
				negActive->nextActivePulse = NULL;

			std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft + 1, comp);
		}
	}
}

/*-------------------------------------------------------------------------------*/

double R3BConstantFraction::FindZero(double A, double B, double C, double time, double nextTime)
{
		//The zero is close
		//Solve the quadratic function
		//x_{1,2}=1/(2*A)*(-B +- sqrt(B^2-4AC))
		double sqrt=TMath::Sqrt(B*B-4*A*C);
		double denominator= 1/(2*A);
		double x_1=(-B+sqrt)*denominator;
		double x_2=(-B-sqrt)*denominator;

		if (x_1 >= time && x_1 <= nextTime) 
			return x_1;
		else if (x_2 >= time && x_2 <= nextTime)
			return x_2;
		else
			return -2; //If this is returned somthing went wrong...
}

bool R3BConstantFraction::comp (lightPulse * hit_a, lightPulse * hit_b)
{
	double a_time, b_time;
	if (hit_a->segment < 0)
		a_time = hit_a->time;
	else
		a_time = hit_a->segmentTime;

	if (hit_b->segment < 0)
		b_time = hit_b->time;
	else
		b_time = hit_b->segmentTime;

	if (a_time < b_time)
		return false;
	else
		return true;
}

void R3BConstantFraction::PulseParameterGenerator (Double_t amplitude, Double_t time, Int_t segment, double* pulseParameters)
{
	double a=0, b=0, c=0;

	//Four quadratic functions f(x)=a(x-b)^2+c ti f(x)=Ax^2+Bx+C
	if(segment==0)
	{
		a=amplitude*pulseShapeParameters.a1;
		b=time;
		c=amplitude*pulseShapeParameters.c1;
	}
	else if(segment==1)
	{
		a=amplitude*pulseShapeParameters.a2;
		b=segmentTimePosition[2]+time;
		c=amplitude*pulseShapeParameters.c2;
	}
	else if(segment==2)
	{
		a=amplitude*pulseShapeParameters.a3;
		b=segmentTimePosition[2]+time;
		c=amplitude*pulseShapeParameters.c3;
	}
	else if(segment == 3)
	{
		a=amplitude*pulseShapeParameters.a4;
		b=segmentTimePosition[4]+time;
		c=amplitude*pulseShapeParameters.c4;
	}
	
	pulseParameters[0]=a;				// A
	pulseParameters[1]=-2*a*b;	// B
	pulseParameters[2]=a*b*b+c;	// C
}


R3BConstantFraction::~R3BConstantFraction()
{
	delete [] segmentTimePosition;
}
