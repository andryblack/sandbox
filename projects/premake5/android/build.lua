local build = {}
local project   = premake.project
local config    = premake.config
local make      = premake.make
local solution 	= premake.solution
local fileconfig = premake.fileconfig
local ndk       = premake.modules.android.ndk

function build.onsolution( sln )
	premake.escaper(make.esc)
	premake.generate(sln, make.getmakefilename(sln, false), build.generate_solution)
	for cfg in solution.eachconfig(sln) do
		local function generate_ant_build( sln )
			build.generate_ant_build(sln, cfg)
		end
		premake.generate(sln, path.join(sln.location, cfg.shortname,'ant.properties'), generate_ant_build)
	end
	premake.escaper(nil)
end

function build.oncleansolution( sln )
	premake.clean.file(slm, make.getmakefilename(sln, false))
end

function build.projectrules(sln)

	local target_api = sln.android_target_api_level or sln.android_api_level or 14

	for prj in solution.eachproject(sln) do
		local install_projects = ''
		if prj.kind == premake.WINDOWEDAPP then
			local vprj = premake.esc(prj.name)
			_p('%s-update: %s-jni', vprj, vprj)
			for _,v in ipairs( sln.android_libs or {} ) do
				_x(1,'@cd ${config}/lib/%s && ${ANDROID} update lib-project -p . --target android-%s',v,tostring(target_api))
			end
			_x(1,'@cd ${config} && ${ANDROID} update project -p . -n %s --target android-%s',vprj, tostring(target_api))
			_p('')
			_p('%s-jni: prebuild-%s', vprj, vprj)
			_p(1,'@echo "==== Building %s jni ===="', prj.name)
			_x(1,'@cd ${config} && ${NDK_BUILD} NDK_MODULE_PATH=%s V=$(verbose)',sln.android_modules_path or '')
			_p('')
			_p('%s-apk: %s-update', vprj, vprj)
			_x(1,'@cd ${config} && ${ANT} ${config}')
			_p('')
			_p('%s: %s-apk', vprj, vprj)
			_p('')
			_p('%s-apk-install: %s-update', vprj, vprj)
			_x(1,'@cd ${config} && ${ANT} ${config} install')
			if install_projects~='' then
				install_projects = install_projects .. ' ' .. vprj .. '-apk-install'
			else
				install_projects = vprj .. '-apk-install'
			end
			_p('')
		end
		_p('install: %s',install_projects)
	end

end

function build.cleanrules( sln )
	_p('clean:')
	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			_p(1,'@echo "==== Cleaning %s ===="', prj.name, cfgvar)
			local prjdir = path.join( prj.name, ndk.JNI)
			_x(1,'@cd ${config} && ${NDK_BUILD} -C %s clean',prjdir)
			_p('')
		end
	end
end

function build.projects(sln)
	local p = {}
	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			table.insert(p,prj.name)
		end
	end
	_p('PROJECTS := %s', table.concat(p, " "))
	_p('')
end


function build.buildRules(prj,event)
	_p('%s-%s:',event,premake.esc(prj.name))
	_p('\t$(%sCMDS-%s)',event:upper(),premake.esc(prj.name):upper())
	_p('')
end

function build.buildCmds(cfg, event, prj)
	_p('  define %sCMDS-%s', event:upper(),premake.esc(prj.name):upper())
	local steps = cfg[event .. "commands"]
	local msg = cfg[event .. "message"]
	if #steps > 0 then
		steps = os.translateCommands(steps)
		msg = msg or string.format("Running %s commands for %s", event, prj.name)
		_p('\t@echo %s', msg)
		_p('\t%s', table.implode(steps, "", "", "\n\t"))
	end
	_p('  endef')
end

function build.generate_solution(sln)
	premake.eol("\n")

	make.header(sln)

	local ndk_dir = _OPTIONS['android-ndk-dir']
	local sdk_dir = _OPTIONS['android-sdk-dir']
	
	if ndk_dir then
		_x('NDK := %s',ndk_dir)
	end

	if sdk_dir then
		_x('SDK := %s',sdk_dir)
	end

	_x('ifeq (,$(NDK))')
	_x(1,'NDK_BUILD := ndk-build')
	_x('else')
	_x(1,'NDK_BUILD := $(NDK)/ndk-build')
	_x('endif')

	_x('ifeq (,$(SDK))')
	_x(1,'ANDROID := android')
	_x('else')
	_x(1,'ANDROID := $(SDK)/tools/android')
	_x('endif')

	local ant = os.getenv ('ANT')
	if ant then
		_x('ANT := %s',ant)
	else
		_x('ANT ?= ant')
	end



	_x('ifndef verbose')
	_x(1,'verbose = 0')
	_x('endif')


	build.projects(sln)

	make.solutionPhonyRule(sln)
	--make.groupRules(sln)

	build.projectrules(sln)
	build.cleanrules(sln)
	--make.helprule(sln)
	for prj in solution.eachproject(sln) do
		build.buildRules(prj,'prebuild')
		for cfg in project.eachconfig(prj) do
			_x('ifeq ($(config),%s)', cfg.shortname)
			build.buildCmds(cfg,'prebuild',prj)
			_p('endif')
		end
	end
end

function build.generate_ant_build( sln, cfg )
	_p('asset.dir=../assets')
	_p('resource.absolute.dir=${basedir}/../res')
	_p('out.dir=../../../bin/android')
	_p('out.other.absolute.dir=${basedir}/bin')
	_p('out.classes.absolute.dir=${out.other.absolute.dir}/classes')
	_p('out.res.absolute.dir=${out.other.absolute.dir}/res')
	_p('out.rs.obj.absolute.dir=${out.other.absolute.dir}/rsObj')
	_p('out.rs.libs.absolute.dir=${out.other.absolute.dir}/rsLibs')
	_p('out.aidl.absolute.dir=${out.other.absolute.dir}/aidl')
	_p('out.dexed.absolute.dir=${out.other.absolute.dir}/dexedLibs')
	_p('out.manifest.abs.file=${out.other.absolute.dir}/AndroidManifest.xml')
	if sln.android_key_store then
		_x('key.store=%s',path.getabsolute(sln.android_key_store))
	end
	if sln.android_key_alias then
		_x('key.alias=%s',sln.android_key_alias)
	end
	for i,v in ipairs( sln.android_libs or {} ) do
		_x('android.library.reference.%d=lib/%s',i,v)
	end
	_p('source.dir=../src')
end

return build