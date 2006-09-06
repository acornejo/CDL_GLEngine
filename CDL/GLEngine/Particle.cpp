#include <CDL/GLEngine/Particle.h>

namespace CDL
{
    ParticleSystem::Particle::Particle(const Vec3t &p, const Vec3t &v, const float &l, const float &r)
    {
        m_position=p;
        m_velocity=v;
        m_radius=r;
    }

    ParticleSystem::Particle::Particle(const Particle &p)
    {
        m_position=p.m_position;
        m_velocity=p.m_velocity;
        m_radius=p.m_radius;
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
        m_position+=m_velocity*dt;  // x=v
        m_velocity+=m_force*dt;     // v=f/m
        m_force=Vec3t();
    }

    ParticleSystem::String::String(Particle *pa, Particle *pb)
    {
         m_pa=pa;
         m_pb=pb;
         m_kd=1;
         m_ks=1;
         m_length=1;
    }

    const float &ParticleSystem::String::getStiffness() const
    {
         return m_ks;
    }

    void ParticleSystem::String::setStiffness(const float &ks)
    {
         m_ks=ks;
    }

    const float &ParticleSystem::String::getDamping() const
    {
         return m_kd;
    }

    void ParticleSystem::String::setDamping(const float &kd)
    {
         m_kd=kd;
    }

    const float &ParticleSystem::String::getLength() const
    {
         return m_length;
    }

    void ParticleSystem::String::setLength(const float &length)
    {
         m_length=length;
    }

    void ParticleSystem::String::applyForce() const
    {
         if (m_pa && m_pb)
         {
              Vec3t x=m_pa->m_position-m_pb->m_position;
              Vec3t v=m_pa->m_velocity-m_pb->m_velocity;
              float xl=x.length(), xl_inv=1.0/xl;

              Vec3t force=x*-(m_ks*(xl-m_length)+m_kd*dot(v,x)*xl_inv)*xl_inv;
              m_pa->m_force+=force;
              m_pb->m_force-=force;
         }
    }

    ParticleSystem::ParticleSystem(const float &v, const float &e)
    {
        viscousity=v;
        elasticity=e;
    }

    ParticleSystem::~ParticleSystem()
    {
        plist::iterator p=particles.begin();
        plist::const_iterator pend=particles.end();

        while (p != pend)
        {
            delete *p;
            p++;
        }

        particles.clear();
    }

    void ParticleSystem::add(Particle *p)
    {
        particles.push_back(p);
    }

    void ParticleSystem::add(String *s)
    {
         strings.push_back(s);
    }

    void ParticleSystem::applyStrings()
    {
        slist::iterator s(strings.begin());
        slist::const_iterator send(strings.end());

        while (s != send)
        {
            (*s)->applyForce();
            s++;
        }
    }

    void ParticleSystem::applyDrag()
    {
        plist::iterator p=particles.begin();
        plist::const_iterator pend=particles.end();

        while (p != pend)
        {
            (*p)->m_force-=(*p)->m_velocity*viscousity;
            p++;
        }
    }

    void ParticleSystem::applyForce(const Vec3t &accel)
    {
        plist::iterator p=particles.begin();
        plist::const_iterator pend=particles.end();

        while (p != pend)
        {
            (*p)->m_force+=accel; // f=m*a
            p++;
        }
    }

    void ParticleSystem::applyCollision(const Plane &pln)
    {
        plist::iterator p=particles.begin();
        plist::const_iterator pend=particles.end();
        Vec3t normal=pln.getNormal();

        while (p != pend)
        {
            float dist=pln.dist((*p)->m_position)-(*p)->m_radius;
            if (dist < 0)
            {
                (*p)->m_position-=normal*dist;
                (*p)->m_velocity-=normal*dot(normal,(*p)->m_velocity)*(1+elasticity);
            }
            p++;
        }
    }

    void ParticleSystem::applyCollision()
    {
        plist::iterator pi=particles.begin();
        plist::const_iterator pend=particles.end();

        while (pi != pend)
        {
            plist::iterator pj=pi+1;
            while (pj != pend)
            {
                Vec3t normal=(*pi)->m_position-(*pj)->m_position;
                float r=(*pi)->m_radius+(*pj)->m_radius;
                if (normal.norm() < r*r)
                {
                    float dist=(r-normal.length())*0.5;
                    normal.normalize();
                    Vec3t dV=normal*dot(normal,((*pj)->m_velocity-(*pi)->m_velocity))*elasticity;
                    (*pi)->m_position+=normal*dist;
                    (*pj)->m_position-=normal*dist;
                    (*pi)->m_velocity+=dV;
                    (*pj)->m_velocity-=dV;
                }
                pj++;
            }
            pi++;
        }
    }

    void ParticleSystem::update(const float &dt)
    {
        plist::iterator p=particles.begin();
        plist::const_iterator pend=particles.end();

        while (p != pend)
        {
            (*p)->update(dt);
            p++;
        }
    }

    void ParticleSystem::reset()
    {
        plist::iterator p=particles.begin();
        plist::const_iterator pend=particles.end();

        while (p != pend)
        {
            (*p)->reset();
            p++;
        }
    }

    void ParticleSystem::render() const
    {
        plist::const_iterator p=particles.begin();
        plist::const_iterator pend=particles.end();

        while (p != pend)
        {
            (*p)->render();
            p++;
        }
    }
}
