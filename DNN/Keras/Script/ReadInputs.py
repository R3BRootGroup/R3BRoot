# python import commands:
import os as TheOS

# Define path to Inputs-file as global:
TheInputsFile = '../../InputFiles/InputsFile.txt'

# ==================================================================================

# Function definition to extract inputs from the R3BRoot Inputs-file:
def GetInputDouble(Description,Unit):
    # This function reads the R3BRoot Inputs-file and returns a
    # floating input from the file that matches the decription-string.
    
    # Declare the Answer:
    TheAnswer = 0.0
    
    # Check if the file exists:
    TheTest = TheOS.path.isfile(TheInputsFile)
    
    if (not(TheTest)):
        print('The Inputs-file does not exists!')
    else:
        # Then we are OK. The file exists:
    
        # Begin by opeing the file:
        TheFile = open(TheInputsFile,'r')
        
        # Declare a counter for testing:
        LineCounter = 0
    
        # Next, loop over the file
        for ThisLine in TheFile:
            
            # Now, ThisLine holds the current line of the file.
            # Test if it contains the decription:
            if (Description in ThisLine):
                # Then, update the counter:
                LineCounter = LineCounter + 1
                
                # Next, test if this input is indeed a double:
                if (not('TYPE=double' in ThisLine)):
                    print('The input you requested is not a double!')
                else:
                    
                    # Then test if it is the correct unit:
                    if (not(Unit in ThisLine)):
                        print('The input you requested has a different usit!')
                    else:
                                            
                        # Now, we are finally in busyness. split the string into 3 parts:
                        LineArray = ThisLine.split(';')
                    
                        # Then, the part we are after, is in the last part:
                        TheInputString = LineArray[3]
                    
                        # Next, we should remove the first part:
                        TheValue = TheInputString.replace(' VALUE=','')
                    
                        # And, finally, convert it to a float:
                        TheAnswer = float(TheValue)
                                        
                        # Close all blocks:
        
        # Next, test if we indeed found only one input:
        if (LineCounter==0):
            TheAnswer = 0.0
            print('The input you requested was NOT present in the file!')
        elif ((LineCounter>1) or (LineCounter<0)):
            TheAnswer = 0.0
            print('')
            print('FATAL ERROR: The Double-input <' + str(Description) + '> was found multiple times! This is a disastrous error in the C++ code!')
            print('')
        
    # Then, return the answer:
    return TheAnswer
# Done.

# ==================================================================================

# Function definition to extract inputs from the R3BRoot Inputs-file:
def GetInputInteger(Description):
    # This function reads the R3BRoot Inputs-file and returns an
    # integer input from the file that matches the decription-string.
    
    # Declare the Answer:
    TheAnswer = 0
    
    # Check if the file exists:
    TheTest = TheOS.path.isfile(TheInputsFile)
    
    if (not(TheTest)):
        print('The Inputs-file does not exists!')
    else:
        # Then we are OK. The file exists:
    
        # Begin by opeing the file:
        TheFile = open(TheInputsFile,'r')
        
        # Declare a counter for testing:
        LineCounter = 0
    
        # Next, loop over the file
        for ThisLine in TheFile:
            
            # Now, ThisLine holds the current line of the file.
            # Test if it contains the decription:
            if (Description in ThisLine):
                # Then, update the counter:
                LineCounter = LineCounter + 1
                
                # Next, test if this input is indeed an integer:
                if (not('TYPE=int' in ThisLine)):
                    print('The input you requested is not an integer!')
                else:
                    
                    # Now, we are finally in busyness. split the string into 3 parts:
                    LineArray = ThisLine.split(';')
                    
                    # Then, the part we are after, is in the last part:
                    TheInputString = LineArray[2]
                    
                    # Next, we should remove the first part:
                    TheValue = TheInputString.replace(' VALUE=','')
                    
                    # And, finally, convert it to an integer:
                    TheAnswer = int(TheValue)
                                        
                    # Close all blocks:
        
        # Next, test if we indeed found only one input:
        if (LineCounter==0):
            TheAnswer = 0
            print('The input you requested was NOT present in the file!')
        elif ((LineCounter>1) or (LineCounter<0)):
            TheAnswer = 0
            print('')
            print('FATAL ERROR: The Integer-input <' + str(Description) + '> was found multiple times! This is a disastrous error in the C++ code!')
            print('')
        
    # Then, return the answer:
    return TheAnswer
# Done.

# ==================================================================================

# Function definition to extract inputs from the R3BRoot Inputs-file:
def GetInputBoolian(Description):
    # This function reads the R3BRoot Inputs-file and returns an
    # boolian input from the file that matches the decription-string.
    
    # Declare the Answer:
    TheAnswer = False
    
    # Check if the file exists:
    TheTest = TheOS.path.isfile(TheInputsFile)
    
    if (not(TheTest)):
        print('The Inputs-file does not exists!')
    else:
        # Then we are OK. The file exists:
    
        # Begin by opeing the file:
        TheFile = open(TheInputsFile,'r')
        
        # Declare a counter for testing:
        LineCounter = 0
    
        # Next, loop over the file
        for ThisLine in TheFile:
            
            # Now, ThisLine holds the current line of the file.
            # Test if it contains the decription:
            if (Description in ThisLine):
                # Then, update the counter:
                LineCounter = LineCounter + 1
                
                # Next, test if this input is indeed an integer:
                if (not('TYPE=bool' in ThisLine)):
                    print('The input you requested is not a boolian!')
                else:
                    
                    # Now, we are finally in busyness. For boolians,
                    # the remaining job is quite easy:
                    if ('VALUE=kTRUE' in ThisLine):
                        TheAnswer = True 
                    elif ('VALUE=kFALSE' in ThisLine):
                        TheAnswer = False 
                    else:
                        print('FATAL ERROR: The Input-Boolian was NOT properly defined! Recheck your C++ code!')
                                        
                    # Close all blocks:
        
        # Next, test if we indeed found only one input:
        if (LineCounter==0):
            TheAnswer = 0
            print('The input you requested was NOT present in the file!')
        elif ((LineCounter>1) or (LineCounter<0)):
            TheAnswer = 0
            print('')
            print('FATAL ERROR: The Boolian-input <' + str(Description) + '> was found multiple times! This is a disastrous error in the C++ code!')
            print('')
        
    # Then, return the answer:
    return TheAnswer
# Done.

# ==================================================================================

# Function definition to extract inputs from the R3BRoot Inputs-file:
def GetInputString(Description):
    # This function reads the R3BRoot Inputs-file and returns an
    # string input from the file that matches the decription-string.
    
    # Declare the Answer:
    TheAnswer = ''
    
    # Check if the file exists:
    TheTest = TheOS.path.isfile(TheInputsFile)
    
    if (not(TheTest)):
        print('The Inputs-file does not exists!')
    else:
        # Then we are OK. The file exists:
    
        # Begin by opeing the file:
        TheFile = open(TheInputsFile,'r')
        
        # Declare a counter for testing:
        LineCounter = 0
    
        # Next, loop over the file
        for ThisLine in TheFile:
            
            # Now, ThisLine holds the current line of the file.
            # Test if it contains the decription:
            if (Description in ThisLine):
                # Then, update the counter:
                LineCounter = LineCounter + 1
                
                # Next, test if this input is indeed an integer:
                if (not('TYPE=string' in ThisLine)):
                    print('The input you requested is not a string!')
                else:
                    
                    # Now, we are finally in busyness. split the string into 3 parts:
                    LineArray = ThisLine.split(';')
                    
                    # Then, the part we are after, is in the last part:
                    TheInputString = LineArray[2]
                    
                    # Next, we should remove the first part:
                    TheValue = TheInputString.replace(' VALUE=','')
                    
                    # And, finally, assign it to the answer:
                    TheAnswer = TheValue
                                        
                    # Close all blocks:
        
        # Next, test if we indeed found only one input:
        if (LineCounter==0):
            TheAnswer = 0
            print('The input you requested was NOT present in the file!')
        elif ((LineCounter>1) or (LineCounter<0)):
            TheAnswer = 0
            print('')
            print('FATAL ERROR: The STRING-input <' + str(Description) + '> was found multiple times! This is a disastrous error in the C++ code!')
            print('')
        
    # Then, return the answer:
    return TheAnswer
# Done.

# ==================================================================================
