/*
 *  sb_font.h
 *  SR
 *
 *  Created by Андрей Куницын on 12.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_FONT_H
#define SB_FONT_H

#include "sb_notcopyable.h"
#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_map.h>
#include <sbstd/sb_string.h>

#include "meta/sb_meta.h"
#include "sb_utf.h"
#include "sb_color.h"
#include "sb_font_data.h"
#include "sb_draw_attributes.h"
#include "sb_rect.h"

namespace Sandbox {

    struct Vector2f;
    
		
	class Graphics;
    class DrawAttributes;
    
    struct TextData;
    
    class FontPass;
    
    class FontDrawAttributes : public DrawAttributes {
        SB_META_OBJECT;
    public:
        virtual bool BeginPass(Graphics& g, const FontPass& pass) const = 0;
        virtual void EndPass(Graphics& g, const FontPass& pass) const = 0;
    };
    
    class FontPass : public meta::object {
        SB_META_OBJECT
    public:
        explicit FontPass( const FontDataPtr& data , const sb::string& name );
        void SetOffset( const Vector2f& pos ) { m_offset = pos; }
        const Vector2f& GetOffset() const { return m_offset; }
        void SetUseColor(bool c) { m_use_color = c; }
        bool GetUseColor() const { return m_use_color; }
        void SetColor(const Color& c) { m_color = c; }
        const Color& GetColor() const { return m_color; }
        const sb::string& GetName() const { return m_name; }
        
        float DrawI( Graphics& g, const DrawAttributes* attributes,
                    const Sandbox::Vector2f& pos , const char* text) const;
        void DrawI( Graphics& g, const DrawAttributes* attributes,
                   const TextData& text) const;
        void DrawCroppedI( Graphics& g, const DrawAttributes* attributes,
                          const Rectf& rect,
                          const TextData& text) const;
    private:
        FontDataPtr    m_data;
        sb::string     m_name;
        Vector2f       m_offset;
        bool           m_use_color;
        Sandbox::Color  m_color;
    };
    typedef sb::intrusive_ptr<FontPass> FontPassPtr;
    
	class Font : public meta::object {
        SB_META_OBJECT
	public:
        
                
		explicit Font(const FontDataPtr& main_data);
		virtual ~Font();
		
        virtual float Draw(Graphics& g,
                           const DrawAttributes* attributes,
                           const Vector2f& pos,const char* text,FontAlign align) const;
        virtual void Draw(Graphics& g,
                           const DrawAttributes* attributes,
                          const TextData& data) const;
        virtual void DrawCropped(Graphics& g,
                                 const DrawAttributes* attributes,
                                 const Rectf& rect,
                                 const TextData& data ) const;
		virtual float GetTextWidth(const char* text) const;
        bool MovePosition(Vector2f& pos,UTF32Char prev,UTF32Char next) const;
        
        /// font size (font units)
        float   GetSize() const { return m_size; }
        /// font 'x' height
        float   GetXHeight() const { return m_xheight; }
        /// font height (px)
        float   GetHeight() const { return m_height; }
        /// font baseline (px) 
        float   GetBaseline() const { return m_baseline; }
        
        virtual void AllocateSymbols( const char* text ) {}
        
        virtual const FontData::Glypth* GetGlyph(GHL::UInt32 code) const {
            return m_data ? m_data->get_glypth(code) : 0;
        }
        
        virtual bool HasGlyph(GHL::UInt32 code) const {
            return m_data ? m_data->has_glypth(code) : 0;
        }
        
        void ClearPasses();
        void AddPass( const FontPassPtr& pass );
    protected:
        
        void    set_height(float height) { m_height = height; }
        void    set_size(float size) { m_size = size; }
        void    set_baseline(float baseline) { m_baseline = baseline; }
        void    set_x_height(float h) {m_xheight = h;}
        
        FontDataPtr    m_data;
	private:
        float   m_size;
        float   m_height;
        float   m_xheight;
        float   m_baseline;
        typedef sb::vector<FontPassPtr> FontPassList;
        FontPassList m_passes;
    };
	
	typedef sb::intrusive_ptr<Font> FontPtr;

}

#endif /*SB_FONT_H*/
