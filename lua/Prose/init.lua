-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Parser = require((...) .. "/Parser")

local collectgarbage = collectgarbage
local pairs = pairs
local setmetatable = setmetatable

local scenegraph = rainbow.scenegraph
local scoped_node = rainbow.scoped_node

local function clear_table(t)
    for k,_ in pairs(t or {}) do
        t[k] = nil
    end
end

local function from_table(t)
    if type(t) ~= "table" or t.version < Parser.version then
        return nil
    end
    collectgarbage("stop")
    local res = Parser.create_resources(t.resources)
    local node = scenegraph:add_node()
    local objs = Parser.create_nodes(node, res, t.nodes)
    collectgarbage("restart")
    return {
        [0] = scoped_node(node),
        node = node,
        objects = objs,
        resources = res
    }
end

return {from_table = from_table}
