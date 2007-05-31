//========================================================================
/** @type     C/C++ Header File
 *  @file     Load.h
 *  @author   alcoco
 *  @date
 *   Created:       18:19:23 29/03/2005
 *   Last Update:   20:45:01 30/05/2007
 */
//========================================================================

#ifndef __LOAD3DS_H__
#define __LOAD3DS_H__

#include <CDL/GLEngine/Model.h>

namespace CDL
{
    using CDL::File;

    MeshObject load3DS(const string &);
    MeshObject load3DS(File &fp);
}

#endif//__LOAD3DS_H__
