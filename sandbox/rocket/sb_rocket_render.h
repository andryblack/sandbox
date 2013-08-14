//
//  sb_rocket_render.h
//  sr-osx
//
//  Created by Andrey Kunitsyn on 8/11/13.
//  Copyright (c) 2013 Andrey Kunitsyn. All rights reserved.
//

#ifndef __sr_osx__sb_rocket_render__
#define __sr_osx__sb_rocket_render__

#include <Rocket/Core/RenderInterface.h>
#include "sb_rect.h"

namespace Sandbox {
    
    class Resources;
    class Graphics;
    
    class RocketRenderInterface : public Rocket::Core::RenderInterface {
    private:
        Resources* m_resources;
        Graphics*   m_graphics;
        Recti m_clip_rect;
    public:
        explicit RocketRenderInterface(Resources* res);
        
        void BeginDraw(Graphics& g);
        void EndDraw(Graphics& g);
        
        /// Called by Rocket when it wants to render geometry that the application does not wish to optimise. Note that
        /// Rocket renders everything as triangles.
        /// @param[in] vertices The geometry's vertex data.
        /// @param[in] num_vertices The number of vertices passed to the function.
        /// @param[in] indices The geometry's index data.
        /// @param[in] num_indices The number of indices passed to the function. This will always be a multiple of three.
        /// @param[in] texture The texture to be applied to the geometry. This may be NULL, in which case the geometry is untextured.
        /// @param[in] translation The translation to apply to the geometry.
        virtual void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);
        
        /// Called by Rocket when it wants to enable or disable scissoring to clip content.
        /// @param[in] enable True if scissoring is to enabled, false if it is to be disabled.
        virtual void EnableScissorRegion(bool enable);
        /// Called by Rocket when it wants to change the scissor region.
        /// @param[in] x The left-most pixel to be rendered. All pixels to the left of this should be clipped.
        /// @param[in] y The top-most pixel to be rendered. All pixels to the top of this should be clipped.
        /// @param[in] width The width of the scissored region. All pixels to the right of (x + width) should be clipped.
        /// @param[in] height The height of the scissored region. All pixels to below (y + height) should be clipped.
        virtual void SetScissorRegion(int x, int y, int width, int height);
        
        /// Called by Rocket when a texture is required by the library.
        /// @param[out] texture_handle The handle to write the texture handle for the loaded texture to.
        /// @param[out] texture_dimensions The variable to write the dimensions of the loaded texture.
        /// @param[in] source The application-defined image source, joined with the path of the referencing document.
        /// @return True if the load attempt succeeded and the handle and dimensions are valid, false if not.
        virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
        /// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
        /// @param[out] texture_handle The handle to write the texture handle for the generated texture to.
        /// @param[in] source The raw 8-bit texture data. Each pixel is made up of four 8-bit values, indicating red, green, blue and alpha in that order.
        /// @param[in] source_dimensions The dimensions, in pixels, of the source data.
        /// @return True if the texture generation succeeded and the handle is valid, false if not.
        virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
        /// Called by Rocket when a loaded texture is no longer required.
        /// @param texture The texture handle to release.
        virtual void ReleaseTexture(Rocket::Core::TextureHandle texture);
        
        virtual int RenderString(Rocket::Core::FontFaceHandle* font_face, const Rocket::Core::WString& string, const Rocket::Core::Vector2f& position, const Rocket::Core::Colourb& colour) const;
    };
    
}

#endif /* defined(__sr_osx__sb_rocket_render__) */
