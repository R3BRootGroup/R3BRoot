#!/bin/bash

# Generates the Geometry of the simulation:

root -l -q NeuLAND_BuildGeometry.cpp
root -l -q VETO_BuildGeometry.cpp
root -l -q NEBULA_BuildGeometry.cpp
root -l -q NEBULA_VETO_BuildGeometry.cpp
#root -l -q GLAD_BuildGeometry.cpp
