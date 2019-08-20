#ifndef TRACKER_ROUTINES_H
#define TRACKER_ROUTINES_H

extern"C" {

void init_from_cpp_();
void single_track_from_cpp_(int *n, bool *dc, double x[], double y[], double z[], bool pat[], double param[], double chi[]);
void double_track_from_cpp_(int *n,double x1[], double y1[], double z1[], bool pat1[],double x2[], double y2[], double z2[], bool pat2[] , double param1[], double param2[], double chi[]);

// for testing purposes
void called_from_cpp_(int *n,double *db);
void called_from_cpp2_(int *n,double db2[], bool bo[], double db3[]);

}

#endif
