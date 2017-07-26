#ifndef SB_FILE_PROVIDER_H
#define SB_FILE_PROVIDER_H

namespace GHL {
    struct Data;
    struct DataStream;
}
namespace Sandbox {

    class FileProvider {
    public:
        virtual ~FileProvider() {}
        virtual GHL::DataStream* OpenFile(const char* fn) = 0;
        virtual GHL::DataStream* OpenFileVariant(const char* fn,bool& variant) {
            variant = false;
            return OpenFile(fn);
        }
        virtual GHL::Data* LoadData(const char* fn);
    };

}

#endif // SB_FILE_PROVIDER_H
