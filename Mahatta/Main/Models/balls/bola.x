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
Frame GeoSphere0 {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
0.000582,0.454415,0.000708,1.000000;;
 }
Mesh GeoSphere01 {
 180;
0.000000;37.873077;0.000000;,
13.661922;35.323101;0.000000;,
4.221766;35.323101;12.993260;,
25.484148;28.016567;0.000000;,
18.160362;30.503807;13.194277;,
7.875034;28.016567;24.236866;,
33.874710;16.937355;0.000000;,
29.705914;19.573034;12.993260;,
21.536957;19.573034;24.236866;,
10.467860;16.937355;32.216766;,
0.000000;37.873077;0.000000;,
-11.052727;35.323101;8.030275;,
-6.936642;30.503807;21.348785;,
-20.617109;28.016567;14.979203;,
-3.177694;19.573034;32.267139;,
-16.395344;19.573034;27.972464;,
-20.617109;28.016567;14.979203;,
-16.395344;19.573034;27.972464;,
-27.405216;16.937355;19.911053;,
0.000000;37.873077;0.000000;,
-11.052727;35.323101;8.030275;,
-11.052727;35.323101;-8.030277;,
-11.052727;35.323101;8.030275;,
-20.617109;28.016567;14.979203;,
-22.447445;30.503807;-0.000003;,
-11.052727;35.323101;8.030275;,
-11.052727;35.323101;-8.030277;,
-11.052727;35.323101;-8.030277;,
-22.447445;30.503807;-0.000003;,
-20.617105;28.016567;-14.979208;,
-20.617109;28.016567;14.979203;,
-31.669834;19.573034;6.948926;,
-20.617109;28.016567;14.979203;,
-31.669834;19.573034;6.948926;,
-22.447445;30.503807;-0.000003;,
-31.669834;19.573034;6.948926;,
-31.669834;19.573034;-6.948933;,
-22.447445;30.503807;-0.000003;,
-31.669834;19.573034;-6.948933;,
-20.617105;28.016567;-14.979208;,
-31.669834;19.573034;-6.948933;,
-27.405216;16.937355;-19.911060;,
0.000000;37.873077;0.000000;,
4.221768;35.323101;-12.993259;,
-20.617105;28.016567;-14.979208;,
-6.936638;30.503807;-21.348785;,
7.875037;28.016567;-24.236862;,
-20.617105;28.016567;-14.979208;,
-16.395338;19.573034;-27.972469;,
-20.617105;28.016567;-14.979208;,
-3.177689;19.573034;-32.267139;,
10.467865;16.937355;-32.216763;,
0.000000;37.873077;0.000000;,
18.160364;30.503807;-13.194275;,
21.536961;19.573036;-24.236862;,
29.705915;19.573036;-12.993259;,
36.536880;5.911114;-8.030273;,
36.536873;5.911112;8.030276;,
34.279037;-5.911114;-14.979200;,
37.322239;-6.435886;0.000003;,
34.279030;-5.911112;14.979205;,
27.405224;-16.937355;-19.911045;,
31.669840;-19.573032;-6.948921;,
31.669836;-19.573032;6.948933;,
27.405216;-16.937355;19.911055;,
18.927761;5.911112;32.267143;,
3.653267;5.911112;37.230125;,
24.838873;-5.911112;27.972467;,
11.533204;-6.435886;35.495552;,
-3.653270;-5.911112;37.230122;,
16.395342;-19.573034;27.972466;,
3.177692;-19.573034;32.267139;,
-10.467861;-16.937355;32.216763;,
-24.838879;5.911112;27.972464;,
-34.279034;5.911112;14.979202;,
-24.838879;5.911112;27.972464;,
-18.927763;-5.911112;32.267136;,
-30.194321;-6.435886;21.937454;,
-24.838879;5.911112;27.972464;,
-30.194321;-6.435886;21.937454;,
-34.279034;5.911112;14.979202;,
-34.279034;5.911112;14.979202;,
-30.194321;-6.435886;21.937454;,
-36.536873;-5.911112;8.030273;,
-21.536961;-19.573034;24.236862;,
-21.536961;-19.573034;24.236862;,
-30.194321;-6.435886;21.937454;,
-21.536961;-19.573034;24.236862;,
-29.705914;-19.573034;12.993258;,
-30.194321;-6.435886;21.937454;,
-36.536873;-5.911112;8.030273;,
-29.705914;-19.573034;12.993258;,
-33.874710;-16.937355;-0.000003;,
-34.279026;5.911112;-14.979210;,
-24.838875;5.911113;-27.972469;,
-36.536873;-5.911112;-8.030281;,
-30.194321;-6.435886;-21.937462;,
-18.927763;-5.911113;-32.267139;,
-36.536873;-5.911112;-8.030281;,
-29.705917;-19.573036;-12.993263;,
-29.705917;-19.573036;-12.993263;,
-29.705917;-19.573036;-12.993263;,
-21.536961;-19.573036;-24.236866;,
-10.467865;-16.937355;-32.216763;,
3.653270;5.911112;-37.230122;,
18.927771;5.911114;-32.267139;,
-3.653269;-5.911112;-37.230122;,
11.533209;-6.435886;-35.495548;,
24.838884;-5.911114;-27.972462;,
3.177694;-19.573038;-32.267139;,
16.395350;-19.573038;-27.972462;,
30.194323;6.435885;21.937460;,
-24.838879;5.911112;27.972464;,
-11.533207;6.435886;35.495552;,
-16.395344;19.573034;27.972464;,
-24.838879;5.911112;27.972464;,
-36.536873;-5.911112;-8.030281;,
-36.536873;-5.911112;8.030273;,
-36.536873;-5.911112;-8.030281;,
-34.279026;5.911112;-14.979210;,
-37.322235;6.435885;-0.000004;,
-36.536873;-5.911112;-8.030281;,
-36.536873;-5.911112;8.030273;,
-36.536873;-5.911112;8.030273;,
-37.322235;6.435885;-0.000004;,
-34.279034;5.911112;14.979202;,
-31.669834;19.573034;-6.948933;,
-34.279026;5.911112;-14.979210;,
-31.669834;19.573034;-6.948933;,
-31.669834;19.573034;-6.948933;,
-31.669834;19.573034;6.948926;,
-37.322235;6.435885;-0.000004;,
-31.669834;19.573034;6.948926;,
-34.279034;5.911112;14.979202;,
-11.533205;6.435886;-35.495556;,
30.194330;6.435887;-21.937458;,
0.000000;-37.873077;0.000000;,
-4.221766;-35.323101;12.993259;,
11.052727;-35.323101;8.030276;,
-7.875034;-28.016567;24.236862;,
6.936640;-30.503807;21.348785;,
20.617109;-28.016567;14.979205;,
0.000000;-37.873077;0.000000;,
-13.661922;-35.323101;-0.000001;,
-4.221766;-35.323101;12.993259;,
-13.661922;-35.323101;-0.000001;,
-25.484150;-28.016567;-0.000002;,
-18.160364;-30.503807;13.194273;,
-13.661922;-35.323101;-0.000001;,
-4.221766;-35.323101;12.993259;,
-18.160364;-30.503807;13.194273;,
-25.484150;-28.016567;-0.000002;,
-33.874710;-16.937355;-0.000003;,
-29.705914;-19.573034;12.993258;,
-29.705914;-19.573034;12.993258;,
-18.160364;-30.503807;13.194273;,
-29.705914;-19.573034;12.993258;,
-21.536961;-19.573034;24.236862;,
-18.160364;-30.503807;13.194273;,
-21.536961;-19.573034;24.236862;,
0.000000;-37.873077;0.000000;,
-4.221767;-35.323101;-12.993259;,
-13.661922;-35.323101;-0.000001;,
-7.875036;-28.016567;-24.236862;,
-18.160364;-30.503807;-13.194275;,
-13.661922;-35.323101;-0.000001;,
-13.661922;-35.323101;-0.000001;,
-25.484150;-28.016567;-0.000002;,
-29.705917;-19.573036;-12.993263;,
-29.705917;-19.573036;-12.993263;,
-25.484150;-28.016567;-0.000002;,
-25.484150;-28.016567;-0.000002;,
-29.705917;-19.573036;-12.993263;,
-33.874710;-16.937355;-0.000003;,
0.000000;-37.873077;0.000000;,
11.052731;-35.323101;-8.030273;,
20.617113;-28.016567;-14.979198;,
6.936643;-30.503807;-21.348782;,
0.000000;-37.873077;0.000000;,
22.447447;-30.503807;0.000004;;

 180;
3;2,1,0;,
3;4,3,1;,
3;2,4,1;,
3;5,4,2;,
3;7,6,3;,
3;4,7,3;,
3;8,7,4;,
3;5,8,4;,
3;9,8,5;,
3;11,2,10;,
3;12,5,2;,
3;11,12,2;,
3;13,12,11;,
3;14,9,5;,
3;12,14,5;,
3;15,14,12;,
3;13,15,12;,
3;18,17,16;,
3;21,20,19;,
3;24,23,22;,
3;26,24,25;,
3;29,28,27;,
3;31,18,30;,
3;24,33,32;,
3;36,35,34;,
3;29,38,37;,
3;41,40,39;,
3;43,21,42;,
3;45,44,21;,
3;43,45,21;,
3;46,45,43;,
3;48,41,47;,
3;45,48,49;,
3;50,48,45;,
3;46,50,45;,
3;51,50,46;,
3;1,43,52;,
3;53,46,43;,
3;1,53,43;,
3;3,53,1;,
3;54,51,46;,
3;53,54,46;,
3;55,54,53;,
3;3,55,53;,
3;6,55,3;,
3;57,56,6;,
3;59,58,56;,
3;57,59,56;,
3;60,59,57;,
3;62,61,58;,
3;59,62,58;,
3;63,62,59;,
3;60,63,59;,
3;64,63,60;,
3;66,65,9;,
3;68,67,65;,
3;66,68,65;,
3;69,68,66;,
3;70,64,67;,
3;68,70,67;,
3;71,70,68;,
3;69,71,68;,
3;72,71,69;,
3;74,73,18;,
3;77,76,75;,
3;80,79,78;,
3;83,82,81;,
3;84,72,76;,
3;77,85,76;,
3;88,87,86;,
3;83,88,89;,
3;92,91,90;,
3;94,93,41;,
3;96,95,93;,
3;94,96,93;,
3;97,96,94;,
3;99,92,98;,
3;96,100,95;,
3;102,101,96;,
3;97,102,96;,
3;103,102,97;,
3;105,104,51;,
3;107,106,104;,
3;105,107,104;,
3;108,107,105;,
3;109,103,106;,
3;107,109,106;,
3;110,109,107;,
3;108,110,107;,
3;61,110,108;,
3;60,67,64;,
3;111,65,67;,
3;60,111,67;,
3;57,111,60;,
3;8,9,65;,
3;111,8,65;,
3;7,8,111;,
3;57,7,111;,
3;6,7,57;,
3;69,76,72;,
3;113,112,76;,
3;69,113,76;,
3;66,113,69;,
3;114,18,73;,
3;113,15,115;,
3;14,15,113;,
3;66,14,113;,
3;9,14,66;,
3;117,116,92;,
3;120,119,118;,
3;122,120,121;,
3;125,124,123;,
3;126,41,93;,
3;120,128,127;,
3;130,129,120;,
3;133,132,131;,
3;18,31,74;,
3;97,106,103;,
3;134,104,106;,
3;97,134,106;,
3;94,134,97;,
3;50,51,104;,
3;134,50,104;,
3;48,50,134;,
3;94,48,134;,
3;41,48,94;,
3;108,58,61;,
3;135,56,58;,
3;108,135,58;,
3;105,135,108;,
3;55,6,56;,
3;135,55,56;,
3;54,55,135;,
3;105,54,135;,
3;51,54,105;,
3;138,137,136;,
3;140,139,137;,
3;138,140,137;,
3;141,140,138;,
3;71,72,139;,
3;140,71,139;,
3;70,71,140;,
3;141,70,140;,
3;64,70,141;,
3;144,143,142;,
3;147,146,145;,
3;149,147,148;,
3;139,150,137;,
3;153,152,151;,
3;147,154,146;,
3;157,156,155;,
3;139,159,158;,
3;72,84,139;,
3;162,161,160;,
3;164,163,161;,
3;165,164,161;,
3;167,164,166;,
3;102,103,163;,
3;164,102,163;,
3;168,102,164;,
3;170,169,164;,
3;173,172,171;,
3;161,175,174;,
3;177,176,175;,
3;161,177,175;,
3;163,177,161;,
3;110,61,176;,
3;177,110,176;,
3;109,110,177;,
3;163,109,177;,
3;103,109,163;,
3;175,138,178;,
3;179,141,138;,
3;175,179,138;,
3;176,179,175;,
3;63,64,141;,
3;179,63,141;,
3;62,63,179;,
3;176,62,179;,
3;61,62,176;;
MeshMaterialList {
 1;
 180;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
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
 0.588235;0.588235;0.588235;1.000000;;
4.000000;
 0.900000;0.900000;0.900000;;
 0.117647;0.117647;0.117647;;
TextureFilename {
"soccertex_1.tga";
}
 }
}

 MeshNormals {
 180;
0.181871;0.974404;0.132137;,
0.348482;0.937315;0.000000;,
0.107687;0.937315;0.331427;,
0.678526;0.734576;0.000000;,
0.487302;0.798240;0.354046;,
0.209676;0.734576;0.645317;,
0.894427;0.447214;0.000000;,
0.787855;0.516037;0.336140;,
0.563148;0.516037;0.645422;,
0.276393;0.447214;0.850651;,
-0.069469;0.974404;0.213802;,
-0.180534;0.924333;0.336178;,
-0.186133;0.798240;0.572858;,
-0.375675;0.778435;0.502899;,
-0.076227;0.516037;0.853168;,
-0.359101;0.522954;0.773024;,
-0.561616;0.598437;0.571368;,
-0.561616;0.598437;0.571368;,
-0.723607;0.447214;0.525731;,
-0.224805;0.974404;0.000000;,
-0.224805;0.974404;0.000000;,
-0.193866;0.947527;-0.254182;,
-0.498310;0.843394;0.200932;,
-0.498310;0.843394;0.200932;,
-0.531926;0.835822;0.135854;,
-0.389534;0.921012;0.000000;,
-0.389534;0.921012;0.000000;,
-0.498310;0.843394;-0.200932;,
-0.498310;0.843394;-0.200932;,
-0.594375;0.778435;-0.201884;,
-0.716952;0.598437;0.357566;,
-0.793377;0.490334;0.360729;,
-0.682446;0.703007;0.200121;,
-0.682446;0.703007;0.200121;,
-0.764307;0.644853;0.000000;,
-0.764307;0.644853;0.000000;,
-0.764307;0.644853;0.000000;,
-0.682446;0.703007;-0.200121;,
-0.682446;0.703007;-0.200121;,
-0.716952;0.598437;-0.357566;,
-0.716952;0.598437;-0.357566;,
-0.723607;0.447214;-0.525731;,
-0.069468;0.974404;-0.213802;,
0.107687;0.937315;-0.331427;,
-0.345084;0.843394;-0.411830;,
-0.186133;0.798240;-0.572858;,
0.209676;0.734576;-0.645317;,
-0.561616;0.598437;-0.571368;,
-0.439810;0.516037;-0.735032;,
-0.401214;0.703007;-0.587204;,
-0.076227;0.516037;-0.853168;,
0.276393;0.447214;-0.850651;,
0.181871;0.974404;-0.132137;,
0.487302;0.798240;-0.354046;,
0.563148;0.516038;-0.645422;,
0.787855;0.516038;-0.336139;,
0.964050;0.165670;-0.207752;,
0.964050;0.165670;0.207753;,
0.902047;-0.165670;-0.398579;,
0.983342;-0.181765;0.000000;,
0.902047;-0.165670;0.398579;,
0.723607;-0.447214;-0.525731;,
0.834966;-0.516037;-0.191147;,
0.834966;-0.516037;0.191147;,
0.723607;-0.447214;0.525731;,
0.495492;0.165670;0.852667;,
0.100323;0.165670;0.981065;,
0.657819;-0.165670;0.734730;,
0.303869;-0.181765;0.935214;,
-0.100324;-0.165670;0.981065;,
0.439810;-0.516037;0.735032;,
0.076227;-0.516037;0.853168;,
-0.276393;-0.447214;0.850651;,
-0.701637;0.306855;0.643075;,
-0.828418;0.306855;0.468575;,
-0.672471;-0.068525;0.736944;,
-0.495492;-0.165670;0.852667;,
-0.659664;-0.183498;0.728816;,
-0.807385;0.063479;0.586600;,
-0.807385;0.063479;0.586600;,
-0.807385;0.063479;0.586600;,
-0.908681;-0.068525;0.411830;,
-0.908681;-0.068525;0.411830;,
-0.896992;-0.183499;0.402161;,
-0.429824;-0.490334;0.758171;,
-0.637984;-0.296004;0.710885;,
-0.743149;-0.395230;0.539929;,
-0.743149;-0.395230;0.539929;,
-0.813307;-0.347803;0.466438;,
-0.873240;-0.296004;0.387083;,
-0.902601;-0.373632;0.213802;,
-0.902601;-0.373632;0.213802;,
-0.942804;-0.333347;0.000000;,
-0.856981;0.153944;-0.491819;,
-0.657819;0.165670;-0.734730;,
-0.896992;-0.183498;-0.402161;,
-0.795540;-0.181764;-0.577994;,
-0.495492;-0.165670;-0.852667;,
-0.902601;-0.373632;-0.213802;,
-0.902601;-0.373632;-0.213802;,
-0.873240;-0.296004;-0.387083;,
-0.743149;-0.395230;-0.539929;,
-0.563148;-0.516037;-0.645422;,
-0.276393;-0.447214;-0.850651;,
0.100324;0.165670;-0.981065;,
0.495492;0.165670;-0.852667;,
-0.100323;-0.165670;-0.981065;,
0.303869;-0.181764;-0.935214;,
0.657819;-0.165670;-0.734730;,
0.076227;-0.516037;-0.853168;,
0.439810;-0.516037;-0.735032;,
0.795540;0.181764;0.577994;,
-0.493070;0.068525;0.867287;,
-0.303870;0.181764;0.935214;,
-0.604550;0.373632;0.703505;,
-0.478944;0.296004;0.826435;,
-0.972069;-0.234695;0.000000;,
-0.972069;-0.234695;0.000000;,
-0.977206;0.068525;-0.200932;,
-0.977206;0.068525;-0.200932;,
-0.978669;0.178023;-0.102540;,
-0.997983;-0.063479;0.000000;,
-0.997983;-0.063479;0.000000;,
-0.977206;0.068525;0.200931;,
-0.977206;0.068525;0.200931;,
-0.977206;0.068525;0.200931;,
-0.855889;0.373632;-0.357566;,
-0.933988;0.296004;-0.200121;,
-0.933988;0.296004;-0.200121;,
-0.918582;0.395230;0.000000;,
-0.918582;0.395230;0.000000;,
-0.933988;0.296004;0.200121;,
-0.933988;0.296004;0.200121;,
-0.933988;0.296004;0.200121;,
-0.303869;0.181764;-0.935214;,
0.795541;0.181765;-0.577994;,
0.069469;-0.974404;0.213802;,
-0.033789;-0.914833;0.402417;,
0.281928;-0.937315;0.204833;,
-0.209676;-0.734576;0.645317;,
0.186133;-0.798240;0.572858;,
0.548939;-0.734576;0.398828;,
-0.181871;-0.974404;0.132137;,
-0.181871;-0.974404;0.132137;,
-0.181871;-0.974404;0.132137;,
-0.521246;-0.843394;0.130342;,
-0.599524;-0.778435;0.186037;,
-0.510191;-0.835822;0.202750;,
-0.315140;-0.921012;0.228962;,
-0.315140;-0.921012;0.228962;,
-0.285037;-0.843394;0.455456;,
-0.790198;-0.598437;0.132137;,
-0.790198;-0.598437;0.132137;,
-0.790198;-0.598437;0.132137;,
-0.669739;-0.703007;0.239231;,
-0.618337;-0.644853;0.449248;,
-0.618337;-0.644853;0.449248;,
-0.618337;-0.644853;0.449248;,
-0.434482;-0.703007;0.563033;,
-0.434482;-0.703007;0.563033;,
-0.181871;-0.974404;-0.132137;,
-0.107687;-0.937315;-0.331427;,
-0.181871;-0.974404;-0.132137;,
-0.209676;-0.734576;-0.645317;,
-0.487302;-0.798241;-0.354046;,
-0.315140;-0.921012;-0.228962;,
-0.521246;-0.843394;-0.130342;,
-0.521246;-0.843394;-0.130342;,
-0.618337;-0.644853;-0.449248;,
-0.669738;-0.703007;-0.239230;,
-0.669738;-0.703007;-0.239230;,
-0.790198;-0.598437;-0.132137;,
-0.790198;-0.598437;-0.132137;,
-0.790198;-0.598437;-0.132137;,
0.069469;-0.974404;-0.213802;,
0.281928;-0.937315;-0.204833;,
0.548939;-0.734576;-0.398828;,
0.186133;-0.798241;-0.572858;,
0.224805;-0.974404;0.000000;,
0.602339;-0.798240;0.000000;;

 180;
3;2,1,0;,
3;4,3,1;,
3;2,4,1;,
3;5,4,2;,
3;7,6,3;,
3;4,7,3;,
3;8,7,4;,
3;5,8,4;,
3;9,8,5;,
3;11,2,10;,
3;12,5,2;,
3;11,12,2;,
3;13,12,11;,
3;14,9,5;,
3;12,14,5;,
3;15,14,12;,
3;13,15,12;,
3;18,17,16;,
3;21,20,19;,
3;24,23,22;,
3;26,24,25;,
3;29,28,27;,
3;31,18,30;,
3;24,33,32;,
3;36,35,34;,
3;29,38,37;,
3;41,40,39;,
3;43,21,42;,
3;45,44,21;,
3;43,45,21;,
3;46,45,43;,
3;48,41,47;,
3;45,48,49;,
3;50,48,45;,
3;46,50,45;,
3;51,50,46;,
3;1,43,52;,
3;53,46,43;,
3;1,53,43;,
3;3,53,1;,
3;54,51,46;,
3;53,54,46;,
3;55,54,53;,
3;3,55,53;,
3;6,55,3;,
3;57,56,6;,
3;59,58,56;,
3;57,59,56;,
3;60,59,57;,
3;62,61,58;,
3;59,62,58;,
3;63,62,59;,
3;60,63,59;,
3;64,63,60;,
3;66,65,9;,
3;68,67,65;,
3;66,68,65;,
3;69,68,66;,
3;70,64,67;,
3;68,70,67;,
3;71,70,68;,
3;69,71,68;,
3;72,71,69;,
3;74,73,18;,
3;77,76,75;,
3;80,79,78;,
3;83,82,81;,
3;84,72,76;,
3;77,85,76;,
3;88,87,86;,
3;83,88,89;,
3;92,91,90;,
3;94,93,41;,
3;96,95,93;,
3;94,96,93;,
3;97,96,94;,
3;99,92,98;,
3;96,100,95;,
3;102,101,96;,
3;97,102,96;,
3;103,102,97;,
3;105,104,51;,
3;107,106,104;,
3;105,107,104;,
3;108,107,105;,
3;109,103,106;,
3;107,109,106;,
3;110,109,107;,
3;108,110,107;,
3;61,110,108;,
3;60,67,64;,
3;111,65,67;,
3;60,111,67;,
3;57,111,60;,
3;8,9,65;,
3;111,8,65;,
3;7,8,111;,
3;57,7,111;,
3;6,7,57;,
3;69,76,72;,
3;113,112,76;,
3;69,113,76;,
3;66,113,69;,
3;114,18,73;,
3;113,15,115;,
3;14,15,113;,
3;66,14,113;,
3;9,14,66;,
3;117,116,92;,
3;120,119,118;,
3;122,120,121;,
3;125,124,123;,
3;126,41,93;,
3;120,128,127;,
3;130,129,120;,
3;133,132,131;,
3;18,31,74;,
3;97,106,103;,
3;134,104,106;,
3;97,134,106;,
3;94,134,97;,
3;50,51,104;,
3;134,50,104;,
3;48,50,134;,
3;94,48,134;,
3;41,48,94;,
3;108,58,61;,
3;135,56,58;,
3;108,135,58;,
3;105,135,108;,
3;55,6,56;,
3;135,55,56;,
3;54,55,135;,
3;105,54,135;,
3;51,54,105;,
3;138,137,136;,
3;140,139,137;,
3;138,140,137;,
3;141,140,138;,
3;71,72,139;,
3;140,71,139;,
3;70,71,140;,
3;141,70,140;,
3;64,70,141;,
3;144,143,142;,
3;147,146,145;,
3;149,147,148;,
3;139,150,137;,
3;153,152,151;,
3;147,154,146;,
3;157,156,155;,
3;139,159,158;,
3;72,84,139;,
3;162,161,160;,
3;164,163,161;,
3;165,164,161;,
3;167,164,166;,
3;102,103,163;,
3;164,102,163;,
3;168,102,164;,
3;170,169,164;,
3;173,172,171;,
3;161,175,174;,
3;177,176,175;,
3;161,177,175;,
3;163,177,161;,
3;110,61,176;,
3;177,110,176;,
3;109,110,177;,
3;163,109,177;,
3;103,109,163;,
3;175,138,178;,
3;179,141,138;,
3;175,179,138;,
3;176,179,175;,
3;63,64,141;,
3;179,63,141;,
3;62,63,179;,
3;176,62,179;,
3;61,62,176;;
 }
MeshTextureCoords {
 180;
0.600000;0.000000;,
0.500000;0.117472;,
0.700000;0.117472;,
0.500000;0.234944;,
0.600000;0.201939;,
0.700000;0.234944;,
0.500000;0.352416;,
0.565623;0.327121;,
0.634377;0.327121;,
0.700000;0.352416;,
0.800000;0.000000;,
0.900000;0.117472;,
0.800000;0.201939;,
0.900000;0.234944;,
0.765623;0.327121;,
0.834377;0.327121;,
0.499260;0.235017;,
0.433636;0.327194;,
0.499260;0.352489;,
0.000000;0.000000;,
-0.000566;0.111472;,
0.100000;0.117472;,
0.300424;0.117472;,
0.300424;0.234944;,
0.200424;0.201939;,
0.300424;0.117472;,
0.199858;0.119187;,
0.298867;0.117472;,
0.198867;0.201939;,
0.298867;0.234944;,
0.499260;0.235017;,
0.564883;0.327194;,
0.300424;0.234944;,
0.234801;0.327121;,
0.399449;0.200824;,
0.400217;0.326006;,
0.433826;0.326006;,
0.198867;0.201939;,
0.233244;0.327121;,
0.100000;0.234944;,
0.034377;0.327121;,
0.100000;0.352416;,
0.200000;0.000000;,
0.300000;0.117472;,
0.100000;0.234944;,
0.200000;0.201939;,
0.300000;0.234944;,
0.100000;0.234944;,
0.165623;0.327121;,
0.100000;0.234944;,
0.234377;0.327121;,
0.300000;0.352416;,
0.400000;0.000000;,
0.400000;0.201939;,
0.365623;0.327121;,
0.434377;0.327121;,
0.465568;0.450115;,
0.534432;0.450115;,
0.434433;0.549885;,
0.500000;0.554355;,
0.565567;0.549885;,
0.400000;0.647584;,
0.465623;0.672879;,
0.534377;0.672879;,
0.600000;0.647584;,
0.665568;0.450115;,
0.734432;0.450115;,
0.634432;0.549885;,
0.700000;0.554355;,
0.765567;0.549885;,
0.665623;0.672879;,
0.734377;0.672879;,
0.800000;0.647584;,
0.464827;0.450188;,
0.533692;0.450188;,
0.865568;0.450115;,
0.834432;0.549885;,
0.900000;0.554355;,
0.465023;0.450115;,
0.499455;0.554355;,
0.533888;0.450115;,
0.533888;0.450115;,
0.499455;0.554355;,
0.565023;0.549885;,
0.865623;0.672879;,
0.864766;0.672879;,
0.499455;0.554355;,
0.464221;0.672879;,
0.532975;0.672879;,
0.499455;0.554355;,
0.366584;0.548454;,
0.332350;0.672463;,
0.397973;0.647168;,
0.065568;0.450115;,
0.134433;0.450115;,
0.034433;0.549885;,
0.100000;0.554355;,
0.165568;0.549885;,
0.432406;0.549469;,
0.463596;0.672463;,
0.065623;0.672879;,
0.065623;0.672879;,
0.134377;0.672879;,
0.200000;0.647584;,
0.265568;0.450115;,
0.334433;0.450115;,
0.234432;0.549885;,
0.300000;0.554355;,
0.365568;0.549885;,
0.265623;0.672879;,
0.334377;0.672879;,
0.600000;0.445645;,
0.865568;0.450115;,
0.800000;0.445645;,
0.433636;0.327194;,
0.865568;0.450115;,
0.432406;0.549469;,
0.366584;0.548454;,
0.432253;0.550999;,
0.463388;0.451230;,
0.397820;0.446759;,
0.432253;0.550999;,
0.398893;0.549985;,
0.033453;0.549885;,
0.007136;0.448181;,
0.064588;0.450115;,
0.034377;0.327121;,
0.463388;0.451230;,
0.432197;0.328235;,
0.432197;0.328235;,
0.398588;0.328235;,
0.007136;0.448181;,
0.033397;0.327121;,
0.064588;0.450115;,
0.200000;0.445645;,
0.400000;0.445645;,
0.700000;1.000000;,
0.800000;0.882528;,
0.600000;0.882528;,
0.800000;0.765056;,
0.700000;0.798061;,
0.600000;0.765056;,
0.098465;1.001449;,
0.198465;0.883977;,
-0.001535;0.883977;,
0.001557;0.882528;,
0.001557;0.765056;,
0.101557;0.798061;,
0.001557;0.882528;,
0.201557;0.882528;,
0.899143;0.798061;,
0.198010;0.763341;,
0.198010;0.645869;,
0.132387;0.671165;,
0.067180;0.672879;,
0.698561;0.798061;,
0.732938;0.672879;,
0.664184;0.672879;,
0.899143;0.798061;,
0.864766;0.672879;,
0.100000;1.000000;,
0.200000;0.882528;,
0.000000;0.882528;,
0.200000;0.765056;,
0.100000;0.798061;,
0.000000;0.882528;,
0.000000;0.882528;,
0.000000;0.765056;,
0.065623;0.672879;,
0.065623;0.672879;,
0.000000;0.765056;,
0.198010;0.763341;,
0.263633;0.671165;,
0.198010;0.645869;,
0.300000;1.000000;,
0.400000;0.882528;,
0.400000;0.765056;,
0.300000;0.798061;,
0.500000;1.000000;,
0.500000;0.798061;;
}
}
 }
