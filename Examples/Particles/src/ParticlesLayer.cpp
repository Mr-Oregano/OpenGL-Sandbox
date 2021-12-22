
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

	props.startColor = 0xffffffff;
	props.endColor = 0x00000000;
	props.position = { -1.0f, 0.0f };
	props.direction = { 1.0f, 0.0f };
	props.precision = 0.8f;
	props.startSize = 0.25f;
	props.endSize = 0.0f;
	props.rotationVel = 0.0f;
	props.speed = 1.0f;
	props.maxlife = 1.0f;

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

void ParticlesLayer::OnUpdate(Timestep ts)
{
	m_Camera.OnUpdate(ts);

	glClear(GL_COLOR_BUFFER_BIT);

	uint32_t winWidth = Application::Get().GetWindow().GetWidth();
	uint32_t winHeight = Application::Get().GetWindow().GetHeight();

	glm::vec2 clickPos;
	clickPos.x = m_Camera.GetZoomLevel()
		* ((float)winWidth / winHeight)
		* (Input::GetMouseX() / winWidth * 2.0f - 1.0f)
		+ m_Camera.GetCamera().GetPosition().x;

	clickPos.y = m_Camera.GetZoomLevel()
		* -(Input::GetMouseY() / winHeight * 2.0f - 1.0f)
		+ m_Camera.GetCamera().GetPosition().y;

	particles->GetProperties().direction = clickPos - particles->GetProperties().position;

	particles->Emit();

	particles->Render(m_Camera);
	particles->Update(ts);
}

void ParticlesLayer::OnImGuiRender()
{
	ImGui::Begin("Particle Properties");
	
	uint32_t &startColor = particles->GetProperties().startColor;
	uint32_t &endColor = particles->GetProperties().endColor;

	ImVec4 startColor4fv = ImGui::ColorConvertU32ToFloat4(startColor);
	ImVec4 endColor4fv = ImGui::ColorConvertU32ToFloat4(endColor);

	ImGui::ColorEdit4("Color #1", &startColor4fv.x, ImGuiColorEditFlags_AlphaBar);
	ImGui::ColorEdit4("Color #2", &endColor4fv.x, ImGuiColorEditFlags_AlphaBar);
	
	startColor = static_cast<uint32_t>(ImGui::ColorConvertFloat4ToU32(startColor4fv));
	endColor = static_cast<uint32_t>(ImGui::ColorConvertFloat4ToU32(endColor4fv));

	ImGui::Separator();
	ImGui::DragFloat("Size #1", &particles->GetProperties().startSize, 0.01f, 0.0f, 10.0f, "%.2f");
	ImGui::DragFloat("Size #2", &particles->GetProperties().endSize, 0.01f, 0.0f, 10.0f, "%.2f");

	ImGui::Separator();
	ImGui::DragFloat("Rotation velocity", &particles->GetProperties().rotationVel, 1.0f, -200.0f, 200.0f, "%.0f");

	ImGui::Separator();
	ImGui::DragFloat("Speed", &particles->GetProperties().speed, 0.01f, 0.0f, 10.0f, "%.2f");

	ImGui::Separator();
	ImGui::DragFloat("Liftime", &particles->GetProperties().maxlife, 0.01f, 0.0f, 10.0f, "%.2f sec");

	ImGui::Separator();
	ImGui::DragFloat("Precision", &particles->GetProperties().precision, 0.01f, 0.0f, 1.0f, "%.2f");

	ImGui::End();
}


