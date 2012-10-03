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
Frame objBox02 {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
0.000000,0.000000,0.000000,1.000000;;
 }
Mesh objBox021 {
 75;
417.030396;760.906799;-170.675827;,
335.678619;718.413818;-132.740875;,
341.294434;718.413818;-185.403214;,
403.756531;718.413818;-214.529755;,
66.346611;501.102753;-323.378052;,
-231.913910;225.725800;-398.540039;,
160.766708;430.157501;-583.893860;,
239.974716;501.102783;-458.575470;,
542.957458;501.102814;-506.280701;,
699.824158;225.725800;-756.465881;,
937.762451;225.725800;-413.497162;,
712.940308;501.102814;-309.043060;,
754.770264;501.102814;-52.046772;,
1047.547974;263.087067;-10.769854;,
555.906677;254.483109;371.333527;,
508.012695;509.352142;139.064560;,
203.000229;501.102753;205.247818;,
25.091381;225.725800;466.009460;,
-191.546890;225.725800;96.829819;,
53.004627;501.102753;0.003648;,
460.525482;776.479797;-288.890625;,
380.874878;776.479797;-263.599579;,
537.848755;776.479797;-128.850967;,
539.828247;776.479797;-227.937408;,
508.012695;509.352142;139.064560;,
374.798462;776.479797;-41.199413;,
508.012695;509.352142;139.064560;,
464.973816;776.479797;-83.248871;,
290.699524;776.479797;-221.550125;,
278.379852;776.479797;-106.022018;,
442.092163;718.413818;-132.318695;,
490.758423;718.413818;-205.055786;,
483.197723;718.413818;-151.486511;,
-663.900818;-102.190201;93.851341;,
-269.509583;55.899403;388.773529;,
-1054.866089;-581.577087;652.809448;,
-1326.709717;-573.031677;338.482635;,
1278.363403;-188.220337;-557.144714;,
1394.258911;-222.922379;161.304688;,
791.261169;-157.899811;-1131.828003;,
-99.422401;-220.417801;-1442.360718;,
101.285988;225.725800;-650.148926;,
444.862061;718.413818;-233.697586;,
508.012695;509.352142;139.064560;,
555.906677;254.483109;371.333527;,
555.906677;254.483109;371.333527;,
464.973816;776.479797;-83.248871;,
379.630096;718.413818;-103.192139;,
790.980591;-458.433563;1159.909912;,
-1427.583374;-671.309326;-1012.163940;,
290.699524;776.479797;-221.550125;,
380.874878;776.479797;-263.599579;,
380.874878;776.479797;-263.599579;,
460.525482;776.479797;-288.890625;,
460.525482;776.479797;-288.890625;,
460.525482;776.479797;-288.890625;,
539.828247;776.479797;-227.937408;,
539.828247;776.479797;-227.937408;,
539.828247;776.479797;-227.937408;,
537.848755;776.479797;-128.850967;,
537.848755;776.479797;-128.850967;,
537.848755;776.479797;-128.850967;,
464.973816;776.479797;-83.248871;,
464.973816;776.479797;-83.248871;,
374.798462;776.479797;-41.199413;,
374.798462;776.479797;-41.199413;,
278.379852;776.479797;-106.022018;,
278.379852;776.479797;-106.022018;,
290.699524;776.479797;-221.550125;,
790.980591;-458.433563;1159.909912;,
-43.002632;222.047943;671.170654;,
790.980591;-458.433563;1159.909912;,
555.906677;254.483109;371.333527;,
-757.834656;-624.559082;1742.630859;,
790.980591;-458.433563;1159.909912;;

 81;
3;2,1,0;,
3;3,2,0;,
3;6,5,4;,
3;7,6,4;,
3;10,9,8;,
3;11,10,8;,
3;14,13,12;,
3;15,14,12;,
3;18,17,16;,
3;19,18,16;,
3;20,8,7;,
3;21,20,7;,
3;22,12,11;,
3;23,22,11;,
3;25,16,24;,
3;27,25,26;,
3;28,4,19;,
3;29,28,19;,
3;31,0,30;,
3;32,31,30;,
3;10,11,12;,
3;13,10,12;,
3;18,19,4;,
3;5,18,4;,
3;20,23,11;,
3;8,20,11;,
3;25,29,19;,
3;16,25,19;,
3;35,34,33;,
3;36,35,33;,
3;13,38,37;,
3;10,13,37;,
3;10,37,39;,
3;9,10,39;,
3;9,39,40;,
3;41,9,40;,
3;3,0,31;,
3;42,3,31;,
3;8,9,6;,
3;7,8,6;,
3;44,43,16;,
3;17,45,16;,
3;28,21,7;,
3;4,28,7;,
3;22,46,15;,
3;12,22,15;,
3;47,30,0;,
3;1,47,0;,
3;13,14,48;,
3;38,13,48;,
3;5,41,40;,
3;49,5,40;,
3;50,2,3;,
3;51,50,3;,
3;51,3,42;,
3;53,52,42;,
3;54,42,31;,
3;56,55,31;,
3;57,31,32;,
3;59,58,32;,
3;60,32,30;,
3;62,61,30;,
3;63,30,47;,
3;64,63,47;,
3;65,47,1;,
3;66,65,1;,
3;67,1,2;,
3;68,67,2;,
3;5,49,33;,
3;18,5,33;,
3;17,70,69;,
3;72,17,71;,
3;17,18,34;,
3;70,17,34;,
3;70,34,35;,
3;73,70,35;,
3;49,36,33;,
3;70,73,74;,
3;6,41,5;,
3;9,41,6;,
3;34,18,33;;
MeshMaterialList {
 1;
 81;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
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
 1.000000;1.000000;1.000000;1.000000;;
2535301200456458800000000000000.000000;
 1.000000;1.000000;1.000000;;
 0.200000;0.200000;0.200000;;
TextureFilename {
"VOLCAN.tga";
}
 }
}

 MeshNormals {
 75;
-0.018212;0.999798;0.008492;,
0.200255;0.959387;-0.198684;,
0.262409;0.964175;0.038827;,
0.155988;0.977110;0.144651;,
-0.617747;0.748793;-0.240203;,
-0.559698;0.780516;-0.278448;,
-0.287272;0.707632;-0.645548;,
-0.257556;0.737214;-0.624644;,
0.285585;0.737535;-0.611951;,
0.218729;0.712041;-0.667199;,
0.648679;0.674493;-0.352526;,
0.711551;0.617550;-0.335152;,
0.663535;0.706768;0.245360;,
0.662870;0.723778;0.191699;,
0.449714;0.678658;0.580673;,
0.471440;0.613222;0.633801;,
-0.248195;0.737528;0.628054;,
-0.170633;0.893666;0.415025;,
-0.622664;0.752315;0.215200;,
-0.685679;0.688035;0.237597;,
0.208705;0.638225;-0.741021;,
-0.277175;0.645634;-0.711569;,
0.702498;0.633545;0.324219;,
0.740251;0.570246;-0.356156;,
0.153336;0.604160;0.781971;,
-0.142984;0.702885;0.696784;,
0.326420;0.635166;0.700010;,
0.326420;0.635166;0.700010;,
-0.631516;0.675414;-0.380793;,
-0.672306;0.670790;0.313123;,
-0.034901;0.974718;-0.220698;,
-0.175892;0.973450;0.146480;,
-0.520201;0.819725;-0.239670;,
-0.553124;0.814393;0.175548;,
-0.551669;0.817125;0.167238;,
-0.557487;0.796236;0.234981;,
-0.519435;0.832139;0.194249;,
0.720207;0.634161;-0.281321;,
0.758045;0.616317;0.213356;,
0.493304;0.673602;-0.550374;,
-0.158709;0.846494;-0.508192;,
-0.298060;0.667901;-0.681960;,
0.043798;0.830065;0.555944;,
0.137606;0.681171;0.719076;,
0.137606;0.681171;0.719076;,
0.099244;0.649626;0.753748;,
0.416049;0.620363;0.664871;,
-0.050721;0.968967;-0.241932;,
0.510133;0.677766;0.529526;,
-0.542631;0.828081;-0.140832;,
0.309096;0.681966;0.662859;,
0.309096;0.681966;0.662859;,
0.233523;0.636068;0.735449;,
0.233523;0.636068;0.735449;,
-0.383984;0.688443;0.615306;,
-0.469518;0.637494;0.610863;,
-0.469518;0.637494;0.610863;,
-0.780119;0.615867;-0.110105;,
-0.725294;0.688287;-0.014490;,
-0.725294;0.688287;-0.014490;,
-0.337922;0.600544;-0.724676;,
-0.389332;0.679204;-0.622176;,
-0.389332;0.679204;-0.622176;,
-0.309096;0.681966;-0.662859;,
-0.309096;0.681966;-0.662859;,
0.408065;0.681966;-0.606965;,
0.408065;0.681966;-0.606965;,
0.727261;0.681966;0.077554;,
0.727261;0.681966;0.077554;,
0.548164;0.812957;0.196512;,
-0.232791;0.944174;0.233116;,
0.076335;0.750846;0.656051;,
0.076335;0.750846;0.656051;,
-0.243887;0.833462;0.495843;,
0.180080;0.709806;0.680990;;

 81;
3;2,1,0;,
3;3,2,0;,
3;6,5,4;,
3;7,6,4;,
3;10,9,8;,
3;11,10,8;,
3;14,13,12;,
3;15,14,12;,
3;18,17,16;,
3;19,18,16;,
3;20,8,7;,
3;21,20,7;,
3;22,12,11;,
3;23,22,11;,
3;25,16,24;,
3;27,25,26;,
3;28,4,19;,
3;29,28,19;,
3;31,0,30;,
3;32,31,30;,
3;10,11,12;,
3;13,10,12;,
3;18,19,4;,
3;5,18,4;,
3;20,23,11;,
3;8,20,11;,
3;25,29,19;,
3;16,25,19;,
3;35,34,33;,
3;36,35,33;,
3;13,38,37;,
3;10,13,37;,
3;10,37,39;,
3;9,10,39;,
3;9,39,40;,
3;41,9,40;,
3;3,0,31;,
3;42,3,31;,
3;8,9,6;,
3;7,8,6;,
3;44,43,16;,
3;17,45,16;,
3;28,21,7;,
3;4,28,7;,
3;22,46,15;,
3;12,22,15;,
3;47,30,0;,
3;1,47,0;,
3;13,14,48;,
3;38,13,48;,
3;5,41,40;,
3;49,5,40;,
3;50,2,3;,
3;51,50,3;,
3;51,3,42;,
3;53,52,42;,
3;54,42,31;,
3;56,55,31;,
3;57,31,32;,
3;59,58,32;,
3;60,32,30;,
3;62,61,30;,
3;63,30,47;,
3;64,63,47;,
3;65,47,1;,
3;66,65,1;,
3;67,1,2;,
3;68,67,2;,
3;5,49,33;,
3;18,5,33;,
3;17,70,69;,
3;72,17,71;,
3;17,18,34;,
3;70,17,34;,
3;70,34,35;,
3;73,70,35;,
3;49,36,33;,
3;70,73,74;,
3;6,41,5;,
3;9,41,6;,
3;34,18,33;;
 }
MeshTextureCoords {
 75;
0.590343;0.352547;,
0.518641;0.352547;,
0.540485;0.388776;,
0.595538;0.388776;,
0.501010;0.584137;,
0.456481;0.727026;,
0.564279;0.687767;,
0.571730;0.597937;,
0.662171;0.590295;,
0.701577;0.725922;,
0.784259;0.655237;,
0.725477;0.549321;,
0.765512;0.489959;,
0.841189;0.568053;,
0.854912;0.423178;,
0.785953;0.410395;,
0.394927;0.462883;,
0.308140;0.534292;,
0.331911;0.648067;,
0.417852;0.537341;,
0.639695;0.469592;,
0.587706;0.473355;,
0.710290;0.412626;,
0.685100;0.448517;,
0.387353;0.372241;,
0.467167;0.384205;,
0.387353;0.372241;,
0.461653;0.347449;,
0.534418;0.461992;,
0.485956;0.432201;,
0.595538;0.316317;,
0.655325;0.352547;,
0.631767;0.316317;,
0.271181;0.752677;,
0.231826;0.678218;,
0.135878;0.788341;,
0.153792;0.878666;,
0.878724;0.785976;,
0.964788;0.646815;,
0.753335;0.892977;,
0.579599;0.989788;,
0.558595;0.788746;,
0.631767;0.388776;,
0.387353;0.372241;,
0.311787;0.391543;,
0.311787;0.391543;,
0.715989;0.381192;,
0.540485;0.316317;,
0.984057;0.436834;,
0.363298;0.971988;,
0.516059;0.432025;,
0.595538;0.432025;,
0.595538;0.432025;,
0.661739;0.423446;,
0.661739;0.423446;,
0.661739;0.423446;,
0.698574;0.352547;,
0.698574;0.352547;,
0.698574;0.352547;,
0.663690;0.281481;,
0.663690;0.281481;,
0.663690;0.281481;,
0.595538;0.273069;,
0.595538;0.273069;,
0.516059;0.273069;,
0.516059;0.273069;,
0.468140;0.352547;,
0.468140;0.352547;,
0.516059;0.432025;,
0.183659;0.381674;,
0.245194;0.577804;,
0.183659;0.381674;,
0.311787;0.391543;,
0.049014;0.638670;,
0.183659;0.381674;;
}
}
 }
