
local _M = {}

_M.assets_rules = {}

local atlas = require '_atlas'

local function load_images( dir  )
	local fullpath = path.join(dir,'images.lua')
	local ctx = { textures = {} , images = {} , animations = {}}
	local sandbox = { textures = ctx.textures }
	function sandbox.load_group( name )
		return {path=name, data=load_images( path.join(dir,name) ) }
	end
	function sandbox._textures( textures )
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
			v._name = k
			v._path = dir
			ctx.textures[k] = v
		end
	end
	function sandbox._images( images )
		for k,v in pairs(images) do
			local tex = assert(v[1],'texture empty for ' .. k .. ' at ' .. dir)
			v.texture = v[1]
			ctx.images[k] = v
		end
	end
	function sandbox._animations( animations )
		for k,v in pairs(animations) do
			ctx.animations[k] = v
		end
	end
	local data = load_sandbox(fullpath,sandbox,{})
	data._images = ctx.images
	data._animations = ctx.animations
	data._textures = ctx.textures
	return data
end

local function find_textures( from, mask )
	local res = r or {}
	local wmask = path.wildcards(path.join(from,mask))
	local function match( name )
		return name:match(wmask)
	end 
	local function filter_group( g )
		local t = g._textures 
		for _,v in pairs(t) do
			if match(v._path .. '/' .. v._name) then
				print('found match texture ' .. v._path .. '/' .. v._name )
				table.insert(res,v)
			end
		end
		local gg = {}
		for k,v in pairs(g) do
			gg[k]=v
		end
		gg._images = nil
		gg._textures = nil
		gg._animations = nil
		for k,v in pairs(gg) do
			filter_group(v.data)
		end
	end
	filter_group(assert(rules.images[from],"not found images '" .. from .."'"))
	return res
end

function _M.assets_rules.build_images( p )
	local i = rules.images or {}
	i[p]=load_images(p)
	rules.images = i
	rules.dest_files[path.join(p,'images.lua')]=true
end

function _M.assets_rules.build_atlas( from, mask , name,  w, h )
	local i = find_textures( from, mask )
	local a = atlas.Atlas.new(w,h)
	a.name = name
	local g = assert(rules.images[from])

	for _,v in ipairs(i) do
		local path = path.join(v._path,v._name .. '.' .. v.type)
		a:add_image( {width=v.texture_info.width+2, height=v.texture_info.height+2, src = v, 
			premultiply = rules.premultiply_images[path]} )
		rules.dest_files[path] = false
		rules.premultiply_images[path] = false
		v._atlas = a
	end
	a:build()
	a:dump()
	for _,v in ipairs(a.images) do
		v.src._placed_to = {v.place_to[1] + 1, v.place_to[2] + 1 }
	end
	a.path = path.join(from,name .. '.png' )
	rules.dest_files[a.path]=true
	
	local atex = { type='png', premultiplied = true, _path = path.join(from) , _name = name }
	g._textures[name] = atex
	function a:apply(  )
		local img = TextureData( w, h )
		for _,v in ipairs(self.images) do
			print(v.src._path,v.src._name)
			local tpath = path.join(v.src._path,v.src._name .. '.' .. v.src.type)
			local i = assert(application:load_texture(tpath),'failed load texture ' .. tpath)
			if v.premultiply then
				i:PremultiplyAlpha()
			end
			img:Place(v.place_to[1]+1,v.place_to[2]+1,i)
		end
		application:store_texture(a.path,img)
	end
	local atlases = rules.atlases or {}
	table.insert(atlases,a)
	rules.atlases = atlases
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
	local function expand_group( group , func , prefix, name )
		local g = {}
		for k,v in pairs(group.data) do
			g[k] = v
		end
		g._textures = nil
		g._images = nil
		g._animations = nil
		local path = group.path
		if path ~= '' then
			path = path .. '/'
		end
		if prefix ~= '' then
			prefix = prefix .. '_'
		end
		for k,v in pairs(g) do
			func(prefix .. k .. '_', v.data[name],path .. v.path)
			expand_group(v,func,prefix..k..'_',name)
		end
	end
	local function print_textures(  prefix, textures , path )
		for k,v in pairs(textures) do
			if not v._atlas then
				v._mapped_name = prefix .. k
				local p =''
				if path and path ~= '' then
					p = "'" .. path .. '/' .. k .. "',"
				end
				x(1,prefix .. k,' = {',
					p,
					f('type',v.type),
					f('smooth',v.smooth,false),
					f('tiled',v.tiled,false),
					f('premultiplied',v.premultiplied,false),
					'},')
			end
		end
	end
	p("_textures {")
	print_textures('',data._textures)
	expand_group({path='',data=data},print_textures,'','_textures')
	p("}")
	local function print_images( prefix, images )
		for k,v in pairs(images) do
			local texture = nil
			if v.texture._atlas then
				texture = string.format('textures.%s,',v.texture._atlas.name)
				if v.rect then
					v.rect[1] = v.rect[1] + v.texture._placed_to[1]
					v.rect[2] = v.rect[2] + v.texture._placed_to[2]
				else
					v.rect = {}
					v.rect[1] = v.texture._placed_to[1]
					v.rect[2] = v.texture._placed_to[2]
					v.rect[3] = v.texture.texture_info.width
					v.rect[4] = v.texture.texture_info.height
				end
			else
				texture = string.format('textures.%s,',v.texture._mapped_name)
			end
			x(1,prefix..k,'={',
					texture,
					f('rect',v.rect),
					f('hotspot',v.hotspot),
					f('size',v.size),
					'},')
		end
	end 
	p("_images {")
	print_images('',data._images)
	expand_group({path='',data=data},print_images,'','_images')
	p("}")
	local function print_animations( prefix , animations )
		for k,v in pairs(animations) do
			x(1,prefix..k,'={',
				string.format("'%s%s',%d,%d,",prefix,v[1],v[2],v[3]),
				f('speed',v.speed),
				f('frameset',v.frameset),
				'},')
		end
	end 
	p("_animations {")
	print_animations('',data._animations)
	expand_group({path='',data=data},print_animations,'','_animations')
	p("}")
	local groups = {}
	for k,v in pairs(data) do
		groups[k]=v
	end
	groups._images = nil
	groups._textures = nil
	groups._animations = nil
	local function expand_group_names( ident, prefix, group )
		local prefix = prefix .. '_'
		for k,v in pairs(group._images) do
			x(ident,k,'=images.',prefix..k,',')
		end
		for k,v in pairs(group._animations) do
			x(ident,k,'=animations.',prefix..k,',')
		end
		local g = {}
		for k,v in pairs(group) do
			g[k]=v
		end
		g._textures = nil
		g._images = nil
		g._animations = nil
		for k,v in pairs(g) do
			x(ident,k,' = {')
			expand_group_names(ident+1,prefix .. k,v.data)
			x(ident,'},')
		end
	end
	for k,v in pairs(groups) do
		assert(type(v)=='table','unknown field ' .. k)
		p(k," = {" )
		expand_group_names(1,k,v.data)
		p("}")
	end
	io.close(file)
	for k,v in pairs(groups) do
		--apply_images(path.join(dir,v.path),v.data)
	end
end

function _M.apply( rules )

	local images = rules.images or {}
	for k,v in pairs(images) do
		apply_images(k,v)
	end
	local atlases = rules.atlases or {}
	for _,v in ipairs(atlases) do
		v:apply()
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