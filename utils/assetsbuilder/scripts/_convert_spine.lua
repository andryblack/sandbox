
local _M = {}

_M.assets_rules = {}

local img = require '_images'

function _M.assets_rules.convert_spine( n )
	local i = rules.convert_spine or {}
	i[n]=n
	rules.convert_spine = i
	rules.dest_files[n .. 'xml']=true
end

function _M.assets_rules.convert_spine_to( n , t , options)
	local i = rules.convert_spine or {}
	i[n]=t
	if options then
		local o = {}
		for k,v in pairs(options) do
			o[k]=v
		end
		o.out = t
		i[n]=o
	end
	rules.convert_spine = i
	rules.dest_files[n .. 'xml']=true
end

local function apply_convert_spine( k,v )
	if type(v)=='table' then
		local a = assert(application:open_spine(k..'.atlas',k..'.json'))
		if v.rename_animations then
			for ak,av in pairs(v.rename_animations) do
				assert(a:RenameAnimation(ak,av),
					'failed rename animation ' .. ak .. ' to ' .. av)
			end
		end
		a:Export(v.out .. '.xml',application)
	else
		assert(application:convert_spine(k..'.atlas',k..'.json',v .. '.xml'))
	end
end

function _M.apply( rules )

	local skelets = rules.convert_spine or {}
	for k,v in pairs(skelets) do
		apply_convert_spine(k,v)
	end

end

return _M