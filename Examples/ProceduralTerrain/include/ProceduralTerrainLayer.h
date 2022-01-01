#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
#include <GLCore/ImGui/ImGuiExt.h>

struct PropertiesUniforms
{
	int algorithmType;
	int seed;
	unsigned int octaves;
	float evolve;
	float scale;
	float lacunarity;
	float persistence;
	float _padding;
	glm::vec2 resolution;
};

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

	PropertiesUniforms m_Props{};
};