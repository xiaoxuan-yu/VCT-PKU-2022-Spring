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

	//Voxelization Settings
	int voxelDimensions_ = 256;
	const float GridWorldSize_ = 160.0f;

	//ShadowMap Settings
	int shadowMapRes = 2048;
	float light_near_plane = 0.1f;
	GLfloat light_far_plane = 100.0f;

	//Transform Matrix
	glm::mat4 projectionX, projectionY, projectionZ;
	std::vector<glm::mat4> depthViewProjectionMatrix;
	//glm::mat4 depthViewProjectionMatrix;

	//Texture and Buffer
	GLuint voxelTexture;
	GLuint texture3DVertexArray;
	GLuint depthFramebuffer;
	GLuint depthTexture;

	//Shaders
	Shader* tracingShader;
	Shader* voxelizationShader;
	Shader* shadowShader;
	Shader* voxelvisualizeShader;
	Shader* depthvisualizeShader;

	float ambientFactor_ = 0.2f;

	VCTRenderer(GLFWWindow* aWindow);
	~VCTRenderer();
	bool init(Camera *mCamera, Light *mLight);
	void render(float deltaTime);
	void voxel_visualize(float deltaTime);
	void depth_visualize(float deltaTime);
	void updateWindowSize(int width, int height) {
		scrWidth = width;
		scrHeight = height;
	}
	

private:
	void CalcVoxelTexture();
	void CalcDepthTexture();
};

