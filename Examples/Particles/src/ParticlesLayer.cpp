
#include "ParticlesLayer.h"

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Instrumentor.h>
#include <GLCore/Events/MouseEvent.h>

using namespace GLCore;
using namespace GLCore::Utils;

ParticlesLayer::ParticlesLayer()
	: m_Camera(16.0f / 9.0f)
{
}
ParticlesLayer ::~ParticlesLayer()
{
}

void ParticlesLayer::OnAttach()
{
	EnableGLDebugging();

	ParticleSystemProp props{};

	props.startColor = 0x10ffffff;
	props.endColor = 0x00ffffff;
	
	props.position = { 0.0f, 0.0f };
	props.direction = { 1.0f, 0.0f };
	props.dir_precision = 0.8f;
	
	props.speed = 1.0f;
	props.speed_precision = 0.9f;
	props.acceleration = -2.5f;
	props.rotationVel = 25.0f;

	props.externalForce = glm::vec2{0.0f, 0.25f};

	props.startSize = 0.1f;
	props.endSize = 0.4f;
	props.maxlife = 3.0f;

	particles = new ParticleSystem(props);
}

void ParticlesLayer::OnDetach()
{
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

static int emitCount = 1;

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

void ParticlesLayer::OnImGuiRender()
{
	ImGui::Begin("Particle Properties");

	ParticleSystemProp &props = particles->GetProperties();
	
	uint32_t &startColor = props.startColor;
	uint32_t &endColor = props.endColor;

	ImVec4 startColor4fv = ImGui::ColorConvertU32ToFloat4(startColor);
	ImVec4 endColor4fv = ImGui::ColorConvertU32ToFloat4(endColor);

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


