#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#include <CDL/CDL.h>
#include <CDL/GLEngine/MultiTexture.h>
#include <CDL/GLEngine/Texture.h>
#include <CDL/GLEngine/RenderableObject.h>

namespace CDL
{
	using CDL::Vec3t;
	using CDL::Vec2t;

	class HeightMap: public RenderableObject
	{
		public:
			typedef unsigned short ushort;

		private:
			size_t *m_ref;
			Vec3t   m_scale;
            size_t  m_width;
            size_t  m_height;
            MultiTexture m_tex;
			Vec2t   *tData;
			Vec3t   *vData;
			ushort  *vIndex;

		public:
			HeightMap();
			HeightMap(const Image &, const Vec3t &, const int &s=1);
			HeightMap(const HeightMap &);
			virtual ~HeightMap();
			const HeightMap &operator=(const HeightMap &);
			void setTexture(const MultiTexture &);
            const MultiTexture &getTexture() const;
			float getHeight(const float &x, const float &y) const;
			void render() const;
			const Vec3t &getScale() const;
	};
}

#endif//__HEIGHTMAP_H__
