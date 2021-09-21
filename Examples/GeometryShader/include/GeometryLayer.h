#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

class GeometryLayer : public GLCore::Layer
{
public:
	GeometryLayer();
	virtual ~GeometryLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	GLCore::Utils::OrthographicCameraController m_Camera;
	GLCore::Ref<GLCore::Utils::Shader> m_Shader = nullptr;

};