#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream> 
#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1I.h>
#include <TStyle.h>

using namespace std;

// some horrible code to convert Angel's angles into 0-127 ADC channel format.

// I am not convinced that the azimuthals are in the right direction. 
// I will add an option to invert them, in case I am right. 

// g++ -o convert_to_febex convert_to_febex.cpp `root-config --cflags --glibs` && ./convert_to_febex

int main()
{ 
	string str[128], strx[128];
	int i,j,a[128],b[128],ax[128],bx[128],x;
	int febex[16]={6,5,4,3,2,1,0,7,8,15,14,13,12,11,10,9};
	int invert[16]={5,4,7,6,1,0,3,2,13,12,15,11,9,8,11,10};//hmm
	float c[128],d[128],cx[128],dx[128],div[128];
    char passit[20];
    const char *name;
    
    ifstream zzt("petal1.angles");

      if (!zzt) 
           {
           cout <<" Could not open file!" << endl;
           exit(-1);
           }
          
    j=0;
    
    while (!zzt.eof()) 
    {
     zzt >> str[j] >> a[j] >> b[j] >> c[j] >> d[j];
     j++;
    }
    
    zzt.close();
    
      ifstream zzzt("petal2.angles");

      if (!zzzt) 
           {
           cout <<" Couldn't open file!" << endl;
           exit(-1);
           }
          
    while (!zzzt.eof()) 
    {
     zzzt >> str[j] >> a[j] >> b[j] >> c[j] >> d[j];
     j++;
    }  
    zzzt.close();
     
	for(i=0;i<128;++i)
	{
	x=invert[i%16]+16*(i/16);
	div[i]=d[x];
	}

	for(i=0;i<128;++i)
	{
	x=febex[i%16]+16*(i/16);
	   ax[i]=a[x];
    	   bx[i]=b[x];
    		   cx[i]=c[x];
    			    //dx[i]=d[x]+180;   // non-inverted
    			  	  dx[i]=div[x]+180; // inverted
    			  	         strx[i]=str[x];
	}
	
	ofstream zzzzt("petals_angels_angles.txt");

	
		for(i=0;i<128;++i)
	{
	name=strx[i].c_str();
    // sprintf(passit,"%s %d %d %.1f %.1f",name,ax[i],bx[i],cx[i],dx[i]); // keeps Angel's notation
    sprintf(passit,"%d %.1f %.1f",i,cx[i],dx[i]); // does not.
	zzzzt << passit << endl;
	}
	zzzzt.close();
	
    cout<<"\npetals_angels_angles.txt created\n"<<endl;
    
	}
