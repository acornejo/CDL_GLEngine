#ifndef __GLE_PARTICLE_H__
#define __GLE_PARTICLE_H__

#include <CDL/CDL.h>
#include <vector>

namespace CDL
{
    using CDL::Vec3t;
    using CDL::Plane;

    class ParticleSystem
    {
        public:
            class String;
            class Particle // para ser formales falta masa, y radius  quizas "sobre"
            {
                protected:
                    Vec3t m_position;
                    Vec3t m_velocity;
                    Vec3t m_force;
                    float m_radius;

                public:
                    Particle(const Vec3t &p=Vec3t(), const Vec3t &v=Vec3t(), const float &l=0, const float &r=0);
                    Particle(const Particle &);
                    virtual ~Particle();
                    Particle& operator=(const Particle &);
                    const Vec3t &getForce() const;
                    void setForce(const Vec3t &);
                    const Vec3t &getPosition() const;
                    void setPosition(const Vec3t &);
                    const Vec3t &getVelocity() const;
                    void setVelocity(const Vec3t &);
                    const float &getRadius() const;
                    void setRadius(const float &);

                    virtual void reset();
                    virtual void update(const float &);
                    virtual void render() const;

                    friend class ParticleSystem;
                    friend class String;
            };
            class String
            {
                protected:
                    float m_kd;
                    float m_ks;
                    float m_length;
                    Particle *m_pa;
                    Particle *m_pb;

                public:
                    String(Particle *pa='\0', Particle *pb='\0');
                    const float &getStiffness() const;
                    void setStiffness(const float &);
                    const float &getDamping() const;
                    void setDamping(const float &);
                    const float &getLength() const;
                    void setLength(const float &);
                    void applyForce() const;
            };
            typedef std::vector<Particle *> plist;
            typedef std::vector<String *>   slist;

        private:
            plist particles;
            slist strings;
            float viscousity;
            float elasticity;

        public:
            ParticleSystem(const float &v=0, const float &e=0);
            virtual ~ParticleSystem();
            void add(Particle *);
            void add(String *);
            void applyStrings();
            void applyDrag();
            void applyForce(const Vec3t &);
            void applyCollision();
            void applyCollision(const Plane &);
            void update(const float &);
            void reset();
            virtual void render() const;

    };
}

#endif//__GLE_PARTICLE__
