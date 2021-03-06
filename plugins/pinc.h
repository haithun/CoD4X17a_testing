#ifndef PLUGIN_INCLUDES
#define PLUGIN_INCLUDES

// Enums used in this file
#define C 0
#define CPP 1
#define GCC 2
#define BORLAND 3
#define INTEL 4
#define UNKNOWN 5


// Detect the language (C/C++)

#ifdef LANG
    #undef LANG
#endif
#ifdef LANG_NAME
    #undef LANG_NAME
#endif

#ifdef __cplusplus
    #define LANG CPP
    #define LANG_NAME "C++"
#else
    #define LANG C
    #define LANG_NAME "C"
#endif


// Detect the compiler

#ifdef COMPILER
    #undef COMPILER
#endif
#ifdef COMPILER_NAME
    #undef COMPILER_NAME
#endif

#ifdef __GNUC__ 		// GCC
    #define COMPILER GCC
    #define COMPILER_NAME "GCC / G++"
#else
#ifdef __BORLANDC__	// Borland C++
    #define COMPILER BORLAND
    #define COMPILER_NAME "Borland C++"
#else
#ifdef __INTEL_COMPILER	// Intel C Compiler
    #define COMPILER INTEL
    #define COMPILER_NAME "Intel Compiler"
#else				// Some other compiler
    #define COMPILER UNKNOWN
    #define COMPILER_NAME "unknown"
#endif
#endif
#endif

// Show a compilation message
#ifdef msg
#undef msg
#endif

#define msg "Compiling a plugin using IceOps Plugin API. Your compiler is " COMPILER_NAME ", you are using " LANG_NAME "."

#pragma message msg

#undef msg

#define CONVAR_T convariable_t

#ifndef __cdecl
    #define __cdecl __attribute__((cdecl))
#endif /*__cdecl*/

#ifdef PCL
    #undef PCL
#endif /*PCL*/

#ifdef __iceimport
    #undef __iceimport
#endif /*__iceimport*/

#define __iceimport extern __cdecl

#if LANG == CPP
    #define PCL extern "C" __attribute__ ((visibility ("default"))) __attribute__ ((cdecl))
#else
    #define PCL __attribute__ ((visibility ("default"))) __attribute__ ((cdecl))
#endif /*LANG == CPP*/

#define Com_Memset memset

#if LANG == CPP
extern "C"{
#endif

#include "declarations.h"

#if LANG == CPP
}
#endif

// We don't need those any more :P
#undef C
#undef CPP
#undef GCC
#undef BORLAND
#undef INTEL
#undef UNKNOWN


#else

#error Please include the Plugin Include file (pinc) only once!

#endif
