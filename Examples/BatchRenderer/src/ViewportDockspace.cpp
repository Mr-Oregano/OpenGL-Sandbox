
#include "ViewportDockspace.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <GLCore/Core/Application.h>

using namespace GLCore;

namespace ImGuiExt {

	ViewportDockSpace::ViewportDockSpace()
	{
		m_Width = Application::Get().GetWindow().GetWidth();
		m_Height = Application::Get().GetWindow().GetHeight();

		GenerateAttachments();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ViewportDockSpace::GenerateAttachments()
	{
		if (m_FrameBufferHandle)
		{
			glDeleteFramebuffers(1, &m_FrameBufferHandle);
			glDeleteTextures(1, &m_ColorAttachment0);
			glDeleteTextures(1, &m_DepthStencilAttachment);
		}

		glCreateFramebuffers(1, &m_FrameBufferHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment0);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment0, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthStencilAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthStencilAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthStencilAttachment, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_ERROR("Framebuffer failure: Framebuffer is incomplete!");
	}

	void ViewportDockSpace::BeginCapture() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferHandle);
		glViewport(0, 0, m_Width, m_Height);
	}

	void ViewportDockSpace::EndCapture() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void ViewportDockSpace::SetViewport(glm::uvec2 dimensions)
	{
		m_Width = dimensions.x;
		m_Height = dimensions.y;
		GenerateAttachments();
	}

	void ViewportDockSpace::Begin()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		// DockSpace
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, { 0.0f, 0.0f }, dockspace_flags);

	}
	void ViewportDockSpace::UpdateViewportWindow()
	{
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration);

		if (Application::Get().GetWindow().GetCursorMode() != CursorMode::Disabled)
			m_Focused = ImGui::IsWindowHovered();

		size_t textureID = m_ColorAttachment0;
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		static ImVec2 m_ViewportSize = { 0.0f, 0.0f };
		static constexpr float m_AspectRatio = 16.0f / 9.0f;

		viewportPanelSize.x = viewportPanelSize.y * m_AspectRatio;

		if ((m_ViewportSize.x != viewportPanelSize.x)
		|| (m_ViewportSize.y != viewportPanelSize.y))
		{
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			
			// m_FrameBuffer->Resize((unsigned int) viewportPanelSize.x, (unsigned int) viewportPanelSize.y);
		}

		ImGui::SetCursorPos((ImGui::GetWindowSize() - m_ViewportSize) * 0.5f);
		ImGui::Image((void *)textureID, m_ViewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
	}
	void ViewportDockSpace::End()
	{
		ImGui::End();
	}
}