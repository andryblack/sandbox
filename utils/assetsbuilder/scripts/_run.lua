
if not update_only then
	if os.isdir(application.dst_path) then
		assert(os.rmdir(application.dst_path))
		assert(os.mkdir(application.dst_path))
	elseif os.isfile(application.dst_path) then
		error('destionation path is file')
	else
		assert(os.mkdir(application.dst_path))
	end
else
	assert(os.mkdir(application.dst_path))
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
	log = log,
	run_options = run_options or {},
	assert = assert
}


local r = require '_rules'

local __all_rules = {}
local __current_rules = {}
local __global_rules = { _name='global' }

function get_rules()
	return __current_rules
end

local function setup_rules( rules )
	assert(rules)
	for _,v in ipairs(__modules.init_rules) do
		v(rules)
	end
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


extend(__modules.rules)

local data = nil

local __current_path = ''

function sandbox.include( file )
	local sp = __current_path
	__current_path = path.getdirectory(file)
	load_sandbox(file,sandbox,data)
	__current_path = sp
end

function get_current_path()
	return __current_path
end
data = load_sandbox('assets.lua',sandbox,data)

print('rules ready, apply it')

local function apply_rules( rules )
	for _,v in ipairs(__modules.apply_rules) do
		v(rules)
	end
end

for _,v in ipairs(__modules.pre_apply_rules) do
	__all_rules = v(__all_rules)
end

for _,rules in ipairs(__all_rules) do
	print('apply rules',rules._name)
	apply_rules(rules)
end

print('post apply rules')
for _,v in ipairs(__modules.post_apply_rules) do
	v()
end
