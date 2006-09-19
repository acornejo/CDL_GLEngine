#include <CDL/GLEngine/Model.h>
#include <GL/gl.h>

namespace CDL
{
    const Color Color::white(1,1,1,1), Color::black(0,0,0,0), Color::red(1,0,0,0), Color::green(0,1,0,0), Color::blue(0,0,1,0);

    AABB::AABB(const Vec3t &min, const Vec3t &max)
    {
        m_min=min;
        m_max=max;
        m_center=(m_min+m_max)*0.5;
        m_length=(m_max-m_center).length();
    }

    AABB::AABB(const AABB &x)
    {
        m_min=x.m_min;
        m_max=x.m_max;
        m_center=x.m_center;
        m_length=x.m_length;
    }

    const AABB &AABB::operator=(const AABB &x)
    {
        if (this != &x)
        {
            m_min=x.m_min;
            m_max=x.m_max;
            m_center=x.m_center;
            m_length=x.m_length;
        }

        return *this;
    }

    const float &AABB::getLength() const
    {
        return m_length;
    }

    void AABB::setLength(const float &l)
    {
        m_length=l;
    }

    const Vec3t &AABB::getMin() const
    {
        return m_min;
    }

    const Vec3t &AABB::getMax() const
    {
        return m_max;
    }

    const Vec3t &AABB::getCenter() const
    {
        return m_center;
    }

    Animation::Track::Track(const int &frame)
    {
        m_frame=frame;
        m_bias=0;
        m_tension=0;
        m_continuity=0;
        m_easefrom=0.5;
        m_easeto=0.5;
    }

    Animation::Track::Track(const Animation::Track &x)
    {
        m_frame=x.m_frame;
        m_bias=x.m_bias;
        m_tension=x.m_tension;
        m_continuity=x.m_continuity;
        m_easefrom=x.m_easefrom;
        m_easeto=x.m_easeto;
    }

    const Animation::Track &Animation::Track::operator=(const Animation::Track &x)
    {
        if (this != &x)
        {
            m_frame=x.m_frame;
            m_bias=x.m_bias;
            m_tension=x.m_tension;
            m_continuity=x.m_continuity;
            m_easefrom=x.m_easefrom;
            m_easeto=x.m_easeto;
        }

        return *this;
    }

    const int &Animation::Track::getFrame() const
    {
        return m_frame;
    }

    void Animation::Track::setFrame(const int &frame)
    {
        m_frame=frame;
    }

    void Animation::Track::setBias(const float &bias)
    {
        m_bias=bias;
    }

    const float &Animation::Track::getBias() const
    {
        return m_bias;
    }

    void Animation::Track::setTension(const float &tension)
    {
        m_tension=tension;
    }

    const float &Animation::Track::getTension() const
    {
        return m_tension;
    }

    void Animation::Track::setContinuity(const float &continuity)
    {
        m_continuity=continuity;
    }

    const float &Animation::Track::getContinuity() const
    {
        return m_continuity;
    }

    void Animation::Track::setEaseFrom(const float &easefrom)
    {
        m_easefrom=easefrom;
    }

    const float &Animation::Track::getEaseFrom() const
    {
        return m_easefrom;
    }

    void Animation::Track::setEaseTo(const float &easeto)
    {
        m_easeto=easeto;
    }

    const float &Animation::Track::getEaseTo() const
    {
        return m_easeto;
    }

    Animation::PosTrack::PosTrack(const int &frame, const Vec3t &pos): Track(frame)
    {
        m_position=pos;
    }

    Animation::PosTrack::PosTrack(const Animation::PosTrack &x)
    {
        *((Animation::Track *)this)=*((Animation::Track *)&x);
        m_position=x.m_position;
    }

    const Animation::PosTrack &Animation::PosTrack::operator=(const Animation::PosTrack &x)
    {
        if (this != &x)
        {
            *((Animation::Track *)this)=*((Animation::Track *)&x);
            m_position=x.m_position;
        }

        return *this;
    }

    const Vec3t &Animation::PosTrack::getPosition() const
    {
        return m_position;
    }

    void Animation::PosTrack::setPosition(const Vec3t  &pos)
    {
        m_position=pos;
    }

    Animation::RotTrack::RotTrack(const int &frame, const float &angl, const Vec3t &axis): Track(frame)
    {
        m_angle=angl;
        m_axis=axis;
    }

    Animation::RotTrack::RotTrack(const Animation::RotTrack &x)
    {
        *((Animation::Track *)this)=*((Animation::Track *)&x);
        m_angle=x.m_angle;
        m_axis=x.m_axis;
    }

    const Animation::RotTrack &Animation::RotTrack::operator=(const Animation::RotTrack &x)
    {
        if (this != &x)
        {
            *((Animation::Track *)this)=*((Animation::Track *)&x);
            m_angle=x.m_angle;
            m_axis=x.m_axis;
        }

        return *this;
    }

    const Vec3t &Animation::RotTrack::getAxis() const
    {
        return m_axis;
    }

    void Animation::RotTrack::setAxis(const Vec3t  &axis)
    {
        m_axis=axis;
    }

    const float &Animation::RotTrack::getAngle() const
    {
        return m_angle;
    }

    void Animation::RotTrack::setAngle(const float &angle)
    {
        m_angle=angle;
    }

    Animation::SclTrack::SclTrack(const int &frame, const Vec3t &scl): Track(frame)
    {
        m_scale=scl;
    }

    Animation::SclTrack::SclTrack(const Animation::SclTrack &x)
    {
        *((Animation::Track *)this)=*((Animation::Track *)&x);
        m_scale=x.m_scale;
    }

    const Animation::SclTrack &Animation::SclTrack::operator=(const Animation::SclTrack &x)
    {
        if (this != &x)
        {
            *((Animation::Track *)this)=*((Animation::Track *)&x);
            m_scale=x.m_scale;
        }

        return *this;
    }

    const Vec3t &Animation::SclTrack::getScale() const
    {
        return m_scale;
    }

    void Animation::SclTrack::setScale(const Vec3t  &scl)
    {
        m_scale=scl;
    }

    Animation::Animation() {}

    Animation::Animation(const Animation &x)
    {
        m_pivot=x.m_pivot;
        m_postrack=x.m_postrack;
        m_scltrack=x.m_scltrack;
        m_rottrack=x.m_rottrack;
    }

    const Animation &Animation::operator=(const Animation &x)
    {
        if (this != &x)
        {
            m_pivot=x.m_pivot;
            m_postrack=x.m_postrack;
            m_scltrack=x.m_scltrack;
            m_rottrack=x.m_rottrack;
        }

        return *this;
    }

    const Vec3t &Animation::getPivot() const
    {
        return m_pivot;
    }

    void Animation::setPivot(const Vec3t &p)
    {
        m_pivot=p;
    }

    int Animation::getPosTrackCount() const
    {
        return m_postrack.size();
    }

    int Animation::getSclTrackCount() const
    {
        return m_scltrack.size();
    }

    int Animation::getRotTrackCount() const
    {
        return m_rottrack.size();
    }

    void Animation::addPosTrack(const PosTrack &pos)
    {
        m_postrack.push_back(pos);
    }

    const Animation::PosTrack &Animation::getPosTrack(const int &i)
    {
        return m_postrack[i];
    }

    void Animation::addRotTrack(const RotTrack &rot)
    {
        m_rottrack.push_back(rot);
    }

    const Animation::RotTrack &Animation::getRotTrack(const int &i)
    {
        return m_rottrack[i];
    }

    void Animation::addSclTrack(const SclTrack &scl)
    {
        m_scltrack.push_back(scl);
    }

    const Animation::SclTrack &Animation::getSclTrack(const int &i)
    {
        return m_scltrack[i];
    }

    MeshTriangle::MeshTriangle(const int &v0, const int &v1, const int &v2)
    {
        m_vlist[0]=v2;
        m_vlist[1]=v1;
        m_vlist[2]=v0;
    }

    MeshTriangle::MeshTriangle(const MeshTriangle &x)
    {
        m_vlist[0]=x.m_vlist[0];
        m_vlist[1]=x.m_vlist[1];
        m_vlist[2]=x.m_vlist[2];
        m_nlist[0]=x.m_nlist[0];
        m_nlist[1]=x.m_nlist[1];
        m_nlist[2]=x.m_nlist[2];
    }

    const MeshTriangle &MeshTriangle::operator=(const MeshTriangle &x)
    {
        if (this != &x)
        {
            m_vlist[0]=x.m_vlist[0];
            m_vlist[1]=x.m_vlist[1];
            m_vlist[2]=x.m_vlist[2];
            m_nlist[0]=x.m_nlist[0];
            m_nlist[1]=x.m_nlist[1];
            m_nlist[2]=x.m_nlist[2];
        }

        return *this;
    }

    int *MeshTriangle::getVertexPtr()
    {
        return m_vlist;
    }

    void MeshTriangle::setVertex0(const int &v0)
    {
        m_vlist[0]=v0;
    }

    const int &MeshTriangle::getVertex0() const
    {
        return m_vlist[0];
    }

    void MeshTriangle::setVertex1(const int &v1)
    {
        m_vlist[1]=v1;
    }

    const int &MeshTriangle::getVertex1() const
    {
        return m_vlist[1];
    }

    void MeshTriangle::setVertex2(const int &v2)
    {
        m_vlist[2]=v2;
    }

    const int &MeshTriangle::getVertex2() const
    {
        return m_vlist[2];
    }

    Vec3t *MeshTriangle::getNormalPtr()
    {
        return m_nlist;
    }

    void MeshTriangle::setNormal0(const Vec3t &n0)
    {
        m_nlist[0]=n0;
    }

    const Vec3t &MeshTriangle::getNormal0() const
    {
        return m_nlist[0];
    }

    void MeshTriangle::setNormal1(const Vec3t &n1)
    {
        m_nlist[1]=n1;
    }

    const Vec3t &MeshTriangle::getNormal1() const
    {
        return m_nlist[1];
    }

    void MeshTriangle::setNormal2(const Vec3t &n2)
    {
        m_nlist[2]=n2;
    }

    const Vec3t &MeshTriangle::getNormal2() const
    {
        return m_nlist[2];
    }

    TextureMap::TextureMap(const char *name)
    {
        setName(name);
    }

    TextureMap::TextureMap(const TextureMap &x)
    {
        strcpy(m_name, x.m_name);
        m_tex=x.m_tex;
    }

    const TextureMap &TextureMap::operator=(const TextureMap &x)
    {
        if (this != &x)
        {
            strcpy(m_name, x.m_name);
            m_tex=x.m_tex;
        }

        return *this;
    }

    void TextureMap::setName(const char *name)
    {
        if (name)
            strcpy(m_name,name);
        else
            strcpy(m_name,"");
        if (strlen(m_name) > 0)
            m_tex=Texture(ImageLib::newInstance().load(m_name),Texture::MIPMAP);
    }

    const char *TextureMap::getName() const
    {
        return m_name;
    }

    void TextureMap::select() const
    {
        if (m_tex.isValid())
            m_tex.select();
        else
            m_tex.deselect();
    }

    Color::Color(const float &red, const float &green, const float &blue, const float &alpha)
    {
        c[0]=red;
        c[1]=green;
        c[2]=blue;
        c[3]=alpha;
    }

    Color::Color(const Color &x)
    {
        c[0]=x.c[0];
        c[1]=x.c[1];
        c[2]=x.c[2];
        c[3]=x.c[3];
    }

    const Color &Color::operator=(const Color &x)
    {
        if (this != &x)
        {
            c[0]=x.c[0];
            c[1]=x.c[1];
            c[2]=x.c[2];
            c[3]=x.c[3];
        }

        return *this;
    }

    void Color::setRed(const float &red)
    {
        c[0]=red;
    }

    const float &Color::getRed() const
    {
        return c[0];
    }

    void Color::setGreen(const float &green)
    {
        c[1]=green;
    }

    const float &Color::getGreen() const
    {
        return c[1];
    }

    void Color::setBlue(const float &blue)
    {
        c[2]=blue;
    }

    const float &Color::getBlue() const
    {
        return c[2];
    }

    void Color::setAlpha(const float &alpha)
    {
        c[3]=alpha;
    }

    const float &Color::getAlpha() const
    {
        return c[3];
    }

    void Color::select() const
    {
        glColor4fv(c);
    }

    const float *Color::getPtr() const
    {
        return c;
    }

    Material::Material(const char *name)
    {
        setName(name);
        m_ambt=Color(0.5,0.5,0.5);
        m_diff=Color(0.5,0.5,0.5);
        m_spec=Color(0.5,0.5,0.5);
        m_shin=0.1;
        m_opac=1;
    }

    Material::Material(const Material &x)
    {
        strcpy(m_name, x.m_name);
        m_ambt=x.m_ambt;
        m_diff=x.m_diff;
        m_spec=x.m_spec;
        m_shin=x.m_shin;
        m_opac=x.m_opac;
        m_tmap=x.m_tmap;
    }

    const Material &Material::operator=(const Material &x)
    {
        if (&x != this)
        {
            strcpy(m_name, x.m_name);
            m_ambt=x.m_ambt;
            m_diff=x.m_diff;
            m_spec=x.m_spec;
            m_shin=x.m_shin;
            m_opac=x.m_opac;
            m_tmap=x.m_tmap;
        }
        return *this;
    }

    void Material::setName(const char *name)
    {
        if (name)
            strcpy(m_name,name);
        else
            strcpy(m_name,"");
    }

    const char *Material::getName() const
    {
        return m_name;
    }

    void Material::setAmbient(const Color &ambt)
    {
        m_ambt=ambt;
    }

    const Color &Material::getAmbient() const
    {
        return m_ambt;
    }

    void Material::setDiffuse(const Color &diff)
    {
        m_diff=diff;
    }

    const Color &Material::getDiffuse() const
    {
        return m_diff;
    }

    void Material::setSpecular(const Color &spec)
    {
        m_spec=spec;
    }

    const Color &Material::getSpecular() const
    {
        return m_spec;
    }

    void Material::setShininess(const float &shin)
    {
        m_shin=shin;
    }

    const float &Material::getShininess() const
    {
        return m_shin;
    }

    void Material::setOpacity(const float &opac)
    {
        m_opac=opac;
    }

    const float &Material::getOpacity() const
    {
        return m_opac;
    }

    TextureMap &Material::getTexture()
    {
        return m_tmap;
    }

    void Material::select() const
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT,  m_ambt.getPtr());
        glMaterialfv(GL_FRONT, GL_DIFFUSE,  m_diff.getPtr());
        glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec.getPtr());
        glMaterialf(GL_FRONT, GL_SHININESS, m_shin*128.0f);
        m_tmap.select();
    }

    MeshObject::MeshObject(const char *name)
    {
        setName(name);
        m_matrix.identity();
    }

    MeshObject::MeshObject(const MeshObject &x)
    {
        strcpy(m_name, x.m_name);
        m_uvlist=x.m_uvlist;
        m_vtxlist=x.m_vtxlist;
        m_trilist=x.m_trilist;
        m_objlist=x.m_objlist;
        m_anim=x.m_anim;
        m_mat=x.m_mat;
        m_matrix=x.m_matrix;
        m_list=x.m_list;
    }

    const MeshObject &MeshObject::operator=(const MeshObject &x)
    {
        if (this != &x)
        {
            strcpy(m_name, x.m_name);
            m_uvlist=x.m_uvlist;
            m_vtxlist=x.m_vtxlist;
            m_trilist=x.m_trilist;
            m_objlist=x.m_objlist;
            m_anim=x.m_anim;
            m_mat=x.m_mat;
            m_matrix=x.m_matrix;
            m_list=x.m_list;
        }

        return *this;
    }

    void MeshObject::setName(const char *name)
    {
        if (name)
            strcpy(m_name, name);
        else
            strcpy(m_name, "");
    }

    const char *MeshObject::getName() const
    {
        return m_name;
    }

    const AABB &MeshObject::getBound() const
    {
        return m_bbox;
    }

    void MeshObject::setBound(const AABB &b)
    {
        m_bbox=b;
    }

    void MeshObject::setMaterial(const Material &mat)
    {
        m_mat=mat;
    }

    int MeshObject::getVertexCount() const
    {
        return m_vtxlist.size();
    }

    void MeshObject::addVertex(const Vec3t &vtx)
    {
        m_vtxlist.push_back(vtx);
    }

    const Vec3t &MeshObject::getVertex(const int &i) const
    {
        return m_vtxlist[i];
    }

    void MeshObject::addTexCoord(const Vec2t &vtx)
    {
        m_uvlist.push_back(vtx);
    }

    const Vec2t &MeshObject::getTexCoord(const int &i) const
    {
        return m_uvlist[i];
    }

    int MeshObject::getMeshObjectCount() const
    {
        return m_objlist.size();
    }

    void MeshObject::addMeshObject(const MeshObject &obj)
    {
        m_objlist.push_back(obj);
    }

    const MeshObject &MeshObject::getMeshObject(const int &i) const
    {
        return m_objlist[i];
    }

    MeshObject &MeshObject::getMeshObject(const int &i)
    {
        return m_objlist[i];
    }

    MeshObject *MeshObject::findMeshObject(const char *name)
    {
        if (name)
        {
            if (!strcmp(name,m_name))
                return this;
            else
            {
                MeshObject *tmp;
                for (int i=0; i<m_objlist.size(); i++)
                    if ((tmp=m_objlist[i].findMeshObject(name)) != '\0')
                        return tmp;
            }
        }

        return '\0';
    }

    int MeshObject::getTriangleCount() const
    {
        return m_trilist.size();
    }

    void MeshObject::addTriangle(const MeshTriangle &tri)
    {
        m_trilist.push_back(tri);
    }

    MeshTriangle &MeshObject::getTriangle(const int &i)
    {
        return m_trilist[i];
    }

    Animation &MeshObject::getAnimation()
    {
        return m_anim;
    }

    void MeshObject::setMatrix(const Mat4t &matrix)
    {
        m_matrix=matrix;
    }

    const Mat4t &MeshObject::getMatrix() const
    {
        return m_matrix;
    }

    void MeshObject::render() const
    {
        std::vector<MeshObject>::const_iterator obj_ptr(m_objlist.begin()), obj_end(m_objlist.end());

        while (obj_ptr != obj_end)
        {
            obj_ptr->render();
            obj_ptr++;
        }

        Vec3t pivot=-m_anim.getPivot();
        glPushMatrix();
        glMultMatrixf((float *)&m_matrix);
        glTranslatef(pivot[0], pivot[1], pivot[2]);
        if (m_list.beginList())
        {
            std::vector<MeshTriangle>::const_iterator tri_end(m_trilist.end()), tri_ptr(m_trilist.begin());
            Mat4t inv=!m_matrix;

            glMultMatrixf((float *)&inv);
            m_mat.select();
            glBegin(GL_TRIANGLES);
            while (tri_ptr != tri_end)
            {
                const int *v=&tri_ptr->getVertex0();
                const Vec3t *n=&tri_ptr->getNormal0();
                for (int j=0; j<3; j++)
                {
                    glNormal3fv((float *)&n[j]);
                    if (m_uvlist.size())
                        glTexCoord2fv((float *)&m_uvlist[v[j]]);
                    if (m_vtxlist.size())
                        glVertex3fv((float *)&m_vtxlist[v[j]]);
                }
                tri_ptr++;
            }
            glEnd();

            m_list.endList();
        }
        glPopMatrix();
    }

    void MeshObject::calculateBound()
    {
        for (int i=0; i<m_objlist.size(); i++) m_objlist[i].calculateBound();

        Vec3t min, max;
        if (m_vtxlist.size() > 0)
        {
            min=max=m_vtxlist[0];
            for (int i=1; i<m_vtxlist.size(); i++)
                for (int j=0; j<3; j++)
                {
                    if (m_vtxlist[i][j] > max[j]) max[j]=m_vtxlist[i][j];
                    if (m_vtxlist[i][j] < min[j]) min[j]=m_vtxlist[i][j];
                }
            for (int i=0; i<m_objlist.size(); i++)
                for (int j=0; j<3; j++)
                {
                    if (m_objlist[i].m_bbox.getMax()[j] > max[j]) max[j]=m_objlist[i].m_bbox.getMax()[j];
                    if (m_objlist[i].m_bbox.getMin()[j] < min[j]) min[j]=m_objlist[i].m_bbox.getMin()[j];
                }
        }
        else
        {
            if (m_objlist.size())
            {
                min=m_objlist[0].m_bbox.getMin();
                max=m_objlist[0].m_bbox.getMax();
                for (int i=1; i<m_objlist.size(); i++)
                    for (int j=0; j<3; j++)
                    {
                        if (m_objlist[i].m_bbox.getMax()[j] > max[j]) max[j]=m_objlist[i].m_bbox.getMax()[j];
                        if (m_objlist[i].m_bbox.getMin()[j] < min[j]) min[j]=m_objlist[i].m_bbox.getMin()[j];
                    }

            }
        }
        m_bbox=AABB(min,max);
    }

    void MeshObject::normalize(const AABB &bb)
    {
        float length=m_anim.getPivot().length();

        for (int i=0; i<m_objlist.size(); i++)
        {
            m_objlist[i].normalize(bb);
            length+=m_objlist[i].m_anim.getPivot().length();
        }

        if (length)
            m_anim.setPivot((m_anim.getPivot()-bb.getCenter())/bb.getLength());
        float *m=(float *)&m_matrix;
        m[12]=(m[12]-bb.getCenter()[0])/bb.getLength();
        m[13]=(m[13]-bb.getCenter()[1])/bb.getLength();
        m[14]=(m[14]-bb.getCenter()[2])/bb.getLength();
        for (int i=0; i<m_vtxlist.size(); i++)
            m_vtxlist[i]=(m_vtxlist[i]-bb.getCenter())/bb.getLength();
    }
}
