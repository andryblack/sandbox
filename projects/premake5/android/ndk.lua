--
-- common.lua
-- Android exporter module for Premake - support code.
-- Copyright (c) 2014 Will Vale and the Premake project
--

local ndk       = {}
local project   = premake.project
local config    = premake.config
local make      = premake.make
local solution 	= premake.solution
local fileconfig = premake.fileconfig

-- Constants
ndk.ANDROID     = 'android'
ndk.JNI         = 'jni'
ndk.MAKEFILE    = 'Android.mk'
ndk.APPMAKEFILE = 'Application.mk'
ndk.MANIFEST    = 'AndroidManifest.xml'
ndk.GLES30      = 'GLESv3'
ndk.GLES20      = 'GLESv2'
ndk.GLES10      = 'GLESv1_CM'
ndk.JAVA        = '.java'



-- Is the given project valid for NDK builds?
function ndk.isValidProject(prj)
	-- Console apps don't make sense
	if prj.kind == premake.CONSOLEAPP then
		return false
	end

	-- Otherwise valid if it contains a C or C++ file (under Visual Studio it's convenient to have non-compiling projects sometimes)
	for cfg in project.eachconfig(prj) do
		for _,f in ipairs(cfg.files) do
			if path.iscppfile(f) or path.iscfile(f) then
				return true
			end
		end
	end

	-- Otherwise invalid
	return false
end

function ndk.onsolution( sln )
	premake.escaper(make.esc)
	
	for cfg in solution.eachconfig(sln) do
		local function generateAppMakefile( sln )
			ndk.generateAppMakefile(sln, cfg)
		end
		print('config:',cfg.name,cfg.shortname)
		premake.generate(sln, path.join(sln.location, cfg.shortname,ndk.JNI, ndk.APPMAKEFILE), generateAppMakefile)
		local function generateAppAndroidMakefile( sln )
			ndk.generateAppAndroidMakefile(sln, cfg)
		end
		
		premake.generate(sln, path.join(sln.location, cfg.shortname,ndk.JNI, ndk.MAKEFILE), generateAppAndroidMakefile)
	end
end

function ndk.onproject( prj )
	premake.escaper(make.esc)
	-- Not all projects can generate something sensible.
	if  not ndk.isValidProject(prj) then
		return
	end

	for cfg in project.eachconfig(prj) do

		local function generateMakefile( prj )
			ndk.generateMakefile(prj, cfg)
		end

	
		-- Generate the ndk-build makefile
		premake.generate(prj, ndk.getMakefileName(prj, cfg, ndk.MAKEFILE), generateMakefile)

		if prj.kind == premake.WINDOWEDAPP then
			-- Generate the application makefile for application projects only
			
		end

	end
end

function ndk.oncleansolution(sln)
	
end

function ndk.oncleanproject (prj)
	-- Need to clean one makefile per configuration
	for cfg in project.eachconfig(prj) do
		premake.clean.file(prj, ndk.getMakefileName(prj, cfg, ndk.MAKEFILE))

		if prj.kind == premake.WINDOWEDAPP then
			premake.clean.file(prj, ndk.getMakefileName(prj, cfg, ndk.APPMAKEFILE))
		end
	end
	
end

-- Map premake cflags onto ndk-build cpp features
function ndk.getCppFeatures(cfg)
	local cppfeatures = {
		-- No flags to map yet
	}

	local flags = table.translate(cfg.flags, cppfeatures)

	-- turn on exceptions and RTTI by default, to match other toolsets
	if not cfg.flags.NoExceptions then
		table.insert(flags, "exceptions")
	end
	if not cfg.flags.NoRTTI then
		table.insert(flags, "rtti")
	end

	return flags
end

-- Need to put makefiles in subdirectories by project configuration
function ndk.getProjectPath(this,cfg)
	-- e.g. c:/root/myconfig/myproject
	return path.join(this.location, cfg.shortname, this.name)
end

-- Makefiles go in the jni directory under the project path
function ndk.getMakefilePath(this,cfg)
	-- e.g. c:/root/myconfig/myproject/jni
	return path.join(ndk.getProjectPath(this,cfg), ndk.JNI)
end

-- Append basename
function ndk.getMakefileName(this, cfg, basename)
	return path.join(ndk.getMakefilePath(this,cfg), basename)
end


-- Generator for the application makefile, which contains app-wide settings.
function ndk.generateAppMakefile(sln, cfg)
	-- write a header showing the build options
	_p('# Android NDK application makefile autogenerated by Premake')
	_p('')

	_p('# Application settings')
	if sln.android_api_level then
		_p('APP_PLATFORM := android-%d', sln.android_api_level)
	end
	if sln.android_abis then
		_p('APP_ABI := %s', table.concat(sln.android_abis, ' '))
	end
	if type(sln.android_stl) == 'string' then
		_p('APP_STL := %s', sln.android_stl)
	end
	_p('APP_OPTIM := %s',cfg.shortname)
	
	if (cfg.shortname == 'debug') then
		_p('NDK_DEBUG := 1')
	end
	
	_p('')			
end

function ndk.generateAppAndroidMakefile( sln, cfg )
	_p('DEPENDENCY_PATH := $(call my-dir)')
	_p('LOCAL_PATH := $(abspath $(DEPENDENCY_PATH))')
	_p('include $(CLEAR_VARS)')

	local location = path.join(sln.location, cfg.shortname,ndk.JNI)
	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			local p = ndk.getMakefileName(prj, cfg, ndk.MAKEFILE)
			p = path.getrelative(location, p)
			_p('include $(DEPENDENCY_PATH)/'..make.esc(p))
		end
	end
end

-- Write a list of makefile dependencies
function ndk.writeDependencies(location, cfg, depends)
	for _,d in ipairs(depends) do
		if ndk.isValidProject(d) then	
			local p = premake.filename(d, ndk.getMakefileName(d, cfg, ndk.MAKEFILE))
			p = path.getrelative(location, p)
			_p('include $(DEPENDENCY_PATH)/'..make.esc(p))
		end
	end
end



-- Write a list of relative paths following the tag, e.g. for source files, includes, ..
function ndk.writeRelativePaths(tag, location, paths, local_path, op)
		
	-- Remap paths relative to project and escape them
	for i,p in ipairs(paths) do
		paths[i] = make.esc(path.getrelative(location, p))
	end

	-- Optionally make paths relative to local_path
	local prefix = ''
	if local_path then
		prefix = '$(LOCAL_PATH)/'
	end			

	-- Call helper
	ndk.writeStrings(tag, prefix, paths, op)
end



-- Write a list of relative paths following the tag, e.g. for source files, includes, ..
function ndk.writeStrings(tag, prefix, strings, op)
	local app = op or ':='
	if #strings == 1 then
		-- Write simple string on one line
		_p("%s %s %s%s", tag, app, prefix, strings[1])
	elseif #strings > 1 then
		-- Write strings with continuation

		-- Continuation character and newline
		local suffix = '\\\n'

		-- Prepend tab
		prefix = '\t'..prefix

		-- Write strings from table
		_p("%s %s %s%s%s", tag, app, suffix, prefix, table.concat(strings, suffix..prefix))
	end
end


-- Create module name for an NDK module
function ndk.getModuleName(prj, cfg)
	if prj.kind == premake.WINDOWEDAPP then
		-- HACK: Want to specify targetname for projects, but only on Android. Does config scoping allow this?
		return cfg.targetname or 'unknown'
	else
		return prj.name or 'unknown'
	end
end

-- Extract list of dependencies the given kind
function ndk.getDependentModules(prj, cfg, kind)
	local names = {}
	for _,d in ipairs(project.getdependencies(prj)) do
		if d.kind == kind then
			table.insert(names, ndk.getModuleName(d, cfg))
		end
	end
	return names 
end




-- Generate the Android.mk makefile which builds a particular module.
function ndk.generateMakefile(prj,cfg)
	_p('# Android NDK project makefile autogenerated by Premake')
	_p('')

	-- This is $(LOCAL_PATH) in the makefile. We need to be relative to this in most cases.
	local local_path = ndk.getMakefilePath(prj,cfg)


	-- We include dependencies using the relative local path, but internal paths
	-- (to files, etc.) are expanded to get rid of the very long path recursion.
	_p('# Preamble')
	_p('DEPENDENCY_PATH := $(call my-dir)')
	_p('LOCAL_PATH := $(abspath $(DEPENDENCY_PATH))')
	_p('include $(CLEAR_VARS)')
	_p('')

	_p('# Project configuration')
	
	_p('LOCAL_MODULE := '..ndk.getModuleName(prj, cfg))
	ndk.writeStrings('LOCAL_CFLAGS', '-D', cfg.defines)
	ndk.writeStrings('LOCAL_CPP_FEATURES', '', ndk.getCppFeatures(cfg))

	-- Join linker options with linked libraries to get single table
	local link_options = cfg.linkoptions
	local links = config.getlinks(cfg, 'system', 'basename')
	for _,v in ipairs(links) do
		table.insert(link_options, '-l'..v)
	end
	ndk.writeStrings('LOCAL_LDLIBS', '', link_options)
	ndk.writeStrings('LOCAL_SHARED_LIBRARIES', '', ndk.getDependentModules(prj, cfg, premake.SHAREDLIB))
	ndk.writeStrings('LOCAL_STATIC_LIBRARIES', '', ndk.getDependentModules(prj, cfg, premake.STATICLIB))

	_p('# Include paths')
	ndk.writeRelativePaths('LOCAL_C_INCLUDES', local_path, cfg.includedirs, true)
	_p('')

	
	
	

	_p('# Source files')
	-- Filter out header files
	local files = {}
	for _,f in ipairs(cfg.files) do
		if path.iscppfile(f) or path.iscfile(f) then
			table.insert(files, f)
		end
	end
	ndk.writeRelativePaths('LOCAL_SRC_FILES', local_path, files, false)
	_p('')
	
	_p('# Build instructions')
	if cfg.kind == premake.STATICLIB then
		_p('include $(BUILD_STATIC_LIBRARY)')
	else
		_p('include $(BUILD_SHARED_LIBRARY)')
	end
	_p('')
	_p('# Project dependencies')
	ndk.writeDependencies(local_path, cfg, project.getdependencies(prj))
	_p('')

end


function ndk.get_main_project( sln )
	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			return prj
		end
	end
	return nil
end






-- Determine which version of OpenGL, if any, to link against.
function ndk.getGlesVersion(sln)
	for prj in solution.eachproject(sln) do
		for cfg in project.eachconfig(prj) do
			local links = config.getlinks(cfg, 'system', 'basename')
			-- Specify highest version we linked against
			if table.contains(links, ndk.GLES30) then
				return 0x00030000
			elseif table.contains(links, ndk.GLES20) then
				return 0x00020000
			elseif table.contains(links, ndk.GLES10) then
				return 0x00010000
			end
		end
	end

	return nil
end

return ndk