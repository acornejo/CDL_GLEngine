//========================================================================
/** @type     C/C++ Header File
 *  @file     RenderableObject.h
 *  @author   acornejo
 *  @date
 *   Created:       11:03:31 02/10/2006
 *   Last Update:   11:04:12 02/10/2006
 */
//========================================================================

#ifndef __RENDERABLEOBJECT_H__
#define __RENDERABLEOBJECT_H__

class RenderableObject
{
    public:
        virtual void render() const=0;
};

#endif//__RENDERABLEOBJECT_H__
