#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <CDL/CDL.h>

namespace CDL
{
    using CDL::Image;
    using CDL::Vec2t;

    class Texture
    {
        public:
            typedef enum {CLAMP=1<<0, MIPMAP=1<<1, NOSCALE=1<<2} Texture_Filter;
            typedef enum {DECAL=1<<3, BLEND=1<<4, REPLACE=1<<5, ADD=1<<6, DETAIL=1<<7, CLOUDS=1<<8, NIGHTLIGHTS=1<<9,\
            NIGHT=1<<10, LODTEX=1<<11} Texture_Type;

        protected:
            static size_t m_textureCount;
            static size_t m_textureBytes;
            static size_t m_tuCount;
            static size_t m_currTu;
            size_t *m_ref;
            size_t  m_ID;
			size_t  m_flag;
            size_t  m_fmt;
			size_t  m_width;
            size_t  m_height;
            size_t  m_unit;
            float m_angle;
            float m_uscale;
            float m_vscale;
            float m_uoffset;
            float m_voffset;
            Texture *m_patch;

        public:
            Texture(const size_t &u=0);
            Texture(const Image &, const size_t &t=MIPMAP, const float &a=0,\
                    const float &us=1, const float &vs=1, const float &uo=0, const float &vo=0, const size_t &l=0);
            Texture(const Texture &);
            virtual ~Texture();
            Texture& operator=(const Texture &);

            void texCoord2f(const float &, const float &) const;
            void texCoord2fv(const float *) const;
            void texCoord(const Vec2t &) const;

            void setAngle(const float &);
            const float &getAngle() const;
            void setUScale(const float &);
            const float &getUScale() const;
            void setVScale(const float &);
            const float &getVScale() const;
            void setUOffset(const float &);
            const float &getUOffset() const;
            void setVOffset(const float &);
            const float &getVOffset() const;
            void setUnit(const size_t &);
            const size_t &getUnit() const;
            const size_t &getFlags() const  {return m_flag;}
            const size_t &getWidth() const  {return m_width;}
            const size_t &getHeight() const {return m_height;}

			void write(const Image &);
            void selectPatch(const Vec2t &, const Vec2t &);
            void select() const;
            void deselect() const;
			bool isValid() const;

            static const size_t &getTextureCount();
            static const size_t &getTextureBytes();
            static const size_t &getTextureUnitCount();
            static const size_t &getCurrentTextureUnit();
    };
}
#endif
