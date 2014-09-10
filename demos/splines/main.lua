-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Functional = require("Functional")
local Path = require("Path")

local P = {
    {0.25, 0.25},
    {0.5, 0.5},
    {0.75, 0.75},
    {0.75, 0.25},
    {0.25, 0.75},
    {0.25, 0.25},
    {0.5, 0.5},
    {0.75, 0.75}}
local S = 0.02

local b = nil
local p = nil
local s = nil

local function convert_to_screen(point)
	local screen = rainbow.platform.screen
	return {point[1] * screen.width, point[2] * screen.height}
end

function init()
	local rainbow = rainbow
	local atlas = rainbow.texture("canvas.png")
	local texture = atlas:create(448, 108, 100, 100)
	b = rainbow.spritebatch(1)
	b:set_texture(atlas)
	rainbow.scenegraph:add_batch(b)

	s = b:create_sprite(64, 64)
	s:set_texture(texture)

	p = Path(Functional.map(convert_to_screen, P), Path.Catmull_Rom)
end

function update(dt)
	if p.progress >= 1.0 then
		p:reset()
	end
	local p = p:iterate(dt * S)
	s:set_position(p[1], p[2])
end
