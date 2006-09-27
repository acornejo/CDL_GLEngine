#ifndef __GLE_PARTICLE_H__
#define __GLE_PARTICLE_H__

#include <CDL/CDL.h>
#include <vector>

namespace CDL
{
    using CDL::Vec3t;
    using CDL::Plane;
    using CDL::Sphere;
    using CDL::ODESolver;

    class ParticleSystem: public ODESolver
    {
        public:
            class Spring;
            class Particle // para ser formales falta masa, aqui asumo masa=1
            {
                protected:
                    Vec3t m_position;
                    Vec3t m_velocity;
                    Vec3t m_force;
                    bool  m_locked;

                public:
                    Particle(const Vec3t &p=Vec3t(), const Vec3t &v=Vec3t());
                    Particle(const Particle &);
                    virtual ~Particle();
                    Particle& operator=(const Particle &);
                    const Vec3t &getForce() const;
                    void setForce(const Vec3t &);
                    const Vec3t &getPosition() const;
                    void setPosition(const Vec3t &);
                    const Vec3t &getVelocity() const;
                    void setVelocity(const Vec3t &);
                    const bool &getLocked() const;
                    void setLocked(const bool &);

                    virtual void reset();
                    virtual void render() const;

                    friend class ParticleSystem;
                    friend class Spring;
            };
            class Spring
            {
                protected:
                    float m_kd;
                    float m_ks;
                    float m_length;
                    Vec3t m_normal;
                    Particle *m_pa;
                    Particle *m_pb;

                public:
                    Spring(Particle *pa='\0', Particle *pb='\0', const float &ks=1, const float &kd=0);
                    Spring(const Spring &);
                    virtual ~Spring();
                    Spring& operator=(const Spring &);
                    const float &getStiffness() const;
                    void setStiffness(const float &);
                    const float &getDamping() const;
                    void setDamping(const float &);
                    const float &getLength() const;
                    void setLength(const float &);
                    const Vec3t &getNormal() const;

                    virtual void  applyForce();
                    virtual void render() const;
            };
            typedef std::vector<Particle *> plist;
            typedef std::vector<Spring *>   slist;

        protected:
            plist m_particles;
            slist m_springs;
            float m_viscousity;
            float m_elasticity;
            Vec3t m_field_force;

            void toArray(float []) const;
            void fromArray(const float []);
            void dydt(float []);

        public:
            ParticleSystem(const float &v=0, const float &e=0);
            virtual ~ParticleSystem();
            void add(Particle *);
            Particle *getParticle(const size_t &) const;
            size_t getParticleCount() const;
            void add(Spring *);
            Spring *getSpring(const size_t &) const;
            size_t getSpringCount() const;
            const float &getViscousity() const;
            void setViscousity(const float &);
            const float &getElasticity() const;
            void setElasticity(const float &);
            const Vec3t &getFieldForce() const;
            void setFieldForce(const Vec3t &);
            void calculateForces();
            void applyCollision(const Plane &);
            void applyCollision(const Sphere &);
            virtual void render() const;
            virtual void reset();
    };
}

#endif//__GLE_PARTICLE__
