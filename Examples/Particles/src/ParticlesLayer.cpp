
#include "ParticlesLayer.h"

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Instrumentor.h>
#include <GLCore/Events/MouseEvent.h>

#include <stb_image/stb_image.h>

// TODO: This should be temporary, will only compile on Windows for now
//
#include <commdlg.h>

using namespace GLCore;
using namespace GLCore::Utils;

ParticlesLayer::ParticlesLayer()
	: m_Camera(16.0f / 9.0f)
{
}
ParticlesLayer ::~ParticlesLayer()
{
}

GLuint LoadTexture(const char *path)
{
	int width;
	int height;
	int channels;

	stbi_uc *data = stbi_load(path, &width, &height, &channels, 4);

	if (data == nullptr)
	{
		LOG_ERROR("Error loading image file.");
		return 0;
	}

	GLuint texture = 0;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);

	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureStorage2D(texture, 8, GL_RGBA8, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (const void *)data);
	glGenerateTextureMipmap(texture);

	return texture;
}

void ParticlesLayer::OnAttach()
{
	EnableGLDebugging();

	ParticleSystemProp props{};

	// TODO: Byte order may be different, little-endian on intel.
	//
	props.startColor = 0x1F999999;
	props.endColor = 0x00333333;
	
	props.position = { 0.0f, 0.0f };
	props.direction = { 1.0f, 0.0f };
	props.dir_precision = 0.875f;
	
	props.speed = 1.0f;
	props.speed_precision = 0.6f;
	props.acceleration = -2.5f;
	props.rotationVel = 25.0f;

	props.externalForce = glm::vec2{0.0f, 0.25f};

	props.startSize = 0.15f;
	props.endSize = 0.75f;
	props.maxlife = 4.0f;

	props.texture = LoadTexture("res/textures/smoke.png");

	particles = new ParticleSystem(props);
}

void ParticlesLayer::OnDetach()
{
	if (particles->GetProperties().texture)
		glDeleteTextures(1, &particles->GetProperties().texture);

	delete particles;
}

void ParticlesLayer::OnEvent(Event& event)
{
	m_Camera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		return true;
	});
}

static int emitCount = 2;

void ParticlesLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);

	glClear(GL_COLOR_BUFFER_BIT);

	uint32_t winWidth = Application::Get().GetWindow().GetWidth();
	uint32_t winHeight = Application::Get().GetWindow().GetHeight();

	glm::vec2 mousePos;
	mousePos.x = m_Camera.GetZoomLevel()
		* ((float)winWidth / winHeight)
		* (Input::GetMouseX() / winWidth * 2.0f - 1.0f)
		+ m_Camera.GetCamera().GetPosition().x;

	mousePos.y = m_Camera.GetZoomLevel()
		* -(Input::GetMouseY() / winHeight * 2.0f - 1.0f)
		+ m_Camera.GetCamera().GetPosition().y;

	particles->GetProperties().direction = mousePos - particles->GetProperties().position;

	if (Input::IsMouseButtonPressed(0))
		for (int i = 0; i < emitCount; ++i)
			particles->Emit();

	particles->Render(m_Camera);
	particles->Update(ts);
}

void ParticlesLayer::ImGuiLoadTexture()
{
	char fileName[MAX_PATH]{};
	OPENFILENAMEA ofn;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.hInstance = GetModuleHandle(nullptr);
	ofn.lpstrFilter = "Image\0*.png\0\0";
	ofn.lpstrCustomFilter = nullptr;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = sizeof(fileName);
	ofn.lpstrFileTitle = nullptr;
	ofn.lpstrInitialDir = nullptr;
	ofn.lpstrTitle = "Select Texture File";
	ofn.Flags = 0;
	ofn.lpstrDefExt = "png";

	if (!GetOpenFileNameA(&ofn))
		return;

	particles->GetProperties().texture = LoadTexture(fileName);
}

void ParticlesLayer::OnImGuiRender()
{
	ImGui::Begin("Particle Properties");

	ParticleSystemProp &props = particles->GetProperties();
	
	uint32_t &startColor = props.startColor;
	uint32_t &endColor = props.endColor;

	ImVec4 startColor4fv = ImGui::ColorConvertU32ToFloat4(startColor);
	ImVec4 endColor4fv = ImGui::ColorConvertU32ToFloat4(endColor);

	if (ImGui::Button("Open Texture..."))
		ImGuiLoadTexture();

	ImGui::ColorEdit4("Start Color", &startColor4fv.x, ImGuiColorEditFlags_AlphaBar);
	ImGui::ColorEdit4("End Color", &endColor4fv.x, ImGuiColorEditFlags_AlphaBar);
	
	startColor = static_cast<uint32_t>(ImGui::ColorConvertFloat4ToU32(startColor4fv));
	endColor = static_cast<uint32_t>(ImGui::ColorConvertFloat4ToU32(endColor4fv));

	ImGui::SliderFloat("Direction Precision", &props.dir_precision, 0.0f, 1.0f);

	ImGui::Separator();
	ImGui::SliderFloat("Speed", &props.speed, 0.0f, 10.0f);
	ImGui::SliderFloat("Speed Precision", &props.speed_precision, 0.0f, 1.0f);
	ImGui::SliderFloat("Acceleration", &props.acceleration, -10.0f, 10.0f);
	ImGui::SliderFloat("Rotation Velocity", &props.rotationVel, -180.0f, 180.0f);

	ImGui::Separator();
	ImGui::DragFloat2("External Force", glm::value_ptr(props.externalForce), 0.01f, -10.0f, 10.0f, "%.2f");

	ImGui::Separator();
	ImGui::SliderFloat("Start Size", &particles->GetProperties().startSize, 0.0f, 10.0f, "%.2f");
	ImGui::SliderFloat("End Size", &particles->GetProperties().endSize, 0.0f, 10.0f, "%.2f");

	ImGui::Separator();
	ImGui::SliderFloat("Lifetime", &props.maxlife, 0.0f, 10.0f, "%.2f sec");
	ImGui::SliderInt("Emit Count", &emitCount, 1, 100);

	ImGui::End();
}


