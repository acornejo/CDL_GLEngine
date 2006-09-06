//========================================================================
/** @type     C++ Source File
 *  @file     Program.cpp
 *  @author   acornejo
 *  @date
 *   Created:       01:56:26 22/03/2006
 *   Last Update:   04:43:55 22/03/2006
 */
//========================================================================
#include "Program.h"
#include <GL/glew.h>

namespace GLEngine
{
    DEFCLASS("Program");

    Program::Shader::Shader(const Shader_Type &type)
    {
        m_ref=new size_t(1);
        if (type == VERTEX)
            m_id=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        else if (type == FRAGMENT)
            m_id=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        else
        {
            Error_send("Unsupported shader type %d\n", type);
            m_id=0;
        }
    }

    Program::Shader::Shader(const Program::Shader &s)
    {
        m_id=s.m_id;
        m_ref=s.m_ref;
        ++(*m_ref);
    }

    Program::Shader::~Shader()
    {
        if (!--(*m_ref))
        {
            if (m_id)
            {
                 glDeleteObjectARB(m_id);
                 m_id=0;
            }
            delete m_ref;
        }
    }

    const Program::Shader &Program::Shader::operator=(const Program::Shader &s)
    {
         if (this != &s)
         {
            if (!--(*m_ref))
            {
                if (m_id)
                {
                     glDeleteObjectARB(m_id);
                     m_id=0;
                }
                delete m_ref;
            }
            m_id=s.m_id;
            m_ref=s.m_ref;
            ++(*m_ref);
         }
    }

    void Program::Shader::load(RandomAccessInputStream &in)
    {
        std::string buf;
        while (!in.isEOF())
        {
            char line[1024];
            in.readLine(line);
            buf=buf+line+'\n';
        }
        const char *cbuf=buf.c_str();
        glShaderSourceARB(m_id,1,&cbuf,NULL);
    }

    void Program::Shader::compile()
    {
         glCompileShaderARB(m_id);
    }

    bool Program::Shader::isCompiled() const
    {
         GLint status;

         glGetObjectParameterivARB(m_id,GL_OBJECT_COMPILE_STATUS_ARB,&status);

         return status == GL_TRUE;
    }

    std::string Program::Shader::getLog() const
    {
         std::string buf;
         GLint size, read;

         glGetObjectParameterivARB(m_id,GL_OBJECT_INFO_LOG_LENGTH_ARB, &size);
         GLcharARB *cbuf=new char[size];
         glGetInfoLogARB(m_id,size,&read,cbuf);
         buf=cbuf;
         delete []cbuf;

         return buf;
    }

    Program::Program()
    {
        m_id=glCreateProgramObjectARB();
        m_ref=new size_t(1);
        m_shaderCount=0;
    }

    Program::Program(const Program &p)
    {
        m_id=p.m_id;
        m_ref=p.m_ref;
        ++(*m_ref);
        m_shader=p.m_shader;
        m_shaderCount=p.m_shaderCount;
    }

    Program::~Program()
    {
        if (!--(*m_ref))
        {
            std::vector<Shader>::iterator begin(m_shader.begin()), end(m_shader.end());

            while (begin != end)
            {
                glDetachObjectARB(m_id,begin->m_id);
                begin++;
            }
            m_shader.clear();
            glDeleteObjectARB(m_id);
            delete m_ref;
        }
    }

    const Program &Program::operator=(const Program &p)
    {
         if (this != &p)
         {
            if (!--(*m_ref))
            {
                std::vector<Shader>::iterator begin(m_shader.begin()), end(m_shader.end());

                while (begin != end)
                {
                    glDetachObjectARB(m_id,begin->m_id);
                    begin++;
                }
                m_shader.clear();

                glDeleteObjectARB(m_id);
                delete m_ref;
            }
            m_id=p.m_id;
            m_ref=p.m_ref;
            ++(*m_ref);
            m_shader=p.m_shader;
            m_shaderCount=p.m_shaderCount;
         }
    }

    void Program::attach(const Shader &s)
    {
        m_shader.push_back(s);
        glAttachObjectARB(m_id, s.m_id);
    }

    const size_t &Program::getShaderCount() const
    {
         return m_shaderCount;
    }

    const Program::Shader &Program::getShader(const size_t &i) const
    {
         return m_shader[i];
    }

    std::string Program::getLog() const
    {
         std::string buf;
         GLint size, read;

         glGetObjectParameterivARB(m_id,GL_OBJECT_INFO_LOG_LENGTH_ARB, &size);
         GLcharARB *cbuf=new char[size];
         glGetInfoLogARB(m_id,size,&read,cbuf);
         buf=cbuf;
         delete []cbuf;

         return buf;
    }

    bool Program::isLinked() const
    {
         GLint status;
         glGetObjectParameterivARB(m_id,GL_OBJECT_LINK_STATUS_ARB,&status);
         return status == GL_TRUE;
    }


    void Program::link()
    {
        glLinkProgramARB(m_id);
    }

    void Program::use() const
    {
        glUseProgramObjectARB(m_id);
    }

    void Program::disable()
    {
         glUseProgramObjectARB(0);
    }

    void Program::setUniform(const char *name, const int &val)
    {
         GLint var=glGetUniformLocationARB(m_id,name);
         glUniform1iARB(var,val);
    }

    void Program::setUniform(const char *name, const float &val)
    {
         GLint var=glGetUniformLocationARB(m_id,name);
         glUniform1fARB(var,val);
    }

    void Program::setUniform(const char *name, const Vec2t &vec)
    {
         GLint var=glGetUniformLocationARB(m_id,name);
         glUniform2fvARB(var,1,(float *)&vec);
    }

    void Program::setUniform(const char *name, const Vec3t &vec)
    {
         GLint var=glGetUniformLocationARB(m_id,name);
         glUniform3fvARB(var,1,(float *)&vec);
    }

    void Program::setUniform(const char *name, const Vec4t &vec)
    {
         GLint var=glGetUniformLocationARB(m_id,name);
         glUniform4fvARB(var,1,(float *)&vec);
    }

}
