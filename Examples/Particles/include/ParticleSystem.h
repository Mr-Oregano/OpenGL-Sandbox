#pragma once

#include <vector>

#include <GLCore.h>
#include <GLCore/Core/Timestep.h>
#include <GLCore/Core/Random.h>
#include <GLCoreUtils.h>

#include <GLCore/Core/Instrumentor.h>

#define POOL_SIZE 100000

struct ParticleSystemProp
{
	glm::vec4 startColor;
	glm::vec4 endColor;
	glm::vec3 position;
		
	float startSize = 0;
	float endSize = 0;

	float rotationVel = 0;
	float speed = 0;
		
	float maxlife = 0;
};

class ParticleSystem
{
private:
	ParticleSystemProp m_Prop;

	struct Particle
	{
		glm::vec4 color{1.0f};
		glm::vec2 position{1.0f};
		glm::vec2 velocity{1.0f};
		float size = 0.0f;
		float rotation = 0.0f;
		float life = 0.0f;
	};

	std::vector<Particle> m_ParticlePool;
	size_t m_InsertIndex = 0;

	GLuint m_VertexArrayHandle = 0;
	GLuint m_VertexBufferHandle = 0;
	GLuint m_ShaderHandle = 0;
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;

	GLint m_ShaderViewProjUniformLoc = -1;

public:
	ParticleSystem(ParticleSystemProp props);
	~ParticleSystem();

	inline void SetParticleProperties(const ParticleSystemProp &prop) { this->m_Prop = prop; }
	inline ParticleSystemProp& GetProperties() { return m_Prop; }

	void Update(GLCore::Timestep ts);
	void Render(GLCore::Utils::OrthographicCameraController &camera);
	void Emit();
};
