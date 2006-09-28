#include <CDL/GLEngine/Particle.h>

namespace CDL
{
    ParticleSystem::Particle::Particle(const Vec3t &p, const Vec3t &v)
    {
        m_position=p;
        m_velocity=v;
        m_locked=false;
        m_force=Vec3t();
    }

    ParticleSystem::Particle::Particle(const Particle &p)
    {
        m_position=p.m_position;
        m_velocity=p.m_velocity;
        m_locked=p.m_locked;
        m_force=p.m_force;
    }

    ParticleSystem::Particle::~Particle() {}

    ParticleSystem::Particle &ParticleSystem::Particle::operator=(const Particle &p)
    {
        if (&p != this)
        {
            m_position=p.m_position;
            m_velocity=p.m_velocity;
            m_locked=p.m_locked;
            m_force=p.m_force;
        }
        return *this;
    }

    const Vec3t &ParticleSystem::Particle::getForce() const
    {
        return m_force;
    }

    void ParticleSystem::Particle::setForce(const Vec3t &f)
    {
        m_force=f;
    }

    const Vec3t &ParticleSystem::Particle::getPosition() const
    {
        return m_position;
    }

    void ParticleSystem::Particle::setPosition(const Vec3t &p)
    {
        m_position=p;
    }

    const Vec3t &ParticleSystem::Particle::getVelocity() const
    {
        return m_velocity;
    }

    void ParticleSystem::Particle::setVelocity(const Vec3t &v)
    {
        m_velocity=v;
    }

    const bool &ParticleSystem::Particle::getLocked() const
    {
        return m_locked;
    }

    void ParticleSystem::Particle::setLocked(const bool &locked)
    {
        m_locked=locked;
    }

    void ParticleSystem::Particle::render() const {}

    ParticleSystem::Spring::Spring(Particle *pa, Particle *pb, const float &ks, const float &kd)
    {
         m_pa=pa;
         m_pb=pb;
         m_ks=ks;
         m_kd=kd;
         if (pa && pb)
         {
             m_normal=m_pa->getPosition()-m_pb->getPosition();
             m_length=m_normal.length();
             m_normal/=m_length;
         }
         else
         {
             m_length=1;
             m_normal=Vec3t(0,1,0);
         }
    }

    ParticleSystem::Spring::Spring(const Spring &s)
    {
        m_pa=s.m_pa;
        m_pb=s.m_pb;
        m_kd=s.m_kd;
        m_ks=s.m_ks;
        m_length=s.m_length;
        m_normal=s.m_normal;
    }

    ParticleSystem::Spring::~Spring() {}

    ParticleSystem::Spring &ParticleSystem::Spring::operator=(const Spring &s)
    {
        if (&s != this)
        {
            m_pa=s.m_pa;
            m_pb=s.m_pb;
            m_kd=s.m_kd;
            m_ks=s.m_ks;
            m_length=s.m_length;
            m_normal=s.m_normal;
        }
        return *this;
    }
    const float &ParticleSystem::Spring::getStiffness() const
    {
         return m_ks;
    }

    void ParticleSystem::Spring::setStiffness(const float &ks)
    {
         m_ks=ks;
    }

    const float &ParticleSystem::Spring::getDamping() const
    {
         return m_kd;
    }

    void ParticleSystem::Spring::setDamping(const float &kd)
    {
         m_kd=kd;
    }

    const float &ParticleSystem::Spring::getLength() const
    {
         return m_length;
    }

    void ParticleSystem::Spring::setLength(const float &length)
    {
         m_length=length;
    }

    const Vec3t &ParticleSystem::Spring::getNormal() const
    {
        return m_normal;
    }

    void ParticleSystem::Spring::applyForce()
    {
         if (m_pa && m_pb)
         {
              m_normal=m_pa->m_position-m_pb->m_position;
              Vec3t v=m_pa->m_velocity-m_pb->m_velocity;
              float n_length=m_normal.length();

              m_normal/=n_length; // normalize
              Vec3t force=m_normal*(m_ks*(n_length-m_length)+m_kd*dot(v,m_normal));
              m_pa->m_force-=force;
              m_pb->m_force+=force;
         }
    }

    void ParticleSystem::Spring::render() const {}

    ParticleSystem::ParticleSystem(const float &v, const float &e)
    {
        m_viscousity=v;
        m_elasticity=e;
    }

    ParticleSystem::~ParticleSystem()
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        while (p != pend)
        {
            delete *p;
            p++;
        }

        m_particles.clear();

        slist::iterator s(m_springs.begin());
        slist::const_iterator send(m_springs.end());

        while (s != send)
        {
            delete *s;
            s++;
        }

        m_springs.clear();
    }


    void ParticleSystem::add(Particle *p)
    {
        m_particles.push_back(p);
        m_size+=6;
    }

    void ParticleSystem::add(Spring *s)
    {
         m_springs.push_back(s);
    }

    ParticleSystem::Particle *ParticleSystem::getParticle(const size_t &i) const
    {
        return m_particles[i];
    }

    size_t ParticleSystem::getParticleCount() const
    {
        return m_particles.size();
    }

    ParticleSystem::Spring *ParticleSystem::getSpring(const size_t &i) const
    {
        return m_springs[i];
    }

    size_t ParticleSystem::getSpringCount() const
    {
        return m_springs.size();
    }

    const float &ParticleSystem::getViscousity() const
    {
        return m_viscousity;
    }

    void ParticleSystem::setViscousity(const float &v)
    {
        m_viscousity=v;
    }

    const float &ParticleSystem::getElasticity() const
    {
        return m_elasticity;
    }

    void ParticleSystem::setElasticity(const float &e)
    {
        m_elasticity=e;
    }

    const Vec3t &ParticleSystem::getFieldForce() const
    {
        return m_field_force;
    }

    void ParticleSystem::setFieldForce(const Vec3t &ff)
    {
        m_field_force=ff;
    }

    void ParticleSystem::calculateForces()
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        while (p != pend)
        {
            (*p)->m_force=m_field_force;
            p++;
        }

        slist::iterator s(m_springs.begin());
        slist::const_iterator send(m_springs.end());

        while (s != send)
        {
            (*s)->applyForce();
            s++;
        }


        p=m_particles.begin();
        if (m_viscousity > 0)
        {
            while (p != pend)
            {
                (*p)->m_force-=(*p)->m_velocity*m_viscousity;
                p++;
            }
        }
    }

    void ParticleSystem::applyCollision(const Plane &pln)
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());
        Vec3t normal=pln.getNormal();

        while (p != pend)
        {
            float dist=pln.dist((*p)->m_position);
            if (dist < 0)
            {
                (*p)->m_position-=normal*dist;
                (*p)->m_velocity-=normal*dot(normal,(*p)->m_velocity)*(1+m_elasticity);
            }
            p++;
        }
    }

    void ParticleSystem::applyCollision(const Sphere &sph)
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());
        float radius=sph.getRadius(), radius2=radius*radius;
        Vec3t point=sph.getPoint();

        while (p != pend)
        {
            Vec3t normal=(*p)->m_position-point;
            if (normal.norm() < radius2)
            {
                float length=normal.length(), dist=length-radius;
                normal/=length;
                (*p)->m_position-=normal*dist;
                (*p)->m_velocity-=normal*dot(normal,(*p)->m_velocity)*(1+m_elasticity);
            }
            p++;
        }
    }

    void ParticleSystem::render() const
    {
        slist::const_iterator s(m_springs.begin());
        slist::const_iterator send(m_springs.end());

        while (s != send)
        {
            (*s)->render();
            s++;
        }

        plist::const_iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        while (p != pend)
        {
            (*p)->render();
            p++;
        }
    }

    void ParticleSystem::toArray(float out[]) const
    {
        plist::const_iterator begin(m_particles.begin()), end(m_particles.end());
        float *v_ptr=out;

        while (begin != end)
        {
            const Particle *part=*begin;
            const float *x=(float *)&part->m_position, *v=(float *)&part->m_velocity;
            *v_ptr++=*x++;
            *v_ptr++=*x++;
            *v_ptr++=*x++;
            *v_ptr++=*v++;
            *v_ptr++=*v++;
            *v_ptr++=*v++;
            begin++;
        }
    }

    void ParticleSystem::fromArray(const float in[])
    {
        plist::iterator begin(m_particles.begin());
        plist::const_iterator end(m_particles.end());
        const float *v_ptr=in;

        while (begin != end)
        {
            Particle *part=*begin;
            float *x=(float *)&part->m_position, *v=(float *)&part->m_velocity;
            if (!part->m_locked)
            {
                *x++=*v_ptr++;
                *x++=*v_ptr++;
                *x++=*v_ptr++;
                *v++=*v_ptr++;
                *v++=*v_ptr++;
                *v++=*v_ptr++;
            }
            else
                v_ptr+=6;
            begin++;
        }
    }

    void ParticleSystem::dydt(float out[])
    {
        calculateForces();
        plist::const_iterator begin(m_particles.begin()), end(m_particles.end());
        float *v_ptr=out;

        while (begin != end)
        {
            const Particle *part=*begin;
            const float *v=(float *)&part->m_velocity, *f=(float *)&part->m_force;
            *v_ptr++=*v++;
            *v_ptr++=*v++;
            *v_ptr++=*v++;
            *v_ptr++=*f++;
            *v_ptr++=*f++;
            *v_ptr++=*f++;
            begin++;
        }
    }
}
