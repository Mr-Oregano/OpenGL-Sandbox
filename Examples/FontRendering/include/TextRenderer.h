#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

#include "Font.h"

class TextRenderer
{
public:
	static void Init();
	static void Shutdown();

	static void Begin(const GLCore::Utils::OrthographicCamera &camera);
	static void End();

	static void DrawString(
		const std::string &text,
		glm::vec2 pos,
		float scale,
		const GLCore::Ref<Font> &font = s_Assets->m_DefaultFont,
		const glm::vec4 &color = { 1.0f, 1.0f, 1.0f, 1.0f });

private:
	struct FontMeshVertex
	{
		glm::vec2 position;
		glm::vec2 texcoords;
	};

	struct Assets
	{
		GLuint vertexArray;
		GLuint vertexBuffer;
		GLuint elementBuffer;

		GLCore::Ref<GLCore::Utils::Shader> shader;
		GLCore::Ref<Font> m_DefaultFont;
	};

private:
	static Assets *s_Assets;

};
