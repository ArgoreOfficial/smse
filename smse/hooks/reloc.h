#pragma once

#include <stdint.h>
#include <core/process.h>

namespace smse
{
	template<typename _Ty>
	struct sReloc
	{
		sReloc( uintptr_t _offset ) :
			m_ptr{ reinterpret_cast<_Ty>( smse::getBaseAddr() + _offset ) }
		{
			
		}

		_Ty m_ptr;
	};
}