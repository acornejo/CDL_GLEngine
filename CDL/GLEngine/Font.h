#ifndef __GLE_FONT_H__
#define __GLE_FONT_H__

#include <CDL/GLEngine/Texture.h>
#include <CDL/GLEngine/DisplayList.h>
#include <stdio.h>

namespace CDL
{
    using CDL::InputStream;

    class Font
    {
		public:
			typedef enum {CENTER=-1, TOP=-2, LEFT=-3, BOTTOM=-4, RIGHT=-5} Font_Alignment;

        private:
            DisplayList m_list;
			Texture     m_tex;
            size_t      m_size;

        public:
            Font(const Image &i=Image());
            Font(InputStream &);
            Font(const Font &);
            Font& operator=(const Font &);
            void print(const size_t &s, const int &x, const int &y, const string &str);
            const size_t &getSize() const;
    };
}
#endif//__GLE_FONT_H__
