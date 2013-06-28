// -----        Corner definition file for Al wrappers in R3BCal         -----
// -----             Created 08/06/12  by Pablo Cabanelas                -----
//
// The wrappings are generated to cover a sertain solid angle
// All values are in cm and are mesured om the frame with radious 49.86932985 cm
// Syntax: <X1/X5>, <Y1/Y5>, <X2/X6>, <Y2/Y6>, <X3/X7>, <Y3/Y7>, <X4/X8>, <Y4/Y8>,
//
 0.0,       9.06038,  8.61693,  2.79981,  5.32555, -7.33,     0.0,      -7.33,         //WrappingA1
 0.0,      -7.33,    -5.32555, -7.33,    -8.61693,  2.79981,  0.0,       9.06038,      //WrappingA2
-9.60862,   0.0,     -5.31000,  6.18700,  5.31000,  6.18700,  9.60862,   0.0,          //WrappingB1_mod
 9.60862,   0.0,      4.33050, -7.42200, -4.33050, -7.42200, -9.60862,   0.0,          //WrappingB2_mod
 0.0,      -8.11689, -7.15389, -3.93231, -8.51298,  4.85925,  0.0,       8.09558,      //WrappingC1
 0.0,       8.09558,  8.51298,  4.85925,  7.15389, -3.93231,  0.0,      -8.11689,      //WrappingC2
 0.0,     -10.08625, -6.80000, -4.3305,  -6.80000,  4.3305,   0.0,      10.08625,      //WrappingD1
 0.0,      10.08625,  6.80000,  4.3305,   6.80000, -4.3305,   0.0,     -10.08625,      //WrappingD2

// Original B wrapping before modification (modification done because of overlapping B-B wrappings in A-B-B combination)
//-9.60862, 0.0, -5.34300,  6.18700,  5.34300,  6.18700,  9.60862,   0.0,      //WrappingB1_orig
// 9.60862, 0.0,  4.33050, -7.42200, -4.33050, -7.42200, -9.60862,   0.0,      //WrappingB2_orig
