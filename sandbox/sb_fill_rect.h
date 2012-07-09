//
//  sb_fill_rect.h
//  YinYang
//
//  Created by Андрей Куницын on 2/16/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef YinYang_sb_fill_rect_h
#define YinYang_sb_fill_rect_h

#include "sb_scene_object.h"
#include "sb_texture.h"
#include "sb_rect.h"

namespace Sandbox {
    
    class FillRect : public SceneObject {
    public:
        virtual void Draw( Graphics& g ) const;
        
        const TexturePtr& GetTexture() const { return m_texture; }
        void SetTexture( const TexturePtr& texture ) { m_texture = texture; }
        
        const Rectf& GetRect() const { return m_rect; }
        void SetRect( const Rectf& rect ) { m_rect = rect; }
    private:
        TexturePtr  m_texture;
        Rectf   m_rect;
    };
    
}

#endif
