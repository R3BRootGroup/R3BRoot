#!/bin/bash

perl createGeometryCLFDemo.pl
cat create_califa_demonstrator_1.C CLF811_Geometry_Demo.geo create_califa_demonstrator_2.C > ../../macros/r3b/geo/create_califa_demonstrator.C
