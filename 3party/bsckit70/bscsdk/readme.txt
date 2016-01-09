Microsoft Visual C++ Browser Toolkit v7.0
Copyright Microsoft Corporation, 1993-2000

Welcome to the Microsoft Visual C++ Browser Toolkit for Windows NT. This toolkit contains a set of powerful tools that allow the Visual C++ programmer to manipulate the contents of browser database (.BSC) files. 

The Browser Toolkit provides both C++ and ANSI C bindings of the BSC API. Both ANSI C and C++ versions of a sample utility (BD) which demonstrates the use of the Toolkit's APIs, are included in this toolkit. BD displays in various formats some of the information in a browser  database file. 

OVERVIEW OF THE TOOLKIT FILES:

bin\
  msbsc70.dll		The BSC API dll, requires DLL version of the CRT (msvcrt*.dll)
help\
  bsc.chm		BSC Help File
include\
  bsc.h			BSC API header 
lib\
  msbsc70.lib		Import library for msbsc70.dll 
samples\
  bd-cxx\
	bd.cpp		Sample bsc dump application (C++ version)
	makefile	Makefile
  bd-can\
	bd.c		Sample bsc dump application (C version)
	makefile	Makefile

