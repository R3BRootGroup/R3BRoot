// Generated Tue Apr  7 20:27:15 2020
unsigned g_fib32m_trig_map[512];
unsigned g_fib32s_trig_map[2];
void fib32_trig_map_setup() {
	g_fib32m_trig_map[0] = 0;
	g_fib32m_trig_map[4] = 0;
	g_fib32m_trig_map[8] = 0;
	g_fib32m_trig_map[12] = 0;
	g_fib32m_trig_map[1] = 0;
	g_fib32m_trig_map[5] = 0;
	g_fib32m_trig_map[9] = 0;
	g_fib32m_trig_map[13] = 0;
	g_fib32m_trig_map[2] = 0;
	g_fib32m_trig_map[6] = 0;
	g_fib32m_trig_map[10] = 0;
	g_fib32m_trig_map[14] = 0;
	g_fib32m_trig_map[3] = 0;
	g_fib32m_trig_map[7] = 0;
	g_fib32m_trig_map[11] = 0;
	g_fib32m_trig_map[15] = 0;
	g_fib32m_trig_map[16] = 0;
	g_fib32m_trig_map[20] = 0;
	g_fib32m_trig_map[24] = 0;
	g_fib32m_trig_map[28] = 0;
	g_fib32m_trig_map[17] = 0;
	g_fib32m_trig_map[21] = 0;
	g_fib32m_trig_map[25] = 0;
	g_fib32m_trig_map[29] = 0;
	g_fib32m_trig_map[18] = 0;
	g_fib32m_trig_map[22] = 0;
	g_fib32m_trig_map[26] = 0;
	g_fib32m_trig_map[30] = 0;
	g_fib32m_trig_map[19] = 0;
	g_fib32m_trig_map[23] = 0;
	g_fib32m_trig_map[27] = 0;
	g_fib32m_trig_map[31] = 0;
	g_fib32m_trig_map[32] = 0;
	g_fib32m_trig_map[36] = 0;
	g_fib32m_trig_map[40] = 0;
	g_fib32m_trig_map[44] = 0;
	g_fib32m_trig_map[33] = 0;
	g_fib32m_trig_map[37] = 0;
	g_fib32m_trig_map[41] = 0;
	g_fib32m_trig_map[45] = 0;
	g_fib32m_trig_map[34] = 0;
	g_fib32m_trig_map[38] = 0;
	g_fib32m_trig_map[42] = 0;
	g_fib32m_trig_map[46] = 0;
	g_fib32m_trig_map[35] = 0;
	g_fib32m_trig_map[39] = 0;
	g_fib32m_trig_map[43] = 0;
	g_fib32m_trig_map[47] = 0;
	g_fib32m_trig_map[48] = 0;
	g_fib32m_trig_map[52] = 0;
	g_fib32m_trig_map[56] = 0;
	g_fib32m_trig_map[60] = 0;
	g_fib32m_trig_map[49] = 0;
	g_fib32m_trig_map[53] = 0;
	g_fib32m_trig_map[57] = 0;
	g_fib32m_trig_map[61] = 0;
	g_fib32m_trig_map[50] = 0;
	g_fib32m_trig_map[54] = 0;
	g_fib32m_trig_map[58] = 0;
	g_fib32m_trig_map[62] = 0;
	g_fib32m_trig_map[51] = 0;
	g_fib32m_trig_map[55] = 0;
	g_fib32m_trig_map[59] = 0;
	g_fib32m_trig_map[63] = 0;
	g_fib32m_trig_map[64] = 0;
	g_fib32m_trig_map[68] = 0;
	g_fib32m_trig_map[72] = 0;
	g_fib32m_trig_map[76] = 0;
	g_fib32m_trig_map[65] = 0;
	g_fib32m_trig_map[69] = 0;
	g_fib32m_trig_map[73] = 0;
	g_fib32m_trig_map[77] = 0;
	g_fib32m_trig_map[66] = 0;
	g_fib32m_trig_map[70] = 0;
	g_fib32m_trig_map[74] = 0;
	g_fib32m_trig_map[78] = 0;
	g_fib32m_trig_map[67] = 0;
	g_fib32m_trig_map[71] = 0;
	g_fib32m_trig_map[75] = 0;
	g_fib32m_trig_map[79] = 0;
	g_fib32m_trig_map[80] = 0;
	g_fib32m_trig_map[84] = 0;
	g_fib32m_trig_map[88] = 0;
	g_fib32m_trig_map[92] = 0;
	g_fib32m_trig_map[81] = 0;
	g_fib32m_trig_map[85] = 0;
	g_fib32m_trig_map[89] = 0;
	g_fib32m_trig_map[93] = 0;
	g_fib32m_trig_map[82] = 0;
	g_fib32m_trig_map[86] = 0;
	g_fib32m_trig_map[90] = 0;
	g_fib32m_trig_map[94] = 0;
	g_fib32m_trig_map[83] = 0;
	g_fib32m_trig_map[87] = 0;
	g_fib32m_trig_map[91] = 0;
	g_fib32m_trig_map[95] = 0;
	g_fib32m_trig_map[96] = 0;
	g_fib32m_trig_map[100] = 0;
	g_fib32m_trig_map[104] = 0;
	g_fib32m_trig_map[108] = 0;
	g_fib32m_trig_map[97] = 0;
	g_fib32m_trig_map[101] = 0;
	g_fib32m_trig_map[105] = 0;
	g_fib32m_trig_map[109] = 0;
	g_fib32m_trig_map[98] = 0;
	g_fib32m_trig_map[102] = 0;
	g_fib32m_trig_map[106] = 0;
	g_fib32m_trig_map[110] = 0;
	g_fib32m_trig_map[99] = 0;
	g_fib32m_trig_map[103] = 0;
	g_fib32m_trig_map[107] = 0;
	g_fib32m_trig_map[111] = 0;
	g_fib32m_trig_map[112] = 0;
	g_fib32m_trig_map[116] = 0;
	g_fib32m_trig_map[120] = 0;
	g_fib32m_trig_map[124] = 0;
	g_fib32m_trig_map[113] = 0;
	g_fib32m_trig_map[117] = 0;
	g_fib32m_trig_map[121] = 0;
	g_fib32m_trig_map[125] = 0;
	g_fib32m_trig_map[114] = 0;
	g_fib32m_trig_map[118] = 0;
	g_fib32m_trig_map[122] = 0;
	g_fib32m_trig_map[126] = 0;
	g_fib32m_trig_map[115] = 0;
	g_fib32m_trig_map[119] = 0;
	g_fib32m_trig_map[123] = 0;
	g_fib32m_trig_map[127] = 0;
	g_fib32m_trig_map[128] = 1;
	g_fib32m_trig_map[132] = 1;
	g_fib32m_trig_map[136] = 1;
	g_fib32m_trig_map[140] = 1;
	g_fib32m_trig_map[129] = 1;
	g_fib32m_trig_map[133] = 1;
	g_fib32m_trig_map[137] = 1;
	g_fib32m_trig_map[141] = 1;
	g_fib32m_trig_map[130] = 1;
	g_fib32m_trig_map[134] = 1;
	g_fib32m_trig_map[138] = 1;
	g_fib32m_trig_map[142] = 1;
	g_fib32m_trig_map[131] = 1;
	g_fib32m_trig_map[135] = 1;
	g_fib32m_trig_map[139] = 1;
	g_fib32m_trig_map[143] = 1;
	g_fib32m_trig_map[144] = 1;
	g_fib32m_trig_map[148] = 1;
	g_fib32m_trig_map[152] = 1;
	g_fib32m_trig_map[156] = 1;
	g_fib32m_trig_map[145] = 1;
	g_fib32m_trig_map[149] = 1;
	g_fib32m_trig_map[153] = 1;
	g_fib32m_trig_map[157] = 1;
	g_fib32m_trig_map[146] = 1;
	g_fib32m_trig_map[150] = 1;
	g_fib32m_trig_map[154] = 1;
	g_fib32m_trig_map[158] = 1;
	g_fib32m_trig_map[147] = 1;
	g_fib32m_trig_map[151] = 1;
	g_fib32m_trig_map[155] = 1;
	g_fib32m_trig_map[159] = 1;
	g_fib32m_trig_map[160] = 1;
	g_fib32m_trig_map[164] = 1;
	g_fib32m_trig_map[168] = 1;
	g_fib32m_trig_map[172] = 1;
	g_fib32m_trig_map[161] = 1;
	g_fib32m_trig_map[165] = 1;
	g_fib32m_trig_map[169] = 1;
	g_fib32m_trig_map[173] = 1;
	g_fib32m_trig_map[162] = 1;
	g_fib32m_trig_map[166] = 1;
	g_fib32m_trig_map[170] = 1;
	g_fib32m_trig_map[174] = 1;
	g_fib32m_trig_map[163] = 1;
	g_fib32m_trig_map[167] = 1;
	g_fib32m_trig_map[171] = 1;
	g_fib32m_trig_map[175] = 1;
	g_fib32m_trig_map[176] = 1;
	g_fib32m_trig_map[180] = 1;
	g_fib32m_trig_map[184] = 1;
	g_fib32m_trig_map[188] = 1;
	g_fib32m_trig_map[177] = 1;
	g_fib32m_trig_map[181] = 1;
	g_fib32m_trig_map[185] = 1;
	g_fib32m_trig_map[189] = 1;
	g_fib32m_trig_map[178] = 1;
	g_fib32m_trig_map[182] = 1;
	g_fib32m_trig_map[186] = 1;
	g_fib32m_trig_map[190] = 1;
	g_fib32m_trig_map[179] = 1;
	g_fib32m_trig_map[183] = 1;
	g_fib32m_trig_map[187] = 1;
	g_fib32m_trig_map[191] = 1;
	g_fib32m_trig_map[192] = 1;
	g_fib32m_trig_map[196] = 1;
	g_fib32m_trig_map[200] = 1;
	g_fib32m_trig_map[204] = 1;
	g_fib32m_trig_map[193] = 1;
	g_fib32m_trig_map[197] = 1;
	g_fib32m_trig_map[201] = 1;
	g_fib32m_trig_map[205] = 1;
	g_fib32m_trig_map[194] = 1;
	g_fib32m_trig_map[198] = 1;
	g_fib32m_trig_map[202] = 1;
	g_fib32m_trig_map[206] = 1;
	g_fib32m_trig_map[195] = 1;
	g_fib32m_trig_map[199] = 1;
	g_fib32m_trig_map[203] = 1;
	g_fib32m_trig_map[207] = 1;
	g_fib32m_trig_map[208] = 1;
	g_fib32m_trig_map[212] = 1;
	g_fib32m_trig_map[216] = 1;
	g_fib32m_trig_map[220] = 1;
	g_fib32m_trig_map[209] = 1;
	g_fib32m_trig_map[213] = 1;
	g_fib32m_trig_map[217] = 1;
	g_fib32m_trig_map[221] = 1;
	g_fib32m_trig_map[210] = 1;
	g_fib32m_trig_map[214] = 1;
	g_fib32m_trig_map[218] = 1;
	g_fib32m_trig_map[222] = 1;
	g_fib32m_trig_map[211] = 1;
	g_fib32m_trig_map[215] = 1;
	g_fib32m_trig_map[219] = 1;
	g_fib32m_trig_map[223] = 1;
	g_fib32m_trig_map[224] = 1;
	g_fib32m_trig_map[228] = 1;
	g_fib32m_trig_map[232] = 1;
	g_fib32m_trig_map[236] = 1;
	g_fib32m_trig_map[225] = 1;
	g_fib32m_trig_map[229] = 1;
	g_fib32m_trig_map[233] = 1;
	g_fib32m_trig_map[237] = 1;
	g_fib32m_trig_map[226] = 1;
	g_fib32m_trig_map[230] = 1;
	g_fib32m_trig_map[234] = 1;
	g_fib32m_trig_map[238] = 1;
	g_fib32m_trig_map[227] = 1;
	g_fib32m_trig_map[231] = 1;
	g_fib32m_trig_map[235] = 1;
	g_fib32m_trig_map[239] = 1;
	g_fib32m_trig_map[240] = 1;
	g_fib32m_trig_map[244] = 1;
	g_fib32m_trig_map[248] = 1;
	g_fib32m_trig_map[252] = 1;
	g_fib32m_trig_map[241] = 1;
	g_fib32m_trig_map[245] = 1;
	g_fib32m_trig_map[249] = 1;
	g_fib32m_trig_map[253] = 1;
	g_fib32m_trig_map[242] = 1;
	g_fib32m_trig_map[246] = 1;
	g_fib32m_trig_map[250] = 1;
	g_fib32m_trig_map[254] = 1;
	g_fib32m_trig_map[243] = 1;
	g_fib32m_trig_map[247] = 1;
	g_fib32m_trig_map[251] = 1;
	g_fib32m_trig_map[255] = 1;
	g_fib32m_trig_map[256] = 2;
	g_fib32m_trig_map[260] = 2;
	g_fib32m_trig_map[264] = 2;
	g_fib32m_trig_map[268] = 2;
	g_fib32m_trig_map[257] = 2;
	g_fib32m_trig_map[261] = 2;
	g_fib32m_trig_map[265] = 2;
	g_fib32m_trig_map[269] = 2;
	g_fib32m_trig_map[258] = 2;
	g_fib32m_trig_map[262] = 2;
	g_fib32m_trig_map[266] = 2;
	g_fib32m_trig_map[270] = 2;
	g_fib32m_trig_map[259] = 2;
	g_fib32m_trig_map[263] = 2;
	g_fib32m_trig_map[267] = 2;
	g_fib32m_trig_map[271] = 2;
	g_fib32m_trig_map[272] = 2;
	g_fib32m_trig_map[276] = 2;
	g_fib32m_trig_map[280] = 2;
	g_fib32m_trig_map[284] = 2;
	g_fib32m_trig_map[273] = 2;
	g_fib32m_trig_map[277] = 2;
	g_fib32m_trig_map[281] = 2;
	g_fib32m_trig_map[285] = 2;
	g_fib32m_trig_map[274] = 2;
	g_fib32m_trig_map[278] = 2;
	g_fib32m_trig_map[282] = 2;
	g_fib32m_trig_map[286] = 2;
	g_fib32m_trig_map[275] = 2;
	g_fib32m_trig_map[279] = 2;
	g_fib32m_trig_map[283] = 2;
	g_fib32m_trig_map[287] = 2;
	g_fib32m_trig_map[288] = 2;
	g_fib32m_trig_map[292] = 2;
	g_fib32m_trig_map[296] = 2;
	g_fib32m_trig_map[300] = 2;
	g_fib32m_trig_map[289] = 2;
	g_fib32m_trig_map[293] = 2;
	g_fib32m_trig_map[297] = 2;
	g_fib32m_trig_map[301] = 2;
	g_fib32m_trig_map[290] = 2;
	g_fib32m_trig_map[294] = 2;
	g_fib32m_trig_map[298] = 2;
	g_fib32m_trig_map[302] = 2;
	g_fib32m_trig_map[291] = 2;
	g_fib32m_trig_map[295] = 2;
	g_fib32m_trig_map[299] = 2;
	g_fib32m_trig_map[303] = 2;
	g_fib32m_trig_map[304] = 2;
	g_fib32m_trig_map[308] = 2;
	g_fib32m_trig_map[312] = 2;
	g_fib32m_trig_map[316] = 2;
	g_fib32m_trig_map[305] = 2;
	g_fib32m_trig_map[309] = 2;
	g_fib32m_trig_map[313] = 2;
	g_fib32m_trig_map[317] = 2;
	g_fib32m_trig_map[306] = 2;
	g_fib32m_trig_map[310] = 2;
	g_fib32m_trig_map[314] = 2;
	g_fib32m_trig_map[318] = 2;
	g_fib32m_trig_map[307] = 2;
	g_fib32m_trig_map[311] = 2;
	g_fib32m_trig_map[315] = 2;
	g_fib32m_trig_map[319] = 2;
	g_fib32m_trig_map[320] = 2;
	g_fib32m_trig_map[324] = 2;
	g_fib32m_trig_map[328] = 2;
	g_fib32m_trig_map[332] = 2;
	g_fib32m_trig_map[321] = 2;
	g_fib32m_trig_map[325] = 2;
	g_fib32m_trig_map[329] = 2;
	g_fib32m_trig_map[333] = 2;
	g_fib32m_trig_map[322] = 2;
	g_fib32m_trig_map[326] = 2;
	g_fib32m_trig_map[330] = 2;
	g_fib32m_trig_map[334] = 2;
	g_fib32m_trig_map[323] = 2;
	g_fib32m_trig_map[327] = 2;
	g_fib32m_trig_map[331] = 2;
	g_fib32m_trig_map[335] = 2;
	g_fib32m_trig_map[336] = 2;
	g_fib32m_trig_map[340] = 2;
	g_fib32m_trig_map[344] = 2;
	g_fib32m_trig_map[348] = 2;
	g_fib32m_trig_map[337] = 2;
	g_fib32m_trig_map[341] = 2;
	g_fib32m_trig_map[345] = 2;
	g_fib32m_trig_map[349] = 2;
	g_fib32m_trig_map[338] = 2;
	g_fib32m_trig_map[342] = 2;
	g_fib32m_trig_map[346] = 2;
	g_fib32m_trig_map[350] = 2;
	g_fib32m_trig_map[339] = 2;
	g_fib32m_trig_map[343] = 2;
	g_fib32m_trig_map[347] = 2;
	g_fib32m_trig_map[351] = 2;
	g_fib32m_trig_map[352] = 2;
	g_fib32m_trig_map[356] = 2;
	g_fib32m_trig_map[360] = 2;
	g_fib32m_trig_map[364] = 2;
	g_fib32m_trig_map[353] = 2;
	g_fib32m_trig_map[357] = 2;
	g_fib32m_trig_map[361] = 2;
	g_fib32m_trig_map[365] = 2;
	g_fib32m_trig_map[354] = 2;
	g_fib32m_trig_map[358] = 2;
	g_fib32m_trig_map[362] = 2;
	g_fib32m_trig_map[366] = 2;
	g_fib32m_trig_map[355] = 2;
	g_fib32m_trig_map[359] = 2;
	g_fib32m_trig_map[363] = 2;
	g_fib32m_trig_map[367] = 2;
	g_fib32m_trig_map[368] = 2;
	g_fib32m_trig_map[372] = 2;
	g_fib32m_trig_map[376] = 2;
	g_fib32m_trig_map[380] = 2;
	g_fib32m_trig_map[369] = 2;
	g_fib32m_trig_map[373] = 2;
	g_fib32m_trig_map[377] = 2;
	g_fib32m_trig_map[381] = 2;
	g_fib32m_trig_map[370] = 2;
	g_fib32m_trig_map[374] = 2;
	g_fib32m_trig_map[378] = 2;
	g_fib32m_trig_map[382] = 2;
	g_fib32m_trig_map[371] = 2;
	g_fib32m_trig_map[375] = 2;
	g_fib32m_trig_map[379] = 2;
	g_fib32m_trig_map[383] = 2;
	g_fib32m_trig_map[384] = 3;
	g_fib32m_trig_map[388] = 3;
	g_fib32m_trig_map[392] = 3;
	g_fib32m_trig_map[396] = 3;
	g_fib32m_trig_map[385] = 3;
	g_fib32m_trig_map[389] = 3;
	g_fib32m_trig_map[393] = 3;
	g_fib32m_trig_map[397] = 3;
	g_fib32m_trig_map[386] = 3;
	g_fib32m_trig_map[390] = 3;
	g_fib32m_trig_map[394] = 3;
	g_fib32m_trig_map[398] = 3;
	g_fib32m_trig_map[387] = 3;
	g_fib32m_trig_map[391] = 3;
	g_fib32m_trig_map[395] = 3;
	g_fib32m_trig_map[399] = 3;
	g_fib32m_trig_map[400] = 3;
	g_fib32m_trig_map[404] = 3;
	g_fib32m_trig_map[408] = 3;
	g_fib32m_trig_map[412] = 3;
	g_fib32m_trig_map[401] = 3;
	g_fib32m_trig_map[405] = 3;
	g_fib32m_trig_map[409] = 3;
	g_fib32m_trig_map[413] = 3;
	g_fib32m_trig_map[402] = 3;
	g_fib32m_trig_map[406] = 3;
	g_fib32m_trig_map[410] = 3;
	g_fib32m_trig_map[414] = 3;
	g_fib32m_trig_map[403] = 3;
	g_fib32m_trig_map[407] = 3;
	g_fib32m_trig_map[411] = 3;
	g_fib32m_trig_map[415] = 3;
	g_fib32m_trig_map[416] = 3;
	g_fib32m_trig_map[420] = 3;
	g_fib32m_trig_map[424] = 3;
	g_fib32m_trig_map[428] = 3;
	g_fib32m_trig_map[417] = 3;
	g_fib32m_trig_map[421] = 3;
	g_fib32m_trig_map[425] = 3;
	g_fib32m_trig_map[429] = 3;
	g_fib32m_trig_map[418] = 3;
	g_fib32m_trig_map[422] = 3;
	g_fib32m_trig_map[426] = 3;
	g_fib32m_trig_map[430] = 3;
	g_fib32m_trig_map[419] = 3;
	g_fib32m_trig_map[423] = 3;
	g_fib32m_trig_map[427] = 3;
	g_fib32m_trig_map[431] = 3;
	g_fib32m_trig_map[432] = 3;
	g_fib32m_trig_map[436] = 3;
	g_fib32m_trig_map[440] = 3;
	g_fib32m_trig_map[444] = 3;
	g_fib32m_trig_map[433] = 3;
	g_fib32m_trig_map[437] = 3;
	g_fib32m_trig_map[441] = 3;
	g_fib32m_trig_map[445] = 3;
	g_fib32m_trig_map[434] = 3;
	g_fib32m_trig_map[438] = 3;
	g_fib32m_trig_map[442] = 3;
	g_fib32m_trig_map[446] = 3;
	g_fib32m_trig_map[435] = 3;
	g_fib32m_trig_map[439] = 3;
	g_fib32m_trig_map[443] = 3;
	g_fib32m_trig_map[447] = 3;
	g_fib32m_trig_map[448] = 3;
	g_fib32m_trig_map[452] = 3;
	g_fib32m_trig_map[456] = 3;
	g_fib32m_trig_map[460] = 3;
	g_fib32m_trig_map[449] = 3;
	g_fib32m_trig_map[453] = 3;
	g_fib32m_trig_map[457] = 3;
	g_fib32m_trig_map[461] = 3;
	g_fib32m_trig_map[450] = 3;
	g_fib32m_trig_map[454] = 3;
	g_fib32m_trig_map[458] = 3;
	g_fib32m_trig_map[462] = 3;
	g_fib32m_trig_map[451] = 3;
	g_fib32m_trig_map[455] = 3;
	g_fib32m_trig_map[459] = 3;
	g_fib32m_trig_map[463] = 3;
	g_fib32m_trig_map[464] = 3;
	g_fib32m_trig_map[468] = 3;
	g_fib32m_trig_map[472] = 3;
	g_fib32m_trig_map[476] = 3;
	g_fib32m_trig_map[465] = 3;
	g_fib32m_trig_map[469] = 3;
	g_fib32m_trig_map[473] = 3;
	g_fib32m_trig_map[477] = 3;
	g_fib32m_trig_map[466] = 3;
	g_fib32m_trig_map[470] = 3;
	g_fib32m_trig_map[474] = 3;
	g_fib32m_trig_map[478] = 3;
	g_fib32m_trig_map[467] = 3;
	g_fib32m_trig_map[471] = 3;
	g_fib32m_trig_map[475] = 3;
	g_fib32m_trig_map[479] = 3;
	g_fib32m_trig_map[480] = 3;
	g_fib32m_trig_map[484] = 3;
	g_fib32m_trig_map[488] = 3;
	g_fib32m_trig_map[492] = 3;
	g_fib32m_trig_map[481] = 3;
	g_fib32m_trig_map[485] = 3;
	g_fib32m_trig_map[489] = 3;
	g_fib32m_trig_map[493] = 3;
	g_fib32m_trig_map[482] = 3;
	g_fib32m_trig_map[486] = 3;
	g_fib32m_trig_map[490] = 3;
	g_fib32m_trig_map[494] = 3;
	g_fib32m_trig_map[483] = 3;
	g_fib32m_trig_map[487] = 3;
	g_fib32m_trig_map[491] = 3;
	g_fib32m_trig_map[495] = 3;
	g_fib32m_trig_map[496] = 3;
	g_fib32m_trig_map[500] = 3;
	g_fib32m_trig_map[504] = 3;
	g_fib32m_trig_map[508] = 3;
	g_fib32m_trig_map[497] = 3;
	g_fib32m_trig_map[501] = 3;
	g_fib32m_trig_map[505] = 3;
	g_fib32m_trig_map[509] = 3;
	g_fib32m_trig_map[498] = 3;
	g_fib32m_trig_map[502] = 3;
	g_fib32m_trig_map[506] = 3;
	g_fib32m_trig_map[510] = 3;
	g_fib32m_trig_map[499] = 3;
	g_fib32m_trig_map[503] = 3;
	g_fib32m_trig_map[507] = 3;
	g_fib32m_trig_map[511] = 3;
	g_fib32s_trig_map[0] = 2;
	g_fib32s_trig_map[1] = 2;
}
