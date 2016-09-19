//
//  sb_background.h
//  pairs
//
//  Created by Андрей Куницын on 1/5/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef pairs_sb_background_h
#define pairs_sb_background_h

#include "sb_scene_object.h"
#include "sb_image.h"
#include <sbstd/sb_vector.h>

namespace Sandbox {
    
    class Resources;
    
    class Background : public SceneObject {
        SB_META_OBJECT
    public:
        Background();
        bool Load( const char* file, Resources* res );
        void Draw( Graphics& g ) const;
        void SetFiltered( bool f );
        void SetFullScreen(bool fs);
        void Clear();
        
        GHL::UInt32 GetWidth() const { return m_width; }
        GHL::UInt32 GetHeight() const { return m_height; }
    protected:
        void AddImage(const Image& image);
        void SetSize(GHL::UInt32 w, GHL::UInt32 h);
    private:
        sb::vector<Image>  m_images;
        bool    m_fullscreen;
        GHL::UInt32 m_width;
        GHL::UInt32 m_height;
    };
    
}

#endif
