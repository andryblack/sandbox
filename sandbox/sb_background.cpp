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
    
    Background::Background() : m_fullscreen(true),m_width(0),m_height(0){
        
    }
    
    void Background::SetFullScreen(bool fs) {
        m_fullscreen = fs;
    }
    
    void Background::Draw(Sandbox::Graphics &g) const {
        Transform2d tr = g.GetTransform();
        if (m_fullscreen && (m_width!=g.GetScreenWidth()||m_height!=g.GetScreenHeight()) && (m_width) && (m_height)) {
            g.SetTransform(tr.scaled(float(g.GetScreenWidth())/m_width,
                                     float(g.GetScreenHeight())/m_height));
        }
        for (size_t i=0;i<m_images.size();i++) {
            g.DrawImage(m_images[i], GetDrawAttributes(), 0, 0);
        }
        g.SetTransform(tr);
    }
    
    bool Background::Load( const char* file, Resources* res ) {
        if (!res) return false;
        if (!file ) return false;
        m_images.clear();
        return res->LoadImageSubdivs( file , m_images, m_width, m_height);
    }
    
    void Background::AddImage(const Image& image) {
        m_images.push_back(image);
    }
    void Background::SetSize(GHL::UInt32 w, GHL::UInt32 h) {
        m_width = w;
        m_height = h;
    }


    void Background::SetFiltered( bool f ) {
        for (size_t i=0;i<m_images.size();++i) {
            m_images[i].GetTexture()->SetFiltered(f);
        }
    }
}
