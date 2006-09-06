#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <CDL/CDL.h>

namespace GLEngine
{
    using CDL::Vec3t;

    class Camera
    {
        private:
            float m_mat[16];
            float m_angleX;
            float m_angleY;
            float m_move;
            float m_strafe;
            float m_jump;
            float m_offset;
            Vec3t m_eye;
            Vec3t m_up;
            Vec3t m_Rup;
            Vec3t m_front;
            Vec3t m_right;
            Vec3t m_v[4];

        public:
            Camera(const Vec3t &e=Vec3t(0,0,0), const Vec3t &p=Vec3t(0,0,-1), const Vec3t &u=Vec3t(0,1,0));
            Camera(const Camera &);
            const Camera &operator=(const Camera &);

            void lookAt(const Vec3t &, const Vec3t &, const Vec3t &);

            void move(const float&);
            void strafe(const float&);
            void jump(const float&);
            void turn(const float&);
            void look(const float&);

            void update(const size_t &dt=1);
            void renderBillboard() const;
            void renderBillboard(const Vec3t &) const;

            void setEye(const Vec3t&);
            const Vec3t &getEye() const;
            void setFront(const Vec3t &);
            const Vec3t &getFront() const;
            void setUp(const Vec3t &);
            const Vec3t &getUp() const;
            const Vec3t &getRight() const;
            const float &getOffset() const;
            void setOffset(const float &);
    };
}

#endif
