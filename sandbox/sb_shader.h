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
#include "sb_matrix4.h"
#include "sb_color.h"

namespace GHL {
	struct ShaderProgram;
	struct Render;
	struct ShaderUniform;
}
namespace Sandbox {
	
	class ShaderUniform : public sb::ref_countered_base_not_copyable {
        SB_META_OBJECT_BASE
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
		virtual void DoSet() SB_OVERRIDE;
		void SetValue(float v) { m_value = v;}
        float GetValue() const { return m_value; }
	private:
		float m_value;
	};
    typedef sb::intrusive_ptr<ShaderFloatUniform> ShaderFloatUniformPtr;
    
    template <class T>
    class ShaderUniformVec : public ShaderUniform {
        SB_META_OBJECT
    public:
        explicit ShaderUniformVec(GHL::ShaderUniform* uniform) : ShaderUniform(uniform) {}
        virtual void DoSet() SB_OVERRIDE;
        void SetValue(const T& v) { m_value = v;}
        const T& GetValue() const { return m_value; }
    private:
        T m_value;
    };
    typedef ShaderUniformVec<Vector2f> ShaderVec2Uniform;
    typedef sb::intrusive_ptr<ShaderVec2Uniform> ShaderVec2UniformPtr;
    typedef ShaderUniformVec<Vector3f> ShaderVec3Uniform;
    typedef sb::intrusive_ptr<ShaderVec3Uniform> ShaderVec3UniformPtr;
    
    class ShaderMat4Uniform : public ShaderUniform {
        SB_META_OBJECT
    public:
        explicit ShaderMat4Uniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform) {}
        virtual void DoSet() SB_OVERRIDE;
        void SetValue(const Matrix4f& v) { m_value = v;}
        const Matrix4f& GetValue() const { return m_value; }
    private:
        Matrix4f m_value;
    };
    typedef sb::intrusive_ptr<ShaderMat4Uniform> ShaderMat4UniformPtr;
    
    class ShaderColorUniform : public ShaderUniform {
        SB_META_OBJECT
    public:
        explicit ShaderColorUniform(GHL::ShaderUniform* uniform) : ShaderUniform(uniform) {}
        virtual void DoSet() SB_OVERRIDE;
        void SetValue(const Color& v) { m_color = v;}
        const Color& GetValue() const { return m_color; }
        void SetAlpha(float a) { m_color.a = a;}
        float GetAlpha() const { return m_color.a;}
    private:
        Color m_color;
    };
    typedef sb::intrusive_ptr<ShaderColorUniform> ShaderColorUniformPtr;
    
	class Shader : public sb::ref_countered_base_not_copyable {
        SB_META_OBJECT_BASE
	public:
		explicit Shader( GHL::ShaderProgram* prg);
		~Shader();
		void Set(GHL::Render* r);
		ShaderFloatUniformPtr GetFloatUniform(const char* name);
        ShaderVec2UniformPtr GetVec2Uniform(const char* name);
        ShaderVec3UniformPtr GetVec3Uniform(const char* name);
        ShaderMat4UniformPtr GetMat4Uniform(const char* name);
        ShaderColorUniformPtr GetColorUniform(const char* name);
        sb::intrusive_ptr<Shader> Clone();
        GHL::ShaderUniform* GetRawUniform(const char* name);
        bool Compare(const sb::intrusive_ptr<Shader>& other) {
            return other && (m_program == other->m_program);
        }
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
