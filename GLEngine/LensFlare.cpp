#include "LensFlare.h"
#include <GL/gl.h>

namespace GLEngine
{

    LensFlare::Flare::Flare(const Texture &tex, const float &position, const float &scale, const float &intensity)
    {
        m_tex=tex;
        m_position=position;
        m_scale=scale;
        m_intensity=intensity;
    }

    LensFlare::Flare::Flare(const LensFlare::Flare &f)
    {
        m_tex=f.m_tex;
        m_position=f.m_position;
        m_scale=f.m_scale;
        m_intensity=f.m_intensity;
        m_pos=f.m_pos;
    }

    LensFlare::Flare::~Flare()
    {}

    LensFlare::Flare &LensFlare::Flare::operator=(const LensFlare::Flare &f)
    {
        if (&f != this)
        {
            m_tex=f.m_tex;
            m_position=f.m_position;
            m_scale=f.m_scale;
            m_intensity=f.m_intensity;
            m_pos=f.m_pos;
        }
        return *this;
    }

    const float &LensFlare::Flare::getPosition() const
    {
        return m_position;
    }

    void LensFlare::Flare::setPosition(const float &p)
    {
        m_position=p;
    }

    const float &LensFlare::Flare::getScale() const
    {
        return m_scale;
    }

    void LensFlare::Flare::setScale(const float &s)
    {
        m_scale=s;
    }

    const float &LensFlare::Flare::getIntensity() const
    {
        return m_intensity;
    }

    void LensFlare::Flare::setIntensity(const float &i)
    {
        m_intensity=i;
    }

    const Texture &LensFlare::Flare::getTexture() const
    {
        return m_tex;
    }

    void LensFlare::Flare::setTexture(const Texture &t)
    {
        m_tex=t;
    }


    void LensFlare::Flare::update(const Vec3t &center, const Vec3t &axis)
    {
        m_pos=center+axis*m_position;
    }

    void LensFlare::Flare::render(const Vec3t corner[4]) const
    {
        m_tex.select();

        Vec3t v0=m_pos+corner[0]*m_scale, v1=m_pos+corner[1]*m_scale, v2=m_pos+corner[2]*m_scale, v3=m_pos+corner[3]*m_scale;
        glBegin(GL_QUADS);
        glColor3f(m_intensity, m_intensity, m_intensity);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3fv((float *)&v0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3fv((float *)&v1);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3fv((float *)&v2);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3fv((float *)&v3);
        glEnd();
    }

    LensFlare::LensFlare(const float &scale, const Vec3t &source)
    {
        m_source=source;
        m_scale=scale;
    }

    LensFlare::LensFlare(const LensFlare &l)
    {
        m_source=l.m_source;
        m_scale=l.m_scale;
        m_flares=l.m_flares;
    }

    LensFlare::~LensFlare()
    {
        m_flares.clear();
    }

    LensFlare& LensFlare::operator=(const LensFlare &l)
    {
        if (&l != this)
        {
            m_flares.clear();
            m_source=l.m_source;
            m_scale=l.m_scale;
            m_flares=l.m_flares;
        }
        return *this;
    }

    const Vec3t &LensFlare::getSource() const
    {
        return m_source;
    }

    void LensFlare::setSource(const Vec3t &s)
    {
        m_source=s;
    }

    const float &LensFlare::getScale() const
    {
        return m_scale;
    }

    void LensFlare::setScale(const float &s)
    {
        m_scale=s;
    }

    void LensFlare::add(const Flare &flare)
    {
        m_flares.push_back(flare);
    }

    void LensFlare::update(const Camera &cam)
    {
        static const float near_clip=CDL::Window::near_clip+0.05;

        Vec3t vdir(cam.getFront());
        vdir.normalize();
        Vec3t ldir(m_source-cam.getEye());
        ldir.normalize();
        Vec3t center(cam.getEye()+vdir*near_clip);
        Vec3t light(cam.getEye()+ldir*(near_clip/dot(ldir,vdir)));
        Vec3t axis(light-center);
        Vec3t dx(axis/axis.length());
        Vec3t dy(cross(dx,vdir));
        dx*=m_scale;
        dy*=m_scale;

        m_corner[0]=dx+dy;
        m_corner[1]=-dx+dy;
        m_corner[2]=-dx-dy;
        m_corner[3]=dx-dy;

        flist::iterator flare=m_flares.begin(), fend=m_flares.end();

        while (flare != fend)
        {
            flare->update(center, axis);
            flare++;
        }
    }

    void LensFlare::render() const
    {
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        flist::const_iterator flare(m_flares.begin()), fend(m_flares.end());

        while (flare != fend)
        {
            flare->render(m_corner);
            flare++;
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

}
