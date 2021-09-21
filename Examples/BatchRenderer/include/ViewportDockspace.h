#pragma once

#include <GLCore.h>
#include <GLCore/Core/Core.h>

#if GLCORE_IMGUI
#define DOCKSPACE_CAPTURE_SCOPE(x) ImGuiExt::ViewportDockSpace::ScopeCapture capture##__LINE__(x)
#else
#define DOCKSPACE_CAPTURE_SCOPE(x)
#endif

namespace ImGuiExt {
	class ViewportDockSpace
	{
	public:
		struct ScopeCapture
		{
			GLCore::Ref<ViewportDockSpace> m_Dockspace;

			ScopeCapture(GLCore::Ref<ViewportDockSpace> dockspace)
				: m_Dockspace(dockspace)
			{
				m_Dockspace->BeginCapture();
			}
			~ScopeCapture()
			{
				m_Dockspace->EndCapture();
			}
		};

	public:
		ViewportDockSpace();

		void GenerateAttachments();
		void BeginCapture() const;
		void EndCapture() const;

		void SetViewport(glm::uvec2 dimensions);

		inline bool IsViewportFocused() const { return m_Focused; }

		void Begin();
		void UpdateViewportWindow();
		void End();

	private:
		GLuint m_FrameBufferHandle;
		GLuint m_ColorAttachment0;
		GLuint m_DepthStencilAttachment;

		unsigned int m_Width;
		unsigned int m_Height;

		bool m_Focused = false;

	};
}
