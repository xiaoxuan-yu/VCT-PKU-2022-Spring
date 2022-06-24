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
	const float GridWorldSize_ = 160.0f;
	int shadowMapRes = 2048;

	glm::mat4 projectionX, projectionY, projectionZ;
	glm::mat4 depthViewProjectionMatrix;

	GLuint voxelTexture;
	GLuint texture3DVertexArray;
	GLuint depthFramebuffer;
	GLuint depthTexture;

	Shader* tracingShader;
	Shader* voxelizationShader;
	Shader* shadowShader;
	Shader* voxelvisualizeShader;

	VCTRenderer(GLFWWindow* aWindow);
	~VCTRenderer();
	bool init(Camera *mCamera, Light *mLight);
	void render(float deltaTime);
	void voxel_visualize(float deltaTime);
	void updateWindowSize(int width, int height) {
		scrWidth = width;
		scrHeight = height;
	}
	

private:
	void CalcVoxelTexture();
	void CalcDepthTexture();
};

