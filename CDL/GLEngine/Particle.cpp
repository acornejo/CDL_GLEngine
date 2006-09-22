#include <CDL/GLEngine/Particle.h>

namespace CDL
{
    ParticleSystem::Particle::Particle(const Vec3t &p, const Vec3t &v, const float &r)
    {
        m_position=p;
        m_velocity=v;
        m_radius=r;
        m_force=Vec3t();
    }

    ParticleSystem::Particle::Particle(const Particle &p)
    {
        m_position=p.m_position;
        m_velocity=p.m_velocity;
        m_radius=p.m_radius;
        m_force=p.m_force;
    }

    ParticleSystem::Particle::~Particle() {}

    ParticleSystem::Particle &ParticleSystem::Particle::operator=(const Particle &p)
    {
        if (&p != this)
        {
            m_position=p.m_position;
            m_velocity=p.m_velocity;
            m_force=p.m_force;
            m_radius=p.m_radius;
        }
        return *this;
    }

    const float &ParticleSystem::Particle::getRadius() const
    {
        return m_radius;
    }

    void ParticleSystem::Particle::setRadius(const float &r)
    {
        m_radius=r;
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

    void ParticleSystem::Particle::reset() {}
    void ParticleSystem::Particle::render() const {}
    void ParticleSystem::Particle::update(const float &dt)
    {
        m_velocity+=m_force*dt;     // v=f/m but we have no mass
        m_position+=m_velocity*dt;  // x=v
        m_force=Vec3t();
    }

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

    void ParticleSystem::Spring::update()
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

    void ParticleSystem::Spring::render() const {
    }

    ParticleSystem::ParticleSystem(const float &v, const float &e)
    {
        m_viscousity=v;
        m_elasticity=e;
        m_drag=false;
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

    const bool &ParticleSystem::hasDrag() const
    {
        return m_drag;
    }

    void ParticleSystem::toggleDrag()
    {
        m_drag=!m_drag;
    }

    void ParticleSystem::add(Particle *p)
    {
        m_particles.push_back(p);
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

    void ParticleSystem::applyForce(const Vec3t &accel)
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        while (p != pend)
        {
            (*p)->m_force+=accel; // f=m*a
            p++;
        }
    }

    void ParticleSystem::applyCollision(const Plane &pln)
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());
        Vec3t normal=pln.getNormal();

        while (p != pend)
        {
            float dist=pln.dist((*p)->m_position)-(*p)->m_radius;
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
            if (normal.norm() < radius2+(*p)->m_radius*(*p)->m_radius)
            {
                float length=normal.length(), dist=length-radius-(*p)->m_radius;
                normal/=length;
                (*p)->m_position-=normal*dist;
                (*p)->m_velocity-=normal*dot(normal,(*p)->m_velocity)*(1+m_elasticity);
//                (*p)->m_position=point+normal*radius;
//                (*p)->m_velocity*=.95f;
            }
            p++;
        }
    }

    void ParticleSystem::applyCollision()
    {
        plist::iterator pi(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        while (pi != pend)
        {
            plist::iterator pj(pi+1);
            while (pj != pend)
            {
                Vec3t normal=(*pi)->m_position-(*pj)->m_position;
                float r=(*pi)->m_radius+(*pj)->m_radius;
                if (normal.norm() < r*r)
                {
                    float length=normal.length(), dist=(length-r)*0.5;
                    normal/=length;
                    Vec3t dV=normal*dot(normal,((*pi)->m_velocity-(*pj)->m_velocity))*(1+m_elasticity);
                    (*pi)->m_position-=normal*dist;
                    (*pj)->m_position+=normal*dist;
                    (*pi)->m_velocity-=dV;
                    (*pj)->m_velocity+=dV;
                }
                pj++;
            }
            pi++;
        }
    }

    void ParticleSystem::update(const float &dt)
    {
        slist::iterator s(m_springs.begin());
        slist::const_iterator send(m_springs.end());

        while (s != send)
        {
            (*s)->update();
            s++;
        }

        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        if (m_drag)
        {
            while (p != pend)
            {
//                (*p)->m_velocity*=(1-m_viscousity);
                (*p)->m_force-=(*p)->m_velocity*m_viscousity;
                (*p)->update(dt);
                p++;
            }
        }
        else
        {
            while (p != pend)
            {
                (*p)->update(dt);
                p++;
            }
        }
    }

    void ParticleSystem::reset()
    {
        plist::iterator p(m_particles.begin());
        plist::const_iterator pend(m_particles.end());

        while (p != pend)
        {
            (*p)->reset();
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
}
