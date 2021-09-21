
#include "TextRenderer.h"

#include <array>

using namespace GLCore;
using namespace GLCore::Utils;

TextRenderer::Assets *TextRenderer::s_Assets = nullptr;

void TextRenderer::Init()
{
	s_Assets = new Assets();

	s_Assets->shader = Shader::FromGLSLTextFiles(
		"res/shaders/text-vert.glsl",
		"res/shaders/text-frag.glsl"
	);

	s_Assets->m_DefaultFont = Font::LoadFromFile("res/fonts/opensans.fnt");

	glCreateVertexArrays(1, &s_Assets->vertexArray);
	glBindVertexArray(s_Assets->vertexArray);

	size_t buffer_size = sizeof(float) * 6 * 4;
	glCreateBuffers(1, &s_Assets->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, s_Assets->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, (unsigned int) buffer_size, nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *) 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *) (2 * sizeof(float)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glCreateBuffers(1, &s_Assets->elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Assets->elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), nullptr, GL_STATIC_DRAW);

	unsigned int vertex = 0;
	for (size_t i = 0; i < 6; i += 6)
	{
		unsigned int elements[] = {
			0 + vertex, 
			1 + vertex, 
			2 + vertex,
			2 + vertex, 
			3 + vertex, 
			0 + vertex
		};
		vertex += 4;
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(elements), elements);
	}
}

void TextRenderer::Shutdown()
{
	delete s_Assets;
	s_Assets = nullptr;
}

void TextRenderer::Begin(const GLCore::Utils::OrthographicCamera &camera)
{
	glUseProgram(s_Assets->shader->GetRendererID());
	GLint loc = glGetUniformLocation(s_Assets->shader->GetRendererID(), "u_ViewProjection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(camera.GetViewProjectionMatrix()));

	glBindVertexArray(s_Assets->vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, s_Assets->vertexBuffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void TextRenderer::End()
{
	glDisable(GL_BLEND);
}

void TextRenderer::DrawString( 
	const std::string &text, 
	glm::vec2 pos, 
	float scale, 
	const Ref<Font> &font,
	const glm::vec4 &color)
{
	// TODO: Update to use with camera later.
	//		 We must determine the "up" and "right" direction of the projection provided by the end-user.
	//		 Temporary: For now, it is assumed that the following vectors point "up" and "right" respectively.
	constexpr glm::vec2 up = { 0.0f, 1.0f };
	constexpr glm::vec2 right = { 1.0f, 0.0f };
	//

	GLuint program = s_Assets->shader->GetRendererID();

	GLint loc = glGetUniformLocation(s_Assets->shader->GetRendererID(), "u_TextColor");
	glUniform4f(loc, color.r, color.g, color.b, color.a);

	loc = glGetUniformLocation(s_Assets->shader->GetRendererID(), "u_AtlasTexels");
	glUniform1f(loc, font->GetAtlasWidth());

	glBindTextureUnit(0, font->GetAtlasHandle());
	FontMeshVertex vertices[4]{};

	pos += glm::vec2(-font->CalcTextLength(text), font->GetFontAttributes().base) / 2.0f * scale;

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		const Font::Glyph &glyph = font->GetGlyph(*c);

		float xtex = glyph.pos.x;
		float ytex = 1.0f - glyph.pos.y;

		glm::vec2 bearing = (right + up) * glm::vec2(glyph.bearing.x, -glyph.bearing.y) * scale;
		glm::vec2 size = glyph.size * scale;

		vertices[0].position = pos + bearing;
		vertices[1].position = pos - up * size + bearing;
		vertices[2].position = pos + (right - up) * size + bearing;
		vertices[3].position = pos + right * size + bearing;

		vertices[0].texcoords = { xtex, ytex };
		vertices[1].texcoords = { xtex, ytex - glyph.size.y };
		vertices[2].texcoords = { xtex + glyph.size.x, ytex - glyph.size.y };
		vertices[3].texcoords = { xtex + glyph.size.x, ytex };

		glBindBuffer(GL_ARRAY_BUFFER, s_Assets->vertexBuffer);
		glBindVertexArray(s_Assets->vertexArray);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		pos += right * glyph.advance * scale;
	}
}