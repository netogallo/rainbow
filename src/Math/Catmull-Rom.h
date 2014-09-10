// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_CATMULL_ROM_H_
#define MATH_CATMULL_ROM_H_

#include <cstdint>

#include "Math/Vec2.h"
#include "Memory/Array.h"

namespace rainbow { namespace catmull_rom
{
    Vec2f derivative(uint32_t num_segments, const Vec2f* points, float t);
    Vec2f solve(uint32_t num_segments, const Vec2f* points, float t);
}}  // rainbow::catmull_rom

#endif
