
#include "ProceduralTerrainLayer.h"

#include <ctime>

using namespace GLCore;
using namespace GLCore::Utils;

GLuint vertexArrayHandle;
GLuint vertexBufferHandle;
GLuint uniformBufferHandle;

ProceduralTerrainLayer::ProceduralTerrainLayer()
{
}

ProceduralTerrainLayer::~ProceduralTerrainLayer()
{
}

void ProceduralTerrainLayer::OnAttach()
{
	EnableGLDebugging();

	m_DockSpace = CreateRef<ImGuiExt::ViewportDockSpace>();

	m_Shader = Shader::FromGLSLTextFiles(
		"res/terrain-vert.glsl",
		"res/terrain-frag.glsl"
	);
	
	GLuint program = m_Shader->GetRendererID();
	glUseProgram(program);

	glCreateBuffers(1, &uniformBufferHandle);

	m_Props.seed = (unsigned int) std::time(nullptr);
	m_Props.octaves = 1u;
	m_Props.scale = 1.0f;
	m_Props.lacunarity = 1.0f;
	m_Props.persistence = 1.0f;
	m_Props.resolution = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };

	glNamedBufferStorage(uniformBufferHandle, sizeof(PropertiesUniforms), &m_Props, GL_MAP_WRITE_BIT);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformBufferHandle);

	float vertices[] =
	{
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f
	};

	glCreateVertexArrays(1, &vertexArrayHandle);
	glBindVertexArray(vertexArrayHandle);

	glCreateBuffers(1, &vertexBufferHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void ProceduralTerrainLayer::OnDetach()
{
	glDeleteVertexArrays(1, &vertexArrayHandle);
	glDeleteBuffers(1, &vertexBufferHandle);
}

void ProceduralTerrainLayer::OnEvent(Event &event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) 
	{
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
		m_Props.resolution = { e.GetWidth(), e.GetHeight() };

		void* uniforms_ptr = glMapNamedBuffer(uniformBufferHandle, GL_WRITE_ONLY);
		std::memcpy(uniforms_ptr, &m_Props, sizeof(PropertiesUniforms));
		glUnmapNamedBuffer(uniformBufferHandle);

		m_DockSpace->SetViewport({ e.GetWidth(), e.GetHeight() });
		return true;
	});
}

static bool uniformsIsDirty = false;

void ProceduralTerrainLayer::OnUpdate(Timestep ts)
{
	DOCKSPACE_CAPTURE_SCOPE(m_DockSpace);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(vertexArrayHandle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (uniformsIsDirty)
	{
		void *uniforms_ptr = glMapNamedBuffer(uniformBufferHandle, GL_WRITE_ONLY);
		std::memcpy(uniforms_ptr, &m_Props, sizeof(PropertiesUniforms));
		glUnmapNamedBuffer(uniformBufferHandle);
		uniformsIsDirty = false;
	}
}

void ProceduralTerrainLayer::OnImGuiRender()
{
	static const char *noise_algorithms[] = { "Simplex Noise", "Perlin Noise" };
	static float ocatvesFloat = (float) m_Props.octaves;

	m_DockSpace->Begin();

	m_DockSpace->UpdateViewportWindow();

	ImGui::Begin("Controls");

	static char buffer[128] = { 0 };
	if (ImGui::InputText("Seed Hasher", buffer, sizeof(buffer)))
	{
		std::hash<std::string> hasher;
		m_Props.seed = static_cast<unsigned int>(hasher(buffer));
		uniformsIsDirty = true;
	}

	uniformsIsDirty = ImGui::InputInt("Seed", &m_Props.seed, 1, 100) || uniformsIsDirty;
	uniformsIsDirty = ImGui::Combo("Noise Generator", &m_Props.algorithmType, noise_algorithms, (int) std::size(noise_algorithms)) || uniformsIsDirty;

	ImGui::Separator();

	uniformsIsDirty = ImGuiExt::Knob("Scale", &m_Props.scale, 1.0f, 100.0f) || uniformsIsDirty;
	ImGuiExt::Help("Scale: Changes the output scale/zoom of the noise generator. The larger the scale, the more output you'll see.");

	ImGui::SameLine();
	if (ImGuiExt::KnobInt("Octaves", &ocatvesFloat, 1, 20, 0.35f))
	{
		m_Props.octaves = (unsigned int) ocatvesFloat;
		uniformsIsDirty = true;
	}
	ImGuiExt::Help("Ocatves: Adds detail to the overall output by stacking 'n' layers of noise called octaves. "
						   "Each octave will be added on to the previous.\n\n"
						   "The behaviour of stacking octaves can be controlled by the Lacunarity and Persistence variables.");

	ImGui::SameLine();
	uniformsIsDirty = ImGuiExt::Knob("Lac", &m_Props.lacunarity, 0.0f, 10.0f) || uniformsIsDirty;
	ImGuiExt::Help("Lacunarity: Determines the change in frequency/scale for each octave. For example, a Lacunarity of 2.0 "
						   "means that each octave of noise will have twice the detail of the previous.\n\n"
						   "A Lacunarity greater than 1.0 means that later octaves will have more detail/scale than the previous.\n\n"
						   "A Lacunarity less than 1.0 means that later octaves will have less detail/scale than the previous.\n\n"
						   "A Lacunarity equal to 1.0 means that later octaves will have the same detail/scale as the previous.");

	ImGui::SameLine();
	uniformsIsDirty = ImGuiExt::Knob("Pers", &m_Props.persistence, 0.0f, 10.0f) || uniformsIsDirty;
	ImGuiExt::Help("Persistence: Determines how much effect succeeding layers of octaves will have on the output noise. "
						   "For example, a Persistence of 0.5 means that each octave will only be added to the output at half the "
						   "strength of the previous.\n\n"
						   "A Persistence greater than 1.0 means that later octaves will have a stronger effect "
						   "than the previous.\n\n"
						   "A Persistence less than 1.0 means that later octaves will have a weaker effect "
						   "than the previous.\n\n"
						   "A Persistence equal to 1.0 means that all octaves will have an equal effect on the "
						   "output noise.");

	ImGui::End();

	m_DockSpace->End();
}