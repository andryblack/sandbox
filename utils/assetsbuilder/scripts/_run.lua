
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

rules = {
	copy_files = {},
	compile_files = {},
	dest_files = {}
}

log:SetPlatformLogEnabled(true)

local function extend( M , pM )
	for k,v in pairs(M) do
		sandbox[k] = v
		print('add rule ', k)
	end
	if pM then
	end
end

local r = require '_rules'

extend(r.assets_rules)


local data = load_sandbox('assets.lua',sandbox,{})
print('rules rady, apply it')
r.apply_rules(rules)
