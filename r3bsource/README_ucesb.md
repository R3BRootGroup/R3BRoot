Using ucesb as unpacker stage for R3BROOT
-----------------------------------------

The versatile unpacker generator [ucesb](http://fy.chalmers.se/~f96hajo/ucesb) can be used as unpacking stage for R3BROOT.
This allows the user to write the unpacker using a human-readable specification, rather than in C/C++ code.
Thorough checking of the data format comes for free as well.
For further information about ucesb, see the [write-up](http://fy.chalmers.se/~f96hajo/ucesb/ucesb_doc.pdf).

Overview
--------

1. Install ucesb (with FAIRROOT version of ROOT)
2. Compile your experiment specific unpacker and test it on data
3. Produce the header file for R3BROOT including the data structure
4. Extract the parts for each detector and level (unpack / raw)
5. Install [R3BROOT](https://www.r3broot.gsi.de)
6. Write detector specific readers
7. Write or modify your R3BROOT steering macro
8. Run the macro


Install ucesb
-------------

Checkout the most recent version of ucesb from the git repository:

    git clone http://fy.chalmers.se/~f96hajo/ucesb/ucesb.git

And also checkout the git repository with experiment unpackers:

    git clone lx-pool.gsi.de:/u/johansso/upexps

Check that

    which root

points to your FAIRROOT installation directory!

Compile the empty ucesb unpacker *using the same version of ROOT* as you used for FAIRROOT.

    make empty/empty

After compilation finished, make sure that the file

    hbook/ext_data_clnt.o

exists. Set the environment variable UCESB_DIR to the 'unpacker' directory, e.g.

    export UCESB_DIR=/u/$USER/path/to/unpacker

Put this in your .bashrc to make it permanent. R3BROOT will look for ucesb at this location.


Compile your experiment specific unpacker
-----------------------------------------

Also use *the same* ROOT version here.

    cd upexps/sNNN
    make


Produce the header file
-----------------------

Run ucesb with the struct_writer header generation option

    ./sNNN --ntuple=<signals>,STRUCT_HH,ext_h101.h

with <signals> something like UNPACK:EVENTNO,UNPACK:TRIGGER,RAW.
This will produce the header file called 'ext_h101.h'.
It contains the structure information for R3BROOT.
The data coming from ucesb will adhere to this format.

You can skip parts of the following step, if you do the header writing like this

    ./sNNN --ntuple=<signals>,id=h101_<detector>,ext_h101_<detector>.h
    # Example
    ./s483 --ntuple=RAW:TOFD,id=h101_tofd,ext_h101_tofd.h

This will already insert the correct detector names for you, if you like and only extract signals related to the specific detector.


Extract the parts for each detector
-----------------------------------

At this point this part of the process is still manual:
For this example we will use the RAW level data of POS.

- Make a copy of ext\_h101.h for each detector (e.g. ext\_h101_raw_pos.h)
- Rename the EXT_STR_h101.h to EXT_STR_h101_raw_pos.h
- Delete all contents of the structure with don't relate to POS
- Delete all contents of the #define EXT_STR_h101_ITEMS_INFO(...) that don't relate to POS
- Rename the #define statement to match the structure name.

Install R3BROOT
---------------

Installation information can be found on the R3BROOT website


Write detector specific reader classes
--------------------------------------

Have a look at r3bsource/R3BUnpackReader.[cxx/h].
Your detector specific reader class should almost be a copy of this, mostly replacing 'unpack' with e.g. 'raw_pos'.
And you will have to make sure that in the Read() function your detector specific data containers are filled with the data from the ucesb structure.


Write or modify your R3BROOT steering macro
-------------------------------------------

An existing steering macro for the s438b experiment can be found in

    macros/r3b/unpack/s438b/unpack_ucesb.C

It shows how the R3BUcesbSource class is used and how Readers are added.


Run the macro
-------------

Run this:

    root -l path/to/your/macro.C

=)

Questions
---------

Either via [Mail](mailto:b.loeher@gsi.de) or in the [GSI forum](https://forum.gsi.de/index.php?t=thread&frm_id=206&%).
