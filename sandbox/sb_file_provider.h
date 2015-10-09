#ifndef SB_FILE_PROVIDER_H
#define SB_FILE_PROVIDER_H

namespace GHL {
struct DataStream;
}
namespace Sandbox {

    class FileProvider {
    public:
        virtual GHL::DataStream* OpenFile(const char* fn) = 0;
        virtual GHL::DataStream* OpenFileVariant(const char* fn,bool& variant) {
            variant = false;
            return OpenFile(fn);
        }

    };

}

#endif // SB_FILE_PROVIDER_H
