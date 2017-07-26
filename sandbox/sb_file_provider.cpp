#include "sb_file_provider.h"
#include "meta/sb_meta.h"
#include <ghl_data.h>
#include <ghl_data_stream.h>

SB_META_DECLARE_KLASS(Sandbox::FileProvider,void);

namespace Sandbox {
    
    GHL::Data* FileProvider::LoadData(const char *fn) {
        GHL::DataStream* ds = OpenFile(fn);
        if (!ds) return 0;
        GHL::Data* d = GHL_ReadAllData(ds);
        ds->Release();
        return d;
    }

}
