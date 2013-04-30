struct Sl_map_ta {
 int ta[10];   //[0..9]
 int taV;      //plus taV
};

//mapping: tac ch
struct Sl_map_tc {
 int tc[10];   //[0..9]
 int tcV;      //plus tcV
};


struct Sl_map_QDC {
 double QDC[10];   //[0..9]
 double QDCV;      //plus QDCV
};

//mapped time info and t17 channel time info
struct Sl_map_tim {
 double tim[10];   //[0..9]
 double timV;      //plus timV
 double tim17[10];
 double tim17V;    //plus tim17V
};

//mapped clock cycle info
struct Sl_map_cl {
 int cl[10];   //[0..9]
 int clV;      //plus clV
};

//mapped event mult and ut
struct Sl_map_hit {
 int hit[10];
 double ut[10];
 int hitV;
 double utV;
};

//mapped tusync
struct Sl_map_tusync {
 double tusync[10];   //[0..9]
 double tusyncV;      //plus tusyncV
};

double gain1[10][20];
double gain2[10][20];
double off1[10][20];
double off2[10][20];
double qdiff[10][20];
double tdiff[10][20];
double qlarg[10][20];
double tlarg[10][20];
double tcutmin[10][20];
double tcutmax[10][20];
