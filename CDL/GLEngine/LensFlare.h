#ifndef __GLE_LENSFLARE_H__
#define __GLE_LENSFLARE_H__

#include "Texture.h"
#include "Camera.h"
#include <vector>

namespace GLEngine
{
    class LensFlare
    {
        public:
            class Flare
            {
                private:
                    float m_position;
                    float m_scale;
                    float m_intensity;
                    Vec3t m_pos;
                    Texture m_tex;

                public:
                    Flare(const Texture &tex=Texture(), const float &p=0, const float &s=1, const float &i=1);
                    Flare(const Flare &);
                    virtual ~Flare();
                    Flare &operator=(const Flare &);
                    const float &getPosition() const;
                    void setPosition(const float &);
                    const float &getScale() const;
                    void setScale(const float &);
                    const float &getIntensity() const;
                    void setIntensity(const float &);
                    const Texture &getTexture() const;
                    void setTexture(const Texture &);
                    void update(const Vec3t&, const Vec3t&);
                    void render(const Vec3t *) const;
            };
            typedef std::vector<Flare> flist;

        private:
            Vec3t  m_source;
            float  m_scale;
            flist  m_flares;
            Vec3t  m_corner[4];

        public:
            LensFlare(const float &s=1, const Vec3t &l=Vec3t());
            LensFlare(const LensFlare &);
            virtual ~LensFlare();
            LensFlare& operator=(const LensFlare &);
            const Vec3t& getSource() const;
            void setSource(const Vec3t &);
            const float& getScale() const;
            void setScale(const float &);
            void add(const Flare &);
            void update(const Camera &);
            void render() const;
    };
}

#endif//__GLE_LENSFLARE_H__
