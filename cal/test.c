
#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>

int main() {

char buff[355];

int i = 0;

while  (i<1000) {

    sprintf(buff,"*pMatrix%i,*pMatrix%i,*pMatrix%i,*pMatrix%i,*pMatrix%i,",
	   i+2,i+4,i+6,i+8,i+10);
     i+=10;
    cout << buff << endl;

}


}


