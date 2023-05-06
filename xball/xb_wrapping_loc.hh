// -----          Definition file for the Al wrappers in R3BCal          -----
// -----               Created 08/06/12  by Pablo Cabanelas              -----
//
// The last six numbers are the wrapping numbers of the adjesent wrappings and is NOT used by R3BCal
// Angles in degrees
// Syntax: XB_WRAPPING( <WrappingNumber>, <WrappingType>, <theta>, <phi>, <psi>, *, *, *, *, *, *)
//
XB_WRAPPING(1, XB_WTYPE_D, 90.000, 180.000, -90.000, 1, 2, 3, 4, 5, 6)                 // D    0.000
XB_WRAPPING(2, XB_WTYPE_B, 90.000, -163.743, 0.000, 0, 2, 6, 7, 8, 9)                  // B    0.051
XB_WRAPPING(3, XB_WTYPE_C, 73.928, -171.645, -32.656, 0, 1, 3, 9, 10, 11)              // C    0.107
XB_WRAPPING(4, XB_WTYPE_C, 73.928, 171.645, 212.656, 0, 2, 4, 11, 12, 13)              // C    0.107
XB_WRAPPING(5, XB_WTYPE_B, 90.000, 163.743, 180.000, 0, 3, 5, 13, 14, 15)              // B    0.051
XB_WRAPPING(6, XB_WTYPE_C, 106.072, 171.645, 147.344, 0, 4, 6, 15, 16, 17)             // C    0.107
XB_WRAPPING(7, XB_WTYPE_C, 106.072, -171.645, 32.656, 0, 1, 5, 7, 17, 18)              // C    0.107
XB_WRAPPING(8, XB_WTYPE_B, 104.686, -153.168, 71.370, 1, 6, 8, 18, 19, 20)             // B    0.019
XB_WRAPPING(9, XB_WTYPE_A, 90.000, -148.283, 0.000, 1, 7, 9, 20, 21, 162)              // A    0.003
XB_WRAPPING(10, XB_WTYPE_B, 75.314, -153.168, -71.370, 1, 2, 8, 10, 21, 22)            // B    0.019
XB_WRAPPING(11, XB_WTYPE_D, 60.000, -159.095, 20.905, 9, 2, 11, 24, 23, 22)            // D    0.017
XB_WRAPPING(12, XB_WTYPE_C, 59.817, 180.000, 90.000, 2, 3, 12, 25, 24, 10)             // C    0.064
XB_WRAPPING(13, XB_WTYPE_D, 60.000, 159.095, -20.905, 3, 13, 27, 26, 25, 11)           // D    0.017
XB_WRAPPING(14, XB_WTYPE_B, 75.314, 153.168, 251.370, 3, 4, 14, 28, 27, 12)            // B    0.019
XB_WRAPPING(15, XB_WTYPE_A, 90.000, 148.283, 180.000, 4, 15, 29, 28, 13, 162)          // A    0.003
XB_WRAPPING(16, XB_WTYPE_B, 104.686, 153.168, 108.630, 4, 5, 16, 30, 29, 14)           // B    0.019
XB_WRAPPING(17, XB_WTYPE_D, 120.000, 159.095, -159.095, 5, 17, 32, 31, 30, 15)         // D    0.017
XB_WRAPPING(18, XB_WTYPE_C, 120.183, 180.000, 270.000, 5, 6, 18, 33, 32, 16)           // C    0.064
XB_WRAPPING(19, XB_WTYPE_D, 120.000, -159.095, 159.095, 6, 7, 19, 34, 33, 17)          // D    0.017
XB_WRAPPING(20, XB_WTYPE_C, 116.612, -138.914, 110.220, 7, 20, 36, 35, 34, 18)         // C    0.022
XB_WRAPPING(21, XB_WTYPE_B, 99.014, -135.670, 144.998, 7, 8, 21, 37, 36, 19)           // B    0.013
XB_WRAPPING(22, XB_WTYPE_B, 80.986, -135.670, 215.002, 8, 9, 22, 38, 37, 20)           // B    0.013
XB_WRAPPING(23, XB_WTYPE_C, 63.388, -138.914, 249.780, 9, 10, 23, 39, 38, 21)          // C    0.014
XB_WRAPPING(24, XB_WTYPE_C, 47.634, -142.679, 132.588, 10, 24, 40, 58, 39, 22)         // C    0.018
XB_WRAPPING(25, XB_WTYPE_B, 45.050, -167.209, 115.890, 10, 11, 25, 41, 40, 23)         // B    0.024
XB_WRAPPING(26, XB_WTYPE_B, 45.050, 167.209, 64.110, 11, 12, 26, 42, 41, 24)           // B    0.024
XB_WRAPPING(27, XB_WTYPE_C, 47.634, 142.679, 47.412, 12, 27, 43, 62, 42, 25)           // C    0.018
XB_WRAPPING(28, XB_WTYPE_C, 63.388, 138.914, -69.780, 12, 13, 28, 44, 43, 26)          // C    0.014
XB_WRAPPING(29, XB_WTYPE_B, 80.986, 135.670, -35.002, 13, 14, 29, 45, 44, 27)          // B    0.013
XB_WRAPPING(30, XB_WTYPE_B, 99.014, 135.670, 35.002, 14, 15, 30, 46, 45, 28)           // B    0.013
XB_WRAPPING(31, XB_WTYPE_C, 116.612, 138.914, 69.780, 15, 16, 31, 47, 46, 29)          // C    0.022
XB_WRAPPING(32, XB_WTYPE_C, 132.366, 142.679, -47.412, 16, 32, 48, 69, 47, 30)         // C    0.018
XB_WRAPPING(33, XB_WTYPE_B, 134.950, 167.209, -64.110, 16, 17, 33, 49, 48, 31)         // B    0.024
XB_WRAPPING(34, XB_WTYPE_B, 134.950, -167.209, 244.110, 17, 18, 34, 50, 49, 32)        // B    0.024
XB_WRAPPING(35, XB_WTYPE_C, 132.366, -142.679, 227.412, 18, 19, 35, 51, 50, 33)        // C    0.018
XB_WRAPPING(36, XB_WTYPE_C, 125.985, -123.613, -1.114, 19, 36, 53, 52, 51, 34)         // C    0.020
XB_WRAPPING(37, XB_WTYPE_D, 108.000, -121.717, 58.283, 19, 20, 37, 54, 53, 35)         // D    0.001
XB_WRAPPING(38, XB_WTYPE_C, 90.000, -120.183, 180.000, 20, 21, 38, 55, 54, 36)         // C    0.013
XB_WRAPPING(39, XB_WTYPE_D, 72.000, -121.717, 121.717, 21, 22, 39, 56, 55, 37)         // D    0.001
XB_WRAPPING(40, XB_WTYPE_C, 54.015, -123.613, 1.114, 22, 23, 58, 57, 56, 38)           // C    0.022
XB_WRAPPING(41, XB_WTYPE_B, 30.325, -149.860, 172.011, 23, 24, 41, 60, 59, 58)         // B    0.018
XB_WRAPPING(42, XB_WTYPE_A, 31.717, 180.000, -90.000, 24, 25, 42, 60, 40, 162)         // A    0.003
XB_WRAPPING(43, XB_WTYPE_B, 30.325, 149.860, 7.989, 25, 26, 62, 61, 60, 41)            // B    0.018
XB_WRAPPING(44, XB_WTYPE_C, 54.015, 123.613, 178.886, 26, 27, 44, 64, 63, 62)          // C    0.022
XB_WRAPPING(45, XB_WTYPE_D, 72.000, 121.717, -121.717, 27, 28, 45, 65, 64, 43)         // D    0.001
XB_WRAPPING(46, XB_WTYPE_C, 90.000, 120.183, 0.000, 28, 29, 46, 66, 65, 44)            // C    0.013
XB_WRAPPING(47, XB_WTYPE_D, 108.000, 121.717, -58.283, 29, 30, 47, 67, 66, 45)         // D    0.001
XB_WRAPPING(48, XB_WTYPE_C, 125.985, 123.613, 181.114, 30, 31, 69, 68, 67, 46)         // C    0.022
XB_WRAPPING(49, XB_WTYPE_B, 149.675, 149.860, -7.989, 31, 32, 49, 71, 70, 69)          // B    0.018
XB_WRAPPING(50, XB_WTYPE_A, 148.283, 180.000, 90.000, 32, 33, 50, 71, 48, 162)         // A    0.003
XB_WRAPPING(51, XB_WTYPE_B, 149.675, -149.860, 187.989, 33, 34, 51, 72, 71, 49)        // B    0.018
XB_WRAPPING(52, XB_WTYPE_D, 144.000, -121.717, -58.283, 34, 35, 52, 73, 72, 50)        // D    0.004
XB_WRAPPING(53, XB_WTYPE_B, 133.643, -102.504, 46.296, 35, 53, 74, 98, 73, 51)         // B    0.021
XB_WRAPPING(54, XB_WTYPE_B, 115.890, -106.368, -25.938, 35, 36, 54, 75, 74, 52)        // B    0.016
XB_WRAPPING(55, XB_WTYPE_C, 98.026, -106.235, -59.672, 36, 37, 55, 76, 75, 53)         // C    0.114
XB_WRAPPING(56, XB_WTYPE_C, 81.974, -106.235, 59.672, 37, 38, 56, 77, 76, 54)          // C    0.114
XB_WRAPPING(57, XB_WTYPE_B, 64.110, -106.368, 25.938, 38, 39, 57, 78, 77, 55)          // B    0.016
XB_WRAPPING(58, XB_WTYPE_B, 46.357, -102.504, -46.296, 39, 58, 79, 93, 78, 56)         // B    0.021
XB_WRAPPING(59, XB_WTYPE_D, 36.000, -121.717, -121.717, 39, 23, 40, 59, 79, 57)        // D    0.004
XB_WRAPPING(60, XB_WTYPE_C, 18.063, -116.764, 175.290, 40, 60, 80, 91, 79, 58)         // C    0.114
XB_WRAPPING(61, XB_WTYPE_B, 16.257, 180.000, -90.000, 40, 41, 42, 61, 80, 59)          // B    0.012
XB_WRAPPING(62, XB_WTYPE_C, 18.063, 116.764, 4.710, 42, 62, 88, 89, 80, 60)            // C    0.114
XB_WRAPPING(63, XB_WTYPE_D, 36.000, 121.717, 121.717, 42, 26, 43, 63, 88, 61)          // D    0.004
XB_WRAPPING(64, XB_WTYPE_B, 46.357, 102.504, 226.296, 43, 64, 87, 109, 88, 62)         // B    0.021
XB_WRAPPING(65, XB_WTYPE_B, 64.110, 106.368, 154.062, 43, 44, 65, 86, 87, 63)          // B    0.016
XB_WRAPPING(66, XB_WTYPE_C, 81.974, 106.235, 120.328, 44, 45, 66, 85, 86, 64)          // C    0.114
XB_WRAPPING(67, XB_WTYPE_C, 98.026, 106.235, 239.672, 45, 46, 67, 84, 85, 65)          // C    0.114
XB_WRAPPING(68, XB_WTYPE_B, 115.890, 106.368, 205.938, 46, 47, 68, 83, 84, 66)         // B    0.016
XB_WRAPPING(69, XB_WTYPE_B, 133.643, 102.504, 133.704, 47, 69, 82, 104, 83, 67)        // B    0.021
XB_WRAPPING(70, XB_WTYPE_D, 144.000, 121.717, 58.283, 47, 31, 48, 70, 82, 68)          // D    0.004
XB_WRAPPING(71, XB_WTYPE_C, 161.937, 116.764, -4.710, 48, 71, 81, 102, 82, 69)         // C    0.114
XB_WRAPPING(72, XB_WTYPE_B, 163.743, 180.000, 90.000, 48, 49, 50, 72, 81, 70)          // B    0.012
XB_WRAPPING(73, XB_WTYPE_C, 161.937, -116.764, 184.710, 50, 51, 73, 100, 81, 71)       // C    0.114
XB_WRAPPING(74, XB_WTYPE_C, 149.817, -90.000, 90.000, 51, 52, 98, 99, 100, 72)         // C    0.013
XB_WRAPPING(75, XB_WTYPE_A, 121.717, -90.000, -90.000, 52, 53, 75, 97, 98, 162)        // A    0.003
XB_WRAPPING(76, XB_WTYPE_B, 106.257, -90.000, -90.000, 53, 54, 76, 96, 97, 74)         // B    0.051
XB_WRAPPING(77, XB_WTYPE_D, 90.000, -90.000, 180.000, 54, 55, 77, 95, 96, 75)          // D    0.000
XB_WRAPPING(78, XB_WTYPE_B, 73.743, -90.000, 90.000, 55, 56, 78, 94, 95, 76)           // B    0.051
XB_WRAPPING(79, XB_WTYPE_A, 58.283, -90.000, 90.000, 56, 57, 93, 94, 77, 162)          // A    0.003
XB_WRAPPING(80, XB_WTYPE_C, 30.183, -90.000, 270.000, 57, 58, 59, 91, 92, 93)          // C    0.013
XB_WRAPPING(81, XB_WTYPE_D, 0.000, 0.000, 0.000, 59, 60, 61, 89, 90, 91)               // D    0.000
XB_WRAPPING(82, XB_WTYPE_D, 180.000, 0.000, 180.000, 70, 71, 72, 100, 101, 102)        // D    0.000
XB_WRAPPING(83, XB_WTYPE_C, 149.817, 90.000, 90.000, 68, 69, 70, 102, 103, 104)        // C    0.013
XB_WRAPPING(84, XB_WTYPE_A, 121.717, 90.000, 270.000, 67, 68, 104, 105, 84, 162)       // A    0.003
XB_WRAPPING(85, XB_WTYPE_B, 106.257, 90.000, 270.000, 66, 67, 83, 105, 106, 85)        // B    0.051
XB_WRAPPING(86, XB_WTYPE_D, 90.000, 90.000, -180.000, 65, 66, 84, 106, 107, 86)        // D    0.000
XB_WRAPPING(87, XB_WTYPE_B, 73.743, 90.000, 90.000, 64, 65, 85, 107, 108, 87)          // B    0.051
XB_WRAPPING(88, XB_WTYPE_A, 58.283, 90.000, 90.000, 63, 64, 86, 108, 109, 162)         // A    0.003
XB_WRAPPING(89, XB_WTYPE_C, 30.183, 90.000, -90.000, 61, 62, 63, 109, 110, 89)         // C    0.013
XB_WRAPPING(90, XB_WTYPE_C, 18.063, 63.236, 175.290, 80, 61, 88, 110, 111, 90)         // C    0.114
XB_WRAPPING(91, XB_WTYPE_B, 16.257, 0.000, 270.000, 91, 80, 89, 111, 112, 113)         // B    0.012
XB_WRAPPING(92, XB_WTYPE_C, 18.063, -63.236, 4.710, 79, 59, 80, 90, 113, 92)           // C    0.114
XB_WRAPPING(93, XB_WTYPE_D, 36.000, -58.283, 121.717, 93, 79, 91, 113, 130, 114)       // D    0.004
XB_WRAPPING(94, XB_WTYPE_B, 46.357, -77.496, 226.296, 94, 78, 57, 79, 92, 114)         // B    0.021
XB_WRAPPING(95, XB_WTYPE_B, 64.110, -73.632, 154.062, 95, 77, 78, 93, 114, 115)        // B    0.016
XB_WRAPPING(96, XB_WTYPE_C, 81.974, -73.765, 120.328, 96, 76, 77, 94, 115, 116)        // C    0.114
XB_WRAPPING(97, XB_WTYPE_C, 98.026, -73.765, 239.672, 97, 75, 76, 95, 116, 117)        // C    0.114
XB_WRAPPING(98, XB_WTYPE_B, 115.890, -73.632, 205.938, 98, 74, 75, 96, 117, 118)       // B    0.016
XB_WRAPPING(99, XB_WTYPE_B, 133.643, -77.496, 133.704, 99, 73, 52, 74, 97, 118)        // B    0.021
XB_WRAPPING(100, XB_WTYPE_D, 144.000, -58.283, 58.283, 100, 73, 98, 118, 135, 119)     // D    0.004
XB_WRAPPING(101, XB_WTYPE_C, 161.937, -63.236, -4.710, 101, 81, 72, 73, 99, 119)       // C    0.114
XB_WRAPPING(102, XB_WTYPE_B, 163.743, 0.000, 90.000, 102, 81, 100, 119, 120, 121)      // B    0.012
XB_WRAPPING(103, XB_WTYPE_C, 161.937, 63.236, 184.710, 103, 82, 70, 81, 101, 121)      // C    0.114
XB_WRAPPING(104, XB_WTYPE_D, 144.000, 58.283, -58.283, 104, 82, 102, 121, 138, 122)    // D    0.004
XB_WRAPPING(105, XB_WTYPE_B, 133.643, 77.496, 46.296, 105, 83, 68, 82, 103, 122)       // B    0.021
XB_WRAPPING(106, XB_WTYPE_B, 115.890, 73.632, -25.938, 106, 84, 83, 104, 122, 123)     // B    0.016
XB_WRAPPING(107, XB_WTYPE_C, 98.026, 73.765, -59.672, 107, 85, 84, 105, 123, 124)      // C    0.114
XB_WRAPPING(108, XB_WTYPE_C, 81.974, 73.765, 59.672, 108, 86, 85, 106, 124, 125)       // C    0.114
XB_WRAPPING(109, XB_WTYPE_B, 64.110, 73.632, 25.938, 109, 87, 86, 107, 125, 126)       // B    0.016
XB_WRAPPING(110, XB_WTYPE_B, 46.357, 77.496, -46.296, 110, 88, 63, 87, 108, 126)       // B    0.021
XB_WRAPPING(111, XB_WTYPE_D, 36.000, 58.283, -121.717, 111, 89, 88, 109, 126, 127)     // D    0.004
XB_WRAPPING(112, XB_WTYPE_B, 30.325, 30.140, 172.011, 112, 90, 89, 110, 127, 128)      // B    0.018
XB_WRAPPING(113, XB_WTYPE_A, 31.717, 0.000, 270.000, 113, 90, 111, 128, 129, 162)      // A    0.003
XB_WRAPPING(114, XB_WTYPE_B, 30.325, -30.140, 7.989, 92, 91, 90, 112, 129, 130)        // B    0.018
XB_WRAPPING(115, XB_WTYPE_C, 54.015, -56.387, 178.886, 115, 94, 93, 92, 130, 131)      // C    0.022
XB_WRAPPING(116, XB_WTYPE_D, 72.000, -58.283, -121.717, 116, 95, 94, 114, 131, 132)    // D    0.001
XB_WRAPPING(117, XB_WTYPE_C, 90.000, -59.817, 0.000, 117, 96, 95, 115, 132, 133)       // C    0.013
XB_WRAPPING(118, XB_WTYPE_D, 108.000, -58.283, -58.283, 118, 97, 96, 116, 133, 134)    // D    0.001
XB_WRAPPING(119, XB_WTYPE_C, 125.985, -56.387, 181.114, 99, 98, 97, 117, 134, 135)     // C    0.022
XB_WRAPPING(120, XB_WTYPE_B, 149.675, -30.140, -7.989, 120, 101, 100, 99, 135, 136)    // B    0.018
XB_WRAPPING(121, XB_WTYPE_A, 148.283, 0.000, 90.000, 121, 101, 119, 136, 137, 162)     // A    0.003
XB_WRAPPING(122, XB_WTYPE_B, 149.675, 30.140, 187.989, 103, 102, 101, 120, 137, 138)   // B    0.018
XB_WRAPPING(123, XB_WTYPE_C, 125.985, 56.387, -1.114, 123, 105, 104, 103, 138, 139)    // C    0.022
XB_WRAPPING(124, XB_WTYPE_D, 108.000, 58.283, 58.283, 124, 106, 105, 122, 139, 140)    // D    0.001
XB_WRAPPING(125, XB_WTYPE_C, 90.000, 59.817, 180.000, 125, 107, 106, 123, 140, 141)    // C    0.013
XB_WRAPPING(126, XB_WTYPE_D, 72.000, 58.283, 121.717, 126, 108, 107, 124, 141, 142)    // D    0.001
XB_WRAPPING(127, XB_WTYPE_C, 54.015, 56.387, 1.114, 127, 110, 109, 108, 125, 142)      // C    0.034
XB_WRAPPING(128, XB_WTYPE_C, 47.634, 37.321, 132.588, 128, 111, 110, 126, 142, 143)    // C    0.018
XB_WRAPPING(129, XB_WTYPE_B, 45.050, 12.791, 115.890, 129, 112, 111, 127, 143, 144)    // B    0.024
XB_WRAPPING(130, XB_WTYPE_B, 45.050, -12.791, 64.110, 130, 113, 112, 128, 144, 145)    // B    0.024
XB_WRAPPING(131, XB_WTYPE_C, 47.634, -37.321, 47.412, 131, 114, 92, 113, 129, 145)     // C    0.018
XB_WRAPPING(132, XB_WTYPE_C, 63.388, -41.086, -69.780, 132, 115, 114, 130, 145, 146)   // C    0.022
XB_WRAPPING(133, XB_WTYPE_B, 80.986, -44.330, -35.002, 133, 116, 115, 131, 146, 147)   // B    0.013
XB_WRAPPING(134, XB_WTYPE_B, 99.014, -44.330, 35.002, 134, 117, 116, 132, 147, 148)    // B    0.013
XB_WRAPPING(135, XB_WTYPE_C, 116.612, -41.086, 69.780, 135, 118, 117, 133, 148, 149)   // C    0.014
XB_WRAPPING(136, XB_WTYPE_C, 132.366, -37.321, -47.412, 136, 119, 99, 118, 134, 149)   // C    0.018
XB_WRAPPING(137, XB_WTYPE_B, 134.950, -12.791, -64.110, 137, 120, 119, 135, 149, 150)  // B    0.024
XB_WRAPPING(138, XB_WTYPE_B, 134.950, 12.791, 244.110, 138, 121, 120, 136, 150, 151)   // B    0.024
XB_WRAPPING(139, XB_WTYPE_C, 132.366, 37.321, 227.412, 139, 122, 103, 121, 137, 151)   // C    0.018
XB_WRAPPING(140, XB_WTYPE_C, 116.612, 41.086, 110.220, 140, 123, 122, 138, 151, 152)   // C    0.014
XB_WRAPPING(141, XB_WTYPE_B, 99.014, 44.330, 144.998, 141, 124, 123, 139, 152, 153)    // B    0.013
XB_WRAPPING(142, XB_WTYPE_B, 80.986, 44.330, 215.002, 142, 125, 124, 140, 153, 154)    // B    0.013
XB_WRAPPING(143, XB_WTYPE_C, 63.388, 41.086, 249.780, 143, 127, 126, 125, 141, 154)    // C    0.022
XB_WRAPPING(144, XB_WTYPE_D, 60.000, 20.905, 20.905, 144, 128, 127, 142, 154, 155)     // D    0.017
XB_WRAPPING(145, XB_WTYPE_C, 59.817, 0.000, 90.000, 145, 129, 128, 143, 155, 156)      // C    0.064
XB_WRAPPING(146, XB_WTYPE_D, 60.000, -20.905, -20.905, 146, 131, 130, 129, 144, 156)   // D    0.017
XB_WRAPPING(147, XB_WTYPE_B, 75.314, -26.832, 251.370, 147, 132, 131, 145, 156, 157)   // B    0.019
XB_WRAPPING(148, XB_WTYPE_A, 90.000, -31.717, 180.000, 148, 133, 132, 146, 157, 162)   // A    0.003
XB_WRAPPING(149, XB_WTYPE_B, 104.686, -26.832, 108.630, 149, 134, 133, 147, 157, 158)  // B    0.019
XB_WRAPPING(150, XB_WTYPE_D, 120.000, -20.905, -159.095, 150, 136, 135, 134, 148, 158) // D    0.017
XB_WRAPPING(151, XB_WTYPE_C, 120.183, 0.000, 270.000, 151, 137, 136, 149, 158, 159)    // C    0.064
XB_WRAPPING(152, XB_WTYPE_D, 120.000, 20.905, 159.095, 152, 139, 138, 137, 150, 159)   // D    0.017
XB_WRAPPING(153, XB_WTYPE_B, 104.686, 26.832, 71.370, 153, 140, 139, 151, 159, 160)    // B    0.019
XB_WRAPPING(154, XB_WTYPE_A, 90.000, 31.717, 0.000, 154, 141, 140, 152, 160, 162)      // A    0.003
XB_WRAPPING(155, XB_WTYPE_B, 75.314, 26.832, -71.370, 155, 143, 142, 141, 153, 160)    // B    0.019
XB_WRAPPING(156, XB_WTYPE_C, 73.928, 8.355, -32.656, 156, 144, 143, 154, 160, 161)     // C    0.107
XB_WRAPPING(157, XB_WTYPE_C, 73.928, -8.355, 212.656, 157, 146, 145, 144, 155, 161)    // C    0.107
XB_WRAPPING(158, XB_WTYPE_B, 90.000, -16.257, 180.000, 158, 148, 147, 146, 156, 161)   // B    0.051
XB_WRAPPING(159, XB_WTYPE_C, 106.072, -8.355, 147.344, 159, 150, 149, 148, 157, 161)   // C    0.107
XB_WRAPPING(160, XB_WTYPE_C, 106.072, 8.355, 32.656, 160, 152, 151, 150, 158, 161)     // C    0.107
XB_WRAPPING(161, XB_WTYPE_B, 90.000, 16.257, 0.000, 161, 155, 154, 153, 152, 159)      // B    0.051
XB_WRAPPING(162, XB_WTYPE_D, 90.000, 0.000, 90.000, 155, 156, 157, 158, 159, 160)      // D    0.000
