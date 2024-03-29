#pragma once

#include <vector>

#include <GLCore.h>
#include <GLCore/Core/Timestep.h>
#include <GLCore/Core/Random.h>
#include <GLCoreUtils.h>

#include <GLCore/Core/Instrumentor.h>

#define POOL_SIZE 100000

enum ParticleBlend
{
	NONE,
	TRANSPARENCY,
	ADDITIVE_TRANSPARENCY
};

// TODO: It may be better to represent particles based on 'start' and 'end'
//		 properties, rather than attributes like speed, or velocity, etc.
//		 this allows us to have better control over how the attributes are 
//		 interpolated over time.
//		
struct ParticleSystemProp
{
	GLuint texture;
	uint32_t startColor;
	uint32_t endColor;
	
	glm::vec2 position;
	glm::vec2 direction;
	float dir_precision;
	
	float speed = 0;
	float speed_precision;
	float acceleration = 0;
	float rotationVel = 0;
	
	glm::vec2 externalForce;
	
	float startSize = 0;
	float endSize = 0;

	float maxlife = 0;
	ParticleBlend blending = TRANSPARENCY;
};

class ParticleSystem
{
private:
	ParticleSystemProp m_Prop;

	struct Particle
	{
		uint32_t color;
		glm::vec2 position{1.0f};
		glm::vec2 velocity{1.0f};
		float size = 0.0f;
		float rotation = 0.0f;
		float life = 0.0f;
	};

	std::vector<Particle> m_ParticlePool;
	size_t m_InsertIndex = 0;

	GLuint m_DefaultTextureHandle = 0;
	GLuint m_VertexArrayHandle = 0;
	GLuint m_VertexBufferHandle = 0;
	GLuint m_ShaderHandle = 0;
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;

	GLint m_ShaderViewProjUniformLoc = -1;

private:
	uint32_t LerpColor(uint32_t c1, uint32_t c2, float value);

public:
	ParticleSystem(ParticleSystemProp props);
	~ParticleSystem();

	inline void SetParticleProperties(const ParticleSystemProp &prop) { this->m_Prop = prop; }
	inline ParticleSystemProp& GetProperties() { return m_Prop; }

	void Update(GLCore::Timestep ts);
	void Render(GLCore::Utils::OrthographicCameraController &camera);
	void Emit();
};
