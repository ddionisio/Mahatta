xof 0302txt 0032
Header {
 1;
 0;
 1;
}
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template FrameTransformMatrix {
 <F6F23F41-7686-11cf-8F52-0040333594A3>
 Matrix4x4 frameMatrix;
}

template Frame {
 <3D82AB46-62DA-11cf-AB39-0020AF71E433>
 [...]
}
template FloatKeys {
 <10DD46A9-775B-11cf-8F52-0040333594A3>
 DWORD nValues;
 array FLOAT values[nValues];
}

template TimedFloatKeys {
 <F406B180-7B3B-11cf-8F52-0040333594A3>
 DWORD time;
 FloatKeys tfkeys;
}

template AnimationKey {
 <10DD46A8-775B-11cf-8F52-0040333594A3>
 DWORD keyType;
 DWORD nKeys;
 array TimedFloatKeys keys[nKeys];
}

template AnimationOptions {
 <E2BF56C0-840F-11cf-8F52-0040333594A3>
 DWORD openclosed;
 DWORD positionquality;
}

template Animation {
 <3D82AB4F-62DA-11cf-AB39-0020AF71E433>
 [...]
}

template AnimationSet {
 <3D82AB50-62DA-11cf-AB39-0020AF71E433>
 [Animation]
}

template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}
Frame Plane01 {
   FrameTransformMatrix {
1.069767,-0.000000,-0.000000,0.000000,
0.000000,0.010467,1.128464,0.000000,
-0.000000,-1.649173,0.015297,0.000000,
0.000373,-0.465736,56.454315,1.000000;;
 }
Mesh Plane011 {
 244;
-69.677994;-51.971695;2.347697;,
-69.767937;-52.520702;2.379179;,
-65.065369;-49.973530;-10.122677;,
-64.931686;-52.314102;-10.369757;,
-58.044884;-47.554806;-21.514093;,
-57.875160;-52.529800;-21.968912;,
-48.957550;-45.366421;-31.390171;,
-48.760788;-52.626583;-32.033905;,
-38.128395;-43.504009;-39.400753;,
-37.914555;-52.618549;-40.203827;,
-25.944815;-42.051003;-45.262455;,
-25.724464;-52.523037;-46.185699;,
-12.842736;-41.073349;-48.768299;,
-12.626679;-52.357628;-49.765015;,
0.708940;-40.615112;-49.794651;,
0.910064;-52.137497;-50.813431;,
14.225114;-40.695694;-48.305397;,
14.401196;-51.873543;-49.293331;,
27.221840;-41.308617;-44.353100;,
27.363670;-51.571590;-45.259232;,
39.233662;-42.422363;-38.077267;,
39.333260;-51.232685;-38.855774;,
49.830303;-43.982788;-29.699648;,
49.881180;-50.854420;-30.312565;,
58.632122;-45.917095;-19.516729;,
58.629536;-50.433151;-19.935965;,
65.323692;-48.138855;-7.889686;,
65.264816;-49.966480;-8.098041;,
69.665207;-50.553516;4.768092;,
69.549217;-49.455902;4.776812;,
-69.857697;-53.078915;2.280927;,
-65.196800;-47.761833;-9.356265;,
-58.210407;-42.820332;-19.948463;,
-49.148392;-38.445599;-29.122780;,
-38.334892;-34.810356;-36.558022;,
-26.156761;-32.060360;-41.995125;,
-13.049752;-30.307005;-45.245407;,
0.517047;-29.621506;-46.196381;,
14.057991;-30.031164;-44.815315;,
27.088253;-31.518000;-41.150101;,
39.141190;-34.019863;-35.327839;,
49.785069;-37.433914;-27.550859;,
58.638569;-41.622177;-18.090403;,
65.384460;-46.418682;-7.277949;,
69.780960;-51.637695;4.505610;,
-69.947853;-53.615173;2.050679;,
-65.324020;-45.791889;-8.115810;,
-58.367470;-38.570423;-17.357103;,
-49.327068;-32.221848;-25.351780;,
-38.526188;-26.987518;-31.824352;,
-26.351273;-23.068476;-36.553596;,
-13.237992;-20.616306;-39.378967;,
0.344324;-19.726171;-40.205040;,
13.909469;-20.432337;-39.004108;,
26.971756;-22.706636;-35.816929;,
39.063419;-26.459866;-30.751553;,
49.751369;-31.546082;-23.980343;,
58.652733;-37.769428;-15.735126;,
65.448601;-44.893093;-6.300410;,
69.895470;-52.649754;3.995262;,
-70.038933;-54.097874;1.693719;,
-65.445274;-44.162796;-6.470608;,
-58.512276;-35.023144;-13.877843;,
-49.487968;-27.016212;-20.274843;,
-38.695221;-20.439981;-25.446268;,
-26.520226;-15.540830;-29.220331;,
-13.398697;-12.503193;-31.473021;,
0.199718;-11.441716;-32.130970;,
13.788226;-12.396317;-31.172993;,
26.880316;-15.330516;-28.630543;,
39.007175;-20.133038;-24.586952;,
49.734493;-26.622673;-19.175602;,
58.678059;-34.556046;-12.576752;,
65.517471;-43.637859;-5.013040;,
70.007706;-53.533085;3.256552;,
-70.131485;-54.496761;1.222963;,
-65.559174;-42.954967;-4.510108;,
-58.641659;-32.359402;-9.693913;,
-49.626423;-23.096516;-14.156919;,
-38.836090;-15.505890;-17.755274;,
-26.656822;-9.866938;-20.375675;,
-13.524530;-6.387847;-21.937218;,
0.090722;-5.197287;-22.392437;,
13.701526;-6.339215;-21.727795;,
26.820583;-9.771145;-19.963774;,
38.978153;-15.365862;-17.154615;,
49.738876;-22.916595;-13.387658;,
58.717388;-32.145290;-8.782104;,
65.592125;-42.713623;-3.486991;,
70.116821;-54.236252;2.321897;,
-70.225960;-54.784801;0.658565;,
-65.664734;-42.226131;-2.339070;,
-58.753269;-30.713749;-5.024318;,
-49.738930;-20.663580;-7.316394;,
-38.944359;-12.439596;-9.150599;,
-26.755943;-6.340183;-10.478169;,
-13.609963;-2.586951;-11.265616;,
0.022980;-1.316369;-11.493925;,
13.654840;-2.574604;-11.157917;,
26.797579;-6.315791;-10.266022;,
38.980644;-12.403773;-8.840487;,
49.767818;-20.617262;-6.917947;,
58.772854;-30.658207;-4.550183;,
65.673294;-42.162987;-1.804687;,
70.222031;-54.716007;1.235298;,
-70.322769;-54.939945;0.027105;,
-65.761360;-42.008614;-0.071951;,
-58.845688;-30.167568;-0.112363;,
-49.823322;-19.840931;-0.108533;,
-39.017277;-11.398532;-0.078501;,
-26.814404;-5.142665;-0.040750;,
-13.651565;-1.297272;-0.011083;,
0.000003;0.000000;0.000003;,
13.651571;-1.297272;-0.010977;,
26.814411;-5.142665;-0.039924;,
39.017288;-11.398533;-0.075836;,
49.823338;-19.840918;-0.102610;,
58.845703;-30.167543;-0.101725;,
65.761375;-42.008556;-0.055396;,
70.322762;-54.939835;0.050258;,
-70.422157;-54.946568;-0.639662;,
-65.848961;-42.307930;2.173104;,
-58.918488;-30.745062;4.787018;,
-49.878918;-20.668531;7.092974;,
-39.053925;-12.435148;8.990898;,
-26.831133;-6.335779;10.395830;,
-13.648168;-2.585486;11.243358;,
0.022980;-1.316372;11.493929;,
0.022980;-1.316372;11.493929;,
13.692868;-2.573153;11.135562;,
26.872124;-6.311694;10.184703;,
39.088966;-12.400434;8.685119;,
49.906090;-20.624765;6.704971;,
58.936310;-30.694181;4.332313;,
65.856407;-42.252106;1.669523;,
70.418701;-54.888054;-1.172837;,
-70.524284;-54.796753;-1.306392;,
-65.927910;-43.102833;4.280429;,
-58.972279;-32.412243;9.420498;,
-49.906502;-23.100872;13.915387;,
-39.055279;-15.494272;17.587965;,
-26.807228;-9.857087;20.291008;,
-13.600942;-6.384725;21.914717;,
0.090722;-5.197290;22.392443;,
0.022980;-1.316372;11.493929;,
0.090722;-5.197290;22.392443;,
13.777572;-6.335916;21.704309;,
0.022980;-1.316372;11.493929;,
26.969648;-9.761486;19.878292;,
39.194744;-15.355810;16.988745;,
50.015312;-22.925175;13.152484;,
59.044113;-32.206326;8.522738;,
65.958054;-42.874763;3.281387;,
70.509720;-54.555237;-2.369793;,
-70.629135;-54.491058;-1.936033;,
-65.999031;-44.346684;6.142805;,
-59.008614;-35.078865;13.549446;,
-49.908314;-27.009449;20.006214;,
-39.024113;-20.416025;25.267878;,
-26.745880;-15.523582;29.132431;,
-13.513322;-12.498054;31.450243;,
0.199719;-11.441719;32.130974;,
0.090722;-5.197290;22.392443;,
0.199719;-11.441719;32.130974;,
13.902280;-12.390413;31.147858;,
0.090722;-5.197290;22.392443;,
27.103851;-15.312549;28.538967;,
39.331917;-20.109884;24.405479;,
50.148880;-26.620117;18.906942;,
59.167683;-34.621346;12.253970;,
66.065659;-43.838352;4.698737;,
70.596008;-53.950901;-3.475977;,
-70.736526;-54.038876;-2.491872;,
-66.063507;-45.970177;7.666891;,
-59.029984;-38.603344;16.962233;,
-49.887932;-32.189556;25.051407;,
-38.964912;-26.944216;31.633629;,
-26.652218;-23.041199;36.462448;,
-13.390841;-20.608679;39.356056;,
0.344324;-19.726175;40.205044;,
0.199719;-11.441719;32.130974;,
0.344324;-19.726175;40.205044;,
14.061513;-20.422810;38.977119;,
0.199719;-11.441719;32.130974;,
27.269697;-22.676607;35.718521;,
39.496162;-26.414875;30.552061;,
50.303402;-31.515627;23.671860;,
59.304764;-37.810276;15.335133;,
66.178246;-45.087620;5.851475;,
70.677971;-53.099434;-4.429798;,
-70.846184;-53.458279;-2.939629;,
-66.122925;-47.885174;8.777919;,
-59.039608;-42.800251;19.485062;,
-49.850075;-38.371029;28.791573;,
-38.883595;-34.739643;36.356575;,
-26.533049;-32.020069;41.901833;,
-13.240833;-30.296362;45.222729;,
0.517047;-29.621510;46.196388;,
0.344324;-19.726175;40.205044;,
0.517047;-29.621510;46.196388;,
14.248001;-30.016838;44.786686;,
0.344324;-19.726175;40.205044;,
27.460522;-31.471601;41.045624;,
39.681755;-33.942917;35.111336;,
50.474419;-37.355965;27.202530;,
59.452408;-41.604717;17.609818;,
66.294609;-46.553436;6.684296;,
70.756302;-52.039165;-5.176131;,
-70.957603;-52.775372;-3.249567;,
-66.179024;-49.989498;9.423340;,
-59.041328;-47.450085;20.990818;,
-49.800346;-45.232300;31.035288;,
-38.787209;-43.397743;39.193367;,
-26.396502;-41.994759;45.169395;,
-13.072057;-41.059193;48.746471;,
0.708941;-40.615120;49.794651;,
0.517047;-29.621510;46.196388;,
0.708941;-40.615120;49.794651;,
14.453073;-40.675369;48.275791;,
0.517047;-29.621510;46.196388;,
27.668362;-41.241470;44.245003;,
39.881870;-42.303097;37.848606;,
50.656612;-43.836983;29.318607;,
59.607151;-45.805717;18.964025;,
66.413376;-48.156528;7.159338;,
70.831871;-50.820679;-5.669454;,
-71.070251;-52.023148;-3.398310;,
-66.233742;-52.172356;9.575293;,
-59.039371;-52.310726;21.405478;,
-49.744972;-52.417160;31.668924;,
-38.683609;-52.469582;39.998528;,
-26.251591;-52.448395;46.096493;,
-12.894247;-52.339603;49.744904;,
0.910064;-52.137501;50.813431;,
0.708941;-40.615120;49.794651;,
0.910064;-52.137501;50.813431;,
14.667088;-51.846180;49.263901;,
0.708941;-40.615120;49.794651;,
27.884382;-51.479836;45.151699;,
40.088947;-51.061745;38.623718;,
50.844116;-50.621979;29.913301;,
59.765167;-50.194172;19.331856;,
66.533051;-49.811813;7.257875;,
70.905724;-49.504429;-5.876571;;

 392;
3;2,1,0;,
3;0,3,2;,
3;4,2,3;,
3;3,5,4;,
3;6,4,5;,
3;5,7,6;,
3;8,6,7;,
3;7,9,8;,
3;10,8,9;,
3;9,11,10;,
3;12,10,11;,
3;11,13,12;,
3;14,12,13;,
3;13,15,14;,
3;16,14,15;,
3;15,17,16;,
3;18,16,17;,
3;17,19,18;,
3;20,18,19;,
3;19,21,20;,
3;22,20,21;,
3;21,23,22;,
3;24,22,23;,
3;23,25,24;,
3;26,24,25;,
3;25,27,26;,
3;28,26,27;,
3;27,29,28;,
3;31,30,1;,
3;1,2,31;,
3;32,31,2;,
3;2,4,32;,
3;33,32,4;,
3;4,6,33;,
3;34,33,6;,
3;6,8,34;,
3;35,34,8;,
3;8,10,35;,
3;36,35,10;,
3;10,12,36;,
3;37,36,12;,
3;12,14,37;,
3;38,37,14;,
3;14,16,38;,
3;39,38,16;,
3;16,18,39;,
3;40,39,18;,
3;18,20,40;,
3;41,40,20;,
3;20,22,41;,
3;42,41,22;,
3;22,24,42;,
3;43,42,24;,
3;24,26,43;,
3;44,43,26;,
3;26,28,44;,
3;46,45,30;,
3;30,31,46;,
3;47,46,31;,
3;31,32,47;,
3;48,47,32;,
3;32,33,48;,
3;49,48,33;,
3;33,34,49;,
3;50,49,34;,
3;34,35,50;,
3;51,50,35;,
3;35,36,51;,
3;52,51,36;,
3;36,37,52;,
3;53,52,37;,
3;37,38,53;,
3;54,53,38;,
3;38,39,54;,
3;55,54,39;,
3;39,40,55;,
3;56,55,40;,
3;40,41,56;,
3;57,56,41;,
3;41,42,57;,
3;58,57,42;,
3;42,43,58;,
3;59,58,43;,
3;43,44,59;,
3;61,60,45;,
3;45,46,61;,
3;62,61,46;,
3;46,47,62;,
3;63,62,47;,
3;47,48,63;,
3;64,63,48;,
3;48,49,64;,
3;65,64,49;,
3;49,50,65;,
3;66,65,50;,
3;50,51,66;,
3;67,66,51;,
3;51,52,67;,
3;68,67,52;,
3;52,53,68;,
3;69,68,53;,
3;53,54,69;,
3;70,69,54;,
3;54,55,70;,
3;71,70,55;,
3;55,56,71;,
3;72,71,56;,
3;56,57,72;,
3;73,72,57;,
3;57,58,73;,
3;74,73,58;,
3;58,59,74;,
3;76,75,60;,
3;60,61,76;,
3;77,76,61;,
3;61,62,77;,
3;78,77,62;,
3;62,63,78;,
3;79,78,63;,
3;63,64,79;,
3;80,79,64;,
3;64,65,80;,
3;81,80,65;,
3;65,66,81;,
3;82,81,66;,
3;66,67,82;,
3;83,82,67;,
3;67,68,83;,
3;84,83,68;,
3;68,69,84;,
3;85,84,69;,
3;69,70,85;,
3;86,85,70;,
3;70,71,86;,
3;87,86,71;,
3;71,72,87;,
3;88,87,72;,
3;72,73,88;,
3;89,88,73;,
3;73,74,89;,
3;91,90,75;,
3;75,76,91;,
3;92,91,76;,
3;76,77,92;,
3;93,92,77;,
3;77,78,93;,
3;94,93,78;,
3;78,79,94;,
3;95,94,79;,
3;79,80,95;,
3;96,95,80;,
3;80,81,96;,
3;97,96,81;,
3;81,82,97;,
3;98,97,82;,
3;82,83,98;,
3;99,98,83;,
3;83,84,99;,
3;100,99,84;,
3;84,85,100;,
3;101,100,85;,
3;85,86,101;,
3;102,101,86;,
3;86,87,102;,
3;103,102,87;,
3;87,88,103;,
3;104,103,88;,
3;88,89,104;,
3;106,105,90;,
3;90,91,106;,
3;107,106,91;,
3;91,92,107;,
3;108,107,92;,
3;92,93,108;,
3;109,108,93;,
3;93,94,109;,
3;110,109,94;,
3;94,95,110;,
3;111,110,95;,
3;95,96,111;,
3;112,111,96;,
3;96,97,112;,
3;113,112,97;,
3;97,98,113;,
3;114,113,98;,
3;98,99,114;,
3;115,114,99;,
3;99,100,115;,
3;116,115,100;,
3;100,101,116;,
3;117,116,101;,
3;101,102,117;,
3;118,117,102;,
3;102,103,118;,
3;119,118,103;,
3;103,104,119;,
3;121,120,105;,
3;105,106,121;,
3;122,121,106;,
3;106,107,122;,
3;123,122,107;,
3;107,108,123;,
3;124,123,108;,
3;108,109,124;,
3;125,124,109;,
3;109,110,125;,
3;126,125,110;,
3;110,111,126;,
3;127,126,111;,
3;111,112,127;,
3;129,128,112;,
3;112,113,129;,
3;130,129,113;,
3;113,114,130;,
3;131,130,114;,
3;114,115,131;,
3;132,131,115;,
3;115,116,132;,
3;133,132,116;,
3;116,117,133;,
3;134,133,117;,
3;117,118,134;,
3;135,134,118;,
3;118,119,135;,
3;137,136,120;,
3;120,121,137;,
3;138,137,121;,
3;121,122,138;,
3;139,138,122;,
3;122,123,139;,
3;140,139,123;,
3;123,124,140;,
3;141,140,124;,
3;124,125,141;,
3;142,141,125;,
3;125,126,142;,
3;143,142,126;,
3;126,127,143;,
3;146,145,144;,
3;147,129,146;,
3;148,146,129;,
3;129,130,148;,
3;149,148,130;,
3;130,131,149;,
3;150,149,131;,
3;131,132,150;,
3;151,150,132;,
3;132,133,151;,
3;152,151,133;,
3;133,134,152;,
3;153,152,134;,
3;134,135,153;,
3;155,154,136;,
3;136,137,155;,
3;156,155,137;,
3;137,138,156;,
3;157,156,138;,
3;138,139,157;,
3;158,157,139;,
3;139,140,158;,
3;159,158,140;,
3;140,141,159;,
3;160,159,141;,
3;141,142,160;,
3;161,160,142;,
3;142,143,161;,
3;164,163,162;,
3;165,146,164;,
3;166,164,146;,
3;146,148,166;,
3;167,166,148;,
3;148,149,167;,
3;168,167,149;,
3;149,150,168;,
3;169,168,150;,
3;150,151,169;,
3;170,169,151;,
3;151,152,170;,
3;171,170,152;,
3;152,153,171;,
3;173,172,154;,
3;154,155,173;,
3;174,173,155;,
3;155,156,174;,
3;175,174,156;,
3;156,157,175;,
3;176,175,157;,
3;157,158,176;,
3;177,176,158;,
3;158,159,177;,
3;178,177,159;,
3;159,160,178;,
3;179,178,160;,
3;160,161,179;,
3;182,181,180;,
3;183,164,182;,
3;184,182,164;,
3;164,166,184;,
3;185,184,166;,
3;166,167,185;,
3;186,185,167;,
3;167,168,186;,
3;187,186,168;,
3;168,169,187;,
3;188,187,169;,
3;169,170,188;,
3;189,188,170;,
3;170,171,189;,
3;191,190,172;,
3;172,173,191;,
3;192,191,173;,
3;173,174,192;,
3;193,192,174;,
3;174,175,193;,
3;194,193,175;,
3;175,176,194;,
3;195,194,176;,
3;176,177,195;,
3;196,195,177;,
3;177,178,196;,
3;197,196,178;,
3;178,179,197;,
3;200,199,198;,
3;201,182,200;,
3;202,200,182;,
3;182,184,202;,
3;203,202,184;,
3;184,185,203;,
3;204,203,185;,
3;185,186,204;,
3;205,204,186;,
3;186,187,205;,
3;206,205,187;,
3;187,188,206;,
3;207,206,188;,
3;188,189,207;,
3;209,208,190;,
3;190,191,209;,
3;210,209,191;,
3;191,192,210;,
3;211,210,192;,
3;192,193,211;,
3;212,211,193;,
3;193,194,212;,
3;213,212,194;,
3;194,195,213;,
3;214,213,195;,
3;195,196,214;,
3;215,214,196;,
3;196,197,215;,
3;218,217,216;,
3;219,200,218;,
3;220,218,200;,
3;200,202,220;,
3;221,220,202;,
3;202,203,221;,
3;222,221,203;,
3;203,204,222;,
3;223,222,204;,
3;204,205,223;,
3;224,223,205;,
3;205,206,224;,
3;225,224,206;,
3;206,207,225;,
3;227,226,208;,
3;208,209,227;,
3;228,227,209;,
3;209,210,228;,
3;229,228,210;,
3;210,211,229;,
3;230,229,211;,
3;211,212,230;,
3;231,230,212;,
3;212,213,231;,
3;232,231,213;,
3;213,214,232;,
3;233,232,214;,
3;214,215,233;,
3;236,235,234;,
3;237,218,236;,
3;238,236,218;,
3;218,220,238;,
3;239,238,220;,
3;220,221,239;,
3;240,239,221;,
3;221,222,240;,
3;241,240,222;,
3;222,223,241;,
3;242,241,223;,
3;223,224,242;,
3;243,242,224;,
3;224,225,243;;
MeshMaterialList {
 1;
 392;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
Material {
 0.752941;0.752941;0.752941;1.000000;;
2535301200456458800000000000000.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.000000;0.000000;;
TextureFilename {
"SPACE3.tga";
}
 }
}

 MeshNormals {
 244;
0.004709;0.987120;0.159911;,
-0.925462;0.270383;-0.265356;,
0.878821;-0.044372;0.475085;,
0.884670;-0.002801;0.466210;,
0.803455;-0.096183;0.587545;,
0.781132;-0.031479;0.623572;,
0.677122;-0.132406;0.723861;,
0.649630;-0.050239;0.758589;,
0.526479;-0.160370;0.834926;,
0.495239;-0.065084;0.866315;,
0.356969;-0.181356;0.916342;,
0.323547;-0.076560;0.943110;,
0.174661;-0.195021;0.965122;,
0.140612;-0.084513;0.986451;,
-0.013919;-0.200957;0.979501;,
-0.047211;-0.088720;0.994937;,
-0.202021;-0.198955;0.958960;,
-0.233435;-0.089038;0.968287;,
-0.382900;-0.189072;0.904234;,
-0.411602;-0.085426;0.907351;,
-0.550073;-0.171562;0.817304;,
-0.575465;-0.077842;0.814113;,
-0.697549;-0.146535;0.701394;,
-0.719195;-0.065771;0.691689;,
-0.820052;-0.111581;0.561306;,
-0.837735;-0.044286;0.544278;,
-0.891394;-0.031502;0.452132;,
-0.868341;0.067495;0.491354;,
0.923337;0.262429;-0.280320;,
0.942660;0.102139;-0.317742;,
-0.927559;0.317080;-0.197725;,
0.880701;-0.151001;0.448959;,
0.805723;-0.226135;0.547424;,
0.679913;-0.292321;0.672507;,
0.529697;-0.344744;0.774966;,
0.360500;-0.383672;0.850197;,
0.178381;-0.408095;0.895343;,
-0.010145;-0.417213;0.908752;,
-0.198324;-0.410704;0.889938;,
-0.379414;-0.388786;0.839578;,
-0.546920;-0.352148;0.759519;,
-0.694842;-0.301625;0.652853;,
-0.817881;-0.235921;0.524797;,
-0.889613;-0.133295;0.436830;,
0.922706;0.318008;-0.217910;,
-0.929267;0.348615;-0.122189;,
0.882427;-0.248810;0.399271;,
0.807774;-0.343653;0.478960;,
0.682430;-0.436485;0.586319;,
0.532595;-0.510786;0.674863;,
0.363679;-0.565793;0.740011;,
0.181728;-0.599874;0.779183;,
-0.006747;-0.611873;0.790927;,
-0.194997;-0.601363;0.774815;,
-0.376273;-0.568708;0.731430;,
-0.544078;-0.514993;0.662391;,
-0.692396;-0.441725;0.570497;,
-0.815910;-0.348788;0.461126;,
-0.887938;-0.228996;0.398907;,
0.922336;0.358211;-0.144852;,
-0.930584;0.363657;-0.042030;,
0.883908;-0.332709;0.328652;,
0.809502;-0.442655;0.385698;,
0.684541;-0.557429;0.469762;,
0.535023;-0.649896;0.539801;,
0.366341;-0.718285;0.591490;,
0.184530;-0.760423;0.622660;,
-0.003902;-0.774852;0.632131;,
-0.192209;-0.761057;0.619555;,
-0.373641;-0.719519;0.585393;,
-0.541693;-0.651661;0.530950;,
-0.690339;-0.559577;0.458591;,
-0.814242;-0.444346;0.373587;,
-0.886455;-0.313628;0.340347;,
0.922250;0.381127;-0.064789;,
-0.931504;0.361608;0.039250;,
0.885067;-0.398303;0.240853;,
0.810818;-0.518013;0.272465;,
0.686137;-0.648891;0.328872;,
0.536856;-0.754868;0.376777;,
0.368347;-0.833249;0.412329;,
0.186642;-0.881426;0.433881;,
-0.001758;-0.897708;0.440588;,
-0.190107;-0.881511;0.432201;,
-0.371653;-0.833407;0.409031;,
-0.539887;-0.755072;0.372005;,
-0.688777;-0.649078;0.322932;,
-0.812961;-0.517647;0.266712;,
-0.885240;-0.382839;0.264166;,
0.922467;0.385646;0.018245;,
-0.932047;0.342573;0.118038;,
0.885848;-0.442200;0.140476;,
0.811652;-0.565826;0.145127;,
0.687136;-0.706131;0.170947;,
0.537997;-0.820263;0.194235;,
0.369595;-0.904730;0.211809;,
0.187955;-0.956614;0.222627;,
-0.000425;-0.974076;0.226222;,
-0.188798;-0.956487;0.222457;,
-0.370413;-0.904472;0.211481;,
-0.538756;-0.819870;0.193791;,
-0.687790;-0.705591;0.170545;,
-0.812136;-0.564898;0.146032;,
-0.884357;-0.433043;0.174317;,
0.923004;0.371560;0.100040;,
-0.932232;0.307447;0.190841;,
0.886208;-0.462125;0.032795;,
0.811962;-0.583619;0.010274;,
0.687486;-0.726185;0.004167;,
0.538389;-0.842695;0.001632;,
0.370019;-0.929024;0.000317;,
0.188400;-0.982092;-0.000160;,
0.000028;-1.000000;0.000137;,
-0.188351;-0.982101;0.001190;,
-0.369984;-0.929033;0.002976;,
-0.538358;-0.842698;0.005537;,
-0.687431;-0.726190;0.009325;,
-0.811808;-0.583654;0.017794;,
-0.883850;-0.461644;0.075459;,
0.923836;0.339646;0.176546;,
-0.932098;0.257826;0.254399;,
0.886132;-0.457065;-0.076559;,
0.811733;-0.570471;-0.125110;,
0.687169;-0.708015;-0.162828;,
0.538010;-0.821001;-0.191055;,
0.369598;-0.904873;-0.211192;,
0.187954;-0.956541;-0.222939;,
0.094263;-0.977605;-0.188156;,
-0.093904;-0.989137;-0.113096;,
-0.188788;-0.957025;-0.220139;,
-0.370389;-0.905818;-0.205683;,
-0.538713;-0.822374;-0.183002;,
-0.687716;-0.709808;-0.152379;,
-0.811994;-0.572943;-0.111365;,
-0.883745;-0.467171;-0.027307;,
0.924938;0.291545;0.243906;,
-0.931686;0.195966;0.305872;,
0.885624;-0.427315;-0.181858;,
0.810975;-0.527062;-0.254017;,
0.686201;-0.652562;-0.321389;,
0.536881;-0.756306;-0.373846;,
0.368352;-0.833529;-0.411761;,
0.186640;-0.881285;-0.434169;,
0.093078;-0.909709;-0.404680;,
-0.094797;-0.937998;-0.333428;,
-0.094797;-0.937998;-0.333428;,
-0.190088;-0.882560;-0.430064;,
-0.094978;-0.938056;-0.333212;,
-0.371607;-0.836029;-0.403688;,
-0.539802;-0.759950;-0.362063;,
-0.688633;-0.657292;-0.306189;,
-0.812685;-0.533321;-0.234760;,
-0.884047;-0.449342;-0.128653;,
0.926272;0.229711;0.298751;,
-0.931063;0.124736;0.342875;,
0.884711;-0.374451;-0.277619;,
0.809728;-0.455642;-0.369772;,
0.684633;-0.562697;-0.463304;,
0.535059;-0.651960;-0.537271;,
0.366348;-0.718686;-0.590999;,
0.184527;-0.760221;-0.622908;,
0.091069;-0.794618;-0.600240;,
-0.096536;-0.838368;-0.536488;,
-0.096536;-0.838368;-0.536488;,
-0.192183;-0.762561;-0.617711;,
-0.096717;-0.838474;-0.536290;,
-0.373575;-0.723281;-0.580781;,
-0.541570;-0.658660;-0.522368;,
-0.690133;-0.571364;-0.444139;,
-0.813844;-0.466839;-0.346003;,
-0.884741;-0.409083;-0.223349;,
0.927779;0.157287;0.338360;,
-0.930278;0.047341;0.363788;,
0.883443;-0.301251;-0.358854;,
0.808057;-0.359904;-0.466382;,
0.682545;-0.443077;-0.581219;,
0.532640;-0.513368;-0.672865;,
0.363688;-0.566294;-0.739623;,
0.181724;-0.599621;-0.779379;,
0.088340;-0.638295;-0.764707;,
-0.099031;-0.695410;-0.711757;,
-0.099031;-0.695410;-0.711757;,
-0.194964;-0.603245;-0.773359;,
-0.099211;-0.695558;-0.711587;,
-0.376191;-0.573415;-0.727788;,
-0.543925;-0.523751;-0.655614;,
-0.692138;-0.456473;-0.559085;,
-0.815413;-0.376938;-0.439340;,
-0.885788;-0.348481;-0.306497;,
0.929397;0.077944;0.360758;,
-0.929404;-0.032576;0.367622;,
0.881886;-0.211569;-0.421325;,
0.806048;-0.244812;-0.538845;,
0.680045;-0.299895;-0.669030;,
0.529748;-0.347711;-0.773604;,
0.360510;-0.384247;-0.849933;,
0.178375;-0.407804;-0.895476;,
0.085033;-0.448836;-0.889559;,
-0.102153;-0.516528;-0.850155;,
-0.102153;-0.516528;-0.850155;,
-0.198287;-0.412866;-0.888945;,
-0.102333;-0.516711;-0.850022;,
-0.379320;-0.394194;-0.837095;,
-0.546745;-0.362212;-0.754899;,
-0.694546;-0.318572;-0.645073;,
-0.817309;-0.268271;-0.509938;,
-0.887136;-0.270669;-0.373803;,
0.931054;-0.004303;0.364855;,
-0.928516;-0.111328;0.354210;,
0.880123;-0.110059;-0.461812;,
0.803805;-0.116318;-0.583411;,
0.677264;-0.140570;-0.722186;,
0.526534;-0.163568;-0.834270;,
0.356979;-0.181976;-0.916215;,
0.174656;-0.194708;-0.965186;,
0.081319;-0.236056;-0.968331;,
-0.105741;-0.310989;-0.944513;,
-0.105741;-0.310989;-0.944513;,
-0.201980;-0.201285;-0.958482;,
-0.105920;-0.311198;-0.944424;,
-0.382798;-0.194902;-0.903039;,
-0.549883;-0.182409;-0.815080;,
-0.697229;-0.164802;-0.697647;,
-0.819434;-0.146462;-0.554145;,
-0.888713;-0.179671;-0.421791;,
0.932683;-0.085285;0.350471;,
-0.934893;-0.071249;0.347705;,
0.843887;-0.173718;-0.507618;,
0.818945;-0.052959;-0.571424;,
0.696291;-0.058882;-0.715340;,
0.548911;-0.068267;-0.833088;,
0.382331;-0.076745;-0.920833;,
0.202497;-0.083394;-0.975726;,
0.078236;-0.086560;-0.993170;,
-0.109609;-0.089441;-0.989942;,
-0.109609;-0.089441;-0.989942;,
-0.171675;-0.089991;-0.981035;,
-0.109787;-0.089665;-0.989903;,
-0.353117;-0.089699;-0.931269;,
-0.522313;-0.087041;-0.848300;,
-0.673223;-0.082397;-0.734834;,
-0.800285;-0.077350;-0.594609;,
-0.898179;-0.084049;-0.431521;,
0.008925;-0.994338;0.105889;;

 392;
3;2,1,0;,
3;0,3,2;,
3;4,2,3;,
3;3,5,4;,
3;6,4,5;,
3;5,7,6;,
3;8,6,7;,
3;7,9,8;,
3;10,8,9;,
3;9,11,10;,
3;12,10,11;,
3;11,13,12;,
3;14,12,13;,
3;13,15,14;,
3;16,14,15;,
3;15,17,16;,
3;18,16,17;,
3;17,19,18;,
3;20,18,19;,
3;19,21,20;,
3;22,20,21;,
3;21,23,22;,
3;24,22,23;,
3;23,25,24;,
3;26,24,25;,
3;25,27,26;,
3;28,26,27;,
3;27,29,28;,
3;31,30,1;,
3;1,2,31;,
3;32,31,2;,
3;2,4,32;,
3;33,32,4;,
3;4,6,33;,
3;34,33,6;,
3;6,8,34;,
3;35,34,8;,
3;8,10,35;,
3;36,35,10;,
3;10,12,36;,
3;37,36,12;,
3;12,14,37;,
3;38,37,14;,
3;14,16,38;,
3;39,38,16;,
3;16,18,39;,
3;40,39,18;,
3;18,20,40;,
3;41,40,20;,
3;20,22,41;,
3;42,41,22;,
3;22,24,42;,
3;43,42,24;,
3;24,26,43;,
3;44,43,26;,
3;26,28,44;,
3;46,45,30;,
3;30,31,46;,
3;47,46,31;,
3;31,32,47;,
3;48,47,32;,
3;32,33,48;,
3;49,48,33;,
3;33,34,49;,
3;50,49,34;,
3;34,35,50;,
3;51,50,35;,
3;35,36,51;,
3;52,51,36;,
3;36,37,52;,
3;53,52,37;,
3;37,38,53;,
3;54,53,38;,
3;38,39,54;,
3;55,54,39;,
3;39,40,55;,
3;56,55,40;,
3;40,41,56;,
3;57,56,41;,
3;41,42,57;,
3;58,57,42;,
3;42,43,58;,
3;59,58,43;,
3;43,44,59;,
3;61,60,45;,
3;45,46,61;,
3;62,61,46;,
3;46,47,62;,
3;63,62,47;,
3;47,48,63;,
3;64,63,48;,
3;48,49,64;,
3;65,64,49;,
3;49,50,65;,
3;66,65,50;,
3;50,51,66;,
3;67,66,51;,
3;51,52,67;,
3;68,67,52;,
3;52,53,68;,
3;69,68,53;,
3;53,54,69;,
3;70,69,54;,
3;54,55,70;,
3;71,70,55;,
3;55,56,71;,
3;72,71,56;,
3;56,57,72;,
3;73,72,57;,
3;57,58,73;,
3;74,73,58;,
3;58,59,74;,
3;76,75,60;,
3;60,61,76;,
3;77,76,61;,
3;61,62,77;,
3;78,77,62;,
3;62,63,78;,
3;79,78,63;,
3;63,64,79;,
3;80,79,64;,
3;64,65,80;,
3;81,80,65;,
3;65,66,81;,
3;82,81,66;,
3;66,67,82;,
3;83,82,67;,
3;67,68,83;,
3;84,83,68;,
3;68,69,84;,
3;85,84,69;,
3;69,70,85;,
3;86,85,70;,
3;70,71,86;,
3;87,86,71;,
3;71,72,87;,
3;88,87,72;,
3;72,73,88;,
3;89,88,73;,
3;73,74,89;,
3;91,90,75;,
3;75,76,91;,
3;92,91,76;,
3;76,77,92;,
3;93,92,77;,
3;77,78,93;,
3;94,93,78;,
3;78,79,94;,
3;95,94,79;,
3;79,80,95;,
3;96,95,80;,
3;80,81,96;,
3;97,96,81;,
3;81,82,97;,
3;98,97,82;,
3;82,83,98;,
3;99,98,83;,
3;83,84,99;,
3;100,99,84;,
3;84,85,100;,
3;101,100,85;,
3;85,86,101;,
3;102,101,86;,
3;86,87,102;,
3;103,102,87;,
3;87,88,103;,
3;104,103,88;,
3;88,89,104;,
3;106,105,90;,
3;90,91,106;,
3;107,106,91;,
3;91,92,107;,
3;108,107,92;,
3;92,93,108;,
3;109,108,93;,
3;93,94,109;,
3;110,109,94;,
3;94,95,110;,
3;111,110,95;,
3;95,96,111;,
3;112,111,96;,
3;96,97,112;,
3;113,112,97;,
3;97,98,113;,
3;114,113,98;,
3;98,99,114;,
3;115,114,99;,
3;99,100,115;,
3;116,115,100;,
3;100,101,116;,
3;117,116,101;,
3;101,102,117;,
3;118,117,102;,
3;102,103,118;,
3;119,118,103;,
3;103,104,119;,
3;121,120,105;,
3;105,106,121;,
3;122,121,106;,
3;106,107,122;,
3;123,122,107;,
3;107,108,123;,
3;124,123,108;,
3;108,109,124;,
3;125,124,109;,
3;109,110,125;,
3;126,125,110;,
3;110,111,126;,
3;127,126,111;,
3;111,112,127;,
3;129,128,112;,
3;112,113,129;,
3;130,129,113;,
3;113,114,130;,
3;131,130,114;,
3;114,115,131;,
3;132,131,115;,
3;115,116,132;,
3;133,132,116;,
3;116,117,133;,
3;134,133,117;,
3;117,118,134;,
3;135,134,118;,
3;118,119,135;,
3;137,136,120;,
3;120,121,137;,
3;138,137,121;,
3;121,122,138;,
3;139,138,122;,
3;122,123,139;,
3;140,139,123;,
3;123,124,140;,
3;141,140,124;,
3;124,125,141;,
3;142,141,125;,
3;125,126,142;,
3;143,142,126;,
3;126,127,143;,
3;146,145,144;,
3;147,129,146;,
3;148,146,129;,
3;129,130,148;,
3;149,148,130;,
3;130,131,149;,
3;150,149,131;,
3;131,132,150;,
3;151,150,132;,
3;132,133,151;,
3;152,151,133;,
3;133,134,152;,
3;153,152,134;,
3;134,135,153;,
3;155,154,136;,
3;136,137,155;,
3;156,155,137;,
3;137,138,156;,
3;157,156,138;,
3;138,139,157;,
3;158,157,139;,
3;139,140,158;,
3;159,158,140;,
3;140,141,159;,
3;160,159,141;,
3;141,142,160;,
3;161,160,142;,
3;142,143,161;,
3;164,163,162;,
3;165,146,164;,
3;166,164,146;,
3;146,148,166;,
3;167,166,148;,
3;148,149,167;,
3;168,167,149;,
3;149,150,168;,
3;169,168,150;,
3;150,151,169;,
3;170,169,151;,
3;151,152,170;,
3;171,170,152;,
3;152,153,171;,
3;173,172,154;,
3;154,155,173;,
3;174,173,155;,
3;155,156,174;,
3;175,174,156;,
3;156,157,175;,
3;176,175,157;,
3;157,158,176;,
3;177,176,158;,
3;158,159,177;,
3;178,177,159;,
3;159,160,178;,
3;179,178,160;,
3;160,161,179;,
3;182,181,180;,
3;183,164,182;,
3;184,182,164;,
3;164,166,184;,
3;185,184,166;,
3;166,167,185;,
3;186,185,167;,
3;167,168,186;,
3;187,186,168;,
3;168,169,187;,
3;188,187,169;,
3;169,170,188;,
3;189,188,170;,
3;170,171,189;,
3;191,190,172;,
3;172,173,191;,
3;192,191,173;,
3;173,174,192;,
3;193,192,174;,
3;174,175,193;,
3;194,193,175;,
3;175,176,194;,
3;195,194,176;,
3;176,177,195;,
3;196,195,177;,
3;177,178,196;,
3;197,196,178;,
3;178,179,197;,
3;200,199,198;,
3;201,182,200;,
3;202,200,182;,
3;182,184,202;,
3;203,202,184;,
3;184,185,203;,
3;204,203,185;,
3;185,186,204;,
3;205,204,186;,
3;186,187,205;,
3;206,205,187;,
3;187,188,206;,
3;207,206,188;,
3;188,189,207;,
3;209,208,190;,
3;190,191,209;,
3;210,209,191;,
3;191,192,210;,
3;211,210,192;,
3;192,193,211;,
3;212,211,193;,
3;193,194,212;,
3;213,212,194;,
3;194,195,213;,
3;214,213,195;,
3;195,196,214;,
3;215,214,196;,
3;196,197,215;,
3;218,217,216;,
3;219,200,218;,
3;220,218,200;,
3;200,202,220;,
3;221,220,202;,
3;202,203,221;,
3;222,221,203;,
3;203,204,222;,
3;223,222,204;,
3;204,205,223;,
3;224,223,205;,
3;205,206,224;,
3;225,224,206;,
3;206,207,225;,
3;227,226,208;,
3;208,209,227;,
3;228,227,209;,
3;209,210,228;,
3;229,228,210;,
3;210,211,229;,
3;230,229,211;,
3;211,212,230;,
3;231,230,212;,
3;212,213,231;,
3;232,231,213;,
3;213,214,232;,
3;233,232,214;,
3;214,215,233;,
3;236,235,234;,
3;237,218,236;,
3;238,236,218;,
3;218,220,238;,
3;239,238,220;,
3;220,221,239;,
3;240,239,221;,
3;221,222,240;,
3;241,240,222;,
3;222,223,241;,
3;242,241,223;,
3;223,224,242;,
3;243,242,224;,
3;224,225,243;;
 }
MeshTextureCoords {
 244;
0.244633;0.607680;,
0.244568;0.609775;,
0.274595;0.604927;,
0.275236;0.615106;,
0.306565;0.599967;,
0.307815;0.622561;,
0.340863;0.595116;,
0.342633;0.629706;,
0.377783;0.590633;,
0.379835;0.636048;,
0.417380;0.586795;,
0.419336;0.640943;,
0.459269;0.583879;,
0.460700;0.643707;,
0.502529;0.582126;,
0.503108;0.643817;,
0.545829;0.581699;,
0.545483;0.641135;,
0.587824;0.582646;,
0.586759;0.635993;,
0.627547;0.584870;,
0.626138;0.629071;,
0.664572;0.588155;,
0.663208;0.621144;,
0.698925;0.592216;,
0.697902;0.612885;,
0.730894;0.596755;,
0.730377;0.604778;,
0.760863;0.601501;,
0.760901;0.597124;,
0.244799;0.611899;,
0.272713;0.595225;,
0.302615;0.577897;,
0.335252;0.560473;,
0.371395;0.543858;,
0.411567;0.529454;,
0.455576;0.519141;,
0.502065;0.514790;,
0.548644;0.517309;,
0.592878;0.526051;,
0.633309;0.539275;,
0.669667;0.555095;,
0.702437;0.572042;,
0.732379;0.589145;,
0.760250;0.605794;,
0.245330;0.613925;,
0.269697;0.586498;,
0.296065;0.557451;,
0.325666;0.527223;,
0.360051;0.496902;,
0.400826;0.468951;,
0.448685;0.447900;,
0.501689;0.439411;,
0.554817;0.446426;,
0.602959;0.466324;,
0.644022;0.493521;,
0.678619;0.523400;,
0.708341;0.553392;,
0.734745;0.582340;,
0.759077;0.609766;,
0.246147;0.615726;,
0.265704;0.579204;,
0.287114;0.539807;,
0.311978;0.497276;,
0.342737;0.451956;,
0.382950;0.406406;,
0.436296;0.368578;,
0.501397;0.352690;,
0.566630;0.367556;,
0.620226;0.404675;,
0.660640;0.449824;,
0.691520;0.494928;,
0.716442;0.537346;,
0.737861;0.576688;,
0.757389;0.613196;,
0.247222;0.617181;,
0.260945;0.573737;,
0.276110;0.526144;,
0.294297;0.473027;,
0.318375;0.412879;,
0.354107;0.345923;,
0.412505;0.281465;,
0.501229;0.250835;,
0.589973;0.281124;,
0.648394;0.345265;,
0.684138;0.411977;,
0.708220;0.471957;,
0.726404;0.524963;,
0.741558;0.572483;,
0.755262;0.615881;,
0.248506;0.618185;,
0.255674;0.570388;,
0.263596;0.517499;,
0.273282;0.457077;,
0.286805;0.385368;,
0.309573;0.297744;,
0.360519;0.195973;,
0.501457;0.131792;,
0.641430;0.196693;,
0.691936;0.298181;,
0.714578;0.385448;,
0.728053;0.456903;,
0.737720;0.517157;,
0.745633;0.569936;,
0.752796;0.617657;,
0.249939;0.618652;,
0.250174;0.569327;,
0.250304;0.514584;,
0.250347;0.451536;,
0.250321;0.375368;,
0.250243;0.278480;,
0.250130;0.152231;,
0.750003;0.000953;,
0.749873;0.153804;,
0.749764;0.279441;,
0.749691;0.375841;,
0.749673;0.451693;,
0.749725;0.514543;,
0.749866;0.569159;,
0.750114;0.618399;,
0.251447;0.618521;,
0.244743;0.570580;,
0.237079;0.517624;,
0.227482;0.457197;,
0.213914;0.385522;,
0.191004;0.297925;,
0.139856;0.196094;,
-0.001457;0.131792;,
0.998543;0.131792;,
0.858198;0.196816;,
0.807497;0.298367;,
0.784726;0.385613;,
0.771221;0.457041;,
0.761662;0.517308;,
0.754029;0.570161;,
0.747353;0.618035;,
0.252951;0.617763;,
0.239668;0.574033;,
0.224754;0.526301;,
0.206654;0.473160;,
0.182531;0.413052;,
0.146645;0.346116;,
0.087971;0.281568;,
-0.001230;0.250835;,
0.998543;0.131792;,
0.998770;0.250835;,
0.909552;0.281227;,
0.998543;0.131792;,
0.850859;0.345464;,
0.814973;0.412167;,
0.790862;0.472121;,
0.772784;0.525167;,
0.757901;0.572844;,
0.744659;0.616545;,
0.254367;0.616380;,
0.235211;0.579438;,
0.214029;0.539830;,
0.189232;0.497250;,
0.158394;0.451976;,
0.117962;0.406473;,
0.064237;0.368620;,
-0.001397;0.352690;,
0.998770;0.250835;,
0.998603;0.352690;,
0.932837;0.367591;,
0.998770;0.250835;,
0.878863;0.404742;,
0.838239;0.449860;,
0.807295;0.494941;,
0.782458;0.537432;,
0.761286;0.577013;,
0.742179;0.613968;,
0.255611;0.614412;,
0.231589;0.586447;,
0.205408;0.557136;,
0.175829;0.526864;,
0.141304;0.496640;,
0.100224;0.468830;,
0.051898;0.447876;,
-0.001689;0.439411;,
0.998603;0.352690;,
0.998311;0.439411;,
0.944598;0.446388;,
0.998603;0.352690;,
0.895990;0.466189;,
0.854627;0.493263;,
0.819902;0.523073;,
0.790219;0.553146;,
0.764019;0.582399;,
0.740049;0.610401;,
0.256608;0.611931;,
0.228969;0.594635;,
0.199199;0.577031;,
0.166527;0.559625;,
0.130175;0.543231;,
0.089618;0.529126;,
0.045065;0.519057;,
-0.002065;0.514790;,
0.998311;0.439411;,
0.997935;0.514790;,
0.950714;0.517201;,
0.998311;0.439411;,
0.905937;0.525692;,
0.865123;0.538630;,
0.828564;0.554262;,
0.795771;0.571239;,
0.765973;0.588671;,
0.738391;0.605993;,
0.257292;0.609040;,
0.227461;0.603536;,
0.195564;0.598350;,
0.161185;0.593651;,
0.124002;0.589580;,
0.083955;0.586248;,
0.041448;0.583737;,
-0.002529;0.582126;,
0.997935;0.514790;,
0.997471;0.582126;,
0.953454;0.581522;,
0.997935;0.514790;,
0.910844;0.582046;,
0.870674;0.583772;,
0.833391;0.586678;,
0.798960;0.590641;,
0.767070;0.595471;,
0.737303;0.600940;,
0.257613;0.605871;,
0.227122;0.612668;,
0.194571;0.620017;,
0.159653;0.627512;,
0.122171;0.634513;,
0.082177;0.640157;,
0.040119;0.643504;,
-0.003108;0.643817;,
0.997471;0.582126;,
0.996892;0.643817;,
0.953702;0.640883;,
0.997471;0.582126;,
0.911739;0.635129;,
0.871875;0.627457;,
0.834526;0.618901;,
0.799726;0.610351;,
0.767272;0.602425;,
0.736855;0.595480;;
}
}
 }