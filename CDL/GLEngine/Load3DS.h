//========================================================================
/** @type     C/C++ Header File
 *  @file     Load.h
 *  @author   alcoco
 *  @date
 *   Created:       18:19:23 29/03/2005
 *   Last Update:   16:25:33 13/09/2006
 */
//========================================================================

#ifndef __LOAD3DS_H__
#define __LOAD3DS_H__

#include <CDL/GLEngine/Model.h>

namespace CDL
{
    using CDL::File;

    MeshObject load3DS(const char *);
    MeshObject load3DS(File &fp);
}

#endif//__LOAD3DS_H__
