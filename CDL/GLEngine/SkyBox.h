#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include <CDL/CDL.h>
#include "Texture.h"

namespace GLEngine
{
    using CDL::Vec3t;
    using CDL::Intersection;
    using CDL::Ray;
    using CDL::Plane;

    class SkyBox
    {
        public:
            typedef enum {UP=0, DOWN, FRONT, BACK, LEFT, RIGHT} SkyBox_Side;

        private:
            Texture side[6];
            Vec3t corner[8];
            Vec3t sun;
			Vec3t dir;

        public:
            SkyBox(const Vec3t &p=Vec3t(), const Vec3t &s=Vec3t());
            SkyBox(const SkyBox &);
            virtual ~SkyBox();
            SkyBox &operator=(const SkyBox &);
            void load(const SkyBox_Side &, const Image &);
            void render() const;
            void setSun(const float&, const float&);
            void setSun(const Vec3t &);
            const Vec3t &getSun() const;
			const Vec3t &getDir() const;
    };
}

#endif//__SKYBOX_H__