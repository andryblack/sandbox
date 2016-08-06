
local _M = {}

_M.assets_rules = {}

local img = require '_images'

function _M.init_rules(rules)
	rules.convert_spine = {}
end

function _M.assets_rules.convert_spine( n )
	local i = assert(get_rules().convert_spine)
	i[n]=n
	get_rules().dest_files[n .. '.xml']=true
end

function _M.assets_rules.convert_spine_to( n , t , options)
	local i = assert(get_rules().convert_spine)
	i[t]=n
	if options then
		local o = {}
		for k,v in pairs(options) do
			o[k]=v
		end
		o.out = t
		o.src = n
		i[t]=o
	end
	get_rules().dest_files[t .. '.xml']=true
end

local function apply_convert_spine( k,v )
	if type(v)=='table' then
		local a = assert(application:open_spine(v.src..'.atlas',v.src..'.json'))
		if v.rename_animations then
			for ak,av in pairs(v.rename_animations) do
				assert(a:RenameAnimation(ak,av),
					'failed rename animation ' .. ak .. ' to ' .. av)
			end
		end
		a:Export(v.out .. '.xml',application)
	else
		assert(application:convert_spine(v..'.atlas',v..'.json',k .. '.xml'))
	end
end

function _M.apply( rules )

	local skelets = rules.convert_spine or {}
	for k,v in pairs(skelets) do
		apply_convert_spine(k,v)
	end

end

return _M