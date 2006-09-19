#ifndef __MODEL_H__
#define __MODEL_H__

#include <CDL/CDL.h>
#include <CDL/GLEngine/Texture.h>
#include <CDL/GLEngine/DisplayList.h>
#include <vector>

namespace CDL
{
    using CDL::Mat4t;
    using CDL::Vec3t;
    using CDL::Vec2t;
    using CDL::ImageLib;

    class AABB
    {
        private:
            Vec3t m_max, m_min, m_center;
            float m_length;

        public:
            AABB(const Vec3t &mi=Vec3t(), const Vec3t &ma=Vec3t());
            AABB(const AABB &);
            const AABB &operator=(const AABB &);
            const float &getLength() const;
            void setLength(const float &);
            const Vec3t &getMin() const;
            const Vec3t &getMax() const;
            const Vec3t &getCenter() const;
    };

    class Animation
    {
        public:
            class Track
            {
                private:
                    int m_frame;
                    float m_bias;
                    float m_tension;
                    float m_continuity;
                    float m_easefrom;
                    float m_easeto;

                public:
                    Track(const int &f=0);
                    Track(const Track &);
                    const Track &operator=(const Track &);
                    const int &getFrame() const;
                    void setFrame(const int &);
                    const float &getBias() const;
                    void setBias(const float &);
                    const float &getTension() const;
                    void setTension(const float &);
                    const float &getContinuity() const;
                    void setContinuity(const float &);
                    const float &getEaseTo() const;
                    void setEaseTo(const float &);
                    const float &getEaseFrom() const;
                    void setEaseFrom(const float &);

            };
            class PosTrack: public Track
            {
                private:
                    Vec3t m_position;
                public:
                    PosTrack(const int &f=0, const Vec3t &p=Vec3t());
                    PosTrack(const PosTrack &);
                    const PosTrack &operator=(const PosTrack &);
                    const Vec3t &getPosition() const;
                    void setPosition(const Vec3t &);
            };
            class RotTrack: public Track
            {
                private:
                    float m_angle;
                    Vec3t m_axis;

                public:
                    RotTrack(const int &f=0, const float &a=0, const Vec3t &r=Vec3t());
                    RotTrack(const RotTrack &);
                    const RotTrack &operator=(const RotTrack &);
                    const float &getAngle() const;
                    void setAngle(const float &);
                    const Vec3t &getAxis() const;
                    void setAxis(const Vec3t &);
            };
            class SclTrack: public Track
            {
                private:
                    Vec3t m_scale;

                public:
                    SclTrack(const int &f=0, const Vec3t &s=Vec3t(1,1,1));
                    SclTrack(const SclTrack &);
                    const SclTrack &operator=(const SclTrack &);
                    const Vec3t &getScale() const;
                    void setScale(const Vec3t &);
            };

        private:
            Vec3t m_pivot;
            std::vector<PosTrack> m_postrack;
            std::vector<SclTrack> m_scltrack;
            std::vector<RotTrack> m_rottrack;

        public:
            Animation();
            Animation(const Animation &);
            const Animation &operator=(const Animation &);
            const Vec3t &getPivot() const;
            void setPivot(const Vec3t &);
            int getPosTrackCount() const;
            void addPosTrack(const PosTrack &);
            const PosTrack &getPosTrack(const int &);
            int getSclTrackCount() const;
            void addSclTrack(const SclTrack &);
            const SclTrack &getSclTrack(const int &);
            int getRotTrackCount() const;
            void addRotTrack(const RotTrack &);
            const RotTrack &getRotTrack(const int &);
    };

    class MeshTriangle
    {
        private:
            int m_vlist[3];
            Vec3t m_nlist[3];

        public:
            MeshTriangle(const int &v0=0, const int &v1=0, const int &v2=0);
            MeshTriangle(const MeshTriangle &);
            const MeshTriangle &operator=(const MeshTriangle &);
            int *getVertexPtr();
            void setVertex0(const int &);
            const int &getVertex0() const;
            void setVertex1(const int &);
            const int &getVertex1() const;
            void setVertex2(const int &);
            const int &getVertex2() const;
            Vec3t *getNormalPtr();
            void setNormal0(const Vec3t &);
            const Vec3t &getNormal0() const;
            void setNormal1(const Vec3t &);
            const Vec3t &getNormal1() const;
            void setNormal2(const Vec3t &);
            const Vec3t &getNormal2() const;
    };

    class TextureMap
    {
        private:
            char  m_name[16];
            Texture m_tex;

        public:
            TextureMap(const char *n='\0');
            TextureMap(const TextureMap &);
            const TextureMap &operator=(const TextureMap &);
            void setName(const char *);
            const char *getName() const;
            Texture &getTex() {return m_tex;}
            void select() const;
    };

    class Color
    {
        public:
            static const Color white, black, red, green, blue;

        private:
            float c[4];

        public:
            Color(const float &r=0, const float &g=0, const float &b=0, const float &a=0);
            Color(const Color &);
            const Color &operator=(const Color &);
            void setRed(const float &);
            const float &getRed() const;
            void setGreen(const float &);
            const float &getGreen() const;
            void setBlue(const float &);
            const float &getBlue() const;
            void setAlpha(const float &);
            const float &getAlpha() const;
            void select() const;
            const float *getPtr() const;
    };

    class Material
    {
        private:
            char m_name[16];
            TextureMap m_tmap;
            Color m_ambt;
            Color m_diff;
            Color m_spec;
            float m_shin;
            float m_opac;

        public:
            Material(const char *name='\0');
            Material(const Material &);
            const Material &operator=(const Material &);
            void setName(const char *);
            const char *getName() const;
            void setAmbient(const Color &);
            const Color &getAmbient() const;
            void setDiffuse(const Color &);
            const Color &getDiffuse() const;
            void setSpecular(const Color &);
            const Color &getSpecular() const;
            void setShininess(const float &);
            const float &getShininess() const;
            void setOpacity(const float &);
            const float &getOpacity() const;
            TextureMap &getTexture();
            void select() const;
    };

    class MeshObject
    {
        private:
            char                  m_name[16];
            std::vector<Vec2t>    m_uvlist;
            std::vector<Vec3t>    m_vtxlist;
            std::vector<MeshTriangle> m_trilist;
            std::vector<MeshObject>   m_objlist;
            AABB                  m_bbox;
            Animation             m_anim;
            Material              m_mat;
            Mat4t                 m_matrix;
            mutable DisplayList           m_list;

        public:
            MeshObject(const char *n='\0');
            MeshObject(const MeshObject &);
            const MeshObject &operator=(const MeshObject &);
            void setName(const char *);
            const char *getName() const;
            void setBound(const AABB &);
            const AABB &getBound() const;
            void setMaterial(const Material &);
            int getVertexCount() const;
            void addVertex(const Vec3t &);
            const Vec3t &getVertex(const int &) const;
            void addTexCoord(const Vec2t &);
            const Vec2t &getTexCoord(const int &) const;
            int getMeshObjectCount() const;
            void addMeshObject(const MeshObject &);
            const MeshObject &getMeshObject(const int &) const;
            MeshObject *findMeshObject(const char *);
            MeshObject &getMeshObject(const int &);
            int getTriangleCount() const;
            void addTriangle(const MeshTriangle &);
            MeshTriangle &getTriangle(const int &);
            void setMatrix(const Mat4t &);
            const Mat4t &getMatrix() const;
            Animation &getAnimation();
            void calculateBound();
            void normalize(const AABB &);
            void render() const;
    };
}

#endif//__MODEL_H__
