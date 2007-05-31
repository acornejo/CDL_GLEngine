#include <CDL/GLEngine/Font.h>
#include <string.h>
#include <ctype.h>
#include <CDL/endian.h>
#include <GL/gl.h>

namespace CDL
{
    DEFCLASS("Font");

    Font::Font(InputStream &in)
    {
        bool swap;
        int fileID, endianness;

        in.readInt(fileID);
        if (fileID != 0x667874ff)
        {
            Error_send("This stream is not in texture font format (%x).\n", fileID);
            return;
        }
        else
        {
            in.readInt(endianness);
            if (endianness == 0x12345678)
            {
                swap=false;
            }
            else if (endianness == 0x78563412)
            {
                swap=true;
            }
            else
            {
                Error_send("Endianness indicator is corrupt.\n");
                return;
            }
        }

        int is_bitmap, width, height, max_ascent, max_descent, num_glyphs;
        float n_xmin=0, n_ymin=0, n_max=1;

        in.readInt(is_bitmap);
        in.readInt(width);
        in.readInt(height);
        in.readInt(max_ascent);
        in.readInt(max_descent);
        in.readInt(num_glyphs);

        if (swap)
        {
            is_bitmap=bswap_32(is_bitmap);
            width=bswap_32(width);
            height=bswap_32(height);
            max_ascent=bswap_32(max_ascent);
            max_descent=bswap_32(max_descent);
            num_glyphs=bswap_32(num_glyphs);
        }

        struct GlyphInfo
        {
            unsigned short c;
            unsigned char width;
            unsigned char height;
            signed char xoffset;
            signed char yoffset;
            signed char advance;
            char dummy; // alignment
            short x;
            short y;
        } glyph[num_glyphs];
        in.read(glyph, num_glyphs*sizeof(GlyphInfo));

        if (swap)
        {
            for (size_t i=0; i<num_glyphs; i++)
            {
                glyph[i].c=bswap_16(glyph[i].c);
                glyph[i].x=bswap_16(glyph[i].x);
                glyph[i].y=bswap_16(glyph[i].y);
            }
        }

        // normalize
        {
            GlyphInfo &g=glyph[0];
            n_max=MAX(g.yoffset,g.yoffset+g.height);
            n_xmin=MIN(g.xoffset,g.xoffset+g.width);
            n_ymin=MIN(g.yoffset,g.yoffset+g.height);
            if (g.xoffset > n_max) n_max=g.xoffset;
            if (g.xoffset+g.width > n_max) n_max=g.xoffset+g.width;
            for (size_t i=1; i<num_glyphs; i++)
            {
                GlyphInfo &g=glyph[i];
                if (g.yoffset > n_max) n_max=g.yoffset;
                if (g.xoffset > n_max) n_max=g.xoffset;
                if (g.yoffset+g.height > n_max) n_max=g.yoffset+g.height;
                if (g.xoffset+g.width > n_max) n_max=g.xoffset+g.width;
                if (g.yoffset < n_ymin) n_ymin=g.yoffset;
                if (g.xoffset < n_xmin) n_xmin=g.xoffset;
                if (g.yoffset+g.height < n_ymin) n_ymin=g.yoffset+g.height;
                if (g.xoffset+g.width < n_xmin) n_xmin=g.xoffset+g.width;
            }
            n_max-=n_ymin;
        }

        Image img(width,height,Image::LUMINANCE);
        if (is_bitmap)
        {
            size_t stride=(width+7)>>3;
            byte bitmap[stride*height];
            in.read(bitmap, stride*height);
            for (int i=0; i<height; i++)
                for (int j=0; j<width; j++)
                    if (bitmap[i*stride+(j>>3)]&(1<<(j&7)))
                        img(j,i)=255;
                    else
                        img(j,i)=0;
        }
        else
            in.read(img.getData(), width*height);

        img.alpha(img);
        m_tex=Texture(img,Texture::CLAMP|Texture::MIPMAP);
        m_size=(size_t)n_max;
        m_list=DisplayList(256);
        for (size_t i=0; i<256; i++)
        {
            int index=-1, curchar=i;
            float h=height, w=width;
            for (size_t j=0; j<num_glyphs; j++)
                if (glyph[j].c == curchar)
                {
                    index=j;
                    break;
                }
            if (index == -1)
            {
                if (islower(i)) curchar=toupper(i);
                if (isupper(i)) curchar=tolower(i);
                for (size_t j=0; j<num_glyphs; j++)
                    if (glyph[j].c == curchar)
                    {
                        index=j;
                        break;
                    }
            }
            m_list.beginList(i);
            if (index != -1)
            {
                GlyphInfo &g=glyph[index];
                glBegin(GL_QUADS);
                glTexCoord2f(g.x/w, g.y/h);
                glVertex2f((g.xoffset-n_xmin)/n_max,(g.yoffset-n_ymin)/n_max);
                glTexCoord2f((g.x+g.width)/w, g.y/h);
                glVertex2f((g.xoffset+g.width-n_xmin)/n_max,(g.yoffset-n_ymin)/n_max);
                glTexCoord2f((g.x+g.width)/w, (g.y+g.height)/h);
                glVertex2f((g.xoffset+g.width-n_xmin)/n_max,(g.yoffset+g.height-n_ymin)/n_max);
                glTexCoord2f(g.x/w,(g.y+g.height)/h);
                glVertex2f((g.xoffset-n_xmin)/n_max,(g.yoffset+g.height-n_ymin)/n_max);
                glEnd();
                glTranslatef((g.advance-n_xmin)/n_max,0,0);
            }
            m_list.endList();
        }
    }

    Font::Font(const Image &i)
    {
        if (i.getType() != Image::NONE)
        {
            Image img=i;
            if (i.getChannels() == 1 || i.getChannels() == 3) img.alpha(i);
            m_tex=Texture(img,Texture::CLAMP|Texture::MIPMAP);
            m_list=DisplayList(256);
            m_size=img.getHeight()/16;
            for (size_t i=0; i<256; i++)
            {
                float cx=float(i%16)/16.0f, cy=float(i/16)/16.0f, dx=1.0f/16.0f;

                m_list.beginList(i);
                glBegin(GL_QUADS);
                glTexCoord2f(cx,cy+dx);
                glVertex2i(0,0);
                glTexCoord2f(cx+dx,cy+dx);
                glVertex2i(1,0);
                glTexCoord2f(cx+dx,cy);
                glVertex2i(1,1);
                glTexCoord2f(cx,cy);
                glVertex2i(0,1);
                glEnd();
                glTranslatef(0.65,0,0);
                m_list.endList();
            }
        }
    }

    Font::Font(const Font &f)
    {
        m_tex=f.m_tex;
        m_list=f.m_list;
        m_size=f.m_size;
    }

    Font& Font::operator=(const Font &f)
    {
        if (this != &f)
        {
            m_tex=f.m_tex;
            m_list=f.m_list;
            m_size=f.m_size;
        }
        return *this;
    }

    void Font::print(const size_t &scale, const int &x, const int &y, const string &str)
    {
        int view[4];
        va_list ap;
        float rx, ry;

        if (str == '\0') return;

        glGetIntegerv(GL_VIEWPORT, view);

        if (x < 0)
        {
            switch (x)
            {
                case CENTER:
                    rx=(view[2]-str.length()*scale*0.65)/2;
                    break;
                case LEFT:
                    rx=0;
                    break;
                case RIGHT:
                    rx=(view[2]-(str.length()+1)*scale*0.65);
                    break;
                default:
                    Error_send("Invalid parameter for X coordinate\n");
            }
        }
        else rx=x;

        if (y < 0)
        {
            switch (y)
            {
                case CENTER:
                    ry=(view[3]-scale)/2.0f;
                    break;
                case BOTTOM:
                    ry=0;
                    break;
                case TOP:
                    ry=view[3]-scale;
                    break;
                default:
                    Error_send("Invalid parameter for Y coordinate\n");
            }
        }
        else ry=view[3]-(y+scale);

        glPushAttrib(GL_ENABLE_BIT);
        m_tex.select();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0,view[2],0,view[3],-1,1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glTranslatef(rx,ry,0);
        glScalef(scale,scale,1);

        m_list.call(str);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glPopAttrib();
    }

    const size_t &Font::getSize() const
    {
        return m_size;
    }
}
