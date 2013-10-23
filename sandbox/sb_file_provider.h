#ifndef SB_FILE_PROVIDER_H
#define SB_FILE_PROVIDER_H

namespace GHL {
struct DataStream;
}
namespace Sandbox {

    class FileProvider {
    public:
        virtual GHL::DataStream* OpenFile(const char* fn) = 0;

    };

}

#endif // SB_FILE_PROVIDER_H
