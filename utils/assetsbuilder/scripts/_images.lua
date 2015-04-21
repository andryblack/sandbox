
local _M = {}

_M.assets_rules = {}

local function load_images( dir  )
	local fullpath = path.join(dir,'images.lua')
	local sandbox = {}
	function sandbox.load_group( name )
		return {path=name, data=load_images( path.join(dir,name) ) }
	end
	local data = load_sandbox(fullpath,sandbox,{})
	rules.dest_files[fullpath]=true
	local textures = data._textures
	if textures then
		for k,v in pairs(textures) do
			local filename = path.join(dir,k .. '.' .. assert(v.type,'not found type for ' .. k .. ' at ' .. dir))
			local tex = assert(application:check_texture(filename),'failed check texture ' .. filename)
			v.texture_info = tex
			if v.premultiplied then
				rules.copy_files[filename]=true
			else
				local pm = rules.premultiply_images or {}
				pm[filename] = true
				rules.premultiply_images = pm
				v.premultiplied = true
			end
			rules.dest_files[filename]=true
		end
	end
	local images = data._images
	if images then
		assert(textures,'images without textures at ' .. dir)
		for k,v in pairs(images) do
			local tex = assert(v.texture,'texture empty for ' .. k .. ' at ' .. dir)
			tex = assert(textures[tex],'not found texture ' .. tex .. ' for ' .. k .. ' at ' .. dir)
			v.texture_data = tex
		end
	end
	return data
end

function _M.assets_rules.build_images( p )
	local i = rules.images or {}
	i[p]=load_images(p)
	rules.images = i
end

local function apply_images( dir, data )
	local fn = path.join(dir,'images.lua')
	print('henerate',fn)
	local file = assert(io.open ( path.join(dst_path,fn) , 'w'))
	
	local function f(name,val,def)
		val = val or def
		local fmt = "%s=%s,"
		if type(val) == 'boolean' then
			val = iif(val,'true','false')
		elseif type(val) == 'string' then
			fmt = "%s='%s',"
		elseif type(val)=='table' then
			val = '{' .. table.concat(val,',') .. '}'
		end
		if not val then
			return ""
		end
		return string.format(fmt,name,tostring(val))
	end
	local function p( ... )
		local args = table.pack(...)
		table.insert(args,'\n')
		file:write(table.unpack(args))
	end
	local function x( offset, ... )
		local fmt = ''
		for _=1,offset do
			fmt = fmt .. '  '
		end
		local args = table.pack(...)
		table.insert(args,'\n')
		file:write(fmt,table.unpack(args))
	end
	local textures = data._textures
	if textures then
		p("_textures = {")
		for k,v in pairs(textures) do
			x(1,k,' = {',
				f('type',v.type),
				f('smooth',v.smooth,false),
				f('tiled',v.tiled,false),
				f('premultiplied',v.premultiplied,false),
				'},')
		end
		p("}")
	end
	local images = data._images
	if images then
		p("_images = {")
		for k,v in pairs(images) do
			x(1,k,'={',
					f('texture',v.texture),
					f('rect',v.rect),
					f('hotspot',v.hotspot),
					f('size',v.size),
					'},')
		end
		p("}")
	end
	local animations = data._animations
	if animations then
		p("_animations = {")
		for k,v in pairs(animations) do
			x(1,k,'={',
				string.format("'%s',%d,%d,",v[1],v[2],v[3]),
				f('speed',v.speed),
				f('frameset',v.frameset),
				'},')
		end
		p("}")
	end
	local groups = {}
	for k,v in pairs(data) do
		groups[k]=v
	end
	groups._images = nil
	groups._textures = nil
	groups._animations = nil
	for k,v in pairs(groups) do
		p(k," = load_group '" .. v.path .. "'")
	end
	io.close(file)
	for k,v in pairs(groups) do
		apply_images(path.join(dir,v.path),v.data)
	end
end

function _M.apply( rules )

	local images = rules.images or {}
	for k,v in pairs(images) do
		apply_images(k,v)
	end

	local pmi = rules.premultiply_images or {}
	for k,v in pairs(pmi) do
		if v then
			print('premultiply image',k)
			local t = assert(application:load_texture(k))
			t:PremultiplyAlpha()
			assert(application:store_texture(k,t),'failed store texture to ' .. k)
		end
	end
end

return _M