local build = {}
local project   = premake.project
local config    = premake.config
local make      = premake.make
local solution 	= premake.solution
local fileconfig = premake.fileconfig
local ndk       = premake.modules.android.ndk

function build.onsolution( sln )
	premake.escaper(nil)

	premake.generate(sln, 'build.gradle', build.generate_build_gradle)
	premake.generate(sln, 'settings.gradle', build.generate_settings_gradle)

	local ndk_dir = _OPTIONS['android-ndk-dir']
	local sdk_dir = _OPTIONS['android-sdk-dir']
	

	function gen_local_properies( sln )
		_p('sdk.dir=' .. sdk_dir)
	end
	premake.generate(sln, 'local.properties', gen_local_properies)
	--premake.generate(sln, 'app/build.gradle', build.generate_app_build_gradle)

	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			local function gen(sln)
				build.generate_app_build_gradle(sln,prj)
			end
			premake.generate(sln, path.join(prj.shortname or prj.name,'build.gradle'),gen)
		end
	end

	premake.escaper(nil)
end

function build.oncleansolution( sln )
	--premake.clean.file(slm, make.getmakefilename(sln, false))
end


function build.buildCmds(cfg, event, prj)

	local steps = cfg[event .. "commands"]
	local msg = cfg[event .. "message"]

	local prev_name = ''

	if #steps > 0 then
		steps = os.translateCommands(steps)
		msg = msg or string.format("Running %s commands for %s", event, prj.name)
		
		for i,v in ipairs(steps) do
			local crnt_name = string.format('%s_cmd_%s_%d',event,cfg.shortname,i)
			_p('task %s (type: Exec) {',crnt_name )
				local cmdline,args = string.match(v,'^([^%s]*)%s(.*)')
				local all_args = {}
				for w in string.gmatch(args, "[^%s]+") do
      				table.insert(all_args,string.format("'%s'",w))
    			end
				_p("\tcommandLine '" .. cmdline .. "'")
				_p("\targs " .. table.concat(all_args,',').. "")
			_p('}')
			if i ~= 1 then
				_p(crnt_name .. ".dependsOn " .. prev_name)
			end
			prev_name = crnt_name
		end

		--_p('\t@echo %s', msg)
		--_p('\t args "-c",' .. table.implode(steps, "'", "'", ","))
	end


	local crnt_name = string.format('%s_cmd_%s',event,cfg.shortname)
	_p('task %s << {',crnt_name )
		_p("\tprintln '%s'",msg)
		
	_p('}')

	if prev_name ~= '' then
		_p(crnt_name .. ".dependsOn " .. prev_name)
	end
	
end

function build.generate_settings_gradle( sln )
	for prj in solution.eachproject(sln) do
		if prj.kind == premake.WINDOWEDAPP then
			_p("include ':%s'",prj.shortname or prj.name)
		end
	end
end

function build.generate_build_gradle(sln)
	premake.eol("\n")

	_x('buildscript {')
    _x(1,'repositories {')
    _x(2,'jcenter()')
    if sln.android_repository then
    	for _,v in ipairs(sln.android_repository) do
    		_x(2,'%s()',v)
    	end
    end
    _x(1,'}')
    _x(1,'dependencies {')
    _x(2,"classpath 'com.android.tools.build:gradle:2.2.3'")
    if sln.android_module and
    	(sln.android_module.gcm or sln.android_module.fcm) then
    	_x(2,"classpath 'com.google.gms:google-services:3.0.0'")
	end
    _x(1,'}')
	_x('}')
	
	-- dependencies {
 --        classpath 'com.android.tools.build:gradle-experimental:0.7.0'
 --    }
	_x('allprojects {')
    _x(1,'repositories {')
    _x(2,'jcenter()')
    _x(1,'}')
	_x('}')


	_x('task clean(type: Delete) {')
    _x(1,'delete rootProject.buildDir')
	_x('}')


end

function build.generate_app_build_gradle( sln , prj )
	premake.eol("\n")

	print('use versions:')
	print('\tandroid_api_level',tostring(sln.android_api_level))
	print('\tandroid_target_api_level',tostring(sln.android_target_api_level))
	print('\tandroid_build_api_level',tostring(sln.android_build_api_level))
	
	local target_api = sln.android_target_api_level or sln.android_api_level or 14

	_x("apply plugin: 'com.android.application'")
	_p('')

	_x('def build_files_location = "%s"' , prj.targetdir)

	_x('android {')
    _x(1,'compileSdkVersion ' .. prj.android_build_api_level or target_api)
    _x(1,'buildToolsVersion "23.0.2"')

    _x(1,'defaultConfig {')
    _x(2,'applicationId "%s"',prj.android_packagename or 'com.sandboxgames.sample')
    _x(2,'minSdkVersion ' .. prj.android_api_level or 9)
    _x(2,'targetSdkVersion ' .. target_api)
    _x(2,'versionCode ' .. prj.android_packageversion or 1)
    _x(2,'versionName "%s"', prj.android_packageversionname or '1.0')
    _x(1,'}')

    local src_dirs = {"'../src'"}
    for _,v in ipairs(prj.android_libs) do
		local ext = path.getextension(v)
		if ext ~= '.jar' then
			local p = '../' .. path.getrelative(sln.location,v)
			table.insert(src_dirs,"'" .. p .. "'")
		end
	end

	local aidl_dirs = {}
	if prj.android_aidl then
		for _,v in ipairs(prj.android_aidl) do
			local p = '../' .. path.getrelative(sln.location,v)
			table.insert(aidl_dirs,"'" .. p .. "'")
		end
	end

    _x(1,'sourceSets {')
    _x(2,'main {')
    _x(3,"manifest.srcFile 'AndroidManifest.xml'")
    _x(3,"java.srcDirs = [" .. table.concat(src_dirs,',') .. "]")
    _x(3,"resources.srcDirs = ['../src']")
    _x(3,"res.srcDirs = ['../res']")
    _x(3,"assets.srcDirs = []")
    if next(aidl_dirs) then
    	_x(3,"aidl.srcDirs = [" .. table.concat(aidl_dirs,',') .. "]")
    end
    --_x(3,"jniLibs.srcDir 'jni'")
    _x(2,'}')
    for cfg in project.eachconfig(prj) do
		_x(2,cfg.shortname .. ' {')
		if cfg.android_assets_path then
			_x(3,'assets.srcDirs = ["%s"]','../' .. path.getrelative(sln.location,cfg.android_assets_path))
		end
		_x(3,'jniLibs.srcDirs = ["../' .. cfg.shortname .. '/libs"]')
		_x(2,'}')
	end
	_x(1,'}')

	_x(1,'signingConfigs {')
		for cfg in project.eachconfig(prj) do
			if cfg.android_key_store then
				 _x(2,cfg.shortname .. ' {')
				 _x(3,'storeFile file("%s")','../' .. path.getrelative(sln.location,cfg.android_key_store))
				 if cfg.android_key_alias then
            		_x(3,'keyAlias "%s"',cfg.android_key_alias)
           		 end
           		 if cfg.android_keystore_pwd then
           		 	_x(3,'storePassword "%s"',cfg.android_keystore_pwd)
           		 else
            		_x(3,'storePassword System.getenv("'..cfg.shortname:upper()..'KSTOREPWD")')
            	 end
            	 if cfg.android_key_pwd then
           		 	_x(3,'keyPassword "%s"',cfg.android_key_pwd)
           		 else
            		_x(3,'keyPassword System.getenv("'..cfg.shortname:upper()..'KEYPWD")')
            	 end
				_x(2,'}')
			end
		end
    _x(1,'}')

	_x(1,'lintOptions {')
    _x(2,'abortOnError false')
  	_x(3,'}')

	--_x(1,'ndk {')
    --_x(2,"moduleName = '%s'",prj.name)
   	--_x(1,'}')

	_x(1,'dependencies {')
	for _,v in ipairs(prj.android_dependencies) do
		_x(2,"compile '" .. v .. "'")
	end
	for _,v in ipairs(prj.android_libs) do
		local ext = path.getextension(v)
		if ext == '.jar' then
			local lp = path.getdirectory(v)
			local ln = path.getname(v)
			_x(2,"compile files('" .. v .."')")
		end
	end
	_x(1,'}')


    _x(1,'buildTypes {')
	for cfg in project.eachconfig(prj) do
		_x(2,cfg.shortname .. ' {')
			if cfg.android_key_store  then
				_x(3,'signingConfig signingConfigs.' .. cfg.shortname)
			end
			if cfg.shortname == 'debug' then
				_x(3,'debuggable = true')
            	_x(3,'jniDebuggable = true')
            	_x(3,'minifyEnabled false')
            else
            	_x(3,'minifyEnabled false')
            	--_x(3,"proguardFile getDefaultProguardFile('proguard-android.txt')")
            end
            
		--_x(3,'jniLibs.srcDirs = ["' .. cfg.shortname .. '/libs"]')
		_x(2,'}')
	end
	_x(1,'}')

	_x(1,'applicationVariants.all { variant ->')
		_x(2,'variant.outputs.each { output ->')
			_x(3,'output.outputFile = new File(build_files_location, output.outputFile.name)')
		_x(2,'}')
	_x(1,'}')

	_x('}')


	if sln.android_module and
    	(sln.android_module.gcm or sln.android_module.fcm) then

    	_x("task copyGoogleServicesJSON(type: Copy) {")
    	_x(1,"description = 'Copy google-services.json'")
    	_x(1,'from ".."')
    	_x(1,'include "google-services.json"')
    	_x(1,'into "."')
		_x('}')
	
    	_x("apply plugin: 'com.google.gms.google-services'")
	end
	
	local ndk_dir = _OPTIONS['android-ndk-dir']

	for cfg in project.eachconfig(prj) do
		build.buildCmds(cfg,'prebuild',prj)
		local all_args = {"'-C'","'" .. path.getabsolute(path.join(sln.location,cfg.shortname)) .. "'"}
		table.insert(all_args,"'V=1'")
		table.insert(all_args,"'j=4'")
		_x(1,'task buildJNI' .. cfg.name .. '(type: Exec) {')
		_x(2,'commandLine "%s"',path.getabsolute(path.join(ndk_dir,'ndk-build')))
		_x(2,"args " .. table.concat(all_args,',').. "")
		_x(1,'}')
	end

	local libname = 'lib' .. prj.name .. '.so'

	for cfg in project.eachconfig(prj) do
		for abi in ipairs(sln.android_abis) do
			_x(1,'task copyJNI' .. cfg.name .. abi .. 'sym(type: Copy) {')
			_x(2,'from ' .. "'" .. path.getabsolute(path.join(sln.location,cfg.shortname,'obj','local',abi,libname)) .. "'")
			_x(2,'into ' .. "'" .. path.getabsolute(path.join(prj.targetdir,cfg.shortname .. '_symbols',abi)) .. "'")
			_x(1,'}')
		end
	end
	for cfg in project.eachconfig(prj) do
		for abi in ipairs(sln.android_abis) do
			_x(1,'task copyJNI' .. cfg.name .. abi.. '(type: Copy) {')
			_x(2,'from ' .. "'" .. path.getabsolute(path.join(sln.location,cfg.shortname,'libs',abi,libname)) .. "'")
			_x(2,'into ' .. "'" .. path.getabsolute(path.join(prj.targetdir,cfg.shortname,abi)) .. "'")
			_x(1,'}')
		end
	end
	

	
	_p('afterEvaluate {')
	for cfg in project.eachconfig(prj) do
		for abi in ipairs(sln.android_abis) do
			_x(1,'copyJNI' .. cfg.name .. abi .. '.dependsOn buildJNI' .. cfg.name)
			_x(1,'copyJNI' .. cfg.name .. abi .. 'sym.dependsOn buildJNI' .. cfg.name)
		end
	end

	_p('}')

	_p('tasks.whenTaskAdded { task ->')
	for cfg in project.eachconfig(prj) do
		_x(1,"if (task.name == 'generate%sAssets') {",cfg.name)
		local abis = {}
		for abi in ipairs(sln.android_abis) do
			table.insert(abis,'copyJNI' .. cfg.name .. abi)
			table.insert(abis,'copyJNI' .. cfg.name .. abi .. 'sym')
		end
		_x(2,'task.dependsOn ' .. 'prebuild_cmd_' .. cfg.shortname .. 
				', buildJNI' .. cfg.name .. 
				', ' ..table.concat(abis,', '))
		_x(1,'}')
		if sln.android_module and
    		(sln.android_module.gcm or sln.android_module.fcm) then
    		_x(1,"if (task.name == 'process%sGoogleServices') {",cfg.name)
			_x(2,'task.dependsOn copyGoogleServicesJSON')
			_x(1,'}')
    	end
	end
	_p('}')
	
	--_p('task build << {' )
	--_p("\tdependsOn " .. crnt_name)
	--_p('}')

	
end

return build