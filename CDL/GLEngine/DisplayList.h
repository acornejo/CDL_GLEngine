//========================================================================
/** @type     C/C++ Header File
 *  @file     DisplayList.h
 *  @author   acornejo
 *  @date
 *   Created:       16:22:37 10/06/2005
 *   Last Update:   19:26:50 05/09/2006
 */
//========================================================================

#ifndef __DISPLAYLIST_H__
#define __DISPLAYLIST_H__

#include <CDL/CDL.h>

namespace CDL
{
    class DisplayList
    {
        private:
            size_t *m_ref;
            size_t m_listCount;
            size_t m_listID;


        public:
            DisplayList(const size_t &l=0);
            DisplayList(const DisplayList &);
            virtual ~DisplayList();
            const DisplayList &operator=(const DisplayList &);
            bool beginList(const size_t &n=0);
            void endList() const;
            void call(const size_t &n) const;
            void call(const char *s) const;
            const size_t &getListID() const;
    };
}

#endif//__DISPLAYLIST_H__
