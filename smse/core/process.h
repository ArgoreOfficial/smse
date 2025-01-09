#pragma once

#include <stdint.h>

namespace smse {

	uintptr_t getBaseAddr();

	void _processInit();
	void _processOnLaunch();

}