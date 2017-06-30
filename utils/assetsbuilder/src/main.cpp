
#include <iostream>
#include <sbstd/sb_string.h>

#include <sys/stat.h>

#include "application.h"

static int usage(const char* path) {
	std::cout << "usage:" << std::endl;
	std::cout << path << " options" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "  --scripts=SCRIPTS_DIR required  path to scripts" << std::endl;
	std::cout << "  --src=SRC             required  source dir" << std::endl;
	std::cout << "  --dst=DST             required  destination dir" << std::endl;
	std::cout << "  --update              optional	update only" << std::endl;
	std::cout << "  --platform=PLATFORM   optional  destination platform" << std::endl;
	std::cout << "  --help                          show this information" << std::endl;
	std::cout << "platforms:" << std::endl;
	std::cout << "  osx       Apple Mac OS X" << std::endl;
	std::cout << "  ios       Apple iOS" << std::endl;
	std::cout << "  android   Google Android" << std::endl;
	std::cout << "  windows   Microsoft Windows" << std::endl;
	std::cout << "  flash     Adobe Flash" << std::endl;
	return 0;
}

static int error(const std::string& text) {
	std::cout << "error: " << text << std::endl;
	std::cout << "use --help for usage information" << std::endl;
	return 1;
}

static sb::string get_argument(const sb::string& val) {
	size_t pos = val.find('=');
	if (pos == val.npos) {
		return sb::string();
	}
	return sb::string(val.c_str()+pos+1);
}

static bool is_dir(const sb::string& path) {
	struct stat buf;
	if (stat(path.c_str(), &buf)==0) {
		return buf.st_mode & S_IFDIR;
	}
	return false;
}

#ifndef EXTERNAL_APP_FACTORY
static Application* create_application() {
    return new Application();
}
#else
Application* create_application();
#endif

int main(int argc, char** argv) {
	if (argc < 2) {
		return usage(argv[0]);
	}
	sb::string src_dir;
	sb::string dst_dir;
    sb::vector<sb::string> scripts_dir;
	sb::string platform;
	bool update_only = false;
	sb::vector<sb::string> other_options;
    int threads = 0;
    int quality = 0;
    
	for(int i=1;i<argc;++i) {
		if (::strcmp(argv[i],"--help")==0) {
			return usage(argv[0]);
		}
		sb::string arg = argv[i];
		if (arg.length()<=2 || arg[0]!='-' || arg[1]!='-')
			return error(std::string("invalid argument '" + arg + "'"));
		const char* opt = arg.c_str();
		if (strncmp(opt+2,"scripts",7)==0) {
			scripts_dir.push_back( get_argument(arg) );
		} else if (strncmp(opt+2,"src",3)==0) {
			src_dir = get_argument(arg);
		} else if (strncmp(opt+2,"dst",3)==0) {
			dst_dir = get_argument(arg);
		} else if (strncmp(opt+2,"platform",8)==0) {
			platform = get_argument(arg);
		} else if (strcmp(opt+2,"update")==0) {
			update_only = true;
        } else if (strncmp(opt+2,"threads",7)==0) {
            threads = atoi(get_argument(arg).c_str());
            if (threads < 0) {
                return error("invalid threads");
            }
        } else if (strncmp(opt+2,"quality",7)==0) {
            quality = atoi(get_argument(arg).c_str());
            if (quality < 0) {
                return error("invalid quality");
            }
        } else {
			other_options.push_back(opt+2);
		}
	}
	if (src_dir.empty()) {
		return error("src option required");
	}
	if (!is_dir(src_dir)) {
		return error(std::string("invalid src path: ") + src_dir);
	}
	if (dst_dir.empty()) {
		return error("dst option required");
	}
//	if (!is_dir(dst_dir)) {
//		return error(std::string("invalid dst path: ") + dst_dir);
//	}
	if (scripts_dir.empty()) {
		return error("scripts option required");
	}
    for (sb::vector<sb::string>::const_iterator it = scripts_dir.begin();it!=scripts_dir.end();++it) {
        if (!is_dir(*it)) {
            return error(std::string("invalid scripts path: ") + *it);
        }
    }
	
	Application* app = create_application();
	app->set_update_only(update_only);
	app->set_paths(scripts_dir,src_dir,dst_dir);
	app->set_options(other_options);
	app->set_platform(platform);
    app->set_threads(threads);
    app->set_quality(quality);
	int result = app->run();
    delete app;
    return result;
}
