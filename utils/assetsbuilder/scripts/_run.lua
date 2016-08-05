
if not update_only then
	if os.isdir(dst_path) then
		assert(os.rmdir(dst_path))
		assert(os.mkdir(dst_path))
	elseif os.isfile(dst_path) then
		error('destionation path is file')
	end
else
	assert(os.mkdir(dst_path))
end

local function wrap( t )
	if t then
		return setmetatable({},
			{
				__index=t,
				__newindex=function(p,v) 
					error('write to protected field ' .. v) 
			end}
			)
	end
	return t
end

local sandbox = {
	os = wrap(os),
	path = wrap(path),
	string = wrap(string),
	table = wrap(table),
	print = print,
	error = error,
	platform = platform,
	log = log
}


local r = require '_rules'

local extensions = require '_extenstions'

local __all_rules = {}
local __current_rules = {}
local __global_rules = { _name='global' }

function get_rules()
	return __current_rules
end

local function setup_rules( rules )
	assert(rules)
	r.init_rules(rules)
	extensions.init_rules(rules)
end

setup_rules( __global_rules )
table.insert(__all_rules,__global_rules)
__current_rules = __global_rules

function sandbox.begin_group(name)
	if name then
		local r = nil
		for _,v in ipairs(__all_rules) do
			if v._name == name then
				r = v
				break
			end
		end
		if not r then
			r = {_name = name }
			setup_rules(r)
			table.insert(__all_rules,r)
		end
		__current_rules = r
	else
		__current_rules = __global_rules
	end
end


log:SetPlatformLogEnabled(true)

local function extend( M )
	for k,v in pairs(M) do
		sandbox[k] = v
		print('add rule ', k)
	end
end


extend(r.assets_rules)

extend(extensions.assets_rules)
extensions.base = r

local data = nil

function sandbox.include( file )
	load_sandbox(file,sandbox,data)
end

data = load_sandbox('assets.lua',sandbox,data)

print('rules ready, apply it')

for _,rules in ipairs(__all_rules) do
	print('apply rules',rules._name)
	r.apply_rules(rules)
	extensions.apply_rules(rules)
end
