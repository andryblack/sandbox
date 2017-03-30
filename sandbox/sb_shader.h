/*
 *  sb_shader.h
 *  SR
 *
 *  Created by Андрей Куницын on 13.02.11.
 *  Copyright 2011 andryblack. All rights reserved.
 *
 */

#ifndef SB_SHADER_H
#define SB_SHADER_H

#include <sbstd/sb_intrusive_ptr.h>
#include <sbstd/sb_vector.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include "meta/sb_meta.h"
#include "sb_vector2.h"
#include "sb_color.h"

namespace GHL {
	struct ShaderProgram;
	struct Render;
	struct ShaderUniform;
}
namespace Sandbox {
	
	class ShaderUniform : public sb::ref_countered_base_not_copyable {
        SB_META_OBJECT
	public:
        virtual ~ShaderUniform() {}
		virtual void DoSet() = 0;
	protected:
		explicit ShaderUniform(GHL::ShaderUniform* uniform) : m_uniform(uniform) {}
		GHL::ShaderUniform* m_uniform;
	};
	typedef sb::intrusive_ptr<ShaderUniform> ShaderUniformPtr;
	
	class ShaderFloatUniform : public ShaderUniform {
        SB_META_OBJECT
	public:
		explicit ShaderFloatUniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform),m_value(1.0f) {}
		virtual void DoSet();
		void SetValue(float v) { m_value = v;}
        float GetValue() const { return m_value; }
	private:
		float m_value;
	};
    typedef sb::intrusive_ptr<ShaderFloatUniform> ShaderFloatUniformPtr;
    
    class ShaderVec2Uniform : public ShaderUniform {
        SB_META_OBJECT
	public:
		explicit ShaderVec2Uniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform) {}
		virtual void DoSet();
		void SetValue(const Vector2f& v) { m_value = v;}
        const Vector2f& GetValue() const { return m_value; }
	private:
		Vector2f m_value;
	};
    typedef sb::intrusive_ptr<ShaderVec2Uniform> ShaderVec2UniformPtr;
    
    class ShaderColorUniform : public ShaderUniform {
        SB_META_OBJECT
    public:
        explicit ShaderColorUniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform) {}
        virtual void DoSet();
        void SetValue(const Color& v) { m_value = v;}
        const Color& GetValue() const { return m_value; }
    private:
        Color m_value;
    };
    typedef sb::intrusive_ptr<ShaderColorUniform> ShaderColorUniformPtr;
    
	class Shader : public sb::ref_countered_base_not_copyable {
        SB_META_OBJECT
	public:
		explicit Shader( GHL::ShaderProgram* prg);
		~Shader();
		void Set(GHL::Render* r);
		ShaderFloatUniformPtr GetFloatUniform(const char* name);
        ShaderVec2UniformPtr GetVec2Uniform(const char* name);
        ShaderColorUniformPtr GetColorUniform(const char* name);
        sb::intrusive_ptr<Shader> Clone();
  	private:
        template <class T>
        ShaderUniformPtr GetUniform(const char* name);
		GHL::ShaderProgram* m_program;
		void SetUniforms();
		sb::map<sb::string,ShaderUniformPtr> m_uniforms_map;
	};
	
	typedef sb::intrusive_ptr<Shader> ShaderPtr;
}

#endif /*SB_SADER_H*/
