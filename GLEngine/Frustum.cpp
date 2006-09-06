#include "Frustum.h"
#include <GL/gl.h>

namespace GLEngine
{
    typedef enum {RIGHT=0, LEFT, BOTTOM, TOP, BACK, FRONT, MAX} FRSTTypes;

    Frustum::Frustum(const Frustum &f)
    {
        side[RIGHT]=f.side[RIGHT];
        side[LEFT]=f.side[LEFT];
        side[BOTTOM]=f.side[BOTTOM];
        side[TOP]=f.side[TOP];
        side[BACK]=f.side[BACK];
        side[FRONT]=f.side[FRONT];
    }

    Frustum& Frustum::operator=(const Frustum &f)
    {
        if (&f != this)
        {
            side[RIGHT]=f.side[RIGHT];
            side[LEFT]=f.side[LEFT];
            side[BOTTOM]=f.side[BOTTOM];
            side[TOP]=f.side[TOP];
            side[BACK]=f.side[BACK];
            side[FRONT]=f.side[FRONT];
        }
        return *this;
    }

    void Frustum::update()
    {
        Mat4t proj, modl, res;

        glGetFloatv(GL_PROJECTION_MATRIX, (float *)&proj);
        glGetFloatv(GL_MODELVIEW_MATRIX,  (float *)&modl);

        res=proj*modl;

        float *clip=(float *)&res;

        side[RIGHT]=  Plane(clip[3]-clip[0], clip[7]-clip[4], clip[11]-clip[8],  clip[15]-clip[12]);
        side[LEFT]=   Plane(clip[3]+clip[0], clip[7]+clip[4], clip[11]+clip[8],  clip[15]+clip[12]);
        side[TOP]=    Plane(clip[3]-clip[1], clip[7]-clip[5], clip[11]-clip[9],  clip[15]-clip[13]);
        side[BOTTOM]= Plane(clip[3]+clip[1], clip[7]+clip[5], clip[11]+clip[9],  clip[15]+clip[13]);
        side[BACK]=   Plane(clip[3]-clip[2], clip[7]-clip[6], clip[11]-clip[10], clip[15]-clip[14]);
        side[FRONT]=  Plane(clip[3]+clip[2], clip[7]+clip[6], clip[11]+clip[10], clip[15]+clip[14]);
    }

    bool Frustum::in(const Vec3t &p, const float &radius) const
    {
        for (size_t i=0; i<6; i++)
            if (side[i].dist(p) <= -radius) return false;
        return true;
    }
}
