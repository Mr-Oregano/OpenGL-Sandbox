#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
#include <GLCore/ImGui/ImGuiExt.h>

// Must be a multiple of 4
#define MAX_MAP_ENTRIES 12

struct PropertiesUniforms
{
	int algorithmType;
	int seed;
	unsigned int octaves;
	float evolve;
	float scale;
	float lacunarity;
	float persistence;
	
	unsigned int mapSize;
	glm::vec4 mapColors[MAX_MAP_ENTRIES];
	glm::vec4 mapRanges[MAX_MAP_ENTRIES / 4];
	
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

	bool MapEntry(float *colors, float *min, float *max, int id = 0);
	virtual void OnImGuiRender() override;

private:
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;
	GLCore::Ref<ImGuiExt::ViewportDockSpace> m_DockSpace;

	PropertiesUniforms m_Props{};
};