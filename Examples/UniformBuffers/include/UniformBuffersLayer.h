#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
#include <GLCore/ImGui/ImGuiExt.h>

class UniformBuffersLayer : public GLCore::Layer
{
public:
	UniformBuffersLayer();
	virtual ~UniformBuffersLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event &event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;

private:
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;
};