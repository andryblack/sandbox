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
#include <sbstd/sb_shared_ptr.h>
#include "sb_resources.h"

namespace Sandbox {
    
    class Resources;
    
    
    class Background : public SceneObject {
        SB_META_OBJECT
    public:
        Background();
        bool Load( const char* file, Resources* res );
        void Draw( Graphics& g ) const SB_OVERRIDE;
        void SetFiltered( bool f );
        void SetFullScreen(bool fs);
        bool GetFullScreen() const { return m_fullscreen; }
        void SetKeepAspect(bool k);
        void Clear();
        
        GHL::UInt32 GetWidth() const { return m_data ? m_data->width : 0; }
        GHL::UInt32 GetHeight() const { return m_data ? m_data->height : 0; }
    protected:
        void SetData(const BackgroundDataPtr& bg) { m_data = bg; }
    private:
        BackgroundDataPtr   m_data;
        bool    m_fullscreen;
        bool    m_keep_aspect;
    };
    
}

#endif
