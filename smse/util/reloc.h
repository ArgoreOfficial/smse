#pragma once

#include <stdint.h>
#include <core/process.h>

namespace smse
{

template<typename _Ty>
struct ptr_reloc
{
	ptr_reloc( uintptr_t _offset ) :
		m_ptr{ reinterpret_cast<_Ty*>( smse::getBaseAddr() + _offset ) }
	{
			
	}

	_Ty operator->() { return *m_ptr; }
	_Ty operator*() { return *m_ptr; }

	_Ty* m_ptr;
};

template<typename _Rty, typename... _Args>
struct ptr_reloc<_Rty( _Args... )>
{
	typedef _Rty( *fptr_t )( _Args... );

	ptr_reloc( uintptr_t _offset ) :
		m_fptr{ reinterpret_cast<fptr_t>( smse::getBaseAddr() + _offset ) }
	{

	}

	_Rty operator()( _Args... _args ) {
		m_fptr( _args... );
	}

	fptr_t m_fptr;
};

}