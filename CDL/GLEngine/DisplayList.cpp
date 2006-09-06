//========================================================================
/** @type     C++ Source File
 *  @file     DisplayList.cpp
 *  @author   acornejo
 *  @date
 *   Created:       16:24:56 10/06/2005
 *   Last Update:   19:26:55 05/09/2006
 */
//========================================================================
#include <CDL/GLEngine/DisplayList.h>
#include <GL/gl.h>

namespace CDL
{
    DEFCLASS("DisplayList");

    DisplayList::DisplayList(const size_t &num)
    {
        m_ref=new size_t(1);
        m_listCount=num;
        if (m_listCount > 0)
            m_listID=glGenLists(m_listCount);
        else
            m_listID=0;
    }

    DisplayList::DisplayList(const DisplayList &d)
    {
        m_ref=d.m_ref;
        ++(*m_ref);
        m_listID=d.m_listID;
        m_listCount=d.m_listCount;
    }

    DisplayList::~DisplayList()
    {
        if (!--(*m_ref))
        {
            if (m_listID > 0)
            {
                glDeleteLists(m_listID, m_listCount);
                m_listID=0;
            }
            delete m_ref;
        }
    }

    const DisplayList &DisplayList::operator=(const DisplayList &d)
    {
        if (&d != this)
        {
            if (!--(*m_ref))
            {
                if (m_listID > 0)
                {
                    glDeleteLists(m_listID, m_listCount);
                    m_listID=0;
                }
                delete m_ref;
            }
            m_ref=d.m_ref;
            ++(*m_ref);
            m_listID=d.m_listID;
            m_listCount=d.m_listCount;
        }

        return *this;
    }

    bool DisplayList::beginList(const size_t &n)
    {
        if (m_listID > 0)
        {
             if (m_listCount == 1 && n == 0)
             {
                  glCallList(m_listID);
                  return false;
             }
             else
             {
                  if (m_listCount < 1 || n >= m_listCount)
                  {
                       Error_send("Unable to begin list %d out of %d\n", n, m_listCount);
                       return false;
                  }
                  else
                  {
                      glNewList(m_listID+n, GL_COMPILE);
                      return true;
                  }
             }
        }
        else
        {
             if (n > 0)
             {
                  Error_send("Unable to begin list %d out of %d\n", n, m_listCount);
                  return false;
             }
             else
             {
                  m_listCount=1;
                  m_listID=glGenLists(m_listCount);
                  glNewList(m_listID, GL_COMPILE_AND_EXECUTE);
                  return true;
             }
        }
    }

    void DisplayList::endList() const
    {
        if (m_listID <= 0)
        {
            Error_send("Tried to finish compiling of invalid list\n");
            return;
        }
        else
            glEndList();
    }

    void DisplayList::call(const size_t &n) const
    {
        if (m_listID <= 0 || n >= m_listCount)
        {
            Error_send("Tried to call invalid list\n");
            return;
        }
        glCallList(m_listID+n);
    }

    void DisplayList::call(const char *str) const
    {
        if (m_listID <= 0)
        {
            Error_send("Tried to call invalid list\n");
            return;
        }
        glListBase(m_listID);
        glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);
    }

    const size_t &DisplayList::getListID() const
    {
         return m_listID;
    }
}
