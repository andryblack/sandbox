
local _M = {}

_M.assets_rules = {}

local img = require '_images'

function _M.assets_rules.convert_spine( n )
	local i = rules.convert_spine or {}
	i[n]=n
	rules.convert_spine = i
	rules.dest_files[n .. 'xml']=true
end

local function apply_convert_spine( k,v )
	assert(application:convert_spine(k..'.atlas',k..'.json',v .. '.xml'))
end

function _M.apply( rules )

	local skelets = rules.convert_spine or {}
	for k,v in pairs(skelets) do
		apply_convert_spine(k,v)
	end

end

return _M