////////////////////////////////////////////////////////////////////////////
//	Module 		: actor_script.cpp
//	Created 	: 17.01.2008
//  Modified 	: 17.01.2008
//	Author		: Dmitriy Iassenev
//	Description : actor script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "actor.h"
#include "level_changer.h"
#include "holder_custom.h"

using namespace luabind;

#pragma optimize("s",on)
void CActor::script_register(lua_State *L)
{
	module(L)
	[
		class_<CActor,CGameObject>("CActor")
			// skif - (транспорт)
			//.def("AttachVehicle", &CActor::attach_Vehicle)
			//.def("DetachVehicle", &CActor::detach_Vehicle)
			// end
			.def(constructor<>())
			
#ifndef	BENCHMARK_BUILD
		,
		class_<CLevelChanger,CGameObject>("CLevelChanger")
			.def(constructor<>())
#endif	//	BENCHMARK_BUILD
	];
}
