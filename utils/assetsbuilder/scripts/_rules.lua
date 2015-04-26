


local _M = {}

_M.assets_rules = {}

local img = require '_images'

for k,v in pairs(img.assets_rules) do
	_M.assets_rules[k] = v
end



local function copy_files_pattern( pattern )
	if type(pattern) ~= 'string' then
		error('copy_files_pattern string expected got ' .. type(pattern) )
	end
	--print('copy_files_pattern:',pattern)
	local files = os.matchfiles(path.join(src_path,pattern))
	for k,v in ipairs(files) do
		local f = path.getrelative(src_path,v)
		--print('copy file',k,f)
		rules.copy_files[f]=f
		rules.dest_files[f]=true
	end
	
end

function _M.assets_rules.copy_file( file )
	local src = file[1]
	local dst = file[2] or src
	rules.copy_files[src]=dst
	rules.dest_files[dst]=true
end

function _M.assets_rules.copy_files( file_or_filelist )
	if type(file_or_filelist) == 'table' then
		for _,v in ipairs(file_or_filelist) do
			copy_files_pattern(v)
		end
	elseif type(file_or_filelist) == 'string' then
		copy_files_pattern(file_or_filelist)
	else
		error('copy_files table or string expected got ' .. type(file_or_filelist))
	end
end


local function expand_dirs( p , d )
	local dir = iif(string.startswith(p, "/"), "/", "")
	for part in p:gmatch("[^/]+") do
		dir = dir .. part

		if (part ~= "" and not path.isabsolute(part) and not os.isdir(dir)) then
			local cd = d[part] or {}
			d[part] = cd
			d = cd
		end

		dir = dir .. "/"
	end
end 

local function add_dest_dir(p, dst_tree)
	if p and #p > 0 then
		expand_dirs(p,dst_tree)
	end
end 


local function make_dst_tree( rules )
	local function doit(dir,childs)

		local have_childs = false
		for k,v in pairs(childs) do
			 doit(iif(dir=='',k,dir .. '/' .. k),v) 
			 have_childs = true
		end
		if not have_childs and dir ~= '' then
			print('mkdir',dir)
			assert(os.mkdir(path.join(dst_path,dir)))
		end
	end
	doit('',rules)
end

local function copy_files( files )
	for v,dst in pairs(files) do
		if dst then
			print('copy',v)
			assert(os.copyfile(path.join(src_path,v),path.join(dst_path,dst)))
		end
	end
end

function _M.apply_rules( rules )
	local dst_tree = {}
	for v,marker in pairs(rules.dest_files or {}) do
		if marker then
			add_dest_dir(path.getdirectory(v),dst_tree)
		end
	end
	assert(dst_path and dst_path~='',"invalid dst_path '" .. tostring(dst_path) .."'")
	print('make destination directories at ' .. tostring(dst_path))
	make_dst_tree(dst_tree)
	copy_files(rules.copy_files or {})
	img.apply( rules )
end

return _M