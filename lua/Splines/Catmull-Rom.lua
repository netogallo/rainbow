-- Catmull-Rom
--
-- https://stackoverflow.com/questions/9489736/catmull-rom-curve-with-no-cusps-and-no-self-intersections
-- http://ideone.com/NoEbVM
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local floor = math.floor
local pow = math.pow

-- Helpers

local function derivative(coeffs, t)
    local tt = t * t
    return {
        3.0 * coeffs[4][1] * tt + 2.0 * coeffs[3][1] * t + coeffs[2][1],
        3.0 * coeffs[4][2] * tt + 2.0 * coeffs[3][2] * t + coeffs[2][2]
    }
end

local function distance_sq(p0, p1)
    local dx = p1[1] - p0[1]
    local dy = p1[2] - p0[2]
    return dx * dx + dy * dy
end

local function num_segments(points)
    return #points - 3
end

local function practically_zero(dist)
    return dist < 0.0001
end

local function solve(coeffs, t)
    local tt = t * t
    local ttt = tt * t
    return {
        coeffs[4][1] * ttt + coeffs[3][1] * tt + coeffs[2][1] * t + coeffs[1][1],
        coeffs[4][2] * ttt + coeffs[3][2] * tt + coeffs[2][2] * t + coeffs[1][2]
    }
end

local function solve_with(points, t, solve, parameterisation)
    local u = t * num_segments(points)
    local segment = floor(u)
    return solve(parameterisation(points, segment + 1), u - segment)
end

-- Uniform (regular)

local function uniform_coefficients(x0, x1, x2, x3)
    local t1 = 0.5 * (x2 - x0)
    local t2 = 0.5 * (x3 - x1)
    return x1, t1, -3.0 * x1 + 3.0 * x2 - 2.0 * t1 - t2, 2.0 * x1 - 2.0 * x2 + t1 + t2
end

local function uniform_parameterisation(points, i, method)
    local p0 = points[i]
    local p1 = points[i + 1]
    local p2 = points[i + 2]
    local p3 = points[i + 3] or {0.0, 0.0}
    local x0, x1, x2, x3 = uniform_coefficients(p0[1], p1[1], p2[1], p3[1])
    local y0, y1, y2, y3 = uniform_coefficients(p0[2], p1[2], p2[2], p3[2])
    return { {x0, y0}, {x1, y1}, {x2, y2}, {x3, y3} }
end

-- Non-uniform

local function nonuniform_parameterisation(x0, x1, x2, x3, dt0, dt1, dt2)
    local t1 = (x1 - x0) / dt0 - (x2 - x0) / (dt0 + dt1) + (x2 - x1) / dt1
    local t2 = (x2 - x1) / dt1 - (x3 - x1) / (dt1 + dt2) + (x3 - x2) / dt2
    t1 = t1 * dt1
    t2 = t2 * dt1
    return x1, t1, -3.0 * x1 + 3.0 * x2 - 2.0 * t1 - t2, 2.0 * x1 - 2.0 * x2 + t1 + t2
end

local function centripetal_parameterisation(points, i)
    local p0 = points[i]
    local p1 = points[i + 1]
    local p2 = points[i + 2]
    local p3 = points[i + 3] or {0.0, 0.0}
    local dt0 = pow(distance_sq(p0, p1), 0.25)
    local dt1 = pow(distance_sq(p1, p2), 0.25)
    local dt2 = pow(distance_sq(p2, p3), 0.25)
    dt1 = practically_zero(dt1) and 1.0 or dt1
    dt0 = practically_zero(dt0) and dt1 or dt0
    dt2 = practically_zero(dt2) and dt1 or dt2
    local x0, x1, x2, x3 =
        nonuniform_parameterisation(p0[1], p1[1], p2[1], p3[1], dt0, dt1, dt2)
    local y0, y1, y2, y3 =
        nonuniform_parameterisation(p0[2], p1[2], p2[2], p3[2], dt0, dt1, dt2)
    return { {x0, y0}, {x1, y1}, {x2, y2}, {x3, y3} }
end

return {
    derivative = function(points, t)
        return solve_with(points, t, derivative, centripetal_parameterisation)
    end,
    solve = function(points, t)
        return solve_with(points, t, solve, centripetal_parameterisation)
    end,
    derivative_u = function(points, t)
        return solve_with(points, t, derivative, uniform_parameterisation)
    end,
    solve_u = function(points, t)
        return solve_with(points, t, solve, uniform_parameterisation)
    end
}
