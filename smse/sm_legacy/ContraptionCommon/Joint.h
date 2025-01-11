/**
 * @file Joint.h
 * 
 */
#pragma once

namespace sm
{

class Joint
{
	enum Types
	{
		Unknown = 1,
		Bearing,
		Piston
	};

	virtual ~Joint() { };

	/*
	 *    1 - unknown
	 *    2 - bearing
	 *    3 - piston
	 **/
	virtual Types getType() { return Unknown; };

	char padding[ 216 - sizeof(void*) ];
};

static_assert( sizeof( Joint ) == 0xd8, "Joint must be 0xd8 bytes" );

}
