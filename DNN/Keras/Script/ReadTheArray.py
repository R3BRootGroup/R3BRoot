# python import commands:
import numpy as np
import os as TheOS

# definition of the reading function:
def ReadArray(FileName):
    # This function reads a TextFile where floating point numbers are separated by ';'
    # It returns a matrix with those values.
    
    # Check if the file exists:
    TheTest = TheOS.path.isfile(FileName)
    
    if TheTest:
        # Then we are OK. The file exists:
    
        # Begin by opeing the file:
        TheFile = open(FileName,'r')
    
        # Begin with a first loop over the file to extract the sizes:
        nColumns = 0
        nRows = 0
    
        for OneLine in TheFile:
            nRows = len(OneLine.split(';'))
            nColumns = nColumns + 1
    
        # Then, close and re-open:
        TheFile.close()
        TheFile = open(FileName,'r')
    
        # Next, use a second loop to actually convert into a matrix:
        TheArray = np.zeros((nColumns,nRows))
        ColCounter = 0
        StringArray = ''
    
        for ThisLine in TheFile:
            # Split the line into a string array:
            StringArray = ThisLine.split(';')
        
            # loop over the elements in the array:
            for RowCounter in range(0,nRows):
                # convert to floating points:
                TheArray[ColCounter,RowCounter] = float(StringArray[RowCounter])
        
            # Then, update ColCounter:
            ColCounter = ColCounter + 1
        
        # Now, the array should be filled. so return it:
        return TheArray
    
    else:
        # then, we give an error message:
        TheArray = np.zeros((1,1))
        print(FileName + ' ==> This is not an existsing file!')
        return TheArray

# Done --------------------------------------------
