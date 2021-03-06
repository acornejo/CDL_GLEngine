//========================================================================
/** @type     C/C++ Header File
 *  @file     MultiTexture.h
 *  @author   acornejo
 *  @date
 *   Created:       21:24:46 12/06/2005
 *   Last Update:   00:11:09 31/05/2007
 */
//========================================================================

#ifndef __MULTITEXTURE_H__
#define __MULTITEXTURE_H__

#include <CDL/GLEngine/Texture.h>
#include <vector>

namespace CDL
{
    class MultiTexture
    {
        private:
            std::vector<Texture> m_texture;

        public:
            MultiTexture();
            MultiTexture(const MultiTexture &);
            MultiTexture(const Texture &);
            virtual ~MultiTexture();
            const MultiTexture &operator=(const MultiTexture &);

            void texCoord(const Vec2t &) const;
            void texCoord2f(const float &, const float &) const;
            void texCoord2fv(const float *) const;

            void clear();
            size_t getTextureCount() const;
            void addTexture(const Texture &);
            void setTexture(const size_t &, const Texture &);
            const Texture &getTexture(const size_t &) const;

            void select() const;
            void deselect() const;
    };
}

#endif//__MULTITEXTURE_H__
