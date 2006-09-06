//========================================================================
/** @type     C++ Source File
 *  @file     GLEngine/MultiTexture.cpp
 *  @author   acornejo
 *  @date
 *   Created:       02:31:58 13/06/2005
 *   Last Update:   01:42:03 18/02/2006
 */
//========================================================================
#include <GLEngine/MultiTexture.h>

namespace GLEngine
{
    DEFCLASS("MultiTexture");

    MultiTexture::MultiTexture()
    {
    }

    MultiTexture::MultiTexture(const Texture &t)
    {
        addTexture(t);
    }


    MultiTexture::MultiTexture(const MultiTexture &m)
    {
        m_texture=m.m_texture;
    }

    MultiTexture::~MultiTexture()
    {
        m_texture.clear();
    }

    const MultiTexture &MultiTexture::operator=(const MultiTexture &m)
    {
        if (&m != this)
            m_texture=m.m_texture;

        return *this;
    }

    void MultiTexture::texCoord(const Vec2t &v) const
    {
         std::vector<Texture>::const_iterator begin(m_texture.begin()), end(m_texture.end());

         while (begin != end)
         {
             begin->texCoord(v);
             begin++;
         }
    }

    void MultiTexture::texCoord2fv(const float *v) const
    {
         std::vector<Texture>::const_iterator begin(m_texture.begin()), end(m_texture.end());

         while (begin != end)
         {
             begin->texCoord2fv(v);
             begin++;
         }
    }

    void MultiTexture::texCoord2f(const float &u, const float &v) const
    {
         std::vector<Texture>::const_iterator begin(m_texture.begin()), end(m_texture.end());

         while (begin != end)
         {
             begin->texCoord2f(u,v);
             begin++;
         }
    }


    size_t MultiTexture::getTextureCount() const
    {
        return m_texture.size();
    }

    void MultiTexture::clear()
    {
         m_texture.clear();
    }

    void MultiTexture::addTexture(const Texture &t)
    {
        m_texture.push_back(t);
        m_texture.back().setUnit(m_texture.size()-1);
    }

    void MultiTexture::setTexture(const size_t &i, const Texture &t)
    {
        if (i >= m_texture.size())
        {
            Error_send("Index %d is out of bounds\n", i);
        }
        else
        {
            m_texture[i]=t;
            m_texture[i].setUnit(i);
        }

    }

    const Texture &MultiTexture::getTexture(const size_t &i) const
    {
        if (i >= m_texture.size())
        {
            Error_send("Index %d is out of bounds\n", i);
            return m_texture.back();
        }
        else
            return m_texture[i];
    }

    void MultiTexture::select() const
    {
         std::vector<Texture>::const_iterator begin(m_texture.begin()), end(m_texture.end());

         while (begin != end)
         {
             begin->select();
             begin++;
         }
    }

    void MultiTexture::deselect() const
    {
         std::vector<Texture>::const_reverse_iterator begin(m_texture.end()), end(m_texture.begin());

         while (begin != end)
         {
             begin->deselect();
             begin++;
         }
    }
}
