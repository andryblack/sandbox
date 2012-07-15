//
//  sb_fill_rect.cpp
//  YinYang
//
//  Created by Андрей Куницын on 2/16/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#include "sb_fill_rect.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::FillRect, Sandbox::SceneObject)

namespace Sandbox {
    
    
    void FillRect::Draw(Sandbox::Graphics &g) const {
        if (m_texture) {
            g.FillRect(m_texture, m_rect);
        }
    }
    
}
