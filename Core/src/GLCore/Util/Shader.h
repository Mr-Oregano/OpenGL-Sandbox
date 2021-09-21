#pragma once

#include <string>

#include <glad/glad.h>

#include <GLCore/Core/Ref.h>

namespace GLCore::Utils {

	class Shader
	{
	public:
		Shader() = default;
		~Shader();

		GLuint GetRendererID() { return m_RendererID; }

		static Ref<Shader> FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		static Ref<Shader> FromGLSLTextFiles(const std::string& vertexShaderPath,
										 const std::string& geometryShaderPath,
										 const std::string& fragmentShaderPath);
	private:
		void LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void LoadFromGLSLTextFiles(const std::string& vertexShaderPath, 
								   const std::string& geometryShaderPath,
								   const std::string& fragmentShaderPath);

		void LinkProgram(GLuint program);
		void ReleaseShaders(GLuint program);

		GLuint CompileShader(GLenum type, const std::string& source);
	private:
		GLuint m_RendererID;
	};

}