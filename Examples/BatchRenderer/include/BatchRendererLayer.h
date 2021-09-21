#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

#include <GLCore/Core/Input.h>
#include <GLCore/Core/Random.h>
#include <GLcore/Core/Instrumentor.h>

#include "ImGuiExt.h"

#include "BatchRenderer2D.h"

class BatchRendererLayer : public GLCore::Layer
{
public:
	BatchRendererLayer();
	virtual ~BatchRendererLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;

	void ImGuiEnironmentHeader()
	{
		if (!ImGui::CollapsingHeader("Environment", ImGuiTreeNodeFlags_DefaultOpen))
			return;

		if (ImGui::ColorPicker4("Clear Color", glm::value_ptr(clearColor)))
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	}
	void ImGuiElementsHeader()
	{

		if (!ImGui::CollapsingHeader("Elements", ImGuiTreeNodeFlags_DefaultOpen))
			return;

		ImGui::PushItemWidth(ImGui::GetFontSize() * -5.0f);
		ImGui::LabelText("", "Quantity: %d", s_ROWS * s_COLS);

		static bool link = true;
		ImGui::Checkbox("Link Sliders", &link);
		ImGuiExt::Help("Sliders will be linked to the same value");

		if (ImGui::SliderInt("Rows", &s_ROWS, 1, 1000))
			if (link) s_COLS = s_ROWS;

		if (ImGui::SliderInt("Columns", &s_COLS, 1, 1000))
			if (link) s_ROWS = s_COLS;

		ImGui::PopItemWidth();

	}
	void ImGuiAssetsHeader() 
	{
		if (!ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen))
			return;

		ImGui::PushItemWidth(ImGui::GetFontSize() * -5.0f);

		ImGui::SliderInt("Textures", &s_TEXTURES, 1, (int)m_Materials.size());
		ImGui::Separator();
		ImGui::LabelText("", "Textures:");

		ImGui::BeginChild("Textures Child", ImVec2(0, 0), true);

		int columns = glm::clamp((int) ImGui::GetWindowContentRegionWidth() / 100, 1, 64);
		ImGui::Columns(columns, 0, false);
		for (const auto &material : m_Materials)
		{
			ImGui::ImageButton((ImTextureID) ((unsigned long long) material.textureHandle), ImVec2(100, 100), ImVec2(1, 1), ImVec2(0, 0));
			ImGui::NextColumn();
		}

		ImGui::EndChild();

		ImGui::PopItemWidth();

	}

	void ImGuiControl_Window(bool *show)
	{
		if (!(*show))
			return;

		ImGui::SetNextWindowSize({ 500.0f, 400.0f }, ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Controls - Batching Layer", show))
		{
			ImGui::End();
			return;
		}

		ImGuiEnironmentHeader();
		ImGuiElementsHeader();
		ImGuiAssetsHeader();

		ImGui::End();

	}
	void OnImGuiRender() override
	{
		static bool Show_ImGuiDemo_Window = true;
		static bool Show_ImGuiControl_Window = true;
		static bool Show_ImGuiBatching_Window = true;

		//////////////////////////////////////
		m_DockSpace->Begin();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				if (ImGui::MenuItem("Controls", nullptr, Show_ImGuiControl_Window))
					Show_ImGuiControl_Window = !Show_ImGuiControl_Window;

				if (ImGui::MenuItem("Batching", nullptr, Show_ImGuiBatching_Window))
					Show_ImGuiBatching_Window = !Show_ImGuiBatching_Window;

				ImGui::Separator();

				if (ImGui::MenuItem("Demo Window", nullptr, Show_ImGuiDemo_Window))
					Show_ImGuiDemo_Window = !Show_ImGuiDemo_Window;

				ImGui::Separator();
				ImGui::MenuItem("Preferences");

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		m_DockSpace->UpdateViewportWindow();

		if (Show_ImGuiDemo_Window)
			ImGui::ShowDemoWindow(&Show_ImGuiDemo_Window);

		ImGuiControl_Window(&Show_ImGuiControl_Window);
		BatchRenderer2D::OnImGuiRender(&Show_ImGuiBatching_Window);

		m_DockSpace->End();

	}

private:
	GLCore::Utils::OrthographicCameraController m_Camera;
	GLCore::Ref<ImGuiExt::ViewportDockSpace> m_DockSpace;
	
	std::vector<Batch::Material> m_Materials;

	glm::vec4 clearColor = { 0.2f, 0.8f, 0.2f, 1.0f };
	int s_ROWS = 10;
	int s_COLS = 10;
	int s_TEXTURES = 41;

	float Viewport_Height = 720.0f;
	float Viewport_Aspect = 16.0f / 9.0f;
	float Viewport_Width = Viewport_Height * Viewport_Aspect;

};