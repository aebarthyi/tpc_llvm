// RUN: %clang_cc1 -triple tpc-none-none -std=rc99 -emit-llvm -O1 -target-cpu goya2 %s -o - | FileCheck %s

void main(int ind) {
  half __local *res = (half __local *) ind;
  res[0] = 0.000000h;
  res[1] = 1.000000h;
  res[2] = 2.000000h;
  res[3] = 3.000000h;
  res[4] = 4.000000h;
  res[5] = 5.000000h;
  res[6] = 6.000000h;
  res[7] = 7.000000h;
  res[8] = 8.000000h;
  res[9] = 9.000000h;
  res[10] = 10.000000h;
  res[11] = 11.000000h;
  res[12] = 12.000000h;
  res[13] = 13.000000h;
  res[14] = 14.000000h;
  res[15] = 15.000000h;
  res[16] = 16.000000h;
  res[17] = 17.000000h;
  res[18] = 18.000000h;
  res[19] = 19.000000h;
  res[20] = 20.000000h;
  res[21] = 21.000000h;
  res[22] = 22.000000h;
  res[23] = 23.000000h;
  res[24] = 24.000000h;
  res[25] = 25.000000h;
  res[26] = 26.000000h;
  res[27] = 27.000000h;
  res[28] = 28.000000h;
  res[29] = 29.000000h;
  res[30] = 30.000000h;
  res[31] = 31.000000h;
  res[32] = 32.000000h;
  res[33] = 33.000000h;
  res[34] = 34.000000h;
  res[35] = 35.000000h;
  res[36] = 36.000000h;
  res[37] = 37.000000h;
  res[38] = 38.000000h;
  res[39] = 39.000000h;
  res[40] = 40.000000h;
  res[41] = 41.000000h;
  res[42] = 42.000000h;
  res[43] = 43.000000h;
  res[44] = 44.000000h;
  res[45] = 45.000000h;
  res[46] = 46.000000h;
  res[47] = 47.000000h;
  res[48] = 48.000000h;
  res[49] = 49.000000h;
  res[50] = 50.000000h;
  res[51] = 51.000000h;
  res[52] = 52.000000h;
  res[53] = 53.000000h;
  res[54] = 54.000000h;
  res[55] = 55.000000h;
  res[56] = 56.000000h;
  res[57] = 57.000000h;
  res[58] = 58.000000h;
  res[59] = 59.000000h;
  res[60] = 60.000000h;
  res[61] = 61.000000h;
  res[62] = 62.000000h;
  res[63] = 63.000000h;
  res[64] = 64.000000h;
  res[65] = 65.000000h;
  res[66] = 66.000000h;
  res[67] = 67.000000h;
  res[68] = 68.000000h;
  res[69] = 69.000000h;
  res[70] = 70.000000h;
  res[71] = 71.000000h;
  res[72] = 72.000000h;
  res[73] = 73.000000h;
  res[74] = 74.000000h;
  res[75] = 75.000000h;
  res[76] = 76.000000h;
  res[77] = 77.000000h;
  res[78] = 78.000000h;
  res[79] = 79.000000h;
  res[80] = 80.000000h;
  res[81] = 81.000000h;
  res[82] = 82.000000h;
  res[83] = 83.000000h;
  res[84] = 84.000000h;
  res[85] = 85.000000h;
  res[86] = 86.000000h;
  res[87] = 87.000000h;
  res[88] = 88.000000h;
  res[89] = 89.000000h;
  res[90] = 90.000000h;
  res[91] = 91.000000h;
  res[92] = 92.000000h;
  res[93] = 93.000000h;
  res[94] = 94.000000h;
  res[95] = 95.000000h;
  res[96] = 96.000000h;
  res[97] = 97.000000h;
  res[98] = 98.000000h;
  res[99] = 99.000000h;
  res[100] = 100.000000h;
  res[101] = 101.000000h;
  res[102] = 102.000000h;
  res[103] = 103.000000h;
  res[104] = 104.000000h;
  res[105] = 105.000000h;
  res[106] = 106.000000h;
  res[107] = 107.000000h;
  res[108] = 108.000000h;
  res[109] = 109.000000h;
  res[110] = 110.000000h;
  res[111] = 111.000000h;
  res[112] = 112.000000h;
  res[113] = 113.000000h;
  res[114] = 114.000000h;
  res[115] = 115.000000h;
  res[116] = 116.000000h;
  res[117] = 117.000000h;
  res[118] = 118.000000h;
  res[119] = 119.000000h;
  res[120] = 120.000000h;
  res[121] = 121.000000h;
  res[122] = 122.000000h;
  res[123] = 123.000000h;
  res[124] = 124.000000h;
  res[125] = 125.000000h;
  res[126] = 126.000000h;
  res[127] = 127.000000h;
}
// CHECK: store half 0.000000e+00
// CHECK: store half 1.000000e+00
// CHECK: store half 2.000000e+00
// CHECK: store half 3.000000e+00
// CHECK: store half 4.000000e+00
// CHECK: store half 5.000000e+00
// CHECK: store half 6.000000e+00
// CHECK: store half 7.000000e+00
// CHECK: store half 8.000000e+00
// CHECK: store half 9.000000e+00
// CHECK: store half 1.000000e+01
// CHECK: store half 1.100000e+01
// CHECK: store half 1.200000e+01
// CHECK: store half 1.300000e+01
// CHECK: store half 1.400000e+01
// CHECK: store half 1.500000e+01
// CHECK: store half 1.600000e+01
// CHECK: store half 1.700000e+01
// CHECK: store half 1.800000e+01
// CHECK: store half 1.900000e+01
// CHECK: store half 2.000000e+01
// CHECK: store half 2.100000e+01
// CHECK: store half 2.200000e+01
// CHECK: store half 2.300000e+01
// CHECK: store half 2.400000e+01
// CHECK: store half 2.500000e+01
// CHECK: store half 2.600000e+01
// CHECK: store half 2.700000e+01
// CHECK: store half 2.800000e+01
// CHECK: store half 2.900000e+01
// CHECK: store half 3.000000e+01
// CHECK: store half 3.100000e+01
// CHECK: store half 3.200000e+01
// CHECK: store half 3.300000e+01
// CHECK: store half 3.400000e+01
// CHECK: store half 3.500000e+01
// CHECK: store half 3.600000e+01
// CHECK: store half 3.700000e+01
// CHECK: store half 3.800000e+01
// CHECK: store half 3.900000e+01
// CHECK: store half 4.000000e+01
// CHECK: store half 4.100000e+01
// CHECK: store half 4.200000e+01
// CHECK: store half 4.300000e+01
// CHECK: store half 4.400000e+01
// CHECK: store half 4.500000e+01
// CHECK: store half 4.600000e+01
// CHECK: store half 4.700000e+01
// CHECK: store half 4.800000e+01
// CHECK: store half 4.900000e+01
// CHECK: store half 5.000000e+01
// CHECK: store half 5.100000e+01
// CHECK: store half 5.200000e+01
// CHECK: store half 5.300000e+01
// CHECK: store half 5.400000e+01
// CHECK: store half 5.500000e+01
// CHECK: store half 5.600000e+01
// CHECK: store half 5.700000e+01
// CHECK: store half 5.800000e+01
// CHECK: store half 5.900000e+01
// CHECK: store half 6.000000e+01
// CHECK: store half 6.100000e+01
// CHECK: store half 6.200000e+01
// CHECK: store half 6.300000e+01
// CHECK: store half 6.400000e+01
// CHECK: store half 6.500000e+01
// CHECK: store half 6.600000e+01
// CHECK: store half 6.700000e+01
// CHECK: store half 6.800000e+01
// CHECK: store half 6.900000e+01
// CHECK: store half 7.000000e+01
// CHECK: store half 7.100000e+01
// CHECK: store half 7.200000e+01
// CHECK: store half 7.300000e+01
// CHECK: store half 7.400000e+01
// CHECK: store half 7.500000e+01
// CHECK: store half 7.600000e+01
// CHECK: store half 7.700000e+01
// CHECK: store half 7.800000e+01
// CHECK: store half 7.900000e+01
// CHECK: store half 8.000000e+01
// CHECK: store half 8.100000e+01
// CHECK: store half 8.200000e+01
// CHECK: store half 8.300000e+01
// CHECK: store half 8.400000e+01
// CHECK: store half 8.500000e+01
// CHECK: store half 8.600000e+01
// CHECK: store half 8.700000e+01
// CHECK: store half 8.800000e+01
// CHECK: store half 8.900000e+01
// CHECK: store half 9.000000e+01
// CHECK: store half 9.100000e+01
// CHECK: store half 9.200000e+01
// CHECK: store half 9.300000e+01
// CHECK: store half 9.400000e+01
// CHECK: store half 9.500000e+01
// CHECK: store half 9.600000e+01
// CHECK: store half 9.700000e+01
// CHECK: store half 9.800000e+01
// CHECK: store half 9.900000e+01
// CHECK: store half 1.000000e+02
// CHECK: store half 1.010000e+02
// CHECK: store half 1.020000e+02
// CHECK: store half 1.030000e+02
// CHECK: store half 1.040000e+02
// CHECK: store half 1.050000e+02
// CHECK: store half 1.060000e+02
// CHECK: store half 1.070000e+02
// CHECK: store half 1.080000e+02
// CHECK: store half 1.090000e+02
// CHECK: store half 1.100000e+02
// CHECK: store half 1.110000e+02
// CHECK: store half 1.120000e+02
// CHECK: store half 1.130000e+02
// CHECK: store half 1.140000e+02
// CHECK: store half 1.150000e+02
// CHECK: store half 1.160000e+02
// CHECK: store half 1.170000e+02
// CHECK: store half 1.180000e+02
// CHECK: store half 1.190000e+02
// CHECK: store half 1.200000e+02
// CHECK: store half 1.210000e+02
// CHECK: store half 1.220000e+02
// CHECK: store half 1.230000e+02
// CHECK: store half 1.240000e+02
// CHECK: store half 1.250000e+02
// CHECK: store half 1.260000e+02
// CHECK: store half 1.270000e+02
