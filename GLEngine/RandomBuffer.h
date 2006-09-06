#ifndef __GLE_RANDOMBUFFER_H__
#define __GLE_RANDOMBUFFER_H__

#include <CDL/CDL.h>

namespace GLEngine
{
	class RandomBuffer
	{
		private:
			float  *m_buffer;
			size_t  m_bits;
			size_t  m_mask;
			mutable size_t m_index;

		public:
			RandomBuffer(const size_t &i=0, const float &a=0, const float &b=1);
			RandomBuffer(const RandomBuffer &);
			virtual ~RandomBuffer();
			RandomBuffer &operator=(const RandomBuffer &);
			void regen(const float &a=0, const float &b=1);
			float get() const;
	};
}

#endif//__GLE_RANDOMBUFFER_H__
