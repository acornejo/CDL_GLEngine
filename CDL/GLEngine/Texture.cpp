#include <CDL/GLEngine/Texture.h>
#include <GL/glew.h>
#include <GL/glu.h>

#define type GL_UNSIGNED_BYTE
#define TEXMAT (1<<31)
#define SPLITX (1<<30)
#define SPLITY (1<<29)

namespace CDL
{
    DEFCLASS("Texture");

    size_t Texture::m_textureCount=0;
    size_t Texture::m_textureBytes=0;
    size_t Texture::m_tuCount=0;
    size_t Texture::m_currTu=0;
    static size_t maxsize=0;

    size_t roundToPowerOfTwo(const size_t &x)
    {
        size_t r=1;

        while (x > r)
            r<<=1;

        return r;
    }

    size_t getSizeOfMipMaps(const size_t &x, const size_t &y)
    {
        size_t w=x, h=y;
        size_t total=w*h;

        while (w > 1 || h > 1)
        {
            if (w > 1) w>>=1;
            if (h > 1) h>>=1;
            total+=w*h;
        }

        return total;
    }

    size_t getChannels(const size_t &fmt)
    {
        switch (fmt)
        {
            case GL_LUMINANCE:
                return 1;
            case GL_LUMINANCE_ALPHA:
                return 2;
            case GL_RGB:
            case GL_BGR:
                return 3;
            case GL_RGBA:
            case GL_BGRA:
                return 4;
            default:
                Error_send("Invalid image format to loaded\n");
                return 0;
        }
    }

    size_t getFormat(const Image::Image_type &fmt)
    {
        switch (fmt)
        {
            case Image::LUMINANCE:
                return GL_LUMINANCE;
            case Image::LUMINANCE_ALPHA:
                return GL_LUMINANCE_ALPHA;
            case Image::RGB:
                return GL_RGB;
            case Image::BGR:
                return GL_BGR;
            case Image::RGBA:
                return GL_RGBA;
            case Image::BGRA:
                return GL_BGRA;
            default:
                Error_send("Invalid image format to load as texture\n");
                return 0;
        }
    }

    void init(const size_t &maxsize, const size_t &m_tuCount)
    {
         if (maxsize == 0)
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, (int *)&maxsize);
        if (m_tuCount == 0)
            glGetIntegerv(GL_MAX_TEXTURE_UNITS, (int *)&m_tuCount);
        glewExperimental=true;
        glewInit();
    }

    Texture::Texture(const size_t &unit)
    {
        init(maxsize, m_tuCount);
        m_ID=0;
        m_ref=new size_t(1);
        m_flag=0;
        m_fmt=0;
        m_width=0;
        m_height=0;
        m_angle=0;
        m_uscale=1;
        m_vscale=1;
        m_uoffset=0;
        m_voffset=0;
        m_unit=unit;
        m_patch='\0';
    }

    Texture::Texture(const Texture &t)
    {
        m_ID=t.m_ID;
        m_ref=t.m_ref;
        ++(*m_ref);
        m_flag=t.m_flag;
        m_fmt=t.m_fmt;
        m_width=t.m_width;
        m_height=t.m_height;
        m_angle=t.m_angle;
        m_uscale=t.m_uscale;
        m_vscale=t.m_vscale;
        m_uoffset=t.m_uoffset;
        m_voffset=t.m_voffset;
        m_unit=t.m_unit;
        m_patch=t.m_patch;
    }

    Texture::Texture(const Image &i, const size_t &flag, const float &angle, const float &uscale, const float &vscale, const float &uoffset, const float &voffset, const size_t &level)
    {
        size_t w=roundToPowerOfTwo(i.getWidth()), h=roundToPowerOfTwo(i.getHeight()), w2=w>>1, h2=h>>1;
        unsigned short xlevel=LOWORD(level)+1, ylevel=LOWORD(level)+1;

        init(maxsize, m_tuCount);
        glGenTextures(1, (GLuint *)&m_ID);
        m_ref=new size_t(1);
        m_flag=flag;
        m_fmt=getFormat(i.getType());
        m_width=MIN(w,maxsize);
        m_height=MIN(h,maxsize);
        setAngle(angle);
        setUScale(uscale);
        setVScale(vscale);
        setUOffset(uoffset);
        setVOffset(voffset);
        m_unit=0;
        m_patch='\0';

        select();
        glPixelStorei(GL_PACK_ALIGNMENT,   1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (m_flag&CLAMP)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }

        if (m_flag&MIPMAP)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            m_textureBytes+=getSizeOfMipMaps(m_width,m_height)*getChannels(m_fmt);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D,0,getChannels(m_fmt),m_width,m_height,0,m_fmt,type,NULL);
            m_textureBytes+=m_width*m_height*getChannels(m_fmt);
        }

        m_textureCount++;
        write(i);

        if (m_flag&LODTEX && (w>maxsize || h>maxsize))
        {
            Image nimage(w, h, i.getType());
            nimage.convert(i);
            m_patch=new Texture[2];
            if (w>maxsize)
            {
                const float &d=1.0f/(1<<xlevel);
                const size_t nlevel=MAKEDWORD(xlevel, HIWORD(level));
                Image img0(w2, h, i.getType()), img1(w2, h, i.getType());
                for (int i=0; i<w2; i++)
                    for (int j=0; j<h; j++)
                        for (int k=0; k<getChannels(m_fmt); k++)
                        {
                            img0(i,j,k)=nimage(i,j,k);
                            img1(i,j,k)=nimage(i+w2,j,k);
                        }
                m_patch[0]=Texture(img0, m_flag|CLAMP, m_angle, m_uscale, m_vscale, m_uoffset  , m_voffset, nlevel);
                m_patch[1]=Texture(img1, m_flag|CLAMP, m_angle, m_uscale, m_vscale, m_uoffset+d, m_voffset, nlevel);
                m_flag|=SPLITX;
            }
            else
            {
                const float &d=1.0f/(1<<ylevel);
                const size_t nlevel=MAKEDWORD(LOWORD(level), ylevel);
                Image img0(w, h2, i.getType()), img1(w, h2, i.getType());
                for (int i=0; i<w; i++)
                    for (int j=0; j<h2; j++)
                        for (int k=0; k<getChannels(m_fmt); k++)
                        {
                            img0(i,j,k)=nimage(i,j,k);
                            img1(i,j,k)=nimage(i,j+h2,k);
                        }
                m_patch[0]=Texture(img0, m_flag|CLAMP, m_angle, m_uscale, m_vscale, m_uoffset, m_voffset  , nlevel);
                m_patch[1]=Texture(img1, m_flag|CLAMP, m_angle, m_uscale, m_vscale, m_uoffset, m_voffset+d, nlevel);
                m_flag|=SPLITY;
            }
        }
    }

    Texture::~Texture()
    {
        if (!--(*m_ref))
        {
            if (m_ID)
            {
                m_textureCount--;
                if (m_flag&MIPMAP)
                    m_textureBytes-=getSizeOfMipMaps(m_width,m_height)*getChannels(m_fmt);
                else
                    m_textureBytes-=m_width*m_height*getChannels(m_fmt);
                glDeleteTextures(1,(GLuint *)&m_ID);
            }
            if (m_patch)
            {
                delete []m_patch;
                m_patch='\0';
            }
            delete m_ref;
        }
    }

    Texture& Texture::operator=(const Texture &t)
    {
        if (this != &t)
        {
            if (!--(*m_ref))
            {
                if (m_ID)
                {
                    m_textureCount--;
                    if (m_flag&MIPMAP)
                        m_textureBytes-=getSizeOfMipMaps(m_width,m_height)*getChannels(m_fmt);
                    else
                        m_textureBytes-=m_width*m_height*getChannels(m_fmt);
                    glDeleteTextures(1,(GLuint *)&m_ID);
                }
                if (m_patch)
                {
                    delete []m_patch;
                    m_patch='\0';
                }
                delete m_ref;
            }
            m_ID=t.m_ID;
            m_ref=t.m_ref;
            ++(*m_ref);
            m_flag=t.m_flag;
            m_fmt=t.m_fmt;
            m_width=t.m_width;
            m_height=t.m_height;
            m_angle=t.m_angle;
            m_uscale=t.m_uscale;
            m_vscale=t.m_vscale;
            m_uoffset=t.m_uoffset;
            m_voffset=t.m_voffset;
            m_unit=t.m_unit;
            m_patch=t.m_patch;
        }
        return *this;
    }

    void Texture::texCoord2fv(const float *v) const
    {
        glMultiTexCoord2fv(GL_TEXTURE0+m_unit, v);
    }

    void Texture::texCoord2f(const float &u, const float &v) const
    {
        glMultiTexCoord2f(GL_TEXTURE0+m_unit, u, v);
    }

    void Texture::texCoord(const Vec2t &v) const
    {
        glMultiTexCoord2fv(GL_TEXTURE0+m_unit, (float *)&v);
    }

    void Texture::setAngle(const float &angle)
    {
        m_angle=angle;
        if (m_angle||m_uscale != 1||m_vscale != 1||m_uoffset||m_voffset) m_flag|=TEXMAT;
        else m_flag&=~TEXMAT;
    }
    const float &Texture::getAngle() const
    {
        return m_angle;
    }
    void Texture::setUScale(const float &uscale)
    {
        m_uscale=uscale;
        if (m_angle||m_uscale!= 1||m_vscale!= 1||m_uoffset||m_voffset) m_flag|=TEXMAT;
        else m_flag&=~TEXMAT;
    }
    const float &Texture::getUScale() const
    {
        return m_uscale;
    }
    void Texture::setVScale(const float &vscale)
    {
        m_vscale=vscale;
        if (m_angle||m_uscale!= 1||m_vscale!= 1||m_uoffset||m_voffset) m_flag|=TEXMAT;
        else m_flag&=~TEXMAT;
    }
    const float &Texture::getVScale() const
    {
        return m_vscale;
    }
    void Texture::setUOffset(const float &uoffset)
    {
        m_uoffset=uoffset;
        if (m_angle||m_uscale!= 1||m_vscale!= 1||m_uoffset||m_voffset) m_flag|=TEXMAT;
        else m_flag&=~TEXMAT;
    }
    const float &Texture::getUOffset() const
    {
        return m_uoffset;
    }
    void Texture::setVOffset(const float &voffset)
    {
        m_voffset=voffset;
        if (m_angle||m_uscale!= 1||m_vscale!= 1||m_uoffset||m_voffset) m_flag|=TEXMAT;
        else m_flag&=~TEXMAT;
    }
    const float &Texture::getVOffset() const
    {
        return m_voffset;
    }

    void Texture::setUnit(const size_t &unit)
    {
        if (unit >= m_tuCount)
        {
            Error_send("Unit %d is beyond maximum capacity of %d\n", unit, m_tuCount);
            m_unit=0;
        }
        else
            m_unit=unit;
    }

    const size_t &Texture::getUnit() const
    {
         return m_unit;
    }

    void Texture::selectPatch(const Vec2t &min, const Vec2t &max)
    {
         if (m_flag&SPLITX)
         {
             if (max[0] <= m_patch[1].m_uoffset)
                 m_patch[0].selectPatch(min,max);
             else if (min[0] >= m_patch[1].m_uoffset)
                 m_patch[1].selectPatch(min,max);
             else
                 select();
         }
         else if (m_flag&SPLITY)
         {
             if (max[1] <= m_patch[1].m_voffset)
                 m_patch[0].selectPatch(min,max);
             else if (min[1] >= m_patch[1].m_voffset)
                 m_patch[1].selectPatch(min,max);
             else
                 select();
         }
         else
             select();
    }

    void Texture::select() const
    {
        if (m_ID)
        {
            if (m_unit != m_currTu)
            {
                m_currTu=m_unit;
                glActiveTexture(GL_TEXTURE0+m_unit);
            }
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_ID);
            if (m_flag&DECAL)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            else if (m_flag&BLEND)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
            else if (m_flag&REPLACE)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            else if (m_flag&ADD)
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
            else if (m_flag&DETAIL)
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  GL_TEXTURE);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  GL_PREVIOUS_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_ADD_SIGNED_EXT);
                // pixel=previous_color+texture_color-0.5
            } else if (m_flag&CLOUDS)
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  GL_PREVIOUS_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  GL_PRIMARY_COLOR_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,  GL_TEXTURE);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_ONE_MINUS_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_INTERPOLATE_EXT);
                // pixel=light*texture_color+previous_color*(1-texture_color)

            } else if (m_flag&NIGHTLIGHTS)
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  GL_PREVIOUS_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  GL_TEXTURE);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,  GL_PRIMARY_COLOR_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_INTERPOLATE_EXT);
                // pixel=previous_color*light+texture_color*(1-light)
            } else if (m_flag&NIGHT)
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  GL_PREVIOUS_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  GL_TEXTURE);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,  GL_PRIMARY_COLOR_EXT);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  GL_INTERPOLATE_EXT);
                // pixel=previous_color*light+texture_color*(1-light)
            } else
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glMatrixMode(GL_TEXTURE);
            glLoadIdentity();
            if (m_flag&TEXMAT)
            {
                if (m_uoffset||m_voffset)
                    glTranslatef(m_uoffset, m_voffset, 0);
                if (m_uscale!=1||m_vscale!=1)
                    glScalef(m_uscale, m_vscale, 1);
                if (m_angle)
                    glRotatef(m_angle, 0,0,1);
            }
            glMatrixMode(GL_MODELVIEW);
        }
        else
        {
            Error_send("Selected empty texture\n");
        }
    }

    void Texture::deselect() const
    {
        if (m_unit != m_currTu)
        {
            m_currTu=m_unit;
            glActiveTexture(GL_TEXTURE0+m_unit);
        }
        glDisable(GL_TEXTURE_2D);
    }

    void Texture::write(const Image &i)
    {
        if (m_flag&MIPMAP)
        {
            gluBuild2DMipmaps(GL_TEXTURE_2D,i.getChannels(),i.getWidth(),i.getHeight(),m_fmt,type,i.getData());
        }
        else
        {
            if (m_width != i.getWidth() || m_height != i.getHeight())
            {
                if (m_flag&NOSCALE)
                    if (i.getWidth() > m_width || i.getHeight() > m_height)
                    {
                        Error_send("Image cannot be bigger than target texture when using NOSCALE\n");
                    }
                    else
                        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,i.getWidth(),i.getHeight(),m_fmt,type,i.getData());
                else
                {
                    Image nimage(m_width,m_height,(Image::Image_type)i.getType());
                    nimage.convert(i);
                    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_width,m_height,m_fmt,type,nimage.getData());
                }
            }
            else
                glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_width,m_height,m_fmt,type,i.getData());
        }
    }

    bool Texture::isValid() const
    {
        return (m_ID != 0);
    }

    const size_t &Texture::getTextureCount()
    {
        return m_textureCount;
    }

    const size_t &Texture::getTextureBytes()
    {
        return m_textureBytes;
    }

    const size_t &Texture::getTextureUnitCount()
    {
         return m_tuCount;
    }

    const size_t &Texture::getCurrentTextureUnit()
    {
         return m_currTu;
    }
}
