#include <CDL/GLEngine/SkyBox.h>
#include <GL/gl.h>

namespace CDL
{
    DEFCLASS("SkyBox");

    SkyBox::SkyBox(const Vec3t &pos, const Vec3t &size)
    {
        corner[0]=Vec3t(pos[0]-size[0]/2.0f, pos[1]-size[1]/2.0f, pos[2]-size[2]/2.0f);
        corner[1]=Vec3t(pos[0]-size[0]/2.0f, pos[1]-size[1]/2.0f, pos[2]+size[2]/2.0f);
        corner[2]=Vec3t(pos[0]-size[0]/2.0f, pos[1]+size[1]/2.0f, pos[2]+size[2]/2.0f);
        corner[3]=Vec3t(pos[0]-size[0]/2.0f, pos[1]+size[1]/2.0f, pos[2]-size[2]/2.0f);
        corner[4]=Vec3t(pos[0]+size[0]/2.0f, pos[1]+size[1]/2.0f, pos[2]-size[2]/2.0f);
        corner[5]=Vec3t(pos[0]+size[0]/2.0f, pos[1]+size[1]/2.0f, pos[2]+size[2]/2.0f);
        corner[6]=Vec3t(pos[0]+size[0]/2.0f, pos[1]-size[1]/2.0f, pos[2]+size[2]/2.0f);
        corner[7]=Vec3t(pos[0]+size[0]/2.0f, pos[1]-size[1]/2.0f, pos[2]-size[2]/2.0f);
		sun=Vec3t();
    }

    SkyBox::SkyBox(const SkyBox &s)
    {
        for (size_t i=0; i<8; i++) corner[i]=s.corner[i];
		for (size_t i=0; i<6; i++) side[i]=s.side[i];
		sun=s.sun;
		dir=s.dir;
    }

	SkyBox::~SkyBox() {}

    SkyBox& SkyBox::operator=(const SkyBox &s)
    {
        if (this != &s)
        {
            for (size_t i=0; i<8; i++) corner[i]=s.corner[i];
			for (size_t i=0; i<6; i++) side[i]=s.side[i];
			sun=s.sun;
			dir=s.dir;
        }
        return *this;
    }
    void SkyBox::load(const SkyBox_Side &sidei, const Image &img)
    {
        side[sidei]=Texture(img, Texture::MIPMAP|Texture::CLAMP);
    }

    void SkyBox::render() const
    {
        glColor3f(1,1,1);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        side[UP].select();
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 1.0f);glVertex3fv((float *)&corner[3]);
        glTexCoord2f(1.0f, 0.0f);glVertex3fv((float *)&corner[4]);
        glTexCoord2f(0.0f, 0.0f);glVertex3fv((float *)&corner[5]);
        glTexCoord2f(0.0f, 1.0f);glVertex3fv((float *)&corner[2]);
        glEnd();
        side[DOWN].select();
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);glVertex3fv((float *)&corner[1]);
        glTexCoord2f(0.0f, 1.0f);glVertex3fv((float *)&corner[6]);
        glTexCoord2f(1.0f, 1.0f);glVertex3fv((float *)&corner[7]);
        glTexCoord2f(1.0f, 0.0f);glVertex3fv((float *)&corner[0]);
        glEnd();
        side[FRONT].select();
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f);glVertex3fv((float *)&corner[0]);
        glTexCoord2f(1.0f, 1.0f);glVertex3fv((float *)&corner[7]);
        glTexCoord2f(1.0f, 0.0f);glVertex3fv((float *)&corner[4]);
        glTexCoord2f(0.0f, 0.0f);glVertex3fv((float *)&corner[3]);
        glEnd();
        side[BACK].select();
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);glVertex3fv((float *)&corner[2]);
        glTexCoord2f(0.0f, 0.0f);glVertex3fv((float *)&corner[5]);
        glTexCoord2f(0.0f, 1.0f);glVertex3fv((float *)&corner[6]);
        glTexCoord2f(1.0f, 1.0f);glVertex3fv((float *)&corner[1]);
        glEnd();
        side[RIGHT].select();
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);glVertex3fv((float *)&corner[5]);
        glTexCoord2f(0.0f, 0.0f);glVertex3fv((float *)&corner[4]);
        glTexCoord2f(0.0f, 1.0f);glVertex3fv((float *)&corner[7]);
        glTexCoord2f(1.0f, 1.0f);glVertex3fv((float *)&corner[6]);
        glEnd();
        side[LEFT].select();
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f);glVertex3fv((float *)&corner[3]);
        glTexCoord2f(0.0f, 0.0f);glVertex3fv((float *)&corner[2]);
        glTexCoord2f(0.0f, 1.0f);glVertex3fv((float *)&corner[1]);
        glTexCoord2f(1.0f, 1.0f);glVertex3fv((float *)&corner[0]);
        glEnd();
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
    }

    void  SkyBox::setSun(const float &heading, const float &altitude)
    {
        float x=sin(DEG2RAD(heading)), y=tan(DEG2RAD(altitude)), z=-cos(DEG2RAD(heading));
        Plane p;
        Ray r=Ray(Vec3t(0.0f,0.0f,0.0f),Vec3t(x, y, z));
        Intersection ip;
        DefType T=0;
		dir=Vec3t(x,y,z);
		dir.normalize();

        p=Plane(corner[3],corner[4],corner[5]);
        ip=p.intersect(r);
        if (ip) T=ip.getT();

        p=Plane(corner[1],corner[6],corner[7]);
        ip=p.intersect(r);
        if (ip && ip.getT() < T) T=ip.getT();

        p=Plane(corner[0],corner[7],corner[4]);
        ip=p.intersect(r);
        if (ip && ip.getT() < T) T=ip.getT();

        p=Plane(corner[2],corner[5],corner[6]);
        ip=p.intersect(r);
        if (ip && ip.getT() < T) T=ip.getT();

        p=Plane(corner[5],corner[4],corner[7]);
        ip=p.intersect(r);
        if (ip && ip.getT() < T) T=ip.getT();

        p=Plane(corner[3],corner[2],corner[1]);
        ip=p.intersect(r);
        if (ip && ip.getT() < T) T=ip.getT();

        sun=r.getPoint()+r.getNormal()*T;
    }

    void SkyBox::setSun(const Vec3t &_sun)
    {
        sun=_sun;
    }

    const Vec3t &SkyBox::getSun() const
    {
        return sun;
    }

	const Vec3t &SkyBox::getDir() const
	{
		return dir;
	}
}
