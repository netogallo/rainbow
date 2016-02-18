// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_RAINBOW_INTEROP_GRAPHICS_USCENENODE_H_
#define DOTNET_RAINBOW_INTEROP_GRAPHICS_USCENENODE_H_

#include "DotNet/Rainbow/Interop/Helper.h"
#include "Graphics/SceneGraph.h"

namespace rainbow
{
    struct USceneNode
    {
        uint32_t size_of_struct = sizeof(USceneNode);
        /*StdCall*/ void (*dtor_fn)(void*) = &noop;
        /*ThisCall*/ bool (*is_enabled_fn)(SceneNode*) = &is_enabled<SceneNode>;
        /*ThisCall*/ void (*set_enabled_fn)(SceneNode*, bool) = &set_enabled<SceneNode>;

        /*ThisCall*/ SceneNode* (*add_child_fn)(SceneNode*) =
            [](SceneNode* parent) { return parent->add_child(); };

        /*ThisCall*/ SceneNode* (*add_child_animation_fn)(SceneNode*, Animation*) =
            [](SceneNode* parent, Animation* animation) { return parent->add_child(*animation); };

        /*ThisCall*/ SceneNode* (*add_child_label_fn)(SceneNode*, Label*) =
            [](SceneNode* parent, Label* label) { return parent->add_child(*label); };

        /*ThisCall*/ SceneNode* (*add_child_node_fn)(SceneNode*, SceneNode*) =
            [](SceneNode* parent, SceneNode* child) {
                return parent->add_child(std::unique_ptr<SceneNode>{child});
            };

        /*ThisCall*/ SceneNode* (*add_child_spritebatch_fn)(SceneNode*, SpriteBatch*) =
            [](SceneNode* parent, SpriteBatch* sprite_batch) {
                return parent->add_child(*sprite_batch);
            };

        /*ThisCall*/ void (*move_fn)(SceneNode*, Vec2f) = &move<SceneNode>;
        /*StdCall*/ void (*remove_fn)(SceneNode*) = &release<SceneNode>;
    };
}

#endif
