//
//  sb_background.cpp
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "sb_background.h"
#include "sb_resources.h"
#include "sb_graphics.h"

SB_META_DECLARE_OBJECT(Sandbox::Background, Sandbox::SceneObject)

namespace Sandbox {
    
    Background::Background() {
        
    }
    
    void Background::Draw(Sandbox::Graphics &g) const {
        for (size_t i=0;i<m_images.size();i++) {
            g.DrawImage(m_images[i], 0, 0);
        }
    }
    
    bool Background::Load( const char* file, Resources* res ) {
        if (!res) return false;
        if (!file ) return false;
        m_images.clear();
        return res->LoadImageSubdivs( file , m_images);
    }
}