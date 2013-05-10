
/******************************************/
/*                                        */
/*      Class simulating a CFD or LE      */
/*                                        */
/*       Each hit generates a pulse       */
/* proportional to the energy of the hit. */
/*  Overlapping pulses are superimposed   */
/*                                        */
/*        Johan Gill & Staffan Wranne     */
/*               10/2 2011                */
/*                                        */
/******************************************/


#include "R3BConstantFraction.h"

#include <cstdlib>
#include <iostream>
#include <limits>
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
  a0 = 1;    // Does not matter, disapear when normalising
  c0 = 0;    // The curve begins at y=0;
  a1 = a0*x1/(x1-x2);
  c1 = a0*x1*x1-a1*(x1-x2)*(x1-x2);
  a2 = -c1/((x3-x2)*(x3-x2)-(x3-x2)*(x3-x4));
  c2 = c1;
  a3 = a2*(x3-x2)/(x3-x4);
  c3 = 0;    // The curve ends at y=0;

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

  threshold *= pulseShapeParameters.c[2];  //Convert MeV to pulseheight

  calibrationTimeShift=0;
  arrayCapacity=0;
  pulses=NULL;
  pulsePointer=NULL;  

  if ( ! useLeadingEdge)
  {
    // Calibrating the time of the CFD
    double thresholdTemp=threshold;
    threshold=0;                          //Setting a low trigger for our calibration pulse
    int nrOfCalibrateHits=1;
    double pmCalibrateHitTimes[] = {0};
    double pmCalibrateEnergies[] = {1};
  
    double cfdTime = Calculate(nrOfCalibrateHits, pmCalibrateHitTimes, pmCalibrateEnergies);

    calibrationTimeShift = pmCalibrateHitTimes[0] - cfdTime;

    //Resetting the real trigger threshold
    threshold=thresholdTemp;
  }
  else
  {
    // Calibrating the time of the LE
    double thresholdTemp=threshold;
    threshold=0.5*pulseShapeParameters.c[2];      //Setting a low trigger for our calibration pulse
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
  delay=-TMath::Abs(_delay);
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

// Prepare the pulses 
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
    if ( useLeadingEdge )
    {
      pulses[index].amplitude = hitAmplitudes[index];
      pulses[index].time = hitTimes[index];
      pulses[index].segmentTime = hitTimes[index];
      pulses[index].segment = 0;
      pulses[index].nextActivePulse = NULL;
      pulses[index].prevoiusActivePulse = NULL;
      pulsePointer[index]=&pulses[index];
    }
    else
    {
      pulses[index +nrOfPaddleHits].amplitude = hitAmplitudes[index];
      pulses[index+nrOfPaddleHits].time = hitTimes[index];
      pulses[index+nrOfPaddleHits].segmentTime = hitTimes[index];
      pulses[index+nrOfPaddleHits].segment = 0;
      pulses[index+nrOfPaddleHits].nextActivePulse = NULL;
      pulses[index+nrOfPaddleHits].prevoiusActivePulse = NULL;
      pulsePointer[index+nrOfPaddleHits]=&pulses[index+nrOfPaddleHits];
      
      pulses[index].amplitude = -hitAmplitudes[index]*fraction;
      pulses[index].time = hitTimes[index] + delay;
      pulses[index].segmentTime = hitTimes[index] + delay;
      pulses[index].segment = 0;
      pulses[index].nextActivePulse = NULL;
      pulses[index].prevoiusActivePulse = NULL;
      pulsePointer[index]=&pulses[index];
    }
  }

  std::make_heap(pulsePointer, pulsePointer + totalNumOfPulses, comp);

  double cfdTime =  IterateThroughTime(totalNumOfPulses);

  if( !TMath::IsNaN(cfdTime))
    cfdTime += calibrationTimeShift;

  return cfdTime;
}

double R3BConstantFraction::IterateThroughTime(int totalNumOfPulses)
{
  int nrOfPulsesLeft = totalNumOfPulses - 1;
  double time = 0, nextTime = 0, valueOfExtremum=0;
  double pulseParameters[3];
  double A,B,C, Atot, Btot, Ctot;
//  double Atemp, Btemp, Ctemp;
  bool triggered = false;
  double timeFirstActive;
  double timeForZero= std::numeric_limits<double>::quiet_NaN();
  pulse* tempPointer=NULL;

/* Function uses double linked list with all active pulses */
  pulse* lastActive=NULL;      //Points to last active pulse
  pulse* firstActive=NULL;   //Points to first active pulse

  //Check all the pulses
  std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
  while (true)
  {
    //Get the pulse with the next segment in time
    pulse* aPulsePointer = pulsePointer[nrOfPulsesLeft];

    // Get start time from the next point in time
    time = aPulsePointer->segmentTime;
    aPulsePointer->segment++;

    // If new active pulse, add to list of active pulses
    if (aPulsePointer->segment == 1)
    {
      if (lastActive == NULL)
      {
        firstActive = aPulsePointer;
        timeFirstActive = time;
      }
      else
        lastActive->prevoiusActivePulse = aPulsePointer;

      aPulsePointer->nextActivePulse = lastActive;
      lastActive = aPulsePointer;
    }

    time-= timeFirstActive;   //Time is relative the first active pulse
    
    // If the pulse is to be removed from the list of active pulses
    if (aPulsePointer->segment > 4)
    {
      nrOfPulsesLeft--;
      firstActive = firstActive->prevoiusActivePulse;
      if (firstActive == NULL)
        lastActive = NULL;
      else
        firstActive->nextActivePulse = NULL;
    }
    else  // Else, update the time and push in to heap
    {
      aPulsePointer->segmentTime = aPulsePointer->time + segmentTimePosition[aPulsePointer->segment];
      std::push_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
    }

    // Pop from heap to get time of stop
    std::pop_heap(pulsePointer, pulsePointer + nrOfPulsesLeft+1, comp);
    nextTime=pulsePointer[nrOfPulsesLeft]->segmentTime - timeFirstActive;

    //Calculate the curve parameters for the current time interval
    A=0; B=0; C=0;
    Atot=0; Btot=0; Ctot=0;

    tempPointer=lastActive;

    while (tempPointer != NULL)  // Loop the active pulses
    {

      PulseParameterGenerator(tempPointer->amplitude, tempPointer->time - timeFirstActive,
                              tempPointer->segment-1, pulseParameters);

      //Sums up all pulses
      Atot += pulseParameters[0];
      Btot += pulseParameters[1];
      Ctot += pulseParameters[2];

      //Sums up positive pulses
      if (tempPointer->amplitude > 0)
      {
        A += pulseParameters[0];
        B += pulseParameters[1];
        C += pulseParameters[2];
      }

      tempPointer = tempPointer->nextActivePulse;
    }

    //Search for leading edge
    double xPosOfExtremum = -B/(2*A);      //Calculated with the derivative
    double valueOfEndPoint = A*nextTime*nextTime + B*nextTime + C;

    if (valueOfEndPoint > threshold)
    {
      if (useLeadingEdge)
        return FindZero(A, B, C-threshold, time, nextTime)+timeFirstActive;
      else
        triggered = true;
    }
    else if (xPosOfExtremum > time && xPosOfExtremum < nextTime)
    {
      valueOfExtremum = A*xPosOfExtremum*xPosOfExtremum + B*xPosOfExtremum + C;
      if (valueOfExtremum > threshold )
      {
        if (useLeadingEdge)
          return FindZero(A, B, C-threshold, time, nextTime)+timeFirstActive;
        else
          triggered = true;
      }
    }
    else
      triggered = false;

    //Search for zero
    if (valueOfEndPoint > 0.1*threshold)
    {
      double valueOfStartPoint = Atot*time*time + Btot*time + Ctot;
      valueOfEndPoint = Atot*nextTime*nextTime + Btot*nextTime + Ctot;

      if(valueOfStartPoint < 0.0 && valueOfEndPoint > 0.0)
        timeForZero = FindZero(Atot, Btot, Ctot, time, nextTime)+timeFirstActive;
      else if (valueOfStartPoint > 0.0 && valueOfEndPoint < 0.0)
        timeForZero = std::numeric_limits<double>::quiet_NaN();
    }

    // Have we passed the threshold and crossed zero?
    if (triggered && ! TMath::IsNaN(timeForZero))
      return timeForZero;

    // If at the end of the last active pulse, remove
    if (pulsePointer[nrOfPulsesLeft]->segment == 4 && firstActive == lastActive)
    {
          if (nrOfPulsesLeft == 0)
        return std::numeric_limits<double>::quiet_NaN();

      nrOfPulsesLeft--;
      firstActive = firstActive->prevoiusActivePulse;
      if (firstActive == NULL)
        lastActive = NULL;
      else
        firstActive->nextActivePulse = NULL;

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

  double sqrt, x_1, x_2;

  sqrt=TMath::Sqrt(B*B-4*A*C);
  
  double denominator= 1/(2*A);
  x_1=(-B-sqrt)*denominator;
  x_2=(-B+sqrt)*denominator;
  
  if (x_1 >= time && x_1 <= nextTime)
    return x_1;
  else if (x_2 >= time && x_2 <= nextTime)
    return x_2;
  else
    Fatal("R3BConstantFraction::FindZero","No zero found"); 
  return 0.;
}

//Compare function for heap
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

  pulseParameters[0]=a;        // A
  pulseParameters[1]=-2*a*b;   // B
  pulseParameters[2]=a*b*b+c;  // C
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
