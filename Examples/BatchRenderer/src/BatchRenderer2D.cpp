
#include "BatchRenderer2D.h"

#include <GLCore/Core/Instrumentor.h>
#include "ImGuiExt.h"

using namespace GLCore;
using namespace GLCore::Utils;

bool BatchRenderer2D::s_Initialized = false;

unsigned int BatchRenderer2D::s_Capacity = 0;
unsigned int BatchRenderer2D::s_TextureSlots = 0;

std::vector<Batch> BatchRenderer2D::s_Batches;
BatchRenderer2D::Assets *BatchRenderer2D::s_Assets = nullptr;

unsigned int BatchRenderer2D::s_FlushCount = 0;

void BatchRenderer2D::Init(unsigned int capacity)
{

	if (s_Initialized)
		return;

	s_Capacity = capacity;

	GLint maxTextureUnits = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	LOG_INFO("Fragment Shader - texture unit max: {0}", maxTextureUnits);

	s_TextureSlots = maxTextureUnits;

	s_Assets = new Assets();
	s_Batches.reserve(10);
	s_Batches.emplace_back(s_Capacity, s_TextureSlots);

	glCreateVertexArrays(1, &s_Assets->vertexArray);
	glBindVertexArray(s_Assets->vertexArray);

	glCreateBuffers(1, &s_Assets->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, s_Assets->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, (unsigned int)(s_Capacity * sizeof(Batch::Vertex)), nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Batch::Vertex), (const void*) offsetof(Batch::Vertex, color));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Batch::Vertex), (const void *)offsetof(Batch::Vertex, position));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Batch::Vertex), (const void *)offsetof(Batch::Vertex, rotation));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Batch::Vertex), (const void *)offsetof(Batch::Vertex, texture));
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Batch::Vertex), (const void *)offsetof(Batch::Vertex, texelSize));
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	s_Assets->shader = Shader::FromGLSLTextFiles(
		"res/shaders/batching-vert.glsl",
		"res/shaders/batching-geom.glsl",
		"res/shaders/batching-frag.glsl"
	);

	GLuint program = s_Assets->shader->GetRendererID();
	glUseProgram(program);
	for (int i = 0; i < maxTextureUnits; ++i)
	{
		std::stringstream ss;
		ss << "u_TextureSlots[" << i << "]";
		int loc = glGetUniformLocation(program, ss.str().c_str());
		glUniform1i(loc, i);
	}

	unsigned char white[] = { 255, 255, 255 };
	// s_Assets->whiteTexture = Texture2D::LoadFromData(white, 1, 1, 3, { Filter::Nearest, WrapMode::ClampToEdge, MipmapMode::None });
	s_Initialized = true;

}
void BatchRenderer2D::Shutdown()
{

	if (!s_Initialized)
		return;

	// TODO: Profile termination session
	// PROFILE_FUNCTION();

	s_Initialized = false;
	s_Batches.clear();

	delete s_Assets;
	s_Assets = nullptr;

}

void BatchRenderer2D::SetCapacity(unsigned int capacity)
{
	for (auto& batch : s_Batches)
		batch.submissions.resize(capacity);
}

void BatchRenderer2D::Begin(const OrthographicCamera &camera, const glm::mat4 &transform, const glm::vec4 &tint)
{
	PROFILE_FUNCTION();

	// TODO: manage blending 
	glEnable(GL_BLEND);

	// TODO: Premultiply alpha
	// glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	s_FlushCount = 0;

	GLuint program = s_Assets->shader->GetRendererID();
	glUseProgram(program);

	GLint loc = glGetUniformLocation(program, "u_ViewProjection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(camera.GetViewProjectionMatrix()));

	loc = glGetUniformLocation(program, "u_Model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

	loc = glGetUniformLocation(program, "u_Tint");
	glUniform4f(loc, tint.r, tint.g, tint.b, tint.a);
}

void BatchRenderer2D::End()
{
	PROFILE_FUNCTION();

	Flush();

	glDisable(GL_BLEND);
}

void BatchRenderer2D::DrawRect(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, float rotation)
{
	DrawRect(pos, size, color, s_Assets->white, rotation);
}

void BatchRenderer2D::DrawRect(const glm::vec2 &pos, const glm::vec2 &size, Batch::Material material, float rotation)
{
	DrawRect(pos, size, { 1.0f, 1.0f, 1.0f, 1.0f }, material, rotation);
}

void BatchRenderer2D::DrawRect(const glm::vec2 &pos, const glm::vec2 &size, const glm::vec4 &color, Batch::Material material, float rotation)
{
	// TODO: consider using bindless textures perhaps.
	//
	size_t textureIndex = 0;
	size_t batchIndex = s_Batches.size();

	for (size_t i = 0; i < s_Batches.size(); ++i)
	{
		// Check if batch is full.
		if (s_Batches[i].submission_count == s_Batches[i].submissions.size())
			continue;

		// Search for texture slot in current batch
		for (textureIndex = 0; textureIndex < s_Batches[i].material_count; ++textureIndex)
			if (material.textureHandle == s_Batches[i].materials[textureIndex].textureHandle)
				break;

		// Check if texture slot is available
		if (textureIndex == s_Batches[i].materials.size())
		{
			textureIndex = 0;
			continue;
		}

		batchIndex = i;
	}

	if (batchIndex == s_Batches.size())
		s_Batches.emplace_back(s_Capacity, s_TextureSlots);
		
	Batch &batch = s_Batches[batchIndex];
	batch.materials[textureIndex] = material;
	batch.material_count += batch.material_count == textureIndex;

	// Submit data for later processing
	// TODO: perhaps try using a compute shader rather than a geometry shader
	//		 to construct elements
	//

	Batch::Vertex &vertex = batch.submissions[batch.submission_count];
	vertex.color = color;
	vertex.position = { pos.x, pos.y, size.x, size.y };
	vertex.rotation = rotation;
	vertex.texture = (float) textureIndex;
	vertex.texelSize = { material.texWidth, material.texHeight }; 
		
	++batch.submission_count;
}

void BatchRenderer2D::Flush()
{

	PROFILE_FUNCTION();
		
	for (auto &batch : s_Batches)
	{
		if (!batch.submission_count)
			continue;

		// Debug functinality
		++s_FlushCount;
		//

		for (unsigned int i = 0; i < batch.material_count; ++i)
			glBindTextureUnit(i, batch.materials[i].textureHandle);

		glBindBuffer(GL_ARRAY_BUFFER, s_Assets->vertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch.submission_count * sizeof(Batch::Vertex), batch.submissions.data());
		glDrawArrays(GL_POINTS, 0, batch.submission_count);

		batch.submission_count = 0;
		batch.material_count = 0;
	}
}

void ImGuiHeader_BatchCapacity()
{
	if (!ImGui::CollapsingHeader("Batch Capacity", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	ImGui::PushItemWidth(ImGui::GetFontSize() * -15.0f);
	static int batch_buffer_cap = BatchRenderer2D::GetCapacity();
	if (ImGui::SliderInt("Batch Buffer Capacity", &batch_buffer_cap, 1, 100000))
		BatchRenderer2D::SetCapacity(batch_buffer_cap);

	ImGuiExt::Help("Specifies the amount of elements capable of being stored in each batch before flushing.");
	ImGui::PopItemWidth();

}
void BatchRenderer2D::OnImGuiRender(bool *show)
{
	if (!(*show))
		return;

	ImGui::SetNextWindowSize({ 500.0f, 400.0f }, ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("BatchRenderer2D", show))
	{
		ImGui::End();
		return;
	}

	ImGui::PushItemWidth(ImGui::GetWindowWidth());
	ImGui::Text("Analytics:");
	ImGui::Separator();
	ImGui::Text("Batch Buffer Capacity: %lu", s_Capacity);
	ImGui::Text("Batch Texture Capacity: %lu", s_TextureSlots);
	ImGui::Text("Flushes per Scene: %lu", s_FlushCount);
	ImGui::PopItemWidth();

	ImGuiHeader_BatchCapacity();

	ImGui::End();

}