Set(CTEST_SOURCE_DIRECTORY $ENV{SOURCEDIR})
Set(CTEST_BINARY_DIRECTORY $ENV{BUILDDIR})
Set(CTEST_SITE $ENV{SITE})
Set(CTEST_BUILD_NAME $ENV{LABEL})
Set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
Set(CTEST_PROJECT_NAME "R3BRoot")

Set(CTEST_UPDATE_COMMAND "svn")

If($ENV{ctest_model} MATCHES Continuous)
  Set(CTEST_SVN_UPDATE_OPTIONS "$ENV{REVISION}")
EndIf()

Set(BUILD_COMMAND "make")
Set(CTEST_BUILD_COMMAND "${BUILD_COMMAND} -j$ENV{number_of_processors}")

If($ENV{ctest_model} MATCHES Nightly OR $ENV{ctest_model} MATCHES Profile)

  Find_Program(GCOV_COMMAND gcov)
  If(GCOV_COMMAND)
    Message("Found GCOV: ${GCOV_COMMAND}")
    Set(CTEST_COVERAGE_COMMAND ${GCOV_COMMAND})
  EndIf(GCOV_COMMAND)

  String(TOUPPER $ENV{ctest_model} _Model)
  Set(ENV{ctest_model} Nightly)

  Set(CTEST_CONFIGURE_COMMAND " \"${CMAKE_EXECUTABLE_NAME}\" \"-G${CTEST_CMAKE_GENERATOR}\" \"${CTEST_SOURCE_DIRECTORY}\" \"-DCMAKE_BUILD_TYPE=${_Model}\" ")

  # get the information about conflicting or localy modified files
  # from svn, extract the relavant information about the file name
  # and put the result in the output variable
  Execute_Process(COMMAND svn stat -u  
                  COMMAND grep ^[CM]
                  COMMAND cut -c21- 
                  OUTPUT_VARIABLE FILELIST
                  )

  # create out of the output a cmake list. This step is done to convert the
  # stream into seperated filenames.
  # The trick is to exchange an "\n" by an ";" which is the separartor in
  # a list created by cmake 
  String(REGEX REPLACE "\n" ";" _result "${FILELIST}")

  ForEach(_file ${_result})
    String(STRIP "${_file}" _file1)
    Set(CTEST_NOTES_FILES ${CTEST_NOTES_FILES} "${CTEST_SOURCE_DIRECTORY}/${_file1}")
  EndForEach(_file ${_result})

  CTEST_EMPTY_BINARY_DIRECTORY(${CTEST_BINARY_DIRECTORY})

EndIf()

Configure_File(${CTEST_SOURCE_DIRECTORY}/CTestCustom.cmake
               ${CTEST_BINARY_DIRECTORY}/CTestCustom.cmake
              )
Ctest_Read_Custom_Files("${CTEST_BINARY_DIRECTORY}")

Ctest_Start($ENV{ctest_model})
If(NOT $ENV{ctest_model} MATCHES Experimental)
  Ctest_Update(SOURCE "${CTEST_SOURCE_DIRECTORY}")
EndIf()
Ctest_Configure(BUILD "${CTEST_BINARY_DIRECTORY}")
Ctest_Build(BUILD "${CTEST_BINARY_DIRECTORY}")
Ctest_Test(BUILD "${CTEST_BINARY_DIRECTORY}" PARALLEL_LEVEL $ENV{number_of_processors})
If(${_Model} MATCHES PROFILE)
  Ctest_Coverage(BUILD "${CTEST_BINARY_DIRECTORY}")
EndIf()
Ctest_Upload(FILES ${CTEST_NOTES_FILES})
Ctest_Submit()
