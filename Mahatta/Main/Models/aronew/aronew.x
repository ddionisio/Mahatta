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
Frame Tube01 {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,-0.000000,-1.000000,0.000000,
0.000000,1.000000,-0.000000,0.000000,
0.000000,0.000000,0.000000,1.000000;;
 }
Mesh Tube011 {
 243;
-0.000001;-0.000000;13.951319;,
4.771632;6.203170;13.109951;,
4.771632;-0.000000;13.109951;,
-0.000001;6.203170;13.951319;,
-0.000001;6.203170;13.951319;,
1.960521;6.203170;5.386486;,
4.771632;6.203170;13.109951;,
-0.000000;6.203170;5.732179;,
-0.000000;6.203170;5.732179;,
1.960521;0.000000;5.386486;,
1.960521;6.203170;5.386486;,
-0.000000;-0.000000;5.732179;,
-0.000000;-0.000000;5.732179;,
4.771632;-0.000000;13.109951;,
1.960521;0.000000;5.386486;,
-0.000000;-0.000000;5.732179;,
-0.000001;-0.000000;13.951319;,
4.771632;-0.000000;13.109951;,
4.771632;-0.000000;13.109951;,
8.967735;6.203170;10.687330;,
8.967735;0.000000;10.687330;,
4.771632;6.203170;13.109951;,
3.684574;6.203170;4.391104;,
8.967735;6.203170;10.687330;,
1.960521;6.203170;5.386486;,
3.684574;0.000000;4.391104;,
3.684574;6.203170;4.391104;,
1.960521;0.000000;5.386486;,
8.967735;0.000000;10.687330;,
3.684574;0.000000;4.391104;,
1.960521;0.000000;5.386486;,
4.771632;-0.000000;13.109951;,
8.967735;0.000000;10.687330;,
12.082196;6.203170;6.975659;,
12.082196;-0.000000;6.975659;,
4.964212;6.203170;2.866089;,
12.082196;6.203170;6.975659;,
3.684574;6.203170;4.391104;,
4.964212;-0.000000;2.866089;,
4.964212;6.203170;2.866089;,
3.684574;0.000000;4.391104;,
12.082196;-0.000000;6.975659;,
4.964212;-0.000000;2.866089;,
3.684574;0.000000;4.391104;,
8.967735;0.000000;10.687330;,
12.082196;-0.000000;6.975659;,
13.739367;6.203170;2.422621;,
13.739367;0.000000;2.422621;,
5.645094;6.203170;0.995382;,
13.739367;6.203170;2.422621;,
4.964212;6.203170;2.866089;,
5.645094;0.000000;0.995382;,
5.645094;6.203170;0.995382;,
4.964212;-0.000000;2.866089;,
13.739367;0.000000;2.422621;,
5.645094;0.000000;0.995382;,
4.964212;-0.000000;2.866089;,
12.082196;-0.000000;6.975659;,
13.739367;0.000000;2.422621;,
13.739367;6.203170;-2.422622;,
13.739367;-0.000000;-2.422622;,
5.645094;6.203170;-0.995383;,
13.739367;6.203170;-2.422622;,
5.645094;6.203170;0.995382;,
5.645094;-0.000000;-0.995383;,
5.645094;6.203170;-0.995383;,
5.645094;6.203170;0.995382;,
5.645094;0.000000;0.995382;,
13.739367;-0.000000;-2.422622;,
5.645094;-0.000000;-0.995383;,
5.645094;0.000000;0.995382;,
13.739367;0.000000;2.422621;,
13.739367;-0.000000;-2.422622;,
12.082196;6.203170;-6.975660;,
12.082196;0.000000;-6.975660;,
4.964212;6.203170;-2.866090;,
12.082196;6.203170;-6.975660;,
5.645094;6.203170;-0.995383;,
4.964212;0.000000;-2.866090;,
4.964212;6.203170;-2.866090;,
5.645094;-0.000000;-0.995383;,
12.082196;0.000000;-6.975660;,
4.964212;0.000000;-2.866090;,
5.645094;-0.000000;-0.995383;,
13.739367;-0.000000;-2.422622;,
12.082196;0.000000;-6.975660;,
8.967733;6.203170;-10.687331;,
8.967733;-0.000000;-10.687331;,
3.684573;6.203170;-4.391104;,
8.967733;6.203170;-10.687331;,
4.964212;6.203170;-2.866090;,
3.684573;-0.000000;-4.391104;,
3.684573;6.203170;-4.391104;,
4.964212;0.000000;-2.866090;,
8.967733;-0.000000;-10.687331;,
3.684573;-0.000000;-4.391104;,
4.964212;0.000000;-2.866090;,
12.082196;0.000000;-6.975660;,
8.967733;-0.000000;-10.687331;,
4.771630;6.203170;-13.109952;,
4.771630;0.000000;-13.109952;,
1.960520;6.203170;-5.386487;,
4.771630;6.203170;-13.109952;,
3.684573;6.203170;-4.391104;,
1.960520;-0.000000;-5.386487;,
1.960520;6.203170;-5.386487;,
3.684573;-0.000000;-4.391104;,
4.771630;0.000000;-13.109952;,
1.960520;-0.000000;-5.386487;,
3.684573;-0.000000;-4.391104;,
8.967733;-0.000000;-10.687331;,
4.771630;0.000000;-13.109952;,
-0.000002;6.203170;-13.951319;,
-0.000002;0.000000;-13.951319;,
-0.000001;6.203170;-5.732179;,
-0.000002;6.203170;-13.951319;,
1.960520;6.203170;-5.386487;,
-0.000001;0.000000;-5.732179;,
-0.000001;6.203170;-5.732179;,
1.960520;-0.000000;-5.386487;,
-0.000002;0.000000;-13.951319;,
-0.000001;0.000000;-5.732179;,
1.960520;-0.000000;-5.386487;,
4.771630;0.000000;-13.109952;,
-0.000002;0.000000;-13.951319;,
-4.771634;6.203170;-13.109950;,
-4.771634;0.000000;-13.109950;,
-1.960521;6.203170;-5.386486;,
-4.771634;6.203170;-13.109950;,
-0.000001;6.203170;-5.732179;,
-1.960521;-0.000000;-5.386486;,
-1.960521;6.203170;-5.386486;,
-0.000001;0.000000;-5.732179;,
-0.000001;0.000000;-5.732179;,
-4.771634;0.000000;-13.109950;,
-1.960521;-0.000000;-5.386486;,
-0.000001;0.000000;-5.732179;,
-0.000002;0.000000;-13.951319;,
-4.771634;0.000000;-13.109950;,
-4.771634;0.000000;-13.109950;,
-8.967737;6.203170;-10.687328;,
-8.967737;-0.000000;-10.687328;,
-4.771634;6.203170;-13.109950;,
-3.684574;6.203170;-4.391103;,
-8.967737;6.203170;-10.687328;,
-1.960521;6.203170;-5.386486;,
-3.684574;-0.000000;-4.391103;,
-3.684574;6.203170;-4.391103;,
-1.960521;-0.000000;-5.386486;,
-8.967737;-0.000000;-10.687328;,
-3.684574;-0.000000;-4.391103;,
-1.960521;-0.000000;-5.386486;,
-4.771634;0.000000;-13.109950;,
-8.967737;-0.000000;-10.687328;,
-12.082198;6.203170;-6.975657;,
-12.082198;0.000000;-6.975657;,
-4.964213;6.203170;-2.866088;,
-12.082198;6.203170;-6.975657;,
-3.684574;6.203170;-4.391103;,
-4.964213;0.000000;-2.866088;,
-4.964213;6.203170;-2.866088;,
-3.684574;-0.000000;-4.391103;,
-12.082198;0.000000;-6.975657;,
-4.964213;0.000000;-2.866088;,
-3.684574;-0.000000;-4.391103;,
-8.967737;-0.000000;-10.687328;,
-12.082198;0.000000;-6.975657;,
-13.739367;6.203170;-2.422618;,
-13.739367;-0.000000;-2.422618;,
-5.645094;6.203170;-0.995381;,
-13.739367;6.203170;-2.422618;,
-4.964213;6.203170;-2.866088;,
-5.645094;-0.000000;-0.995381;,
-5.645094;6.203170;-0.995381;,
-4.964213;0.000000;-2.866088;,
-13.739367;-0.000000;-2.422618;,
-5.645094;-0.000000;-0.995381;,
-4.964213;0.000000;-2.866088;,
-12.082198;0.000000;-6.975657;,
-13.739367;-0.000000;-2.422618;,
-13.739367;6.203170;2.422624;,
-13.739367;0.000000;2.422624;,
-5.645094;6.203170;0.995384;,
-13.739367;6.203170;2.422624;,
-5.645094;6.203170;-0.995381;,
-5.645094;0.000000;0.995384;,
-5.645094;6.203170;0.995384;,
-5.645094;-0.000000;-0.995381;,
-13.739367;0.000000;2.422624;,
-5.645094;0.000000;0.995384;,
-5.645094;-0.000000;-0.995381;,
-13.739367;-0.000000;-2.422618;,
-13.739367;0.000000;2.422624;,
-12.082194;6.203170;6.975663;,
-12.082194;-0.000000;6.975663;,
-4.964211;6.203170;2.866091;,
-12.082194;6.203170;6.975663;,
-5.645094;6.203170;0.995384;,
-4.964211;-0.000000;2.866091;,
-4.964211;6.203170;2.866091;,
-5.645094;0.000000;0.995384;,
-12.082194;-0.000000;6.975663;,
-4.964211;-0.000000;2.866091;,
-5.645094;0.000000;0.995384;,
-13.739367;0.000000;2.422624;,
-12.082194;-0.000000;6.975663;,
-8.967731;6.203170;10.687332;,
-8.967731;0.000000;10.687332;,
-3.684572;6.203170;4.391105;,
-8.967731;6.203170;10.687332;,
-4.964211;6.203170;2.866091;,
-3.684572;0.000000;4.391105;,
-3.684572;6.203170;4.391105;,
-4.964211;-0.000000;2.866091;,
-8.967731;0.000000;10.687332;,
-3.684572;0.000000;4.391105;,
-4.964211;-0.000000;2.866091;,
-12.082194;-0.000000;6.975663;,
-8.967731;0.000000;10.687332;,
-4.771628;6.203170;13.109953;,
-4.771628;-0.000000;13.109953;,
-1.960519;6.203170;5.386487;,
-4.771628;6.203170;13.109953;,
-3.684572;6.203170;4.391105;,
-1.960519;0.000000;5.386487;,
-1.960519;6.203170;5.386487;,
-3.684572;0.000000;4.391105;,
-4.771628;-0.000000;13.109953;,
-1.960519;0.000000;5.386487;,
-3.684572;0.000000;4.391105;,
-8.967731;0.000000;10.687332;,
-4.771628;-0.000000;13.109953;,
-0.000001;6.203170;13.951319;,
-0.000001;-0.000000;13.951319;,
-1.960519;6.203170;5.386487;,
-0.000000;-0.000000;5.732179;,
-0.000000;6.203170;5.732179;,
-1.960519;0.000000;5.386487;,
-0.000001;-0.000000;13.951319;,
-0.000000;-0.000000;5.732179;,
-1.960519;0.000000;5.386487;,
-4.771628;-0.000000;13.109953;,
-0.000001;-0.000000;13.951319;;

 144;
3;2,1,0;,
3;1,3,0;,
3;6,5,4;,
3;5,7,4;,
3;10,9,8;,
3;9,11,8;,
3;14,13,12;,
3;17,16,15;,
3;20,19,18;,
3;19,21,18;,
3;23,22,6;,
3;22,5,6;,
3;26,25,24;,
3;25,9,24;,
3;29,28,27;,
3;32,31,30;,
3;34,33,20;,
3;33,19,20;,
3;36,35,23;,
3;35,22,23;,
3;39,38,37;,
3;38,25,37;,
3;42,41,40;,
3;45,44,43;,
3;47,46,34;,
3;46,33,34;,
3;49,48,36;,
3;48,35,36;,
3;52,51,50;,
3;51,38,50;,
3;55,54,53;,
3;58,57,56;,
3;60,59,47;,
3;59,46,47;,
3;62,61,49;,
3;61,48,49;,
3;65,64,63;,
3;64,51,66;,
3;69,68,67;,
3;72,71,70;,
3;74,73,60;,
3;73,59,60;,
3;76,75,62;,
3;75,61,62;,
3;79,78,77;,
3;78,64,77;,
3;82,81,80;,
3;85,84,83;,
3;87,86,74;,
3;86,73,74;,
3;89,88,76;,
3;88,75,76;,
3;92,91,90;,
3;91,78,90;,
3;95,94,93;,
3;98,97,96;,
3;100,99,87;,
3;99,86,87;,
3;102,101,89;,
3;101,88,89;,
3;105,104,103;,
3;104,91,103;,
3;108,107,106;,
3;111,110,109;,
3;113,112,100;,
3;112,99,100;,
3;115,114,102;,
3;114,101,102;,
3;118,117,116;,
3;117,104,116;,
3;121,120,119;,
3;124,123,122;,
3;126,125,113;,
3;125,112,113;,
3;128,127,115;,
3;127,114,115;,
3;131,130,129;,
3;130,132,129;,
3;135,134,133;,
3;138,137,136;,
3;141,140,139;,
3;140,142,139;,
3;144,143,128;,
3;143,127,128;,
3;147,146,145;,
3;146,130,145;,
3;150,149,148;,
3;153,152,151;,
3;155,154,141;,
3;154,140,141;,
3;157,156,144;,
3;156,143,144;,
3;160,159,158;,
3;159,146,158;,
3;163,162,161;,
3;166,165,164;,
3;168,167,155;,
3;167,154,155;,
3;170,169,157;,
3;169,156,157;,
3;173,172,171;,
3;172,159,171;,
3;176,175,174;,
3;179,178,177;,
3;181,180,168;,
3;180,167,168;,
3;183,182,170;,
3;182,169,170;,
3;186,185,184;,
3;185,172,184;,
3;189,188,187;,
3;192,191,190;,
3;194,193,181;,
3;193,180,181;,
3;196,195,183;,
3;195,182,183;,
3;199,198,197;,
3;198,185,197;,
3;202,201,200;,
3;205,204,203;,
3;207,206,194;,
3;206,193,194;,
3;209,208,196;,
3;208,195,196;,
3;212,211,210;,
3;211,198,210;,
3;215,214,213;,
3;218,217,216;,
3;220,219,207;,
3;219,206,207;,
3;222,221,209;,
3;221,208,209;,
3;225,224,223;,
3;224,211,223;,
3;228,227,226;,
3;231,230,229;,
3;233,232,220;,
3;232,219,220;,
3;4,7,222;,
3;7,221,222;,
3;236,235,234;,
3;235,224,234;,
3;239,238,237;,
3;242,241,240;;
MeshMaterialList {
 1;
 144;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
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
 0.000000;0.000000;0.000000;;
TextureFilename {
"MAYA.tga";
}
 }
}

 MeshNormals {
 243;
0.173648;0.000000;0.984808;,
0.173648;0.000000;0.984808;,
0.173648;0.000000;0.984808;,
0.173648;0.000000;0.984808;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.173648;0.000000;-0.984808;,
-0.286295;0.000000;-0.958142;,
-0.173648;0.000000;-0.984808;,
-0.173648;0.000000;-0.984808;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
0.500000;0.000000;0.866026;,
0.596733;0.000000;0.802440;,
0.686627;0.000000;0.727009;,
0.500000;0.000000;0.866026;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.500000;0.000000;-0.866025;,
-0.596733;0.000000;-0.802440;,
-0.500000;0.000000;-0.866025;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.835196;0.000000;0.549952;,
0.893871;0.000000;0.448325;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.766045;0.000000;-0.642788;,
-0.835196;0.000000;-0.549952;,
-0.766045;0.000000;-0.642788;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
0.972922;0.000000;0.231132;,
0.993300;0.000000;0.115566;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.939693;0.000000;-0.342020;,
-0.972922;0.000000;-0.231132;,
-0.939693;0.000000;-0.342020;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.993300;0.000000;-0.115566;,
0.972922;0.000000;-0.231132;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-0.993300;0.000000;0.115566;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
0.893871;0.000000;-0.448325;,
0.835196;0.000000;-0.549952;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.939693;0.000000;0.342020;,
-0.893871;0.000000;0.448325;,
-0.939693;0.000000;0.342020;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
0.686627;0.000000;-0.727010;,
0.596733;0.000000;-0.802440;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.766044;0.000000;0.642788;,
-0.686627;0.000000;0.727010;,
-0.766044;0.000000;0.642788;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.396567;0.000000;-0.918006;,
0.286295;0.000000;-0.958142;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.500000;0.000000;0.866025;,
-0.396567;0.000000;0.918006;,
-0.500000;0.000000;0.866025;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
0.058674;0.000000;-0.998277;,
-0.058675;0.000000;-0.998277;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-0.173648;0.000000;0.984808;,
-0.173648;0.000000;0.984808;,
-0.173648;0.000000;0.984808;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.173648;0.000000;-0.984808;,
-0.173648;0.000000;-0.984808;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.173648;0.000000;0.984808;,
0.286295;0.000000;0.958142;,
0.173648;0.000000;0.984808;,
0.173648;0.000000;0.984808;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.500000;0.000000;-0.866025;,
-0.596733;0.000000;-0.802440;,
-0.686628;0.000000;-0.727009;,
-0.500000;0.000000;-0.866025;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.500000;0.000000;0.866025;,
0.596733;0.000000;0.802440;,
0.500000;0.000000;0.866025;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.835196;0.000000;-0.549952;,
-0.893871;0.000000;-0.448325;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.766045;0.000000;0.642787;,
0.835196;0.000000;0.549952;,
0.766045;0.000000;0.642787;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.972922;0.000000;-0.231132;,
-0.993300;0.000000;-0.115566;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.939693;0.000000;0.342020;,
0.972922;0.000000;0.231132;,
0.939693;0.000000;0.342020;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.993300;0.000000;0.115566;,
-0.972922;0.000000;0.231132;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
1.000000;0.000000;-0.000000;,
0.993300;0.000000;-0.115566;,
1.000000;0.000000;-0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.000000;-1.000000;0.000000;,
-0.893870;0.000000;0.448325;,
-0.835196;0.000000;0.549952;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.939692;0.000000;-0.342020;,
0.893870;0.000000;-0.448325;,
0.939692;0.000000;-0.342020;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.686627;0.000000;0.727010;,
-0.596733;0.000000;0.802440;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.766044;0.000000;-0.642788;,
0.686627;0.000000;-0.727010;,
0.766044;0.000000;-0.642788;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
-0.396567;0.000000;0.918006;,
-0.286295;0.000000;0.958142;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.500000;0.000000;-0.866026;,
0.396567;0.000000;-0.918006;,
0.500000;0.000000;-0.866026;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.173648;0.000000;0.984808;,
-0.173648;0.000000;0.984808;,
0.173648;0.000000;-0.984808;,
0.173648;0.000000;-0.984808;,
0.173648;0.000000;-0.984808;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
-0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;,
0.000000;-1.000000;-0.000000;;

 144;
3;2,1,0;,
3;1,3,0;,
3;6,5,4;,
3;5,7,4;,
3;10,9,8;,
3;9,11,8;,
3;14,13,12;,
3;17,16,15;,
3;20,19,18;,
3;19,21,18;,
3;23,22,6;,
3;22,5,6;,
3;26,25,24;,
3;25,9,24;,
3;29,28,27;,
3;32,31,30;,
3;34,33,20;,
3;33,19,20;,
3;36,35,23;,
3;35,22,23;,
3;39,38,37;,
3;38,25,37;,
3;42,41,40;,
3;45,44,43;,
3;47,46,34;,
3;46,33,34;,
3;49,48,36;,
3;48,35,36;,
3;52,51,50;,
3;51,38,50;,
3;55,54,53;,
3;58,57,56;,
3;60,59,47;,
3;59,46,47;,
3;62,61,49;,
3;61,48,49;,
3;65,64,63;,
3;64,51,66;,
3;69,68,67;,
3;72,71,70;,
3;74,73,60;,
3;73,59,60;,
3;76,75,62;,
3;75,61,62;,
3;79,78,77;,
3;78,64,77;,
3;82,81,80;,
3;85,84,83;,
3;87,86,74;,
3;86,73,74;,
3;89,88,76;,
3;88,75,76;,
3;92,91,90;,
3;91,78,90;,
3;95,94,93;,
3;98,97,96;,
3;100,99,87;,
3;99,86,87;,
3;102,101,89;,
3;101,88,89;,
3;105,104,103;,
3;104,91,103;,
3;108,107,106;,
3;111,110,109;,
3;113,112,100;,
3;112,99,100;,
3;115,114,102;,
3;114,101,102;,
3;118,117,116;,
3;117,104,116;,
3;121,120,119;,
3;124,123,122;,
3;126,125,113;,
3;125,112,113;,
3;128,127,115;,
3;127,114,115;,
3;131,130,129;,
3;130,132,129;,
3;135,134,133;,
3;138,137,136;,
3;141,140,139;,
3;140,142,139;,
3;144,143,128;,
3;143,127,128;,
3;147,146,145;,
3;146,130,145;,
3;150,149,148;,
3;153,152,151;,
3;155,154,141;,
3;154,140,141;,
3;157,156,144;,
3;156,143,144;,
3;160,159,158;,
3;159,146,158;,
3;163,162,161;,
3;166,165,164;,
3;168,167,155;,
3;167,154,155;,
3;170,169,157;,
3;169,156,157;,
3;173,172,171;,
3;172,159,171;,
3;176,175,174;,
3;179,178,177;,
3;181,180,168;,
3;180,167,168;,
3;183,182,170;,
3;182,169,170;,
3;186,185,184;,
3;185,172,184;,
3;189,188,187;,
3;192,191,190;,
3;194,193,181;,
3;193,180,181;,
3;196,195,183;,
3;195,182,183;,
3;199,198,197;,
3;198,185,197;,
3;202,201,200;,
3;205,204,203;,
3;207,206,194;,
3;206,193,194;,
3;209,208,196;,
3;208,195,196;,
3;212,211,210;,
3;211,198,210;,
3;215,214,213;,
3;218,217,216;,
3;220,219,207;,
3;219,206,207;,
3;222,221,209;,
3;221,208,209;,
3;225,224,223;,
3;224,211,223;,
3;228,227,226;,
3;231,230,229;,
3;233,232,220;,
3;232,219,220;,
3;4,7,222;,
3;7,221,222;,
3;236,235,234;,
3;235,224,234;,
3;239,238,237;,
3;242,241,240;;
 }
MeshTextureCoords {
 243;
0.848758;0.128660;,
0.970164;0.090495;,
0.848758;0.090495;,
0.970164;0.128660;,
0.409485;0.076047;,
0.465695;0.321611;,
0.546293;0.100170;,
0.409485;0.311699;,
0.833225;0.698538;,
0.985886;0.680696;,
0.833225;0.680696;,
0.985886;0.698538;,
0.409188;0.639915;,
0.545668;0.850936;,
0.465263;0.630027;,
0.409188;0.639915;,
0.409188;0.875001;,
0.545668;0.850936;,
0.984857;0.103431;,
0.830934;0.124308;,
0.984857;0.124308;,
0.830934;0.103431;,
0.515126;0.350150;,
0.666600;0.169630;,
0.833225;0.680696;,
0.985886;0.629320;,
0.833225;0.629320;,
0.465263;0.630027;,
0.665686;0.781644;,
0.514575;0.601557;,
0.465263;0.630027;,
0.545668;0.850936;,
0.665686;0.781644;,
0.830934;0.184422;,
0.984857;0.184422;,
0.551814;0.393873;,
0.755895;0.276047;,
0.833225;0.629320;,
0.985886;0.550609;,
0.833225;0.550609;,
0.514575;0.601557;,
0.754767;0.675481;,
0.551176;0.557938;,
0.514575;0.601557;,
0.665686;0.781644;,
0.754767;0.675481;,
0.830934;0.276521;,
0.984857;0.276521;,
0.571336;0.447509;,
0.803408;0.406588;,
0.833225;0.550609;,
0.985886;0.454055;,
0.833225;0.454055;,
0.551176;0.557938;,
0.802165;0.545254;,
0.570651;0.504432;,
0.551176;0.557938;,
0.754767;0.675481;,
0.802165;0.545254;,
0.830934;0.389499;,
0.984857;0.389499;,
0.571336;0.504586;,
0.803408;0.545507;,
0.833225;0.454055;,
0.985886;0.351305;,
0.833225;0.351305;,
0.833225;0.454055;,
0.570651;0.504432;,
0.802165;0.406669;,
0.570651;0.447491;,
0.570651;0.504432;,
0.802165;0.545254;,
0.802165;0.406669;,
0.830934;0.509726;,
0.984857;0.509726;,
0.551814;0.558221;,
0.755895;0.676047;,
0.833225;0.351305;,
0.985886;0.254751;,
0.833225;0.254751;,
0.570651;0.447491;,
0.754767;0.276442;,
0.551176;0.393985;,
0.570651;0.447491;,
0.802165;0.406669;,
0.754767;0.276442;,
0.830934;0.622703;,
0.984857;0.622703;,
0.515125;0.601945;,
0.666600;0.782465;,
0.833225;0.254751;,
0.985886;0.176040;,
0.833225;0.176040;,
0.551176;0.393985;,
0.665686;0.170279;,
0.514575;0.350366;,
0.551176;0.393985;,
0.754767;0.276442;,
0.665686;0.170279;,
0.830934;0.714803;,
0.984857;0.714803;,
0.465695;0.630484;,
0.546293;0.851924;,
0.833225;0.176040;,
0.985886;0.124664;,
0.833225;0.124664;,
0.514575;0.350366;,
0.545668;0.100987;,
0.465263;0.321896;,
0.514575;0.350366;,
0.665686;0.170279;,
0.545668;0.100987;,
0.830934;0.774917;,
0.984857;0.774917;,
0.409485;0.640395;,
0.409485;0.876047;,
0.833225;0.124664;,
0.985886;0.106822;,
0.833225;0.106822;,
0.465263;0.321896;,
0.409188;0.076922;,
0.409188;0.312008;,
0.465263;0.321896;,
0.545668;0.100987;,
0.409188;0.076922;,
0.830934;0.795794;,
0.984857;0.795794;,
0.353274;0.630484;,
0.272677;0.851924;,
0.826163;0.496593;,
0.984838;0.487750;,
0.826163;0.487750;,
0.984838;0.496593;,
0.409188;0.312008;,
0.272708;0.100987;,
0.353113;0.321896;,
0.409188;0.312008;,
0.409188;0.076922;,
0.272708;0.100987;,
0.989123;0.117575;,
0.830148;0.160045;,
0.989123;0.160045;,
0.830148;0.117575;,
0.303844;0.601945;,
0.152369;0.782465;,
0.826163;0.487750;,
0.984838;0.462288;,
0.826163;0.462288;,
0.353113;0.321896;,
0.152690;0.170280;,
0.303801;0.350366;,
0.353113;0.321896;,
0.272708;0.100987;,
0.152690;0.170280;,
0.830148;0.239863;,
0.989123;0.239863;,
0.267155;0.558221;,
0.063074;0.676047;,
0.826163;0.462288;,
0.984838;0.423279;,
0.826163;0.423279;,
0.303801;0.350366;,
0.063609;0.276442;,
0.267200;0.393985;,
0.303801;0.350366;,
0.152690;0.170280;,
0.063609;0.276442;,
0.830148;0.347400;,
0.989123;0.347400;,
0.247633;0.504586;,
0.015562;0.545507;,
0.826163;0.423279;,
0.984838;0.375427;,
0.826163;0.375427;,
0.267200;0.393985;,
0.016211;0.406669;,
0.247725;0.447491;,
0.267200;0.393985;,
0.063609;0.276442;,
0.016211;0.406669;,
0.830148;0.469688;,
0.989123;0.469688;,
0.247633;0.447509;,
0.015562;0.406588;,
0.826163;0.375427;,
0.984838;0.324503;,
0.826163;0.324503;,
0.247725;0.447491;,
0.016211;0.545254;,
0.247725;0.504432;,
0.247725;0.447491;,
0.016211;0.406669;,
0.016211;0.545254;,
0.830148;0.591975;,
0.989123;0.591975;,
0.267155;0.393873;,
0.063075;0.276047;,
0.826163;0.324503;,
0.984838;0.276651;,
0.826163;0.276651;,
0.247725;0.504432;,
0.063610;0.675481;,
0.267200;0.557938;,
0.247725;0.504432;,
0.016211;0.545254;,
0.063610;0.675481;,
0.830148;0.699513;,
0.989123;0.699513;,
0.303844;0.350149;,
0.152370;0.169630;,
0.826163;0.276651;,
0.984838;0.237642;,
0.826163;0.237642;,
0.267200;0.557938;,
0.152690;0.781644;,
0.303801;0.601557;,
0.267200;0.557938;,
0.063610;0.675481;,
0.152690;0.781644;,
0.830148;0.779331;,
0.989123;0.779331;,
0.353274;0.321611;,
0.272677;0.100170;,
0.826163;0.237642;,
0.984838;0.212180;,
0.826163;0.212180;,
0.303801;0.601557;,
0.272709;0.850936;,
0.353113;0.630027;,
0.303801;0.601557;,
0.152690;0.781644;,
0.272709;0.850936;,
0.830148;0.821801;,
0.989123;0.821801;,
0.826163;0.212180;,
0.984838;0.203337;,
0.826163;0.203337;,
0.353113;0.630027;,
0.409188;0.875001;,
0.409188;0.639915;,
0.353113;0.630027;,
0.272709;0.850936;,
0.409188;0.875001;;
}
}
 }
