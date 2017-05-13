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
#include "sb_texture.h"

SB_META_DECLARE_OBJECT(Sandbox::Background, Sandbox::SceneObject)

namespace Sandbox {
    
    Background::Background() : m_fullscreen(true),m_keep_aspect(false){
        
    }
    
    void Background::SetFullScreen(bool fs) {
        m_fullscreen = fs;
    }
    
    void Background::SetKeepAspect(bool k) {
        m_keep_aspect = k;
    }
    
    void Background::Draw(Sandbox::Graphics &g) const {
        if (!m_data)
            return;
        Transform2d tr = g.GetTransform();
        if (m_fullscreen) {
            GHL::UInt32 sw = g.GetDrawWidth();
            GHL::UInt32 sh = g.GetDrawHeight();
            if (sw != m_data->width || sh != m_data->height) {
                float sx = float(sw)/m_data->width;
                float sy = float(sh)/m_data->height;
                int ox = 0;
                int oy = 0;
                if (m_keep_aspect) {
                    if (sx > sy) {
                        sy = sx;
                        oy = -int((int)m_data->height*sx-(int)sh) / 2;
                    } else if (sy > sx) {
                        sx = sy;
                        ox = -int((int)m_data->width*sy-(int)sw) / 2;
                        
                    }
                }
                g.SetTransform(tr.translated(float(ox),float(oy)).scaled( sx,sy));
            }
        }
        for (size_t i=0;i<m_data->images.size();i++) {
            g.DrawImage(m_data->images[i], GetDrawAttributes().get(), 0, 0);
        }
        g.SetTransform(tr);
    }
    
    bool Background::Load( const char* file, Resources* res ) {
        if (!res) return false;
        if (!file ) return false;
        m_data = res->LoadBackground(file);
        return m_data;
    }
    
    void Background::Clear() {
        m_data.reset();
    }
    

    void Background::SetFiltered( bool f ) {
        if (m_data) {
            for (size_t i=0;i<m_data->images.size();++i) {
                m_data->images[i].GetTexture()->SetFiltered(f);
            }
        }
        
    }
}
