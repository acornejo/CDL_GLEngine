#ifndef __GLE_FRUSTUM_H__
#define __GLE_FRUSTUM_H__

#include <CDL/CDL.h>

namespace GLEngine
{
    using CDL::Plane;
    using CDL::Vec3t;
    using CDL::Mat4t;

    class Frustum
    {
        private:
            Plane side[6];

        public:
			Frustum(){};
            Frustum(const Frustum &);
            Frustum& operator=(const Frustum &);
            void update();
            bool in(const Vec3t &, const float &radius=0) const;
    };
}

#endif//__GLE_FRUSTUM_H__
