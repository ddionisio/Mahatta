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
Frame Box01 {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
0.000000,0.000000,0.000000,1.000000;;
 }
Mesh Box011 {
 18;
94.152000;193.585999;-249.585999;,
17.777216;192.879761;-249.774658;,
94.152000;193.020004;250.151978;,
18.147419;192.243637;250.151978;,
94.152000;5.848000;-249.020004;,
-89.951355;5.848000;-249.020004;,
94.152000;193.585999;-249.585999;,
17.777216;192.879761;-249.774658;,
94.152000;193.585999;-249.585999;,
-89.951355;5.848000;249.019989;,
94.152000;5.848000;249.019989;,
17.036810;194.151993;249.019989;,
94.152000;194.151993;249.019989;,
-91.042244;1.636494;-250.151993;,
-91.042244;1.636494;250.151978;,
17.777216;192.879761;-249.774658;,
18.147419;192.243637;250.151978;,
17.777216;192.879761;-249.774658;;

 8;
3;2,1,0;,
3;1,2,3;,
3;6,5,4;,
3;5,8,7;,
3;11,10,9;,
3;10,11,12;,
3;15,14,13;,
3;14,17,16;;
MeshMaterialList {
 1;
 8;
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
"PORTE.tga";
}
 }
}

 MeshNormals {
 18;
0.009249;-0.999957;-0.001133;,
0.009732;-0.999952;-0.001206;,
0.009732;-0.999952;-0.001206;,
0.010214;-0.999947;-0.001280;,
0.000000;0.003015;0.999995;,
-0.001260;0.004251;0.999990;,
0.000000;0.003015;0.999995;,
-0.002521;0.005487;0.999982;,
-0.002521;0.005487;0.999982;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.869147;-0.494554;0.000000;,
0.868430;-0.495812;-0.000638;,
0.869147;-0.494554;0.000000;,
0.867710;-0.497069;-0.001275;,
0.867710;-0.497069;-0.001275;;

 8;
3;2,1,0;,
3;1,2,3;,
3;6,5,4;,
3;5,8,7;,
3;11,10,9;,
3;10,11,12;,
3;15,14,13;,
3;14,17,16;;
 }
MeshTextureCoords {
 18;
0.962985;1.747930;,
0.581111;1.748873;,
0.962986;-0.750760;,
0.582963;-0.750760;,
0.962985;0.960231;,
0.042469;0.960231;,
0.962985;0.021541;,
0.581111;0.025072;,
0.962985;0.021541;,
0.957531;0.960231;,
0.037014;0.960231;,
0.422590;0.018711;,
0.037014;0.018711;,
1.750760;0.981289;,
-0.750760;0.981289;,
1.748873;0.025072;,
-0.750760;0.028253;,
1.748873;0.025072;;
}
}
 }
