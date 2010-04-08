#include "R3BConstantFraction.h"

#include <cstdlib>
#include <iostream>
#include "TMath.h"

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

	double a0, c0, a1, c1, a2, c2, a3, c3;

	//Calculate the parameters
	a0 = 1;		// Does not matter, disapear when normalising
	c0 = 0;		// The curve begins at y=0;
	a1 =	a0*x1/(x1-x2);
	c1 = a0*x1*x1-a1*(x1-x2)*(x1-x2);
	a2 = -c1/((x3-x2)*(x3-x2)-(x3-x2)*(x3-x4));
	c2 = c1;
	a3 = a2*(x3-x2)/(x3-x4);
	c3 = 0;		// The curve ends at y=0;

	//Normalise Pulse
	double area=PrimitiveParabel(x1,x0,a0,c0);
	area+=PrimitiveParabel(x2,x2,a1,c1)-PrimitiveParabel(x1,x2,a1,c1);
	area+=PrimitiveParabel(x3,x2,a2,c2)-PrimitiveParabel(x2,x2,a2,c2);
	area+=PrimitiveParabel(x4,x4,a3,c3)-PrimitiveParabel(x3,x4,a3,c3);

	pulseShapeParameters.offset = new double[nrOfPulseSegments];
	pulseShapeParameters.a = new double[nrOfPulseSegments];
	pulseShapeParameters.c = new double[nrOfPulseSegments];

	//Storing the values
	pulseShapeParameters.a[0] = a0/area;
	pulseShapeParameters.c[0] = c0/area;
	pulseShapeParameters.offset[0]=x0;

	pulseShapeParameters.a[1] = a1/area;
	pulseShapeParameters.c[1] = c1/area; 
	pulseShapeParameters.offset[1]=x2;

	pulseShapeParameters.a[2] = a2/area;
	pulseShapeParameters.c[2] = c2/area;
	pulseShapeParameters.offset[2]=x2;

	pulseShapeParameters.a[3] = a3/area;
	pulseShapeParameters.c[3] = c3/area;
	pulseShapeParameters.offset[3]=x4;

	calibrationTimeShift=0;
	arrayCapacity=0;
	if ( ! useLeadingEdge)
	{
		// Calibrating the time of the CFD
		double thresholdTemp=threshold;
		threshold=0;				//Setting a low trigger for our calibration pulse
		int nrOfCalibrateHits=1;
		double pmCalibrateHitTimes[] = {0};
		double pmCalibrateEnergies[] = {1};
	
		double cfdTime = Calculate(nrOfCalibrateHits, pmCalibrateHitTimes, pmCalibrateEnergies);

		calibrationTimeShift = pmCalibrateHitTimes[0] - cfdTime;

		//Resetting the real trigger threshold
		threshold=thresholdTemp;
	}
}

void R3BConstantFraction::SetParameters(double _threshold, double _delay, double _fraction)
{
	threshold=_threshold;
	delay=_delay;
	fraction=_fraction;
	useLeadingEdge=false;
}

void R3BConstantFraction::SetParameters(double _threshold)
{
	threshold=_threshold;
	useLeadingEdge=true;
}

//The primitive function for the curves
double R3BConstantFraction::PrimitiveParabel(double x, double xCentre,double a, double c)
{
	return a/3*(x-xCentre)*(x-xCentre)*(x-xCentre)+c*x;
}

double R3BConstantFraction::Calculate(int nrOfPaddleHits, double* hitTimes, double* hitAmplitudes)
{
	int totalNumOfPulses=0;

	if(useLeadingEdge)
		totalNumOfPulses = nrOfPaddleHits;
	else
		totalNumOfPulses = nrOfPaddleHits*2;

	//If allocated array is too short reallocate
	if (totalNumOfPulses > arrayCapacity)
	{
		pulses = (pulse*) realloc(pulses, sizeof(pulse)*totalNumOfPulses);
		pulsePointer = (pulse**) realloc(pulsePointer, sizeof(pulse*)*totalNumOfPulses);
		arrayCapacity=totalNumOfPulses;
		if (pulses==NULL || pulsePointer==NULL)
			Fatal("R3BConstantFraction::Calculate", "Error (re)allocating memory");
	}

	//Fill the arrays
	for (int index =0; index < nrOfPaddleHits; index++)
	{
		pulses[index].amplitude = hitAmplitudes[index];
		pulses[index].time = hitTimes[index];
		pulses[index].segmentTime = hitTimes[index];
		pulses[index].segment = 0;
		pulses[index].nextActivePulse = NULL;
		pulses[index].prevoiusActivePulse = NULL;
		pulsePointer[index]=&pulses[index];

		if(! useLeadingEdge)
		{
			pulses[index+nrOfPaddleHits].amplitude = -hitAmplitudes[index]*fraction;
			pulses[index+nrOfPaddleHits].time = hitTimes[index] + delay;
			pulses[index+nrOfPaddleHits].segmentTime = hitTimes[index] + delay;
			pulses[index+nrOfPaddleHits].segment = 0;
			pulses[index+nrOfPaddleHits].nextActivePulse = NULL;
			pulses[index+nrOfPaddleHits].prevoiusActivePulse = NULL;
			pulsePointer[index+nrOfPaddleHits]=&pulses[index+nrOfPaddleHits];
		}
	}

	std::make_heap(pulsePointer, pulsePointer + totalNumOfPulses, comp);

	double cfdTime =	IterateThroughTime(totalNumOfPulses);

	if( !TMath::IsNaN(cfdTime))
		cfdTime += calibrationTimeShift;

	return cfdTime;
}

//double R3BConstantFraction::IterateThroughTime(int totalNumOfPulses, pulse** pulsePointer)
double R3BConstantFraction::IterateThroughTime(int totalNumOfPulses)
{
	int nrOfPulsesLeft = totalNumOfPulses - 1;
	double time = 0, nextTime = 0, valueOfExtremum=0;
	double pulseParameters[3];
	double A,B,C;
	bool triggered = false;
	pulse* active=NULL;
	pulse* negActive=NULL;
	pulse* tempPointer=active;

	//Check all the pulses
	std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
	while (true)
	{
		pulse* &aPulsePointer = pulsePointer[nrOfPulsesLeft];

		// Get start time from the next point in time
		time = aPulsePointer->segmentTime;
		aPulsePointer->segment++;

		// If new active pulse, add to list of active pulses
		if (aPulsePointer->segment == 1)
		{
			if (active == NULL)
				negActive = aPulsePointer;
			else
				active->prevoiusActivePulse = aPulsePointer;
			aPulsePointer->nextActivePulse = active;
			active = aPulsePointer;
		}
		
		// If the pulse is to be removed from the list of active pulses
		if (aPulsePointer->segment > 4)
		{
			nrOfPulsesLeft--;
			negActive = negActive->prevoiusActivePulse;
			if (negActive == NULL)
				active = NULL;
			else
				negActive->nextActivePulse = NULL;
		}
		else	// Else, update the time and push in to heap
		{
			aPulsePointer->segmentTime = aPulsePointer->time + segmentTimePosition[aPulsePointer->segment];
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
			if (tempPointer->amplitude > 0 || triggered)
			{
				PulseParameterGenerator(tempPointer->amplitude, tempPointer->time, tempPointer->segment-1, pulseParameters);

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

			if (valueOfEndPoint > threshold)
			{
				if (useLeadingEdge)
					return FindZero(A, B, C-threshold, time, nextTime);
				else
					triggered = true;
			}
			else if (xPosOfExtremum > time && xPosOfExtremum < nextTime)
			{
				valueOfExtremum = A*xPosOfExtremum*xPosOfExtremum + B*xPosOfExtremum + C;
				if (valueOfExtremum > threshold )
				{
					if (useLeadingEdge)
						return FindZero(A, B, C-threshold, time, nextTime);
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

			std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
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
		double x_1=(-B-sqrt)*denominator;
		double x_2=(-B+sqrt)*denominator;

		if (x_1 >= time && x_1 <= nextTime) 
			return x_1;
		else if (x_2 >= time && x_2 <= nextTime)
			return x_2;
		else
			Fatal("R3BConstantFraction::FindZero","No zero found"); 
}

bool R3BConstantFraction::comp (pulse * hit_a, pulse * hit_b)
{
	if (hit_a->segmentTime < hit_b->segmentTime)
		return false;
	else
		return true;
}


void R3BConstantFraction::PulseParameterGenerator (double amplitude, double time, int segment, double* pulseParameters)
{
	double a=0, b=0, c=0;

	//Four quadratic functions f(x)=a(x-b)^2+c to f(x)=Ax^2+Bx+C
	a=amplitude*pulseShapeParameters.a[segment];	
	b=pulseShapeParameters.offset[segment]+time;
	c=amplitude*pulseShapeParameters.c[segment];

	pulseParameters[0]=a;				// A
	pulseParameters[1]=-2*a*b;	// B
	pulseParameters[2]=a*b*b+c;	// C
}


R3BConstantFraction::~R3BConstantFraction()
{
	delete [] pulses;
	delete [] pulsePointer;
	delete [] segmentTimePosition;
 	delete [] pulseShapeParameters.offset;
	delete [] pulseShapeParameters.a;
	delete [] pulseShapeParameters.c;
}
