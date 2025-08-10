#pragma once

#include <IR_Log.hpp>

#ifdef IR_USE_TRACY
	#ifndef TRACY_ENABLE
		#define TRACY_ENABLE
	#endif

	#include <tracy/Tracy.hpp>

	#define IR_ZONE ZoneScoped
	#define IR_ZONE_NAME(name) ZoneScopedN(name)
	#define IR_ZONE_FUNC IR_ZONE_NAME(__func__)
	#define IR_THREAD_NAME(name) tracy::SetThreadName(name)
#else
	#define IR_ZONE
	#define IR_ZONE_NAME(name)
	#define IR_ZONE_FUNC
	#define IR_THREAD_NAME(name)
#endif
