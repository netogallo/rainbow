// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Recorder.h"

#include <lua.hpp>

NS_RAINBOW_LUA_BEGIN
{
    constexpr bool Recorder::is_constructible;

    const char Recorder::class_name[] = "recorder";

    const luaL_Reg Recorder::functions[]{
        {"get_average_power",  &Recorder::get_average_power},
        {"get_low_pass",       &Recorder::get_low_pass},
        {"get_peak_power",     &Recorder::get_peak_power},
        {"pause",              &Recorder::pause},
        {"record",             &Recorder::record},
        {"stop",               &Recorder::stop},
        {"update",             &Recorder::update},
        {nullptr,              nullptr}};

    Recorder::Recorder(lua_State*) {}

    int Recorder::get_average_power(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        lua_pushnumber(L, self->recorder.average_power());
        return 1;
    }

    int Recorder::get_low_pass(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        lua_pushnumber(L, self->recorder.low_pass());
        return 1;
    }

    int Recorder::get_peak_power(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        lua_pushnumber(L, self->recorder.peak_power());
        return 1;
    }

    int Recorder::pause(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        self->recorder.pause();
        return 0;
    }

    int Recorder::record(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        self->recorder.record();
        return 0;
    }

    int Recorder::stop(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        self->recorder.stop();
        return 0;
    }

    int Recorder::update(lua_State* L)
    {
        Recorder* self = Bind::self(L);
        if (!self)
            return 0;

        self->recorder.update();
        return 0;
    }
} NS_RAINBOW_LUA_END
