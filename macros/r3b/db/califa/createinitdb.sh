if [ "$#" -ne 1 ] ; then
 echo "ERROR!! At least one arguments should be provided (type of db)" 1>&2
else
 source ~/Work/r3broot/macros/r3b/db/califa/dbconfigo.sh local_pgsql > /dev/null
 if [ "$?" -eq "0" ]; then
  if [ "$1" == "local_pgsql" ]; then
   psql -d r3b -c "DROP TABLE USERS" > /dev/null 2>&1
   psql -d r3b -c "CREATE TABLE USERS(USERNAME VARCHAR(25) PRIMARY KEY,PASSWORD TEXT,INIT BOOLEAN DEFAULT FALSE)" > /dev/null 2>&1
   if [ "$?" -eq "0" ]; then
    psql -d r3b -c "INSERT INTO USERS VALUES('Darmstadt','',DEFAULT)" > /dev/null 2>&1
    psql -d r3b -c "INSERT INTO USERS VALUES('Lund','',DEFAULT)" > /dev/null 2>&1
    psql -d r3b -c "INSERT INTO USERS VALUES('Madrid','',DEFAULT)" > /dev/null 2>&1
    psql -d r3b -c "INSERT INTO USERS VALUES('Munich','',DEFAULT)" > /dev/null 2>&1
    psql -d r3b -c "INSERT INTO USERS VALUES('Santiago','',DEFAULT)" > /dev/null 2>&1
    if [ "$?" -eq "0" ]; then 
     psql -d r3b -c "DROP TABLE LOCATIONS" > /dev/null 2>&1
     psql -d r3b -c "CREATE TABLE LOCATIONS(NAME VARCHAR(10))" > /dev/null 2>&1
     if [ "$?" -eq "0" ]; then
      psql -d r3b -c "INSERT INTO LOCATIONS VALUES('Darmstadt')" > /dev/null 2>&1
      psql -d r3b -c "INSERT INTO LOCATIONS VALUES('Lund')" > /dev/null 2>&1
      psql -d r3b -c "INSERT INTO LOCATIONS VALUES('Madrid')" > /dev/null 2>&1
      psql -d r3b -c "INSERT INTO LOCATIONS VALUES('Munich')" > /dev/null 2>&1
      psql -d r3b -c "INSERT INTO LOCATIONS VALUES('Santiago')" > /dev/null 2>&1
     fi
    fi
   fi
  fi
 else
  echo "ERROR!! dbconfig is not available" 1>&2
 fi
fi
 



