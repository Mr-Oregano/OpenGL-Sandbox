
#include "Font.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <stb_image/stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

Font::Font(const std::string &file)
{
	LoadGlyphs(file);
}

Font::~Font()
{
}

std::vector<std::string> SplitSpace(const std::string &s)
{
	std::istringstream iss(s);
	return std::vector<std::string>(
		std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());
}

std::string GetPairValue(const std::string &s)
{
	size_t delimiter = s.find('=');
	return s.substr(delimiter + 1);
}

void Font::LoadGlyphs(const std::string &file)
{
	// Load file
	std::string working_dir = file.substr(0, file.find_last_of('/') + 1);
	std::ifstream fnt_file(file);
	assert(fnt_file.is_open()); // Failed to open file?

	// Header (size: 4 lines)

	// Line 1
	std::string line;
	std::getline(fnt_file, line);

	// Line 2
	std::getline(fnt_file, line);
	std::vector<std::string> values = SplitSpace(line);

	glm::vec2 dimensions = { std::stoi(GetPairValue(values[3])), std::stoi(GetPairValue(values[4])) };
	m_Attributes.lineHeight = (float) std::stoi(GetPairValue(values[1])) / dimensions.y;
	m_Attributes.base = (float) std::stoi(GetPairValue(values[2])) / dimensions.y;

	// Line 3
	std::getline(fnt_file, line);
	values = SplitSpace(line);

	std::string atlas_name = GetPairValue(values[2]);
	atlas_name = atlas_name.substr(1, atlas_name.find_last_of("\"") - 1);

	stbi_set_flip_vertically_on_load(true);

	int channels;
	stbi_uc *data = stbi_load((working_dir + atlas_name).c_str(), &m_AtlasWidth, &m_AtlasHeight, &channels, 0);
	assert(data != nullptr);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_AtlasTextureHandle);
	glBindTexture(GL_TEXTURE_2D, m_AtlasTextureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, channels % 2 == 0 ? 2 : 1);

	GLint format;
	switch (channels)
	{
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	default: format = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, m_AtlasWidth, m_AtlasHeight, 0, format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	stbi_image_free(data);

	// Line 4
	std::getline(fnt_file, line);
	values = SplitSpace(line);
	size_t glyph_count = (size_t) std::stoi(GetPairValue(values[1]));

	// Body
	for (size_t i = 0; i < glyph_count; ++i)
	{
		// Read line
		std::getline(fnt_file, line);
		values = SplitSpace(line);

		Glyph glyph;
		unsigned char id = (unsigned char) std::stoi(GetPairValue(values[1]));

		// Read in glyph attributes
		glyph.pos =		glm::vec2(std::stoi(GetPairValue(values[2])), std::stoi(GetPairValue(values[3]))) / dimensions;
		glyph.size =	glm::vec2(std::stoi(GetPairValue(values[4])), std::stoi(GetPairValue(values[5]))) / dimensions;
		glyph.bearing = glm::vec2(std::stoi(GetPairValue(values[6])), std::stoi(GetPairValue(values[7]))) / dimensions;
		glyph.advance = (float) std::stoi(GetPairValue(values[8])) / dimensions.x;

		m_Glyphs.insert(std::pair<unsigned char, Glyph>(id, glyph));
	}

	// TODO: Implement kerning

}

float Font::CalcTextLength(const std::string &text)
{
	float length = 0.0f;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		const Font::Glyph &glyph = GetGlyph(*c);
		length += glyph.advance;
	}
	return length;
}

Ref<Font> Font::LoadFromFile(const std::string &file)
{
	return CreateRef<Font>(file);
}
