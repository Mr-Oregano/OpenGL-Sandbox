#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

#include "Texture.h"

class TexturesLayer: public GLCore::Layer
{
public:
	TexturesLayer();
	virtual ~TexturesLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;

private:
	GLCore::Utils::OrthographicCameraController m_Camera;
	
	GLCore::Ref<Texture2D> m_LandTex;
	GLCore::Ref<Texture2D> m_SunsetTex;
	GLCore::Ref<GLCore::Utils::Shader> m_Shader;
};