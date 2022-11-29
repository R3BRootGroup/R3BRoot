PSP event mixing in S515 experiment
------------------------------------

Principal investigators:
Eleonora Kudaibergenova
Andrea Horvat
Hans Toshihide Törnqvst

Solution implementation into R3BRoot:
Ivana Lihtar
Eleonora Kudaibergenova

In S515 experiment it was noticed that PSP data does not correlate with other detectors.
Later it was discovered that the reason for this was that PSP internal WhiteRabbit timestamps 
were not properly read out from its buffer (because the buffer was not emptied properly).
Nevertheless, a regularity was found in data in terms of shift between PSP data and other
detector data (PSP data was correlated with MUSIC data to investigate this). The analysis was 
done and shifts were found for each run (their occurence was marked in terms of WhiteRabbit Master 
timestamps).

------------------------------------------------------------------------------------------------------

Brief description of the solution implemented in R3BS515PspxReader:

In header file we define:
   fShift = shift to be applied to PSP data
   fEventBuffer[20] = buffer in which PSP data is temporarily stored and from which it is read
                      into mapped data
Reading the events:

If fShift != 0 :
1. We skip TPAT=0 events (including them would cause ambiguity of shift number and these events
   we do not need)
2. "fShift" number of events is stored in buffer.
3. In event "fShift+1" we start reading from the buffer : we read first element from the
   buffer and store it into mapped data. 
4. In the buffer we overwrite the previously read element with the current event ("fShift+1"th event).
5. We translate the elements in the buffer to one place before so that in the next event the element 
   to be read is again the first one (element [0]) in the buffer.
   
Example: fShift = 3
(x = empty)

1. event : fEventbuffer[ ev1, x,x,x...] 		MappedData = empty
2. event : fEventBuffer[ ev1, ev2, x,x,x...] 		MappedData = empty
3. event : fEventBuffer[ ev1, ev2, ev3, x,x,x...] 	MappedData = empty
4. event : fEventBuffer[ev1] = MappedData[ev1]          MappedData = [ev1] 
           -> fEventBuffer [ ev2, ev3, ev4, x,x,x...]       
5. event : fEventBuffer[ev2] = MappedData[ev2]    	MappedData = [ev1,ev2]
           -> fEventBuffer [ ev3, ev4, ev5, x,x,x... ]
... and so on.

If fShift == 0 : reading as regularly (as in R3BPspxReader)

------------------------------------------------------------------------------------------------------

Shifts per run:
--------------------------
Run		Shift
--------------------------
483-500		0
504-507		4
509-516		1
517		1,2,3,6,7
518-562		7
--------------------------

With this procedure (runwise application of shifts) one can NOT look at run 517! For this comparison
with WhiteRabbit Master timestamp will have to be implemented.

------------------------------------------------------------------------------------------------------

For more information contact:
Eleonora Kudaibergenova
Ivana Lihtar
