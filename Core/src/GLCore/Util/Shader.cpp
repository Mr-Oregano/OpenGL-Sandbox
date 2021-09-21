#include "glpch.h"
#include "Shader.h"

#include <fstream>

namespace GLCore::Utils {

	static std::string ReadFileAsString(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize((size_t)in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			LOG_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	GLuint Shader::CompileShader(GLenum type, const std::string& source)
	{
		GLuint shader = glCreateShader(type);

		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			LOG_ERROR("{0}", infoLog.data());
			// HZ_CORE_ASSERT(false, "Shader compilation failure!");
		}

		return shader;
	}

	Ref<Shader> Shader::FromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		Ref<Shader> shader = CreateRef<Shader>();
		shader->LoadFromGLSLTextFiles(vertexShaderPath, fragmentShaderPath);
		return shader;
	}

	Ref<Shader> Shader::FromGLSLTextFiles(const std::string &vertexShaderPath, const std::string &geometryShaderPath, const std::string &fragmentShaderPath)
	{
		Ref<Shader> shader = CreateRef<Shader>();
		shader->LoadFromGLSLTextFiles(vertexShaderPath, geometryShaderPath, fragmentShaderPath);
		return shader;
	}
	
	void Shader::LoadFromGLSLTextFiles(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		std::string vertexSource = ReadFileAsString(vertexShaderPath);
		std::string fragmentSource = ReadFileAsString(fragmentShaderPath);

		GLuint program = glCreateProgram();
		int glShaderIDIndex = 0;
			
		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
		glAttachShader(program, vertexShader);

		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
		glAttachShader(program, fragmentShader);

		LinkProgram(program);
		m_RendererID = program;
	}
	void Shader::LoadFromGLSLTextFiles(
		const std::string &vertexShaderPath, 
		const std::string &geometryShaderPath, 
		const std::string &fragmentShaderPath)
	{

		std::string vertexSource = ReadFileAsString(vertexShaderPath);
		std::string geometrySource = ReadFileAsString(geometryShaderPath);
		std::string fragmentSource = ReadFileAsString(fragmentShaderPath);

		GLuint program = glCreateProgram();
		int glShaderIDIndex = 0;

		GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
		glAttachShader(program, vertexShader);

		GLuint geometryShader = CompileShader(GL_GEOMETRY_SHADER, geometrySource);
		glAttachShader(program, geometryShader);

		GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
		glAttachShader(program, fragmentShader);

		LinkProgram(program);
		m_RendererID = program;

	}

	void Shader::LinkProgram(GLuint program)
	{

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			ReleaseShaders(program);
			glDeleteProgram(program);

			LOG_ERROR("{0}", infoLog.data());
			// HZ_CORE_ASSERT(false, "Shader link failure!");
		}

		ReleaseShaders(program);

	}
	void Shader::ReleaseShaders(GLuint program)
	{

		GLint shaderCount = 0;
		glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaderCount);

		GLuint *shaders = new GLuint[shaderCount];
		glGetAttachedShaders(program, shaderCount, nullptr, shaders);

		for (int i = 0; i < shaderCount; ++i)
		{
			glDetachShader(program, shaders[i]);
			glDeleteShader(shaders[i]);
		}

		delete[] shaders;

	}
}