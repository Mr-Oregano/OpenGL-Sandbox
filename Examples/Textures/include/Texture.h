#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

enum class Filter
{
	Nearest = 0,
	Linear
};
enum class WrapMode
{
	ClampToEdge = 0,
	ClampToBorder,
	Repeat,
	Mirror
};
enum class MipmapMode
{
	None = 0,
	Nearest,
	Linear,
	Anisotropic
};

struct TextureProps
{
	Filter filter;
	WrapMode wrap;
	MipmapMode mipmap;
};

class Texture2D
{

public:
	Texture2D(unsigned char *data, int width, int height, int channels, const TextureProps &props);
	~Texture2D();

	inline GLuint GetContextID() const { return m_ContextID; }
	inline void Bind(GLuint unit = 0) const { glBindTextureUnit(unit, m_ContextID); }
	inline void Unbind(GLuint unit = 0) const { glBindTextureUnit(unit, 0); }

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetChannels() const { return m_Channels; }

	inline Filter GetFilter() const { return m_Props.filter; }
	inline WrapMode GetWrapMode() const { return m_Props.wrap; }

	void SetFilter(Filter filter);
	void SetWrapMode(WrapMode wrap);
	void SetMipmapMode(MipmapMode mipmap);
		
private:
	void PremultiplyAlpha(unsigned char *data);
	void LoadTexture(unsigned char *data);
	void UpdateMipmapMode();

private:
	GLuint m_ContextID = 0;

	int m_Width = 0;
	int m_Height = 0;
	int m_Channels = 0;

	TextureProps m_Props;

public:
	static GLCore::Ref<Texture2D> LoadFromFile(
		const std::string &path, 
		const TextureProps& props = { Filter::Linear, WrapMode::ClampToEdge, MipmapMode::Linear });

	static GLCore::Ref<Texture2D> LoadFromData(unsigned char *data, int width, int height, int channels,
		const TextureProps &props = { Filter::Linear, WrapMode::ClampToEdge, MipmapMode::Linear });
};
