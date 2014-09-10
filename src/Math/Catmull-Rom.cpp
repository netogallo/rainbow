// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Math/Catmull-Rom.h"

#include <array>
#include <cmath>

namespace
{
    using a3f = std::array<float, 3>;
    using a4f = std::array<float, 4>;
    using a4Vec2f = std::array<Vec2f, 4>;

    bool near_zero(float dist) { return dist < 0.0001f; }

    a4f nonuniform_parameterisation(const a4f& x, const a3f& dt)
    {
        const float t1 =
            ((x[1] - x[0]) / dt[0] - (x[2] - x[0]) / (dt[0] + dt[1]) +
             (x[2] - x[1]) / dt[1]) *
            dt[1];
        const float t2 =
            ((x[2] - x[1]) / dt[1] - (x[3] - x[1]) / (dt[1] + dt[2]) +
             (x[3] - x[2]) / dt[2]) *
            dt[1];
        return {{x[1],
                 t1,
                 -3.0f * x[1] + 3.0f * x[2] - 2.0f * t1 - t2,
                 2.0f * x[1] - 2.0f * x[2] + t1 + t2}};
    }

    template <typename F, typename G>
    Vec2f solve_with(uint32_t segments,
                     const Vec2f* points,
                     float t,
                     F&& parameterisation,
                     G&& solve)
    {
        const float u = t * segments;
        const float segment = std::floor(u);
        return solve(parameterisation(points, segment), u - segment);
    }

    a4Vec2f zip4(const a4f& x, const a4f& y)
    {
        return {{{x[0], y[0]}, {x[1], y[1]}, {x[2], y[2]}, {x[3], y[3]}}};
    }

    a4Vec2f centripetal_parameterisation(const Vec2f* points, uint32_t i)
    {
        const a4Vec2f p{
            {points[i], points[i + 1], points[i + 2], points[i + 3]}};
        a3f dt{{std::sqrt(p[0].distance(p[1])),
                std::sqrt(p[1].distance(p[2])),
                std::sqrt(p[2].distance(p[3]))}};
        dt[1] = (near_zero(dt[1]) ? 1.0f : dt[1]);
        dt[0] = (near_zero(dt[0]) ? dt[1] : dt[0]);
        dt[2] = (near_zero(dt[2]) ? dt[1] : dt[2]);
        return zip4(
            nonuniform_parameterisation({{p[0].x, p[1].x, p[2].x, p[3].x}}, dt),
            nonuniform_parameterisation(
                {{p[0].y, p[1].y, p[2].y, p[3].y}}, dt));
    }

    Vec2f cr_derivative(const a4Vec2f& coefs, float t)
    {
        const float tt = t * t;
        return {3.0f * coefs[3].x * tt + 2.0f * coefs[2].x * t + coefs[1].x,
                3.0f * coefs[3].y * tt + 2.0f * coefs[2].y * t + coefs[1].y};
    }

    Vec2f cr_solve(const a4Vec2f& coefs, float t)
    {
        const float tt = t * t;
        const float ttt = tt * t;
        return {
            coefs[3].x * ttt + coefs[2].x * tt + coefs[1].x * t + coefs[0].x,
            coefs[3].y * ttt + coefs[2].y * tt + coefs[1].y * t + coefs[0].y};
    }
}

namespace rainbow { namespace catmull_rom
{
    Vec2f derivative(uint32_t segments, const Vec2f* points, float t)
    {
        return solve_with(segments,
                          points,
                          t,
                          centripetal_parameterisation,
                          cr_derivative);
    }

    Vec2f solve(uint32_t segments, const Vec2f* points, float t)
    {
        return solve_with(
            segments, points, t, centripetal_parameterisation, cr_solve);
    }
}}  // rainbow::catmull_rom
