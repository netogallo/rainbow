// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Animation.h"

#include "Lua/lua_Sprite.h"

namespace
{
    const char kErrorHandlingAnimationStateEvent[] =
        "An error occurred while handling an animation state event";

    Animation::Frames get_frames(lua_State* L, int n)
    {
        const size_t count = lua_rawlen(L, n);
        auto frames = std::make_unique<Animation::Frame[]>(count + 1);
        int i = 0;
        std::generate_n(frames.get(),
                        count,
                        [L, n, &i]
                        {
                            lua_rawgeti(L, n, ++i);
                            return lua_tointeger(L, -1);
                        });
        lua_pop(L, i);
        frames[count] = Animation::kAnimationEnd;
        return Animation::Frames(
            // The cast is necessary for Visual Studio 2013.
            static_cast<const Animation::Frame*>(frames.release()));
    }
}

NS_RAINBOW_LUA_BEGIN
{
    constexpr bool Animation::is_constructible;

    const char Animation::class_name[] = "animation";

    const luaL_Reg Animation::functions[]{
        {"current_frame",   &Animation::current_frame},
        {"is_stopped",      &Animation::is_stopped},
        {"set_delay",       &Animation::set_delay},
        {"set_frame_rate",  &Animation::set_frame_rate},
        {"set_frames",      &Animation::set_frames},
        {"set_listener",    &Animation::set_listener},
        {"set_sprite",      &Animation::set_sprite},
        {"jump_to",         &Animation::jump_to},
        {"rewind",          &Animation::rewind},
        {"start",           &Animation::start},
        {"stop",            &Animation::stop},
        {nullptr,           nullptr}};

    Animation::Animation(lua_State* L)
    {
        // rainbow.animation(<sprite>, frames{}, fps, loop_delay = 0)
        checkargs<nil_or<Sprite>, void*, lua_Number, nil_or<lua_Number>>(L);

        SpriteRef sprite;
        replacetable(L, 1);
        if (lua_isuserdata(L, 1))
            sprite = touserdata<Sprite>(L, 1)->get();
        animation_ = std::make_unique<::Animation>(
            sprite, get_frames(L, 2), lua_tointeger(L, 3),
            optinteger(L, 4, 0));
    }

    int Animation::current_frame(lua_State* L)
    {
        return get1i(L, [](::Animation* animation) {
            return animation->current_frame();
        });
    }

    int Animation::is_stopped(lua_State* L)
    {
        return get1b(L, [](const ::Animation* animation) {
            return animation->is_stopped();
        });
    }

    int Animation::set_delay(lua_State* L)
    {
        // <animation>:set_delay(delay_in_ms)
        return set1i(L, [](::Animation* animation, int delay) {
            animation->set_delay(delay);
        });
    }

    int Animation::set_frame_rate(lua_State* L)
    {
        // <animation>:set_frame_rate(fps)
        return set1i(L, [](::Animation* animation, int fps) {
            animation->set_frame_rate(fps);
        });
    }

    int Animation::set_frames(lua_State* L)
    {
        // <animation>:set_frames({f1, f2, ...})
        checkargs<Animation, void*>(L);

        Animation* self = Bind::self(L);
        if (!self)
            return 0;

        self->animation_->set_frames(get_frames(L, 2));
        return 0;
    }

    int Animation::set_listener(lua_State* L)
    {
        checkargs<Animation, nil_or<void*>>(L);

        Animation* self = Bind::self(L);
        if (!self)
            return 0;

        if (!lua_istable(L, 2))
        {
            self->listener_.reset();
            self->animation_->set_callback(nullptr);
            return 0;
        }
        lua_settop(L, 2);
        self->listener_.reset(L);
        self->animation_->set_callback(
            [L, self](::Animation*, ::Animation::Event e) {
                self->listener_.get();
                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);
                    self->animation_->set_callback(nullptr);
                    return;
                }

                switch (e)
                {
                    case ::Animation::Event::Start:
                        lua_getfield(L, -1, "on_animation_start");
                        break;
                    case ::Animation::Event::End:
                        lua_getfield(L, -1, "on_animation_end");
                        break;
                    case ::Animation::Event::Complete:
                        lua_getfield(L, -1, "on_animation_complete");
                        break;
                    case ::Animation::Event::Frame:
                        lua_getfield(L, -1, "on_animation_frame");
                        break;
                }
                if (!lua_isfunction(L, -1))
                {
                    lua_pop(L, 2);
                    return;
                }

                lua_insert(L, -2);
                call(L, 1, 0, 0, kErrorHandlingAnimationStateEvent);
            });
        return 0;
    }

    int Animation::set_sprite(lua_State* L)
    {
        // <animation>:set_sprite(<sprite>)
        return set1ud<Sprite>(
            L,
            [](::Animation* animation, const SpriteRef& sprite) {
              animation->set_sprite(sprite);
            });
    }

    int Animation::jump_to(lua_State* L)
    {
        return set1i(L, [](::Animation* animation, int frame) {
            animation->jump_to(static_cast<unsigned int>(frame));
        });
    }

    int Animation::rewind(lua_State* L)
    {
        Animation* self = Bind::self(L);
        if (!self)
            return 0;

        self->animation_->rewind();
        return 0;
    }

    int Animation::start(lua_State* L)
    {
        Animation* self = Bind::self(L);
        if (!self)
            return 0;

        self->animation_->start();
        return 0;
    }

    int Animation::stop(lua_State* L)
    {
        Animation* self = Bind::self(L);
        if (!self)
            return 0;

        self->animation_->stop();
        return 0;
    }
} NS_RAINBOW_LUA_END
