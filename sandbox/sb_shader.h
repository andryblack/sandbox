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

namespace GHL {
	struct ShaderProgram;
	struct Render;
	struct ShaderUniform;
}
namespace Sandbox {
	
	class ShaderUniform : public NotCopyable {
	public:
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
    
	class Shader {
	public:
		explicit Shader( GHL::ShaderProgram* prg);
		~Shader();
		void Set(GHL::Render* r);
		ShaderFloatUniformPtr GetFloatUniform(const char* name);
	private:
		GHL::ShaderProgram* m_program;
		sb::vector<ShaderUniformPtr> m_uniforms;
		void SetUniforms();
		sb::map<sb::string,ShaderFloatUniformPtr > m_float_uniforms;
	};
	
	typedef sb::shared_ptr<Shader> ShaderPtr;
}

#endif /*SB_SADER_H*/
