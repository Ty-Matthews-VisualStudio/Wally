// Genesis3D.h: interface for the CGenesis3D class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GENESIS3D_H_
#define _GENESIS3D_H_


// The Genesis3D SDK requires compiling with VC6
#if _MSC_VER >= 1200
#ifdef GENESIS_3D

#pragma once

#include "./GenesisSDK/include/bitmap.h"
#include "./GenesisSDK/include/ram.h"
#include "./GenesisSDK/include/genesis.h"


BOOL AddTexture (geVFile *BaseFile, const char *Path, CPackageDoc *pDoc);
void LoadTXLFile ();


#endif // #ifdef GENESIS_3D
#endif // #if _MSC_VER >= 1200
#endif // #ifndef _GENESIS3D_H_


