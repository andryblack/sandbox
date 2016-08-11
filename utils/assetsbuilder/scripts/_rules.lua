


local _M = {}
_M.definitions = {}

local img = require '_images'
local convert_spine = require '_convert_spine'
local luapp = require '_luapp'

function _M.init_rules(rules) 
	rules.copy_files = {}
	rules.compile_files = {}
	rules.dest_files = {}
	rules.call_functions = {}
	img.init_rules(rules)
	convert_spine.init_rules(rules)
end

_M.assets_rules = {}


function _M.assets_rules.require_version( v )
	if not host_version or host_version < v then
		print('required host version ',v,'but run on',host_version)
		print('please update utils')
		os.exit(1)
	end
end


function _M.assets_rules.call( v )
	table.insert(get_rules().call_functions,v)
end

function _M.assets_rules.define( n , v )
	_M.definitions[n]=v
end


function _M.assets_rules.use_variant( v , scale , override_base )
	img.use_variants[v]={scale=scale,override_base=override_base}
end

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
	--print('process_files_pattern:',pattern,action)
	local files = os.matchfiles(path.join(src_path,pattern))
	for k,v in ipairs(files) do
		local f = path.getrelative(src_path,v) or v
		--print(action,k,f)
		assert(get_rules()[assert(action)])[f]=f
		assert(not get_rules().dest_files[f],'conflict rules for file ' .. f)
		get_rules().dest_files[f]=f
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
		assert(get_rules()[assert(action)])[f]=path.join(dst,fn)
		assert(not get_rules().dest_files[path.join(dst,fn)],'conflict rules for file ' .. path.join(dst,fn))
		get_rules().dest_files[path.join(dst,fn)]=f
	end
	
end

function _M.assets_rules.copy_file( file )

	local src = file[1]
	local dst = file[2] or src

	assert(not get_rules().dest_files[dst],'conflict rules for file ' .. dst)
	get_rules().copy_files[src]=dst
	get_rules().dest_files[dst]=src
end

function _M.assets_rules.compile_file( file )
	local src = file[1]
	local dst = file[2] or src
	assert(not get_rules().dest_files[dst],'conflict rules for file ' .. dst)
	get_rules().compile_files[src]=dst
	get_rules().dest_files[dst]=src
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

function _M.assets_rules.premultiply_images( file_or_filelist )
	if type(file_or_filelist) == 'table' then
		for _,v in ipairs(file_or_filelist) do
			process_files_pattern(v,action_premultiply_images)
		end
	elseif type(file_or_filelist) == 'string' then
		process_files_pattern(file_or_filelist,action_premultiply_images)
	else
		error('premultiply_images table or string expected got ' .. type(file_or_filelist))
	end
end

function _M.assets_rules.premultiply_images_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_premultiply_images)
end



local function expand_dirs( p , d )
	local dir = iif(string.startswith(p, "/"), "/", "")
	for part in p:gmatch("[^/]+") do
		dir = dir .. part

		if (part ~= "" and not path.isabsolute(part)
			-- and not os.isdir(dir)) 
			) then
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
			--print('mkdir',dir)
			assert(os.mkdir(path.join(application.dst_path,dir)))
		end
	end
	doit('',rules)
end



local function do_copy_file( src, dst  )
	if update_only then
		if not os.check_file_new(src,dst) then
			--print('skip not new')
			return true
		end
	end
	return os.copyfile(src,dst)
end

local function copy_files( files )
	for v,dst in pairs(files) do
		if dst then
			--print('copy',v)	
			if type(dst) == 'string' then
				assert(do_copy_file(path.join(src_path,v),path.join(application.dst_path,dst)))
			else
				assert(do_copy_file(path.join(src_path,v),path.join(application.dst_path,v)))
			end
		end
	end
end

local function compile_files( files )
	for v,dst in pairs(files) do
		if dst then
			--print('compile',v)
			local source = assert(io.open(path.getabsolute(path.join(src_path,v)),'r'))
			
			local pp_source = assert(luapp.preprocess(source,_M.definitions))
			source:close()

			local shunk = assert(load(pp_source,'@'..dst,'t'))
			local binary = string.dump(shunk)
			local out = assert(io.open(path.join(application.dst_path,dst),'wb'))
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
	print('make destination directories at ' .. tostring(application.dst_path))
	make_dst_tree(dst_tree)
	copy_files(rules.copy_files or {})
	compile_files(rules.compile_files or {})

	for _,v in ipairs(rules.call_functions) do
		v(_G)
	end

	img.apply( rules )
	convert_spine.apply( rules )
end

return _M