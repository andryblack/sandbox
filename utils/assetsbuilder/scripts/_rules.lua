


local _M = {}

_M.assets_rules = {}

local img = require '_images'
local convert_spine = require '_convert_spine'

for k,v in pairs(img.assets_rules) do
	_M.assets_rules[k] = v
end

for k,v in pairs(convert_spine.assets_rules) do
	_M.assets_rules[k] = v
end

local action_copy_files = 'copy_files'
local action_compile_files = 'compile_files'
local action_premultiply_images = 'premultiply_images'

local function process_files_pattern( pattern , action )
	if type(pattern) ~= 'string' then
		error('process_files_pattern string expected got ' .. type(pattern) )
	end
	--print('copy_files_pattern:',pattern)
	local files = os.matchfiles(path.join(src_path,pattern))
	for k,v in ipairs(files) do
		local f = path.getrelative(src_path,v)
		--print('copy file',k,f)
		rules[assert(action)][f]=f
		rules.dest_files[f]=f
	end
end

local function process_files_pattern_to( src, pattern, dst , action)
	if type(pattern) ~= 'string' then
		error('process_files_pattern_to string expected got ' .. type(pattern) )
	end
	--print('copy_files_pattern:',pattern)
	local files = os.matchfiles(path.join(src_path,src,pattern))
	for k,v in ipairs(files) do
		local f = path.getrelative(src_path,v)
		local fn = path.getrelative(src,f)
		local r = rules[assert(action)] 
		if not r then
			r = {}
			rules[assert(action)] = r
		end
		--print('process_files_pattern_to',f,fn,action)
		r[f]=path.join(dst,fn)
		rules.dest_files[path.join(dst,fn)]=f
	end
	
end

function _M.assets_rules.copy_file( file )

	local src = file[1]
	local dst = file[2] or src

	if rules.dest_files[dst] then
		print('skip',src,dst,rules.dest_files[dst])
		return
	end

	rules.copy_files[src]=dst
	rules.dest_files[dst]=src
end

function _M.assets_rules.compile_file( file )
	local src = file[1]
	local dst = file[2] or src
	rules.compile_files[src]=dst
	rules.dest_files[dst]=src
end

function _M.assets_rules.copy_files( file_or_filelist )
	if type(file_or_filelist) == 'table' then
		for _,v in ipairs(file_or_filelist) do
			process_files_pattern(v,action_copy_files)
		end
	elseif type(file_or_filelist) == 'string' then
		process_files_pattern(file_or_filelist,action_copy_files)
	else
		error('copy_files table or string expected got ' .. type(file_or_filelist))
	end
end

function _M.assets_rules.compile_files( file_or_filelist )
	if type(file_or_filelist) == 'table' then
		for _,v in ipairs(file_or_filelist) do
			process_files_pattern(v,action_compile_files)
		end
	elseif type(file_or_filelist) == 'string' then
		process_files_pattern(file_or_filelist,action_compile_files)
	else
		error('compile_files table or string expected got ' .. type(file_or_filelist))
	end
end

function _M.assets_rules.copy_files_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_copy_files)
end

function _M.assets_rules.compile_files_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_compile_files)
end

function _M.assets_rules.premultiply_images_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_premultiply_images)
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
			if type(dst) == 'string' then
				assert(os.copyfile(path.join(src_path,v),path.join(dst_path,dst)))
			else
				assert(os.copyfile(path.join(src_path,v),path.join(dst_path,v)))
			end
		end
	end
end

local function compile_files( files )
	for v,dst in pairs(files) do
		if dst then
			print('compile',v)
			local source = assert(io.open(path.getabsolute(path.join(src_path,v)),'r'))
			local source_data = source:read('*a')
			source:close()

			local shunk = assert(load(source_data,dst,'t'))
			local binary = string.dump(shunk)
			local out = assert(io.open(path.join(dst_path,dst),'wb'))
			out:write(binary)
			out:close()
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
	compile_files(rules.compile_files or {})
	img.apply( rules )
	convert_spine.apply( rules )
end

return _M