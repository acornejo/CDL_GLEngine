#include <CDL/GLEngine/HeightMap.h>
#include <GL/gl.h>

namespace CDL
{
    DEFCLASS("HeightMap");

    HeightMap::HeightMap()
    {
        m_ref=new size_t(1);
        vData='\0';
        tData='\0';
        vIndex='\0';
    }

    HeightMap::HeightMap(const Image &img, const Vec3t &size, const int &step, const int &detail)
    {
        m_ref=new size_t(1);
        if (img.getType() == Image::NONE || !size[0] || !size[2])
        {
            Error_send("Invalid image or scale for heightmap\n");
            vData='\0';
            tData='\0';
            vIndex='\0';
        }
        else
        {
            m_scale=size;
            m_detail=detail;
            m_width=img.getWidth()/step;
            m_height=img.getHeight()/step;

            size_t w=m_width, h=m_height, wh=m_width*m_height;
            float w2=w*0.5f, h2=h*0.5f, w1=1.0f/w, h1=1.0f/h;

            vData=new Vec3t[wh];
            tData=new Vec2t[wh];
            vIndex=new ushort[wh+(h-2)*(w-1)];

            for (size_t y=0, k=0; y<h; y++)
                for (size_t x=0; x<w; x++, k++)
                {
                    float height=0;
                    for (size_t c=0; c<img.getChannels(); c++)
                        height+=img(x*step,y*step,c);
                    height/=255.0f*img.getChannels();
                    vData[k]=Vec3t((x-w2)*w1, height, (y-h2)*h1)*size;
                    tData[k]=Vec2t(x*w1, y*h1);
                }

            int k=0, x=0, y=0;
            vIndex[k++]=0;
            while (k < wh+(h-2)*(w-1))
            {
                vIndex[k++]=(y+1)*w+x;
                if (!(y%2))
                {
                    for (x=1; x<w; x++)
                    {
                        vIndex[k++]=y*w+x;
                        vIndex[k++]=(y+1)*w+x;
                    }
                    x--;
                }
                else
                {
                    for (x=w-2; x>-1; x--)
                    {
                        vIndex[k++]=y*w+x;
                        vIndex[k++]=(y+1)*w+x;
                    }
                    x++;
                }
                y++;
            }
        }
    }

    HeightMap::HeightMap(const HeightMap &h)
    {
        m_ref=h.m_ref;
        ++(*m_ref);
        vData=h.vData;
        vIndex=h.vIndex;
        tData=h.tData;
        m_tex=h.m_tex;
        m_scale=h.m_scale;
        m_detail=h.m_detail;
        m_width=h.m_width;
        m_height=h.m_height;
    }

    HeightMap::~HeightMap()
    {
        if (!--(*m_ref))
        {
            delete m_ref;
            if (vData) delete []vData;
            if (tData) delete []tData;
            if (vIndex) delete []vIndex;
        }
    }

    const HeightMap &HeightMap::operator=(const HeightMap &h)
    {
        if (this != &h)
        {
            if (!--(*m_ref))
            {
                delete m_ref;
                if (vData) delete []vData;
                if (vIndex) delete []vIndex;
                if (tData) delete []tData;
            }
            m_ref=h.m_ref;
            ++(*m_ref);
            vData=h.vData;
            vIndex=h.vIndex;
            tData=h.tData;
            m_tex=h.m_tex;
            m_scale=h.m_scale;
            m_detail=h.m_detail;
            m_width=h.m_width;
            m_height=h.m_height;
        }
        return *this;
    }

    void HeightMap::setTextures(const Image &tex, const Image &det)
    {
        Texture t1(tex,Texture::MIPMAP|Texture::CLAMP);
        m_tex.addTexture(t1);
        if (det.getType() != Image::NONE)
        {
            Texture t2(det,Texture::MIPMAP|Texture::DETAIL);
            t2.setUScale(m_detail);
            t2.setVScale(m_detail);
            m_tex.addTexture(t2);
        }
    }

    float HeightMap::getHeight(const float &s, const float &t) const
    {
        float s0=m_scale[0]*0.5, s2=m_scale[2]*0.5;

        if (ceil(s) > s0 || floor(s) < -s0 || ceil(t) > s2 || floor(t) < -s2)
        {
            Error_send("Invalid range %f%f\n", s, t);
            return 0;
        }
        else
        {
            float u=s*m_width/m_scale[0]+0.5*m_width, v=t*m_height/m_scale[2]+0.5*m_height;
            size_t uInt=(size_t)u, vInt=(size_t)v;
            float uFrac=u-uInt, vFrac=v-vInt;

            return vData[uInt+0+(vInt+0)*m_width][1]*(1-uFrac)*(1-vFrac)+
                vData[uInt+1+(vInt+0)*m_width][1]*uFrac*(1-vFrac)+
                vData[uInt+1+(vInt+1)*m_width][1]*uFrac*vFrac+
                vData[uInt+0+(vInt+1)*m_width][1]*(1-uFrac)*vFrac;
        }
    }

    void HeightMap::render() const
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        m_tex.select();

        size_t limit=m_width*m_height+(m_height-2)*(m_width-1);
        glBegin(GL_TRIANGLE_STRIP);
        for (size_t i=0; i<limit; i++)
        {
            m_tex.texCoord(tData[vIndex[i]]);
            glVertex3fv((float *)&vData[vIndex[i]]);
        }
        glEnd();

        m_tex.deselect();
    }

    const Vec3t &HeightMap::getScale() const
    {
        return m_scale;
    }
}
