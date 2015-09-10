# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

#add_subdirectory (landdb)
#add_subdirectory (losdb)
if(FAIRDB_FOUND)
add_subdirectory (commondb)
add_subdirectory (caldb)
endif(FAIRDB_FOUND)

