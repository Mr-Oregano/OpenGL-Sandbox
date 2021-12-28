#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>

#include "ParticleSystem.h"

class ParticlesLayer : public GLCore::Layer
{
public:
	ParticlesLayer();
	virtual ~ParticlesLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	
	void ImGuiLoadTexture();
	virtual void OnImGuiRender() override;

	bool OnMouseButtonPressed(GLCore::MouseButtonPressedEvent& e);
private:
	GLCore::Utils::OrthographicCameraController m_Camera;
	ParticleSystem *particles = nullptr;

};