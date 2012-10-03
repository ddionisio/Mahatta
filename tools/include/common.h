#ifndef COMMON_H 
#define COMMON_H

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4630 )

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/
#ifdef	_MSC_VER
	// The Borland compiler gives lots of warnings if you redefine the assert
	// macro - I haven't explored how to avoid them so I'll avoid redefining it.
	#define	USEMICROSOFTASSERTDIALOG
#endif
#ifdef	USEMICROSOFTASSERTDIALOG
	#include <assert.h>
#else
	#undef	assert	// Make sure we've wiped out any previous definitions.
	#ifdef  NDEBUG
		// In release mode, asserts do nothing.
		#define assert(exp)     ((void)0)
	#else
		// In debug mode we need an assert macro and an assert function.
		// _csassert() returns true if the user requests debugging.
		bool _csassert(const tCHAR* expr, const tCHAR* filename, unsigned int lineno);
		#define assert(exp) \
				do { \
					if (!(exp)) { \
						dprintf("%s(%d): Assertion failed\n%s\n", __FILE__, __LINE__, #exp); \
						if (_csassert(#exp, __FILE__, __LINE__)) \
							DEBUG_BREAK(); \
						} \
				} while (0)
	#endif
#endif


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// For DLL stuff

#if (!defined(WIN32) && !defined(_WIN32) && !defined(__WIN32__) && !defined(_WIN32_WCE) && !defined(_XBOX)) || (defined(__GNUC__) && defined(WIN32))
    #ifndef __cdecl
        #define __cdecl
    #endif
    #ifndef __stdcall
        #define __stdcall
    #endif
#endif 

#if defined(_WIN32_WCE)
    #define F_API _cdecl
    #define F_CALLBACKAPI _cdecl
#else
    #define F_API __stdcall
    #define F_CALLBACKAPI __stdcall
#endif

#ifdef DLL_EXPORTS
    #define DLL_API __declspec(dllexport)
#else
    #if defined(__LCC__) || defined(__MINGW32__) || defined(__CYGWIN32__)
        #define DLL_API F_API
    #else
        #define DLL_API
    #endif /* __LCC__ ||  __MINGW32__ || __CYGWIN32__ */
#endif /* DLL_EXPORTS */

#ifdef WIN32
    #define MOD_GETPROC(_x, _y)                                                                       \
    {                                                                                             \
        *((u32 *)&instance->_x) = (u32)GetProcAddress((HMODULE)instance->module, _y);    \
        if (!instance->_x)                                                                        \
        {                                                                                         \
            FreeLibrary((HMODULE)instance->module);                                                        \
            free(instance);                                                                       \
            return NULL;                                                                          \
        }                                                                                         \
    }
#else
    #define MOD_GETPROC(_x, _y)                                                                       \
    {                                                                                             \
        char tmp[] = _y;                                                                          \
        *(strchr(tmp, '@')) = 0;                                                                  \
        *((u32 *)&instance->_x) = (u32)dlsym(instance->module, &tmp[1]);        \
        if (!instance->_x)                                                                        \
        {                                                                                         \
            dlclose(instance->module);                                                            \
            free(instance);                                                                       \
            return NULL;                                                                          \
        }                                                                                         \
    }
#endif
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


//SDL types
#include "SDL//SDL_types.h"

//standard includes
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#endif

//STL stuff
#include "string"

#include "list"
#include "stack"
#include "vector"

#include "iostream"
#include "functional"
#include "map"
#include "bitset"
#include "set"

#include "algorithm"

//#include "iostream"
//#include "functional"

using namespace std;

/********************************************************************
*																	*
*							Qualifiers								*
*																	*
********************************************************************/

#define PUBLIC				// Semantic used to indicate publicly exposed methods
#define PROTECTED			// Semantic used to indicate methods shared among implementations
#define PRIVATE		static	// Semantic used to indicate implementation-specific attributes and methods

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define PATH_SIZE	 80
#define MAX_FILES    15
#define MAXCHARBUFF  255
#define DMAXCHARBUFF 512

#define BASE_AMOUNT	1	// Count of elements of base array
#define BASE_EXTENT	1	// Index of element beyond base array

/********************************************************************
*																	*
*							Return codes							*
*																	*
********************************************************************/

#define RETCODE_BREAK	-1	// General-purpose break code
#define RETCODE_FAILURE	 0	// General-purpose failure
#define RETCODE_SUCCESS	 1	// General-purpose success

#define COMP_LT	-1	// Less than result of compare method
#define COMP_EQ	 0	// Equal result of compare method
#define COMP_GT	 1	// Greater than result of compare method

#ifndef TRUE
#define TRUE	 1
#endif

#ifndef FALSE
#define FALSE	 0
#endif

#define SEARCH_NOTFOUND	-1	// Indicates search without a find

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/
typedef signed char Schar;
typedef signed short Sshort;
typedef signed long Slong;
typedef signed int Sint;
typedef unsigned char Uchar;
typedef unsigned short Ushort;
typedef unsigned long Ulong;
typedef unsigned int Uint;

typedef Uint8				u8;
typedef Sint8				s8;
typedef Uint16				u16;
typedef Sint16				s16;
typedef Uint32				u32;
typedef Sint32				s32;
typedef Sint64				s64;
typedef Uint64				u64;
typedef float				f32;
typedef double				f64;

typedef wchar_t 			tCHAR;

typedef u32 FLAGS;		// General flags type
typedef s32 RETCODE;	// Return code type

#ifdef WIN32
typedef HWND		HMAIN;		//This is the main handle, use this instead of HWND
typedef HINSTANCE	HMAININST;	//This is the main instance handle, use this instead of HINSTANCE
typedef HMODULE		HMOD;		//The module used for DLL interface
#endif

#ifndef INLINE
#ifdef _MSC_VER
  #if (_MSC_VER >= 1200)
  #define INLINE __forceinline
  #else
  #define INLINE __inline
  #endif
#else
  #ifdef __cplusplus
  #define INLINE inline
  #else
  #define INLINE
  #endif
#endif
#endif

/********************************************************************
*																	*
*							Structs									*
*																	*
********************************************************************/
typedef struct _iRect {
   s32 left;
   s32 top;
   s32 right;
   s32 bottom;
} iRect;

typedef struct _iPoint {
	s32 x,y;
} iPoint;

/********************************************************************
*																	*
*							Methods									*
*																	*
********************************************************************/
													
typedef RETCODE (* COMPARE)	(void *, void *);	// Callback function for sorting comparisons 
typedef RETCODE (* EQUIVAL) (void *, void *);	// Callback function for searching comparisons

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define MESSAGE_BOX(Message,Caption) MessageBoxW (NULL, Message, Caption, MB_OK)	// User messages

#ifdef NDEBUG
#define ASSERT_MSG(Cond,Msg,Caption)
#else // NDEBUG
#define ASSERT_MSG(Cond,Msg,Caption)	\
	if (!(Cond)) {						\
		MESSAGE_BOX((Msg),(Caption));	\
		assert(Cond);					\
	}
#endif // NDEBUG
// Assertion message report

#define ALIGNED(data)	(((s32)(data) + sizeof(s32) - 1) & ~(sizeof(s32) - 1))
// Extract the aligned version of a datum

#define NUM_ELEMENTS(array)	((sizeof array) / (sizeof((array)[0]))) // Get the element count of a static array

#define SETFLAG(data,flag)   (data) |= (flag)	// Set a flag value in a datum
#define CLEARFLAG(data,flag) (data) &= ~(flag)	// Clear a flag value in a datum
#define FLIPFLAG(data,flag)	 (data) ^= (flag)	// Flip a flag value in a datum

#define GETFLAGS(data,mask)	 ((data) & (mask))				// Grab flags from a datum : partial
#define TESTFLAGS(data,mask) (((data) & (mask)) == (mask))	// Grab flags from a datum : total

#define MAX(one,two) ((one) > (two) ? (one) : (two))
#define MIN(one,two) ((one) < (two) ? (one) : (two))
#define GETSIGN(val) ((val) < 0 ? -1 : 1)

#define MAKETAG_4(ch0, ch1, ch2, ch3)							\
                ((u32)(u8)(ch0) | ((u32)(u8)(ch1) << 8) |       \
                ((u32)(u8)(ch2) << 16) | ((u32)(u8)(ch3) << 24 ))

// Constructs an integer tag from four characters

/////////////////////////////////
//for your convenience...
#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((s16)(((u8)((u32)(a) & 0xff)) | ((s16)((u8)((u32)(b) & 0xff))) << 8))
#endif

#ifndef MAKELONG
#define MAKELONG(a, b)      ((s32)(((s16)((u32)(a) & 0xffff)) | ((u32)((s16)((u32)(b) & 0xffff))) << 16))
#endif

#ifndef LOWORD
#define LOWORD(l)           ((s16)((u32)(l) & 0xffff))
#endif

#ifndef HIWORD
#define HIWORD(l)           ((s16)((u32)(l) >> 16))
#endif

#ifndef LOBYTE
#define LOBYTE(w)           ((u8)((u32)(w) & 0xff))
#endif

#ifndef HIBYTE
#define HIBYTE(w)           ((u8)((u32)(w) >> 8))
#endif
//////////////////////////////////

//Gets the index of an array as if it was a double array
//Rip-off from the CS170 sliding puzzle assignment, hehehe
#define CELLIND(row, col, numcols) (((row) * (numcols)) + (col))
#define CELL(grid, row, col, numcols) ((grid)[CELLIND(row,col,numcols)])

#ifdef WIN32
#define LOAD_LIB(path) (LoadLibrary(path))
#define FREE_LIB(lib) (FreeLibrary(lib))
#define GET_PROC_ADDR(lib, procName) (GetProcAddress(lib, procName))
#endif

#endif // COMMON_H