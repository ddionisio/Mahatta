#ifndef _skybox_h
#define _skybox_h

void SkyDestroy();

//call this to load skybox
u32 SkyLoad(const tCHAR *modelfile);

//this is called in WorldRender
void SkyRender();

#endif