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

#include <sbstd/sb_shared_ptr.h>
#include "sb_notcopyable.h"
#include <sbstd/sb_vector.h>
#include <sbstd/sb_string.h>
#include <sbstd/sb_map.h>
#include "sb_vector2.h"

namespace GHL {
	struct ShaderProgram;
	struct Render;
	struct ShaderUniform;
}
namespace Sandbox {
	
	class ShaderUniform : public NotCopyable {
	public:
        virtual ~ShaderUniform() {}
		virtual void DoSet() = 0;
	protected:
		explicit ShaderUniform(GHL::ShaderUniform* uniform) : m_uniform(uniform) {}
		GHL::ShaderUniform* m_uniform;
	};
	typedef sb::shared_ptr<ShaderUniform> ShaderUniformPtr;
	
	class ShaderFloatUniform : public ShaderUniform {
	public:
		explicit ShaderFloatUniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform),m_value(1.0f) {}
		virtual void DoSet();
		void SetValue(float v) { m_value = v;}
	private:
		float m_value;
	};
    typedef sb::shared_ptr<ShaderFloatUniform> ShaderFloatUniformPtr;
    
    class ShaderVec2Uniform : public ShaderUniform {
	public:
		explicit ShaderVec2Uniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform) {}
		virtual void DoSet();
		void SetValue(const Vector2f& v) { m_value = v;}
	private:
		Vector2f m_value;
	};
    typedef sb::shared_ptr<ShaderVec2Uniform> ShaderVec2UniformPtr;
    
	class Shader {
	public:
		explicit Shader( GHL::ShaderProgram* prg);
		~Shader();
		void Set(GHL::Render* r);
		ShaderFloatUniformPtr GetFloatUniform(const char* name);
        ShaderVec2UniformPtr GetVec2Uniform(const char* name);
        void SetTextureStage(const char* uniform_name,int stage);
	private:
        template <class T>
        ShaderUniformPtr GetUniform(const char* name);
		GHL::ShaderProgram* m_program;
		void SetUniforms();
		sb::map<sb::string,ShaderUniformPtr> m_uniforms_map;
	};
	
	typedef sb::shared_ptr<Shader> ShaderPtr;
}

#endif /*SB_SADER_H*/
