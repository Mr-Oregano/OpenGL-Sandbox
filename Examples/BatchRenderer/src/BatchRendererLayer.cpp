
#include "BatchRendererLayer.h"

#include <filesystem>

#include <stb_image/stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

BatchRendererLayer::BatchRendererLayer()
	: m_Camera(16.0f / 9.0f)
{
}
BatchRendererLayer::~BatchRendererLayer()
{
}

void BatchRendererLayer::OnAttach() 
{
	EnableGLDebugging();

	m_DockSpace = CreateRef<ImGuiExt::ViewportDockSpace>();

	BatchRenderer2D::Init(100000);

	for (const auto &entry : std::filesystem::directory_iterator("res/textures/"))
	{
		stbi_set_flip_vertically_on_load(true);

		int width = 0;
		int height = 0;
		int channels = 0;

		stbi_uc *data = stbi_load(entry.path().string().c_str(), &width, &height, &channels, 0);
		
		GLuint textureHandle;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLint format;
		switch (channels)
		{
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
		default: format = GL_RGBA; 
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, channels % 2 == 0 ? 2 : 1);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		m_Materials.push_back(Batch::Material{ textureHandle, width, height });
	}

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);

}

void BatchRendererLayer::OnDetach()
{
	BatchRenderer2D::Shutdown();
}

void BatchRendererLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());

		Viewport_Width = (float) e.GetWidth();
		Viewport_Height = (float) e.GetHeight();
		Viewport_Aspect = Viewport_Width / Viewport_Height;

		m_DockSpace->SetViewport({ Viewport_Width, Viewport_Height });
		return true;
	});

}

void BatchRendererLayer::OnUpdate(Timestep ts)
{

	PROFILE_FUNCTION();
	DOCKSPACE_CAPTURE_SCOPE(m_DockSpace);
	
	m_Camera.OnUpdate(ts);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::vec2 tile = glm::vec2(128.0f / Viewport_Width) * 10.0f;
	BatchRenderer2D::Begin(m_Camera.GetCamera(), glm::translate(glm::mat4(1.0), 
		{ (tile.x - tile.x * s_COLS) / 2.0f, (tile.y - tile.y * s_ROWS) / 2.0f, 0.0f }));

	{
		PROFILE_SCOPE("Submission");

		int textureSelection = 0;
		for (int row = 0; row < s_ROWS; ++row)
		{
			for (int col = 0; col < s_COLS; ++col)
			{
				auto &material = m_Materials[textureSelection];
				textureSelection = (textureSelection + 1) % s_TEXTURES;

				glm::vec2 size = { material.texWidth / Viewport_Width, material.texHeight / Viewport_Width };

				BatchRenderer2D::DrawRect(
					{ col * (tile.x + 0.25f), row * (tile.y + 0.25f) },
					size * 20.0f,
					material);
			}
		}
	}

	BatchRenderer2D::End();
}
