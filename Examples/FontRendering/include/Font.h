#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

#include <unordered_map>
#include <string>
#include <array>

// #include <Graphics/Texture.h>

class Font
{
public:
	struct Glyph
	{
		glm::vec2 pos;
		glm::vec2 size;
		glm::vec2 bearing;
		float advance;
	};
	struct Attributes
	{
		float lineHeight;
		float spaceWidth;
		float base;
	};

public:
	Font(const std::string &file);
	~Font();

	float CalcTextLength(const std::string &text);

	inline const Glyph &GetGlyph(char c) const { return m_Glyphs.at(c); }

	inline GLuint GetAtlasHandle() const { return m_AtlasTextureHandle; }
	inline int GetAtlasWidth() const { return m_AtlasWidth; }
	inline int GetAtlasHeight() const { return m_AtlasHeight; }

	inline Attributes GetFontAttributes() const { return m_Attributes; }

public:
	static GLCore::Ref<Font> LoadFromFile(const std::string &file);

private:
	void LoadGlyphs(const std::string &file);

private:
	std::unordered_map<unsigned char, Glyph> m_Glyphs;

	GLuint m_AtlasTextureHandle;
	int m_AtlasWidth;
	int m_AtlasHeight;

	Attributes m_Attributes;

};
