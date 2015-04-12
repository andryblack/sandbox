#ifndef SB_BITMASK_H_INCLUDED
#define SB_BITMASK_H_INCLUDED

#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_vector.h>
#include <string.h>
#include <ghl_types.h>

namespace GHL {
    struct Image;
}
namespace Sandbox {

    class Bitmask : public sb::ref_countered_base_not_copyable {
    private:
        size_t  m_width;
        size_t  m_height;
        typedef GHL::Byte store_t;
        sb::vector<store_t> m_data;
        static const size_t bits_count = sizeof(store_t)*8;
    public:
        explicit Bitmask(GHL::Image* img);
        bool Get(int x,int y) const;
    };
    
}

#endif /*SB_BITMASK_H_INCLUDED*/