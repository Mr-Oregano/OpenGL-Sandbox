#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
#include <GLCore/ImGui/ImGuiExt.h>

class ProceduralTerrainLayer : public GLCore::Layer
{
public:
	ProceduralTerrainLayer();
	virtual ~ProceduralTerrainLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event &event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;

private:
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;
	GLCore::Ref<ImGuiExt::ViewportDockSpace> m_DockSpace;

	bool m_Evolution = true;

	float m_Zoom = 1.0f;

	float m_Evolve = 0.0f;
	int m_Seed = 0;

	float m_Scale = 1.0f;
	float m_Lacunarity = 1.0f;
	float m_Persistence = 1.0f;
	float m_Octaves = 1;

};