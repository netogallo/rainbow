// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Dynamics/Fixture.h"

#include <Box2D/Dynamics/b2Fixture.h>

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Body.h"
#include "ThirdParty/Box2D/StableWorld.h"

NS_B2_LUA_BEGIN
{
	int Filter(lua_State *L)
	{
		const b2Filter filter;
		return Filter(L, filter);
	}

	int Filter(lua_State *L, const b2Filter &filter)
	{
		lua_createtable(L, 0, 3);
		luaR_rawsetinteger(L, "categoryBits", filter.categoryBits);
		luaR_rawsetinteger(L, "maskBits", filter.maskBits);
		luaR_rawsetinteger(L, "groupIndex", filter.groupIndex);
		return 1;
	}

	b2Filter GetFilter(lua_State *L)
	{
		b2Filter filter;
		filter.categoryBits = luaR_getinteger(L, "categoryBits");
		filter.maskBits = luaR_getinteger(L, "maskBits");
		filter.groupIndex = luaR_getinteger(L, "groupIndex");
		return filter;
	}

	int FixtureDef(lua_State *L)
	{
		lua_createtable(L, 0, 6);

		luaR_rawsetnil(L, "shape");
		luaR_rawsetnumber(L, "friction", 0.2f);
		luaR_rawsetnumber(L, "restitution", 0.0f);
		luaR_rawsetnumber(L, "density", 0.0f);
		luaR_rawsetboolean(L, "isSensor", false);

		lua_pushliteral(L, "filter");
		Filter(L);
		lua_rawset(L, -3);

		return 1;
	}

	b2FixtureDef GetFixtureDef(lua_State *L)
	{
		b2FixtureDef fd;

		luaR_getfield(L, "shape");
		fd.shape = GetShape(L);
		lua_pop(L, 1);

		fd.friction = luaR_getnumber(L, "friction");
		fd.restitution = luaR_getnumber(L, "restitution");
		fd.density = luaR_getnumber(L, "density");
		fd.isSensor = luaR_getboolean(L, "isSensor");

		luaR_getfield(L, "filter");
		fd.filter = GetFilter(L);
		lua_pop(L, 1);

		return fd;
	}

	void Fixture::Init(lua_State *L)
	{
		luaR_rawsetcfunction(L, "Filter", &Filter);
		luaR_rawsetcfunction(L, "FixtureDef", &FixtureDef);

		Rainbow::Lua::reg<Fixture>(L);
	}

	Fixture::Fixture(lua_State *L)
	    : fixture_(static_cast<b2Fixture*>(lua_touserdata(L, -1))) {}

	int Fixture::GetType(lua_State *L)
	{
		return get1i(L, [](const b2Fixture *fixture) {
			return fixture->GetType();
		});
	}

	int Fixture::GetShape(lua_State *L)
	{
		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		return Shape(L, self->get()->GetShape());
	}

	int Fixture::SetSensor(lua_State *L)
	{
		return set1b(L, [](b2Fixture *fixture, const bool sensor) {
			fixture->SetSensor(sensor);
		});
	}

	int Fixture::IsSensor(lua_State *L)
	{
		return get1b(L, [](const b2Fixture *fixture) {
			return fixture->IsSensor();
		});
	}

	int Fixture::SetFilterData(lua_State *L)
	{
		Rainbow::Lua::Argument<void*>::is_required(L, 2);

		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		self->get()->SetFilterData(GetFilter(L));
		return 0;
	}

	int Fixture::GetFilterData(lua_State *L)
	{
		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		const b2Filter &filter = self->get()->GetFilterData();
		return Filter(L, filter);
	}

	int Fixture::Refilter(lua_State *L)
	{
		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		self->get()->Refilter();
		return 0;
	}

	int Fixture::GetBody(lua_State *L)
	{
		return get1ud<Body>(L, [](b2Fixture *fixture) {
			return fixture->GetBody();
		});
	}

	int Fixture::GetNext(lua_State *L)
	{
		return get1ud<Fixture>(L, [](b2Fixture *fixture) {
			return fixture->GetNext();
		});
	}

	int Fixture::TestPoint(lua_State *L)
	{
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);

		return get1b(
		    L,
		    [](const b2Fixture *fixture, b2Vec2 p) {
		      return fixture->TestPoint(p);
		    },
		    Vec2(L, 2, 3));
	}

	int Fixture::RayCast(lua_State *)
	{
		return -1;
	}

	int Fixture::GetMassData(lua_State *L)
	{
		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		b2MassData mass;
		self->get()->GetMassData(&mass);
		return MassData(L, mass);
	}

	int Fixture::SetDensity(lua_State *L)
	{
		return set1f(L, [](b2Fixture *fixture, const float density) {
			fixture->SetDensity(density);
		});
	}

	int Fixture::GetDensity(lua_State *L)
	{
		return get1f(L, [](const b2Fixture *fixture) {
			return fixture->GetDensity();
		});
	}

	int Fixture::GetFriction(lua_State *L)
	{
		return get1f(L, [](const b2Fixture *fixture) {
			return fixture->GetFriction();
		});
	}

	int Fixture::SetFriction(lua_State *L)
	{
		return set1f(L, [](b2Fixture *fixture, const float friction) {
			fixture->SetFriction(friction);
		});
	}

	int Fixture::GetRestitution(lua_State *L)
	{
		return get1f(L, [](const b2Fixture *fixture) {
			return fixture->GetRestitution();
		});
	}

	int Fixture::SetRestitution(lua_State *L)
	{
		return set1f(L, [](b2Fixture *fixture, const float restitution) {
			fixture->SetRestitution(restitution);
		});
	}

	int Fixture::GetAABB(lua_State *)
	{
		return -1;
	}

	int Fixture::Dump(lua_State *L)
	{
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);

		Fixture *self = Bind::self(L);
		if (!self)
			return 0;

		self->get()->Dump(static_cast<int32>(lua_tointeger(L, 2)));
		return 0;
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::Lua::Fixture;

	template<>
	const char Fixture::Bind::class_name[] = "b2Fixture";

	template<>
	const bool Fixture::Bind::is_constructible = false;

	template<>
	const luaL_Reg Fixture::Bind::functions[] = {
	    {"GetType",         &Fixture::GetType},
	    {"GetShape",        &Fixture::GetShape},
	    {"SetSensor",       &Fixture::SetSensor},
	    {"IsSensor",        &Fixture::IsSensor},
	    {"SetFilterData",   &Fixture::SetFilterData},
	    {"GetFilterData",   &Fixture::GetFilterData},
	    {"Refilter",        &Fixture::Refilter},
	    {"GetBody",         &Fixture::GetBody},
	    {"GetNext",         &Fixture::GetNext},
	    {"TestPoint",       &Fixture::TestPoint},
	    {"RayCast",         &Fixture::RayCast},
	    {"GetMassData",     &Fixture::GetMassData},
	    {"SetDensity",      &Fixture::SetDensity},
	    {"GetDensity",      &Fixture::GetDensity},
	    {"GetFriction",     &Fixture::GetFriction},
	    {"SetFriction",     &Fixture::SetFriction},
	    {"GetRestitution",  &Fixture::GetRestitution},
	    {"SetRestitution",  &Fixture::SetRestitution},
	    {"GetAABB",         &Fixture::GetAABB},
	    {"Dump",            &Fixture::Dump},
	    {nullptr,           nullptr}
	};
} NS_RAINBOW_LUA_END
