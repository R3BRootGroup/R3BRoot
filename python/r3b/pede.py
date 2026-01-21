#!/usr/bin/env python3

## \file
# tiny PEDE implementation
#
# \author Claus Kleinwort, DESY, 2016-2024 (Claus.Kleinwort@desy.de)
#
#  \copyright
#  Copyright (c) 2016 - 2024 Deutsches Elektronen-Synchroton,
#  Member of the Helmholtz Association, (DESY), HAMBURG, GERMANY \n\n
#  This library is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Library General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version. \n\n
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Library General Public License for more details. \n\n
#  You should have received a copy of the GNU Library General Public
#  License along with this program (see the file COPYING.LIB for more
#  details); if not, write to the Free Software Foundation, Inc.,
#  675 Mass Ave, Cambridge, MA 02139, USA.
#

import numpy as np
from scipy.stats import chi2
import array
import pandas as pd
import math
import argparse
import time
import os
from threadpoolctl import threadpool_limits
from tqdm import tqdm


## Millepede-II (binary) record.
#
#  Containing information for local (track) and global fit.
#
#  The data blocks are collected in two arrays, a real array
#  (containing float or double values) and integer array, of same length.
#  A positive record length indicate _float_ and a negative one _double_ values.
#  The content of the record is:
#
# \verbatim
#         real array              integer array
#     0   0.0                     error count (this record)
#     1   RMEAS, measured value   0                            __iMeas   -+
#     2   local derivative        index of local derivative               |
#     3   local derivative        index of local derivative               |
#     4   ...                                                             | block
#         SIGMA, error (>0)       0                            __iErr     |
#         global derivative       label of global derivative              |
#         global derivative       label of global derivative              |
#         ...                                                            -+
#         RMEAS, measured value   0                            __position
#         local derivative        index of local derivative
#         local derivative        index of local derivative
#         ...
#         SIGMA, error            0
#         global derivative       label of global derivative
#         global derivative       label of global derivative
#         ...
#         global derivative       label of global derivative   __recLen
# \endverbatim
#
# Special data block (other/debug information).
# Contains no local derivatives and (error) SIGMA is negative (-Number of SPecial data words).
#
# \verbatim
#         real array              integer array
#         0.0                     0                 __iMeas   -+
#         -float(NSP)             0                 __iErr     |
#         special data            special data                 | special block (2+NSP words)
#         special data            special data                 |
#         ...                                                 -+
# \endverbatim
#
class MilleRecord(object):

    ## Create MP-II binary record.
    #
    def __init__(self, doublePrec=False):
        ## flag for storage in as *double* values
        self.__doublePrecision = doublePrec
        ## position in record, usually start of next data block; int
        self.__position = 1
        ## number of data blocks in record; int
        self.__numData = 0
        ## record length; int
        self.__recLen = 0
        ## position of value in current data block; int
        self.__iMeas = 0
        ## position of error in current data block; int
        self.__iErr = 0
        ## array with markers (0) and labels; array(int32)
        self.__inder = array.array("i")
        ## array with values, errors and derivatives; (float32 or float64)
        self.__glder = array.array("d" if doublePrec else "f")

    ## Add data block to (end of) record.
    #
    #  @param dataList list with measurement, error, labels and derivatives; list
    #
    def addData(self, dataList):
        if self.__numData == 0:  # first word is error counter
            self.__inder.append(0)
            self.__glder.append(0.0)
        self.__numData += 1

        aMeas, aPrec, indLocal, derLocal, labGlobal, derGlobal = dataList
        self.__inder.append(0)
        self.__glder.append(aMeas)
        self.__inder.fromlist(indLocal)
        self.__glder.fromlist(derLocal)
        self.__inder.append(0)
        self.__glder.append(1.0 / math.sqrt(aPrec))  # convert to error
        self.__inder.fromlist(labGlobal)
        self.__glder.fromlist(derGlobal)

    ## Get data block from current position in record.
    #
    #  @return list with measurement, error, labels and derivatives; list
    #
    def getData(self):
        aMeas = self.__glder[self.__iMeas]
        indLocal = []
        derLocal = []
        for i in range(self.__iMeas + 1, self.__iErr):
            indLocal.append(self.__inder[i])
            derLocal.append(self.__glder[i])
        aPrec = 1.0 / self.__glder[self.__iErr] ** 2  # convert to precision
        indGlobal = []
        derGlobal = []
        for i in range(self.__iErr + 1, self.__position):
            indGlobal.append(self.__inder[i])
            derGlobal.append(self.__glder[i])
        return aMeas, aPrec, indLocal, derLocal, indGlobal, derGlobal

    ## Print record.
    def printRecord(self):
        print(" MilleRecord, len: ", len(self.__inder))
        print(self.__inder)
        print(self.__glder)

    ## Write record to file.
    #
    #  @param aFile (binary) file
    #
    def writeRecord(self, aFile):
        header = array.array("i")  # header with number of words
        header.append(
            -len(self.__inder) * 2
            if self.__doublePrecision
            else len(self.__inder) * 2
        )
        header.tofile(aFile)
        self.__glder.tofile(aFile)
        self.__inder.tofile(aFile)

    ## Read record from file.
    #
    #  @param aFile (binary) file
    #  @param aType file type ('C' or 'F'ortran)
    #
    def readRecord(self, aFile, aType):
        if aType == "F":
            # FOrtran record info
            lenf = array.array("i")
            lenf.fromfile(aFile, 1)
        header = array.array("i")  # header with number of words
        header.fromfile(aFile, 1)
        self.__recLen = abs(header[0] // 2)
        if header[0] < 0:
            self.__glder = array.array("d")
        else:
            self.__glder = array.array("f")
        self.__glder.fromfile(aFile, self.__recLen)
        self.__inder.fromfile(aFile, self.__recLen)
        if aType == "F":
            # FOrtran record info
            lenf = array.array("i")
            lenf.fromfile(aFile, 1)

    ## Locate next data block.
    #
    #  @return next block exists; bool
    #
    def moreData(self):
        if self.__position < self.__recLen:
            while (
                self.__position < self.__recLen
                and self.__inder[self.__position] != 0
            ):
                self.__position += 1
            self.__iMeas = self.__position
            self.__position += 1
            while (
                self.__position < self.__recLen
                and self.__inder[self.__position] != 0
            ):
                self.__position += 1
            self.__iErr = self.__position
            self.__position += 1
            # special data?
            if (
                self.__iMeas + 1 == self.__iErr
                and self.__glder[self.__iErr] < 0
            ):
                self.__position += int(-self.__glder[self.__iErr])
            else:
                while (
                    self.__position < self.__recLen
                    and self.__inder[self.__position] != 0
                ):
                    self.__position += 1
            self.__numData += 1
            return True
        else:
            return False

    ## Get special data tag from block.
    #
    #  @return tag or -1 for ordinary data block; int
    #
    def specialDataTag(self):
        aTag = -1
        if self.__iMeas + 1 == self.__iErr and self.__glder[self.__iErr] < 0:
            aTag = int(-self.__glder[self.__iErr] * 10.0 + 0.5) % 10
        return aTag


## Pede
#
# Implements only very basic PEDE functionality.
# Performs local and global fits. Features:
#     - fix parameters
#     - apply (linear equality) constraints
#     - reject bad records (based on Chi2 or failure of local fit)
#
class Pede(object):

    ## Constructor
    #
    def __init__(self):
        ## binary files
        self.__binaryFiles = []
        ## number of parameters
        self.__numPar = 0
        ## parameter counters
        self.__parCounters = {}
        ## parameter indices
        self.__parIndices = {}
        ## matrix
        self.__matrix = None
        ## vector
        self.__vector = None
        ## sum(chi2)
        self.__sumChi2 = 0.0
        ## sum(ndf)
        self.__sumNdf = 0
        ## number of records
        self.__numRec = 0
        ## number of rejects (Chi2,ndf)
        self.__numRejects = 0
        ## bad local fits (rank deficit)
        self.__numBad = 0
        ## rank of global matrix
        self.__numCons = 0
        ## list constraints
        self.__listCons = []
        ## output dataframe
        self.__dataframe = pd.DataFrame()

        #
        print()
        print(" TinyPede - a simple PEDE implementation in python")
        print()

    ## Add binary file
    #
    # Add C (default) or Fortran type binary file to list of files.
    # Optionally specify maximum number of records to read from that file.
    #
    # @param[in]  binaryFileName   name of MP-II binary file
    # @param[in]  maxRec           maximal number of records
    # @param[in]  fileType         'C' or 'F'ortran type
    #
    def addBinaryFile(self, binaryFileName, maxRec=1000000, fileType="C"):
        self.__binaryFiles.append((binaryFileName, maxRec, fileType))

    ## Define Parameters
    #
    # Read all binary files to get list of all global parameters.
    # Apply (minimum number of) entries cut and list of fixed parameters
    # to get list of free/variable parameters.
    #
    # @param[in] entriesCut  entries cut
    # @param[in] fixedPar    fixed parameters (labels)
    #
    def defineParameters(self, entriesCut=25, fixedPar=[]):
        for fileName, maxRecord, fileType in self.__binaryFiles:
            # open binary file
            if fileType == "F":
                print(" Reading Fortran binary file ", fileName)
            else:
                print(" Reading binary file ", fileName)
            binaryFile = open(fileName, "rb")
            nRec = 0
            try:

                while nRec < maxRecord:
                    nRec += 1
                    # read record
                    rec = MilleRecord()
                    rec.readRecord(binaryFile, fileType)
                    nData = 0
                    while rec.moreData():
                        aTag = rec.specialDataTag()
                        if aTag >= 0:
                            continue
                        # get data
                        nData += 1
                        indGlobal = rec.getData()[4]
                        for label in indGlobal:
                            if label not in self.__parCounters:
                                self.__parCounters[label] = 0
                            self.__parCounters[label] += 1

            except EOFError:
                pass

            print("   records found ", nRec)
            binaryFile.close()
        # fix parameters
        for label in fixedPar:
            if label in self.__parCounters:
                self.__parCounters[label] *= -1
        # active parameters
        for l in sorted(self.__parCounters):
            if self.__parCounters[l] >= entriesCut:
                self.__parIndices[l] = self.__numPar
                self.__numPar += 1

        print(" Found in binary files")
        print("   total number of parameters ", len(self.__parCounters))
        print("   minimum number of entries  ", entriesCut)
        print("   number of free parameters  ", self.__numPar)
        print()

    ## Add constraints
    #
    # Add (linear equality) constraints from text file.
    # Accept only constraints with at least one variable parameter.
    # Assume zero r.h.s. (C*p=0).
    #
    # @param[in] textFile  file name (text file with constraints)
    #
    def addConstraints(self, textFile):
        if self.__numPar == 0:
            print(" no variable parameters defined ")
            return
        # read text file
        print(" Reading constraint file ", textFile)
        nline = 0
        nCon = 0
        pairs = []  # (index, coefficient)
        tf = open(textFile)
        for line in iter(tf):
            nline += 1
            fields = line.split()
            if len(fields) < 2:
                continue
            if fields[0] == "Constraint":
                # next constraint
                nCon += 1
                # previous constraint had variable parameters (pairs)?
                if len(pairs) > 0:
                    self.__numCons += 1
                    self.__listCons.append(pairs)
                    pairs = []
                continue
            if fields[0] == "*":
                continue
            # label and coefficient
            label = int(fields[0])
            value = float(fields[1])
            if label in self.__parIndices:
                # variable parameter (label)
                pairs.append((self.__parIndices[label], value))

        tf.close()
        # last constraint
        if len(pairs) > 0:
            self.__numCons += 1
            self.__listCons.append(pairs)

        print("   number of all constraints  ", nCon)
        print("   number of accepted constr. ", self.__numCons)
        # print self.__listCons

    ## Construct linear equation system
    #
    # Perform local fits, reject bad records (local fit failed) and
    # records with too large Chi2(ndf) and build up global matrix and vector.
    #
    # @param[in] chi2Factor    reject local fits with prob(chi2/chi2Factor,ndf) < 0.0027
    #
    def construct(self, chi2Factor=50.0):
        # create global matrix and vector
        # size: number of parameters + number of Lagrange multipliers for constraints
        # no initial non-zero values for global parameters (simplifying local fit, Lagrange multipliers)
        self.__vector = np.zeros(self.__numPar + self.__numCons)
        self.__matrix = np.zeros(
            (self.__numPar + self.__numCons, self.__numPar + self.__numCons)
        )
        print()
        print(" Constructing linear equations system")
        print("   local chi2 scaling factor  ", chi2Factor)
        for fileName, maxRecord, fileType in self.__binaryFiles:
            # open binary file
            binaryFile = open(fileName, "rb")
            nRec = 0
            try:
                while nRec < maxRecord:
                    # read record
                    rec = MilleRecord()
                    rec.readRecord(binaryFile, fileType)
                    nRec += 1
                    dataBlocks = []
                    # number of local parameters
                    numLoc = 0
                    # number of measurements
                    numMeas = 0

                    while rec.moreData():
                        aTag = rec.specialDataTag()
                        if aTag >= 0:
                            continue
                        # get data
                        dataBlocks.append(rec.getData())
                        numLoc = max(numLoc, dataBlocks[-1][2][-1])
                        numMeas += 1

                    # print(" numLoc ", nRec, numLoc, numMeas)
                    vecMeas = np.zeros(numMeas)
                    vecMeasW = np.zeros(numMeas)
                    locDer = np.zeros((numMeas, numLoc))
                    locDerW = np.zeros((numMeas, numLoc))
                    gloDer = np.zeros((numMeas, self.__numPar))
                    gloDerW = np.zeros((numMeas, self.__numPar))
                    # loop over data blocks
                    for iData, data in enumerate(dataBlocks):
                        # aRes = data.getResidual()[0]
                        (
                            aMeas,
                            aPrec,
                            indLocal,
                            derLocal,
                            indGlobal,
                            derGlobal,
                        ) = data
                        # update local-local matrix
                        for i, der in zip(indLocal, derLocal):
                            locDer[iData, i - 1] = der
                            locDerW[iData, i - 1] = der * aPrec
                            vecMeas[iData] = aMeas
                            vecMeasW[iData] = aMeas * aPrec
                        # update local-global matrix
                        for l, der in zip(indGlobal, derGlobal):
                            if l not in self.__parIndices:
                                continue
                            i = self.__parIndices[l]
                            gloDer[iData, i] = der
                            gloDerW[iData, i] = der * aPrec

                    # local fit
                    matLocLoc = np.dot(locDerW.T, locDer)
                    vecLoc = np.dot(locDerW.T, vecMeas)
                    try:
                        locCov = np.linalg.inv(matLocLoc)
                    except np.linalg.LinAlgError as e:
                        print("   local fit failed:", e)
                        self.__numBad += 1
                        continue
                    locSol = np.dot(locCov, vecLoc)
                    # chi2, ndf
                    locNdf = numMeas - numLoc
                    locChi2 = np.dot(
                        vecMeas - np.dot(locDer, locSol),
                        vecMeasW - np.dot(locDerW, locSol),
                    )
                    prob = 1.0 - chi2.cdf(locChi2 / chi2Factor, locNdf)
                    # check rank
                    nRank = np.linalg.matrix_rank(matLocLoc)
                    if nRank < numLoc:
                        self.__numBad += 1
                        print(
                            " ??? bad local fit ",
                            nRec,
                            locNdf,
                            locChi2,
                            numLoc,
                            nRank,
                        )
                        for i in range(numLoc):
                            gcor2 = 1.0 - 1.0 / (
                                matLocLoc[i, i] * locCov[i, i]
                            )  # global correlation (squared)
                            print(
                                "    locPar ",
                                i + 1,
                                matLocLoc[i, i],
                                locCov[i, i],
                                gcor2,
                            )
                        continue
                    # reject (at 3 sigma)
                    if prob < 0.0027:
                        self.__numRejects += 1
                        continue
                    # accepted
                    self.__sumNdf += locNdf
                    self.__sumChi2 += locChi2
                    # update global matrix, vector (parameter part)
                    self.__matrix[: self.__numPar, : self.__numPar] += np.dot(
                        gloDerW.T, gloDer
                    )
                    self.__vector[: self.__numPar] += np.dot(
                        gloDerW.T, vecMeas
                    )
                    # account for correlations (via local parameters)
                    matGloLoc = np.dot(gloDerW.T, locDer)
                    self.__matrix[: self.__numPar, : self.__numPar] -= np.dot(
                        matGloLoc, np.dot(locCov, matGloLoc.T)
                    )
                    self.__vector[: self.__numPar] -= np.dot(matGloLoc, locSol)

            except EOFError:
                pass

            binaryFile.close()
            self.__numRec += nRec

        # add Lagrange multipliers
        indexCons = self.__numPar
        for pairs in self.__listCons:
            for idx, val in pairs:
                self.__matrix[indexCons, idx] = val
                self.__matrix[idx, indexCons] = val
            indexCons += 1

        # record statistics
        print("   total number of records    ", self.__numRec)
        print("   number of rejected records ", self.__numRejects)
        print("   number of bad records      ", self.__numBad)
        # matrix information
        matSize = self.__numPar + self.__numCons
        nonZero = np.count_nonzero(self.__matrix)
        print("   size of global matrix      ", matSize)
        print(
            "   fraction of elements <> 0. ",
            int(100.0 * nonZero / matSize**2),
            "%",
        )

    ## Dump global matrix
    def dump(self):
        print(" Global matrix, non-zero elements")
        for i in range(self.__numPar + self.__numCons):
            for j in range(i, self.__numPar + self.__numCons):
                if self.__matrix[i, j] != 0.0:
                    print("  ", i, j, self.__matrix[i, j])

    ## Solve linear equation system
    #
    # Solve linear equation system (global fit) and print results.
    #
    def solve(self):
        print()
        print(" Solving linear equations system")
        # solve equation system
        try:
            aCov = np.linalg.inv(self.__matrix)
        except np.linalg.LinAlgError as e:
            print("   matrix inversion failed:", e)
            print("   >>> improve input data <<<")
            return
        aSol = np.dot(aCov, self.__vector)
        print("   sum(ndf)                 ", self.__sumNdf - self.__numPar)
        print("   initial sum(chi2)        ", self.__sumChi2)
        deltaChi2 = np.dot(self.__vector, aSol)
        print("   chi2 reduction by fit    ", deltaChi2)
        print("   final sum(chi2)          ", self.__sumChi2 - deltaChi2)
        print(
            "   final sum(chi2)/sum(ndf) ",
            (self.__sumChi2 - deltaChi2) / (self.__sumNdf - self.__numPar),
        )
        # print()
        # print(" Solution")
        # print("   parameter label, #entries, correction, error")
        par_len = len(self.__parCounters)
        labels = np.zeros(par_len)
        entries = np.zeros(par_len)
        corrections = np.zeros(par_len)
        errors = np.zeros(par_len)
        for idx, l in enumerate(sorted(self.__parCounters)):
            if l in self.__parIndices:
                i = self.__parIndices[l]
                labels[idx] = l
                entries[idx] = self.__parCounters[l]
                corrections[idx] = aSol[i]
                errors[idx] = (
                    math.sqrt(aCov[i, i])
                    if aCov[i, i] > 0.0
                    else -math.sqrt(-aCov[i, i])
                )
            else:
                labels[idx] = l
                entries[idx] = self.__parCounters[l]
        self.__dataframe = pd.DataFrame(
            {
                "label": labels,
                "entry": entries,
                "correction": corrections,
                "error": errors,
            }
        )
        return

    @property
    def dataframe(self):
        """The dataframe property."""
        return self.__dataframe


if __name__ == "__main__":
    print(time.asctime())
    parser = argparse.ArgumentParser(
        prog="mini-pede", description="Minimum millipede algorithm in python"
    )
    parser.add_argument(
        "-b",
        "--binary-file",
        type=str,
        required=True,
        help="set the binary file name",
    )
    parser.add_argument(
        "-o",
        "--output-file",
        type=str,
        default="millepede.res.json",
        required=False,
        help="set the output file",
    )
    parser.add_argument(
        "-n",
        "--num-of-entries",
        default=100000,
        type=int,
        required=False,
        help="Number of entries to analyze",
    )
    parser.add_argument(
        "-m",
        "--min-entries",
        default=25,
        type=int,
        required=False,
        help="minimum entries for each parameter",
    )
    parser.add_argument(
        "-p",
        "--fix-parameters",
        type=int,
        nargs="*",
        help="Set the list of fix parameters",
    )
    parser.add_argument(
        "-c",
        "--constraint",
        type=str,
        required=False,
        default="",
        help="Add constraint filename",
    )
    parser.add_argument(
        "-t",
        "--threads",
        default=os.cpu_count(),
        type=int,
        required=False,
        help="Number of threads for numpy calculation",
    )
    parser.add_argument(
        "--huge-cut",
        default=50,
        type=float,
        required=False,
        help="Number of threads for numpy calculation",
    )
    args = parser.parse_args()
    #
    # Some steering parameters
    #
    # maximum number of records to read (for each file)
    # minimum number of entries (appearances in binary files) for variable global parameters

    #
    # get pede instance
    #
    p = Pede()

    #
    # MP2 test case (from pede -t)
    #
    # add (Fortran) binary file(s)
    p.addBinaryFile(args.binary_file, args.num_of_entries, "C")
    # define parameters
    p.defineParameters(args.min_entries, fixedPar=args.fix_parameters)
    # add constraints
    if args.constraint != "":
        p.addConstraints(args.constraint)
    # construct linear equation system

    with threadpool_limits(limits=args.threads, user_api="blas"):
        p.construct(args.huge_cut)
        p.solve()

    p.dataframe.to_json(args.output_file)

    print()
    print(time.asctime())
