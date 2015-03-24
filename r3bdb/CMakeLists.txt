# Recurse into the given subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.

add_subdirectory (landdb)
if(FAIRDB_FOUND)
add_subdirectory (commondb)
add_subdirectory (caldb)
add_subdirectory (losdb)
endif(FAIRDB_FOUND)

