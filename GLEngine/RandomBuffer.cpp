#include "RandomBuffer.h"

namespace GLEngine
{
	DEFCLASS("RandomBuffer");

	RandomBuffer::RandomBuffer(const size_t &bits, const float &a, const float &b)
	{
		if (bits > 16)
		{
			m_buffer=new float[1];
			m_buffer[0]=0;
			m_bits=0;
			m_mask=0;
			m_index=0;
			Error_send("RandomBuffer maximum size is 16 bits wide\n");
		} else
		{
			size_t num=1<<bits;
			m_buffer=new float[num];
			m_bits=bits;
			m_mask=0;
			for (size_t i=0; i<m_bits; i++)
				m_mask|=1<<i;
			m_index=0;
		}
		regen(a,b);
	}

	RandomBuffer::~RandomBuffer()
	{
		delete []m_buffer;
	}

	void RandomBuffer::regen(const float &a, const float &b)
	{
		CDL::Random rnd;
		rnd.randomize();
		size_t num=1<<m_bits;

		for (size_t i=0; i<num; i++)
			m_buffer[i]=rnd.uniform(a,b);
	}
	float RandomBuffer::get() const
	{
		m_index++;
		m_index&=m_mask;
		return m_buffer[m_index];
	}
};
