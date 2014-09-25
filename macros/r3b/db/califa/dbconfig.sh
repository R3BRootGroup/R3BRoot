# add for MySql libs
# MAc OSx
# export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/usr/local/mysql/lib
# Linux

# Log File 
# -- If the string is NULL the default file "fairdbinfo.log"
# -- will be created in the working directory 
export FAIRDB_LOGFILE_DB=""



if [ $1 = "local_mysql" ]
then
# Local MySQL server
export FAIRDB_TSQL_URL="xxx"
export FAIRDB_TSQL_USER="xxx"
export FAIRDB_TSQL_PSWD="xxx"
fi

if [ $1 = "local_pgsql" ]
then
# Local PgSQL server
export FAIRDB_TSQL_URL="xxx"
export FAIRDB_TSQL_USER="xxx"
export FAIRDB_TSQL_PSWD="xxx"
fi


if [ $1  = "local_sqlite" ]
then
# SQLite
export FAIRDB_TSQL_URL="xxx"
export FAIRDB_TSQL_USER="xxx"
export FAIRDB_TSQL_PSWD="xxx"
fi

echo $1 " session configured as: "
set | grep TSQL*
