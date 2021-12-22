
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

static int particleSpawn = 1;

void ParticlesLayer::OnAttach()
{
	EnableGLDebugging();

	ParticleSystemProp props{};

	props.startColor = 0xffffffff;
	props.endColor = 0x00000000;
	props.position = { 0.0f, 0.0f };
	props.startSize = 0.25f;
	props.endSize = 0.0f;
	props.rotationVel = 200.0f;
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

	if (Input::IsMouseButtonPressed(0))
	{
		uint32_t winWidth = Application::Get().GetWindow().GetWidth();
		uint32_t winHeight = Application::Get().GetWindow().GetHeight();

		particles->GetProperties().position.x = m_Camera.GetZoomLevel()
			* ((float)winWidth / winHeight)
			* (Input::GetMouseX() / winWidth * 2.0f - 1.0f)
			+ m_Camera.GetCamera().GetPosition().x;

		particles->GetProperties().position.y = m_Camera.GetZoomLevel()
			* -(Input::GetMouseY() / winHeight * 2.0f - 1.0f)
			+ m_Camera.GetCamera().GetPosition().y;

		for (int i = 0; i < particleSpawn; ++i)
			particles->Emit();
	}

	particles->Update(ts);
	particles->Render(m_Camera);
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
	ImGui::DragFloat("Size #1", &particles->GetProperties().startSize, 0.01f, 0.0f, 10.0f, "%.3f");
	ImGui::DragFloat("Size #2", &particles->GetProperties().endSize, 0.01f, 0.0f, 10.0f, "%.3f");

	ImGui::Separator();
	ImGui::DragFloat("Rotation velocity", &particles->GetProperties().rotationVel, 0.01f, -25.0f, 25.0f, "%.3f");

	ImGui::Separator();
	ImGui::DragFloat("Speed", &particles->GetProperties().speed, 0.01f, 0.0f, 10.0f, "%.3f");

	ImGui::Separator();
	ImGui::DragFloat("Liftime", &particles->GetProperties().maxlife, 0.01f, 0.0f, 10.0f, "%.3f sec");

	ImGui::Separator();
	ImGui::DragInt("Spawn amount", &particleSpawn, 1, 1, 100);

	ImGui::End();
}


