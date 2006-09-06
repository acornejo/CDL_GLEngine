#include "Camera.h"
#include <GL/gl.h>

using CDL::Quaternion;
using CDL::Vec2t;

namespace GLEngine
{
    Camera::Camera(const Vec3t &eye, const Vec3t &front, const Vec3t &up)
    {
        m_up=up;
        m_up.normalize();
        m_mat[3]=m_mat[7]=m_mat[11]=m_mat[12]=m_mat[13]=m_mat[14]=0;
        m_mat[15]=1;
        lookAt(eye, front, up);
    }

    Camera::Camera(const Camera &c)
    {
        m_eye=c.m_eye;
        m_front=c.m_front;
        m_up=c.m_up;
        m_right=c.m_right;
        m_Rup=c.m_Rup;
        m_angleX=c.m_angleX;
        m_angleY=c.m_angleY;
        m_move=c.m_move;
        m_strafe=c.m_strafe;
        m_jump=c.m_jump;
        for (size_t i=0; i<4; i++) m_v[i]=c.m_v[i];
        for (size_t i=0; i<16; i++) m_mat[i]=c.m_mat[i];
    }

    const Camera &Camera::operator=(const Camera &c)
    {
        if (&c != this)
        {
            m_eye=c.m_eye;
            m_front=c.m_front;
            m_up=c.m_up;
            m_right=c.m_right;
            m_Rup=c.m_Rup;
            m_angleX=c.m_angleX;
            m_angleY=c.m_angleY;
            m_move=c.m_move;
            m_strafe=c.m_strafe;
            m_jump=c.m_jump;
            for (size_t i=0; i<4; i++) m_v[i]=c.m_v[i];
            for (size_t i=0; i<16; i++) m_mat[i]=c.m_mat[i];
        }

        return *this;
    }

    void Camera::lookAt(const Vec3t &eye, const Vec3t &front, const Vec3t &up)
    {
        m_eye=eye;
        m_front=front;
        m_front.normalize();
        m_Rup=up;
        m_Rup.normalize();
        m_right=cross(m_front,m_Rup);
        m_angleX=0;
        m_angleY=0;
        m_move=0;
        m_strafe=0;
        m_jump=0;
    }

    void Camera::move(const float &dist)
    {
        m_move+=dist;
    }

    void Camera::jump(const float &dist)
    {
        m_jump+=dist;
    }

    void Camera::strafe(const float &dist)
    {
        m_strafe+=dist;
    }

    void Camera::turn(const float &angle)
    {
        m_angleX+=angle;
    }

    void  Camera::look(const float &angle)
    {
        m_angleY+=angle;
    }

    void Camera::update(const size_t &dt)
    {
        if (!dt) return;
        const float f=(float)dt;
        Quaternion R(0,0,0,1);

        R.rotate(m_up,m_angleX*f);
        R.rotate(m_right,m_angleY*f);
        m_Rup=R*m_Rup;
        m_front=R*m_front;
        m_Rup.normalize();
        m_front.normalize();
        m_right=cross(m_front,m_Rup);

        m_eye+=(m_front*m_move+m_up*m_jump+m_right*m_strafe)*f;
        m_angleX=m_angleY=m_move=m_jump=m_strafe=0;

        m_mat[0]=m_right[0];  m_mat[4]=m_right[1];  m_mat[8]=m_right[2];
        m_mat[1]=m_Rup[0];    m_mat[5]=m_Rup[1];    m_mat[9]=m_Rup[2];
        m_mat[2]=-m_front[0]; m_mat[6]=-m_front[1]; m_mat[10]=-m_front[2];

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0,0,-m_offset);
        glMultMatrixf(m_mat);
        glTranslatef(-m_eye[0], -m_eye[1], -m_eye[2]);

        m_v[0]=-m_right-m_Rup;
        m_v[1]=m_right-m_Rup;
        m_v[2]=m_right+m_Rup;
        m_v[3]=-m_right+m_Rup;
    }

    void Camera::renderBillboard() const
    {
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3fv((float *)&m_v[0]);
        glTexCoord2f(1,0);
        glVertex3fv((float *)&m_v[1]);
        glTexCoord2f(1,1);
        glVertex3fv((float *)&m_v[2]);
        glTexCoord2f(0,1);
        glVertex3fv((float *)&m_v[3]);
        glEnd();
    }

    void Camera::renderBillboard(const Vec3t &at) const
    {
        Vec3t front=at-m_eye;
        front.normalize();
        Vec3t right=cross(front,m_Rup);
        right.normalize();
        Vec3t up=cross(right,front);
        up.normalize();

        Vec3t v0=-right-up, v1=right-up, v2=right+up, v3=-right+up;

        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3fv((float *)&v0);
        glTexCoord2f(1,0);
        glVertex3fv((float *)&v1);
        glTexCoord2f(1,1);
        glVertex3fv((float *)&v2);
        glTexCoord2f(0,1);
        glVertex3fv((float *)&v3);
        glEnd();
    }

    void Camera::setEye(const Vec3t &eye)
    {
        m_eye=eye;
    }

    const Vec3t &Camera::getEye() const
    {
        return m_eye;
    }

    void Camera::setFront(const Vec3t &front)
    {
        m_front=front;
    }

    const Vec3t &Camera::getFront() const
    {
        return m_front;
    }

    void Camera::setUp(const Vec3t &up)
    {
        m_up=up;
    }

    const Vec3t &Camera::getUp() const
    {
        return m_up;
    }

    const Vec3t &Camera::getRight() const
    {
        return m_right;
    }

    void Camera::setOffset(const float &offset)
    {
        m_offset=offset;
    }

    const float &Camera::getOffset() const
    {
        return m_offset;
    }
}
