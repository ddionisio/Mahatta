#ifndef _cfg_h
#define _cfg_h

//This includes different types of config files:
//Our plain vanilla *.ini and *.cfg
//Quake 3 Shader files *.shader

//The Config File Loader/Saver

/*
ex. inside a file:
[section]
item1=something
item2=10

#comment

item3=1.0
[section2]
...
*/

/////////////////////////////////////
// Name:	CfgFileLoad
// Purpose:	load a config file
// Output:	config file loaded
// Return:	the new config file
/////////////////////////////////////
hCFG CfgFileLoad(const tCHAR *filename);

//The Shader File Loader

/*
ex. inside a file:
section
{
	var		param
	var2	param param2

	// comment
}

section2 {
	var param
}
*/

/////////////////////////////////////
// Name:	ShaderFileLoad
// Purpose:	loads a given shader file
// Output:	shader file ready to be parsed
// Return:	something new
/////////////////////////////////////
hSHADERF ShaderFileLoad(const tCHAR *filename);

#endif