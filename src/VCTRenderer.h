#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <unordered_map>

#include "Camera.h"
#include "GLFWInput.h"
#include "Window.h"
#include "Light.h"
#include "Model.h"
#include "Shader.h"

class VCTRenderer
{
public:
	const float sceneScale = .05f;
	Light* light;
	GLFWWindow* m_window;
	GLFWwindow* window;
	Camera* camera;
	Model* model;

	int scrWidth, scrHeight;
	int voxelDimensions_ = 256;

	Shader* stdShader;
	Shader* voxelizationShader;
	VCTRenderer(GLFWWindow* aWindow);
	~VCTRenderer();
	bool init(Camera *mCamera);
	void update(float deltaTime);

private:

};

