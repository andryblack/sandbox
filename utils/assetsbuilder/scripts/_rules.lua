


local _M = {}
_M.definitions = {}

local _images = require '_images'

local luapp = require '_luapp'

_M.all_dest_files = {}

local dest_files_mt = {}
function dest_files_mt:__index( k )
	return self._[k]
end
function dest_files_mt:__newindex( k,v )
	self._[k]=v
	_M.all_dest_files[k]=v
end
function dest_files_mt:__pairs()
	return pairs(self._)
end
function dest_files_mt:__ipairs()
	return ipairs(self._)
end

function _M.init_rules(rules) 
	rules.copy_files = {}
	rules.compile_files = {}
	rules.dest_files = setmetatable({_={}},dest_files_mt)
	rules.call_functions = {}
	rules.encode_sounds = {}
end

_M.assets_rules = {}


function _M.assets_rules.require_version( v )
	if not host_version or host_version < v then
		log.error('required host version ',v,'but run on',host_version)
		log.error('please update utils')
		os.exit(1)
	end
end


function _M.assets_rules.call( v )
	table.insert(get_rules().call_functions,v)
end

function _M.assets_rules.define( n , v )
	_M.definitions[n]=v
end




local action_copy_files = 'copy_files'
local action_compile_files = 'compile_files'
local action_premultiply_images = 'premultiply_images'
local action_convert_to_jpeg = 'convert_to_jpeg'
local action_encode_sounds = 'encode_sounds'

local file_remap = {}
function file_remap.convert_to_jpeg( f )
	return path.replaceextension(f,'jpg')
end
function file_remap.encode_sounds( f )
	return path.replaceextension(f,'ogg')
end
function file_remap.premultiply_images( f )
	return path.replaceextension(f,_images.image_file_format.ext)
end

local function process_files_pattern( pattern , action )
	if type(pattern) ~= 'string' then
		error('process_files_pattern string expected got ' .. type(pattern) )
	end
	--print('process_files_pattern:',pattern,action)
	local files = os.matchfiles(path.join(src_path,pattern))
	for k,v in ipairs(files) do
		local f = path.getrelative(src_path,v) or v
		local df = file_remap[action] and file_remap[action](f) or f
		--print(action,k,f)
		assert(get_rules()[assert(action)])[f]=df
		assert(not get_rules().dest_files[df],'conflict rules for file ' .. df)
		get_rules().dest_files[df]=f
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
		local df = path.join(dst,file_remap[action] and file_remap[action](fn) or fn)
		assert(get_rules()[assert(action)])[f]=df
		assert(not get_rules().dest_files[df],'conflict rules for file ' .. df)
		get_rules().dest_files[df]=f
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

function _M.assets_rules.encode_sounds( file_or_filelist )
	if type(file_or_filelist) == 'table' then
		for _,v in ipairs(file_or_filelist) do
			process_files_pattern(v,action_encode_sounds)
		end
	elseif type(file_or_filelist) == 'string' then
		process_files_pattern(file_or_filelist,action_encode_sounds)
	else
		error('encode_sounds table or string expected got ' .. type(file_or_filelist))
	end
end

function _M.assets_rules.copy_files_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_copy_files)
end

function _M.assets_rules.compile_files_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_compile_files)
end

function _M.assets_rules.encode_sounds_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_encode_sounds)
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

function _M.assets_rules.convert_to_jpeg( file_or_filelist )
	if type(file_or_filelist) == 'table' then
		for _,v in ipairs(file_or_filelist) do
			process_files_pattern(v,action_convert_to_jpeg)
		end
	elseif type(file_or_filelist) == 'string' then
		process_files_pattern(file_or_filelist,action_convert_to_jpeg)
	else
		error('convert_to_jpeg table or string expected got ' .. type(file_or_filelist))
	end
end

function _M.assets_rules.premultiply_images_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_premultiply_images)
end

function _M.assets_rules.convert_to_jpeg_to( src, pattern, dst )
	process_files_pattern_to(src, pattern, dst, action_convert_to_jpeg)
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
			
			local source = assert(io.open(path.getabsolute(path.join(src_path,v)),'r'))
			
			local pp_source = assert(luapp.preprocess(source,_M.definitions))
			source:close()

			local shunk = assert(load(pp_source,'@'..dst,'t'))
			local binary = lua.compile(shunk)
			local full_dst = path.join(application.dst_path,dst)
			--print('compile',v,full_dst)
			local out = assert(io.open(full_dst,'wb'))
			out:write(binary)
			out:close()
		end
	end
end

local function encode_sounds( files )
	for src,dst in pairs(files) do
		if dst then
			if not update_only or os.check_file_new(path.join(src_path,src),path.join(application.dst_path,dst)) then
				if not application:encode_sound(src,dst) then
					error('failed encode sound ' .. src .. '->' .. dst)
				end
			end
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
	log.info('make destination directories at ' .. tostring(application.dst_path))
	make_dst_tree(dst_tree)
	copy_files(rules.copy_files or {})
	compile_files(rules.compile_files or {})
	encode_sounds(rules.encode_sounds or {})

	for _,v in ipairs(rules.call_functions) do
		v(_G)
	end

end

function _M.chek_files(  )
	log.info('check files')
	local files = os.matchfiles(path.join(application.dst_path,"**.*"))
	for _,v in ipairs(files) do
		local file = path.getrelative(application.dst_path,v)
		if not _M.all_dest_files[file] then
			if update_only then
				log.warning('remove unknonw file:',file)
			else
				error('produced unknonw file: ' .. file)
			end
		else
			--print('cheked file',file)
			_M.all_dest_files[file] = nil
		end
	end
	if next(_M.all_dest_files) then
		for k,v in pairs(all_dest_files) do
			log.error("not produced declared file:",k)
		end
		error('not all files produced')
	end
end

return _M