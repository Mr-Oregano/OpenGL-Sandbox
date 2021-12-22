
#include "ParticleSystem.h"

using namespace GLCore;
using namespace GLCore::Utils;

ParticleSystem::ParticleSystem(ParticleSystemProp props)
	: m_Prop(props)
{
	LOG_INFO("Creating particle system resources...");

	m_ParticlePool.resize(POOL_SIZE);

	glCreateBuffers(1, &m_VertexBufferHandle);
	glNamedBufferStorage(m_VertexBufferHandle, m_ParticlePool.size() * sizeof(Particle), nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &m_VertexArrayHandle);
	glVertexArrayVertexBuffer(m_VertexArrayHandle, 0, m_VertexBufferHandle, 0, sizeof(Particle));
	
	glVertexArrayAttribFormat(m_VertexArrayHandle, 0, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(Particle, color));
	glVertexArrayAttribFormat(m_VertexArrayHandle, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Particle, position));
	glVertexArrayAttribFormat(m_VertexArrayHandle, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Particle, velocity));
	glVertexArrayAttribFormat(m_VertexArrayHandle, 3, 1, GL_FLOAT, GL_FALSE, offsetof(Particle, size));
	glVertexArrayAttribFormat(m_VertexArrayHandle, 4, 1, GL_FLOAT, GL_FALSE, offsetof(Particle, rotation));
	glVertexArrayAttribFormat(m_VertexArrayHandle, 5, 1, GL_FLOAT, GL_FALSE, offsetof(Particle, life));

	glVertexArrayAttribBinding(m_VertexArrayHandle, 0, 0);
	glVertexArrayAttribBinding(m_VertexArrayHandle, 1, 0);
	glVertexArrayAttribBinding(m_VertexArrayHandle, 2, 0);
	glVertexArrayAttribBinding(m_VertexArrayHandle, 3, 0);
	glVertexArrayAttribBinding(m_VertexArrayHandle, 4, 0);
	glVertexArrayAttribBinding(m_VertexArrayHandle, 5, 0);

	glEnableVertexArrayAttrib(m_VertexArrayHandle, 0);
	glEnableVertexArrayAttrib(m_VertexArrayHandle, 1);
	glEnableVertexArrayAttrib(m_VertexArrayHandle, 2);
	glEnableVertexArrayAttrib(m_VertexArrayHandle, 3);
	glEnableVertexArrayAttrib(m_VertexArrayHandle, 4);
	glEnableVertexArrayAttrib(m_VertexArrayHandle, 5);

	// Shader

	m_Shader = Shader::FromGLSLTextFiles(
		"res/vert.glsl",
		"res/geom.glsl",
		"res/frag.glsl"
	);

	m_ShaderHandle = m_Shader->GetRendererID();
	m_ShaderViewProjUniformLoc = glGetUniformLocation(m_ShaderHandle, "u_ViewProjection");

	assert(m_ShaderViewProjUniformLoc != -1);
}

ParticleSystem::~ParticleSystem()
{
	LOG_INFO("Deleting particle system resources...");

	glDeleteVertexArrays(1, &m_VertexArrayHandle);
	glDeleteBuffers(1, &m_VertexBufferHandle);
}

void ParticleSystem::Update(GLCore::Timestep ts)
{
	PROFILE_FUNCTION();
	size_t deadCount = 0;
	
	for (size_t i = 0; i < m_InsertIndex; ++i)
	{
		Particle &p = m_ParticlePool[i];
		p.life -= ts;

		float interpolate = 1.0f - p.life / m_Prop.maxlife;
		p.color = LerpColor(m_Prop.startColor, m_Prop.endColor, interpolate);

		p.position += (float)ts * p.velocity;
		p.size = glm::mix(m_Prop.startSize, m_Prop.endSize, interpolate);
		p.rotation += ts * m_Prop.rotationVel;

		if (p.life < 0.0f)
			++deadCount;
	}

	for (size_t i = deadCount; i < m_InsertIndex; ++i)
		m_ParticlePool[i - deadCount] = m_ParticlePool[i];

	m_InsertIndex -= deadCount;
}

uint32_t ParticleSystem::LerpColor(uint32_t c1, uint32_t c2, float value)
{
	uint32_t r1 = c1 >> 24 & 0xff;
	uint32_t g1 = c1 >> 16 & 0xff;
	uint32_t b1 = c1 >> 8 & 0xff;
	uint32_t a1 = c1 & 0xff;
	
	uint32_t r2 = c2 >> 24 & 0xff;
	uint32_t g2 = c2 >> 16 & 0xff;
	uint32_t b2 = c2 >> 8 & 0xff;
	uint32_t a2 = c2 & 0xff;
	
	uint32_t r3 = static_cast<uint32_t>(r1 * (1.0f - value) + r2 * value);
	uint32_t g3 = static_cast<uint32_t>(g1 * (1.0f - value) + g2 * value);
	uint32_t b3 = static_cast<uint32_t>(b1 * (1.0f - value) + b2 * value);
	uint32_t a3 = static_cast<uint32_t>(a1 * (1.0f - value) + a2 * value);

	return (r3 << 24) | (g3 << 16) | (b3 << 8) | a3;
}

void ParticleSystem::Render(GLCore::Utils::OrthographicCameraController &camera)
{
	if (m_InsertIndex == 0)
		return; // Nothing to render.

	PROFILE_FUNCTION();

	// Submit camera to GPU

	glProgramUniformMatrix4fv(m_ShaderHandle, m_ShaderViewProjUniformLoc, 1, GL_FALSE, glm::value_ptr(camera.GetCamera().GetViewProjectionMatrix()));

	// Enable shader & fixed pipeline state

	glUseProgram(m_ShaderHandle);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Map client batch to GPU buffer

	glNamedBufferSubData(m_VertexBufferHandle, 0, m_InsertIndex * sizeof(Particle), m_ParticlePool.data());

	// Draw batch

	glBindVertexArray(m_VertexArrayHandle);
	glDrawArrays(GL_POINTS, 0, (GLsizei) m_InsertIndex);
}

void ParticleSystem::Emit()
{
	if (m_InsertIndex == m_ParticlePool.size())
	{
		// TODO: In the future may want to consider dynamically expanding the pool
		//		 size to fit the number of particles.
		//
		LOG_WARN("Pool not large enough skipping particles");
		return;
	}

	Particle &p = m_ParticlePool[m_InsertIndex];

	float rotation = Random::Float() * 360.0f;

	p.position = m_Prop.position;
	p.size = m_Prop.startSize;
	p.rotation = rotation;

	p.velocity = { glm::cos(rotation), glm::sin(rotation) };
	p.velocity *= m_Prop.speed;

	p.color = m_Prop.startColor;
	p.life = m_Prop.maxlife;
		
	++m_InsertIndex;
}
