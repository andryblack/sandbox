//
//  sb_touch_info.h
//  pairs
//
//  Created by Андрей Куницын on 1/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef pairs_sb_touch_info_h
#define pairs_sb_touch_info_h

#include "sb_vector2.h"

namespace Sandbox {
    
    
    class TouchInfo {
    public:
        enum Type {
            BEGIN,
            MOVE,
            END
        };
        TouchInfo( Type type, const Vector2f& pos );
        
        /// touch type
        Type    GetType() const { return m_type; }
        /// touch position
        const Vector2f& GetPosition() const { return m_position; }
    private:
        Type        m_type;
        Vector2f    m_position;
    };
    
}

#endif
