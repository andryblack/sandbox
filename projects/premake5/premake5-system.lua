

print('hello premake5')


local p = premake
local api = p.api

local xcode = require 'xcode'

local old_getfiletype = xcode.getfiletype
local old_getbuildcategory = xcode.getbuildcategory

-- function xcode.getbuildcategory(node)
-- 	local res = old_getbuildcategory(node)
-- 	print('overrided getbuildcategory',node.path, node.isResource, res)
-- 	return res
-- end

-- function xcode.getfiletype( node, cfg )
-- 	local res = old_getfiletype( node, cfg )
-- 	print('overrided getfiletype',node.path, res)
-- 	return res
-- end


-- ios

table.insert(premake.option.get('os').allowed,{'ios',"Apple iOS"})
table.insert(premake.option.get('os').allowed,{'flash',"Crossbridge flash"})
		
