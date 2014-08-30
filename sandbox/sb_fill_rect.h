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
    
    class FillRect : public SceneObjectWithPosition {
        SB_META_OBJECT
    public:
        virtual void Draw( Graphics& g ) const;
        
        const TexturePtr& GetTexture() const { return m_texture; }
        void SetTexture( const TexturePtr& texture ) { m_texture = texture; }
        
        Rectf GetRect() const { return Rectf(GetPos().x,GetPos().y,m_size.x,m_size.y); }
        void SetRect( const Rectf& rect ) { SetPos(rect.GetTopLeft()); m_size=rect.GetSize(); }
    private:
        TexturePtr  m_texture;
        Vector2f    m_size;
    };
    
}

#endif
