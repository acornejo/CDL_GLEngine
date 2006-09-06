//========================================================================
/** @type     C++ Source File
 *  @file     Load.cpp
 *  @author   alcoco
 *  @date     
 *   Created:       18:05:33 29/03/2005
 *   Last Update:   19:31:20 05/09/2006
 */
//========================================================================

#include <CDL/GLEngine/Load3DS.h>
#include <vector>
#include <string>
#include <map>

namespace CDL
{
    enum
    {
        CHUNK_MAIN         = 0x4D4D,           // Main
        CHUNK_MODEL        = 0x3D3D,           //   3D Model
        CHUNK_OBJECT       = 0x4000,           //     Object
        CHUNK_TRIMESH      = 0x4100,           //       Mesh
        CHUNK_VERTLIST     = 0x4110,           //         Vertex list
        CHUNK_POLYLIST     = 0x4120,           //         Polygon list
        CHUNK_OBJMAT       = 0x4130,           //         Material
        CHUNK_UVLIST       = 0x4140,           //         UV list
        CHUNK_SMLIST       = 0x4150,           //         Smoothing groups
        CHUNK_OBJMTX       = 0x4160,           //         Matrix
        CHUNK_MATERIAL     = 0xAFFF,           //     Material
        CHUNK_MATNAME      = 0xA000,           //       Name
        CHUNK_MATAMBT      = 0xA010,           //       Ambient color
        CHUNK_MATDIFF      = 0xA020,           //       Diffuse color
        CHUNK_MATSPEC      = 0xA030,           //       Specular color
        CHUNK_MATSHIN      = 0xA040,           //       Shininnes
        CHUNK_MATOPAC      = 0xA050,           //       Opacity
        CHUNK_MATTEX       = 0xA200,           //       Map
        CHUNK_MATMPFL      = 0xA300,           //         Filename
        CHUNK_MATMPAL      = 0xA35C,           //         Angle
        CHUNK_MATMPUT      = 0xA354,           //         U Tile
        CHUNK_MATMPVT      = 0xA356,           //         V Tile
        CHUNK_MATMPUO      = 0xA358,           //         U Offset
        CHUNK_MATMPVO      = 0xA35A,           //         V Offset
        CHUNK_KFDATA       = 0xB000,           //   KeyFrame Data
        CHUNK_KFHDR        = 0xB00A,           //    Header
        CHUNK_KFOBJECT     = 0xB002,           //     Object Description
        CHUNK_KFHIERARCHY  = 0xB010,           //       Hierarcy
        CHUNK_KFPIVOT      = 0xB013,           //       Pivot
        CHUNK_KFPOSTR      = 0xB020,           //       Position Key Frames
        CHUNK_KFROTTR      = 0xB021,           //       Rotation Key Frames
        CHUNK_KFSCLTR      = 0xB022,           //       Scaling  Key Frames
    };


    typedef std::vector<Object> olist_t;
    typedef std::vector<Material> mlist_t;
    typedef std::map<std::string,int> plist_t;

    class Chunk
    {
        private:
            File &m_fp;
#pragma pack(2)
            struct
            {
                unsigned short id;
                unsigned int   len;
            } chunk;
#pragma pack()
            unsigned int   m_pos;
        public:
            Chunk(File &fp): m_fp(fp) {}
            void read() {m_pos=m_fp.tell();m_fp.read(&chunk,sizeof(chunk));}
            bool operator=(const unsigned short &id) const {return chunk.id == id;}
            operator unsigned short() const {return chunk.id;}
            int getEnd() const {return m_pos+chunk.len;}
            void finish() {m_fp.seek(m_pos+chunk.len);}
    };

    void addChildren(Object &node, const int &node_id, olist_t &olist, plist_t &plist)
    {
        for (int i=0; i<olist.size(); i++)
            if (plist[olist[i].getName()] == node_id)
            {
                addChildren(olist[i],i,olist,plist);
                node.addObject(olist[i]);
            }
    }

    short readShort(File &fp)
    {
        short val;
        fp.readShort(val);
        return val;
    }

    int readInt(File &fp)
    {
        int val;
        fp.readInt(val);
        return val;
    }

    float readFloat(File &fp)
    {
        float val;
        fp.readFloat(val);
        return val;
    }

    const char *readString(File &fp)
    {
        static char str[25], c;
        int i=0;

        fp.readChar(c);
        while(c != EOF && c != '\0')
        {
            str[i++]=tolower(c);
            fp.readChar(c);
        }
        str[i]='\0';

        return str;
    }

    Color readColor(File &fp)
    {
        Chunk chunk(fp);
        Color color;

        chunk.read();
        switch(chunk)
        {
            case 0x0010:
                color.setRed(readFloat(fp));
                color.setGreen(readFloat(fp));
                color.setBlue(readFloat(fp));
                break;
            case 0x0011:
                {
                    char val;
                    fp.readChar(val);
                    color.setRed(((unsigned char)val)/255.0f);
                    fp.readChar(val);
                    color.setGreen(((unsigned char)val)/255.0f);
                    fp.readChar(val);
                    color.setBlue(((unsigned char)val)/255.0f);
                }
                break;
        }
        chunk.finish();

        return color;
    }

    Vec3t readPoint(File &fp)
    {
        float x=readFloat(fp), z=-readFloat(fp), y=readFloat(fp);

        return Vec3t(x,y,z);
    }

    Vec2t readUV(File &fp)
    {
        return Vec2t(readFloat(fp),-readFloat(fp));  
    }

    float readPercent(File &fp)
    {
        Chunk chunk(fp);
        float percent=0;

        chunk.read();
        switch(chunk)
        {
            case 0x0030:
                percent=readShort(fp)/100.0f;
                break;
            case 0x0031:
                percent=readFloat(fp);
                break;
        }
        chunk.finish();

        return percent;
    }

    int readKeyCount(File &fp)
    {
        readShort(fp); // skip flags;
        readInt(fp);   // skip nu1
        readInt(fp);   // skip nu2

        return readInt(fp);
    }

    void readKeyTrack(File &fp, Animation::Track *track)
    {
        track->setFrame(readInt(fp));
        unsigned short flag=readShort(fp);

        if (flag&(1<<0)) track->setTension(readFloat(fp));
        if (flag&(1<<1)) track->setContinuity(readFloat(fp));
        if (flag&(1<<2)) track->setBias(readFloat(fp));
        if (flag&(1<<3)) track->setEaseTo(readFloat(fp));
        if (flag&(1<<4)) track->setEaseFrom(readFloat(fp));
    }

    void processChunks(const int &end, File &fp, olist_t &olist, mlist_t &mlist, plist_t &plist)
    {
        static char name[64];
        static int objindex;
        Chunk chunk(fp);

        while (fp.tell() < end)
        {
            chunk.read();

            switch(chunk)
            {
                case CHUNK_MAIN:
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_MODEL:
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_OBJECT:
                    strcpy(name, readString(fp));
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_TRIMESH:
                    olist.push_back(Object(name));
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_VERTLIST:
                    {
                        Object &obj=olist.back();
                        int num=readShort(fp);
                        for (int i=0; i<num; i++)
                            obj.addVertex(readPoint(fp));
                    }
                    break;
                case CHUNK_POLYLIST:
                    {
                        Object &obj=olist.back();
                        int num=readShort(fp);
                        for (int i=0; i<num; i++, readShort(fp))
                            obj.addTriangle(MeshTriangle(readShort(fp),readShort(fp),readShort(fp)));
                        processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    }
                    break;
                case CHUNK_UVLIST:
                    {
                        Object &obj=olist.back();
                        int num=readShort(fp);
                        for (int i=0; i<num; i++)
                            obj.addTexCoord(readUV(fp));
                    }
                    break;
                case CHUNK_SMLIST:
                    {
                        Object &obj=olist.back();
                        int numtri=obj.getTriangleCount(), numvtx=obj.getVertexCount();
                        int tlist[numtri];
                        Vec3t Nt[numtri];

                        for (int i=0; i<numtri; i++)
                        {
                            tlist[i]=readInt(fp);
                            const int *v=&obj.getTriangle(i).getVertex0();
                            Nt[i]=-cross((obj.getVertex(v[0])-obj.getVertex(v[1])),(obj.getVertex(v[2])-obj.getVertex(v[1])));
                            Nt[i].normalize();
                        }
                        for (int i=0; i<numtri; i++)
                        {
                            if (tlist[i])
                            {
                                const int *vi=&obj.getTriangle(i).getVertex0();
                                for (int j=0; j<3; j++)
                                {
                                    Vec3t N;
                                    for (int k=0; k<numtri; k++)
                                    {
                                        const int *vk=&obj.getTriangle(k).getVertex0();
                                        if (tlist[k]&tlist[i] && (vi[j] == vk[0] || vi[j] == vk[1] || vi[j] == vk[2]))
                                            N+=Nt[k];
                                    }
                                    N.normalize();
                                    obj.addNormal(N);
                                }
                            }
                            else
                            {
                                obj.addNormal(Nt[i]);
                                obj.addNormal(Nt[i]);
                                obj.addNormal(Nt[i]);
                            }
                        }
                    }
                    break;
                case CHUNK_OBJMAT:
                    {
                        strcpy(name,readString(fp));
                        std::vector<Material>::const_iterator mat_ptr=mlist.begin(), mat_end=mlist.end();
                        while (mat_ptr != mat_end)
                        {
                            if (!strcmp(name, mat_ptr->getName()))
                            {
                                olist.back().setMaterial(*mat_ptr);
                                break;
                            }
                            mat_ptr++;
                        }
                    }
                    break;
                case CHUNK_OBJMTX:
                    {
                        Mat4t mat;
                        mat.identity();
                        for (int i=0; i<4; i++)
                            for (int j=0; j<3; j++)
                                mat[i][j]=readFloat(fp);
                        mat.translate(readPoint(fp));
                        olist.back().setMatrix(mat);
                    }
                    break;
                case CHUNK_MATERIAL:
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_MATNAME:
                    strcpy(name,readString(fp));
                    mlist.push_back(Material(name));
                    break;
                case CHUNK_MATAMBT:
                    mlist.back().setAmbient(readColor(fp));
                    break;
                case CHUNK_MATDIFF:
                    mlist.back().setDiffuse(readColor(fp));
                    break;
                case CHUNK_MATSPEC:
                    mlist.back().setSpecular(readColor(fp));
                    break;
                case CHUNK_MATSHIN:
                    mlist.back().setShininess(readPercent(fp));
                    break;
                case CHUNK_MATOPAC:
                    mlist.back().setOpacity(readPercent(fp));
                    break;
                case CHUNK_MATTEX:
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_MATMPFL:
                    mlist.back().getTexture()=TextureMap(readString(fp));
                    break;
                case CHUNK_MATMPAL:
                    mlist.back().getTexture().getTex().setAngle(readFloat(fp));
                    break;
                case CHUNK_MATMPUT:
                    mlist.back().getTexture().getTex().setUScale(readFloat(fp));
                    break;
                case CHUNK_MATMPVT:
                    mlist.back().getTexture().getTex().setVScale(readFloat(fp));
                    break;
                case CHUNK_MATMPUO:
                    mlist.back().getTexture().getTex().setUOffset(readFloat(fp));
                    break;
                case CHUNK_MATMPVO:
                    mlist.back().getTexture().getTex().setVOffset(readFloat(fp));
                    break;
                case CHUNK_KFDATA:
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_KFOBJECT:
                    processChunks(chunk.getEnd(),fp,olist,mlist,plist);
                    break;
                case CHUNK_KFHIERARCHY:
                    strcpy(name,readString(fp));
                    readShort(fp); // skip flags1
                    readShort(fp); // skip flags2
                    plist[std::string(name)]=readShort(fp);
                    for (int i=0; i<olist.size(); i++)
                        if (!strcmp(olist[i].getName(),name))
                            objindex=i;
                    break;
                case CHUNK_KFPIVOT:
                    olist[objindex].getAnimation().setPivot(readPoint(fp));
                    break;
                case CHUNK_KFPOSTR:
                    {
                        int num=readKeyCount(fp);
                        for (int i=0; i<num; i++)
                        {
                            Animation::PosTrack track;
                            readKeyTrack(fp,&track);
                            track.setPosition(readPoint(fp));
                            olist[objindex].getAnimation().addPosTrack(track);
                        }
                    }
                    break;
                case CHUNK_KFROTTR:
                    {
                        int num=readKeyCount(fp);
                        for (int i=0; i<num; i++)
                        {
                            Animation::RotTrack track;
                            readKeyTrack(fp,&track);
                            track.setAngle(readFloat(fp));
                            track.setAxis(readPoint(fp));
                            olist[objindex].getAnimation().addRotTrack(track);
                        }
                    }
                    break;
                case CHUNK_KFSCLTR:
                    {
                        int num=readKeyCount(fp);
                        for (int i=0; i<num; i++)
                        {
                            Animation::SclTrack track;
                            readKeyTrack(fp,&track);
                            track.setScale(readPoint(fp));
                            olist[objindex].getAnimation().addSclTrack(track);
                        }
                    }
                    break;
            }
            chunk.finish();
        }
    }

    Object load3DS(const char *fname)
    {
        File file(fname,File::READ);
        Object obj=load3DS(file);
        file.close();
        return obj;
    }

    Object load3DS(File &fp)
    {
        olist_t olist;
        plist_t plist;
        mlist_t mlist;
        Object dummy;

        processChunks(fp.size(), fp, olist, mlist, plist);

        int num=0;
        for (int i=0; i<olist.size(); i++) if (plist[olist[i].getName()] == -1) num++;

        if (num == 0) printf("ERROR, no objects\n");

        if (num < 2)
        {
            for (int i=0; i<olist.size(); i++)
                if (plist[olist[i].getName()] == -1)
                {
                    addChildren(olist[i],i,olist,plist);
                    dummy=olist[i];
                    break;
                }
        }
        else
        {
            dummy.setName("$$$DUMMY");
            for (int i=0; i<olist.size(); i++)
                if (plist[olist[i].getName()] == -1)
                {
                    addChildren(olist[i],i,olist,plist);
                    dummy.addObject(olist[i]);
                }
        }

        return dummy;
    }
}
