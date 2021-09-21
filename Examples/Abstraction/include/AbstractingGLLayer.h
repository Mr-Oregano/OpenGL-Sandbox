#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"
#include "RenderCommand.h"

class AbstractingGLLayer : public GLCore::Layer
{
public:
	AbstractingGLLayer();
	virtual ~AbstractingGLLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;

private:
	GLCore::Utils::OrthographicCameraController m_Camera;
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;

	GLCore::Ref<VertexArray> m_VertexArray = nullptr;

};