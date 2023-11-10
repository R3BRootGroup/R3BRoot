The following concerns R3BPspReader, not R3BPspxReader.

The class R3BPspReader was apparently used to read PSP data from 
the Mesytec MADC32.

However, it used an ext_h101 struct which is incompatible with a later
file used for PSPs with the R3BPspxReader.

As both structs share the same name, they can not both be used at the same
time. R3BPspReader was thus not compiled in recent versions. 

If you require the code, go to git revision 74611a335e93938963313.
Some work would be required to make it compile. Preferably rename the struct
in ext_h101_psp_jun16.h.

I have removed these files because having dead code is bad practice and it
also broke CI. 

        deleted:    R3BPspReader.cxx
        deleted:    R3BPspReader.h
        deleted:    ext_h101_psp_jun16.h
	
Cheers,
   Philipp Klenze
   