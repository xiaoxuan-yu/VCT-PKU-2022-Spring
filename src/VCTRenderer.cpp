#include "VCTRenderer.h"
VCTRenderer::VCTRenderer(GLFWWindow* aWindow, GLFWInput* aInput)
{
	window = aWindow->getGLFWwindow();
	input = aInput;
	std::cout << "Creating VCT Render" << std::endl;
	scrWidth = aWindow->getWidth();
	scrHeight = aWindow->getHeight();
}

VCTRenderer::~VCTRenderer()
{
	delete model;
}

bool VCTRenderer::init(Camera *mCamera, Light *mLight) {
	std::cout << "Initializing VCT Render" << std::endl;
	//imgui init
	// std::cout << "Loading UI ..." << std::endl;
	initUI();

	camera = mCamera;
	light = mLight;

	// load Shaders
	tracingShader = new Shader("shader/tracingShader.vs", "shader/tracingShader.fs");
	voxelizationShader = new Shader("shader/voxelization.vs", "shader/voxelization.fs", "shader/voxelization.gs");
	shadowShader = new Shader("shader/shadow_map.vs", "shader/shadow_map.fs", "shader/shadow_map.gs");
	voxelvisualizeShader = new Shader("shader/voxelization_visualizer.vs", "shader/voxelization_visualizer.fs");
	depthvisualizeShader = new Shader("shader/depth_visualize.vs", "shader/depth_visualize.fs");

	//load model
	std::cout << "Loading models..." << std::endl;
	model = new Model("data/sponza.obj");
	std::cout << "Model loaded." << std::endl;

	glGenVertexArrays(1, &texture3DVertexArray);
	//frame buffer for depthTexture and shadow_map
	glGenFramebuffers(1, &depthFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);

	//generate depth texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	for(int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapRes, shadowMapRes, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE); // do not render color

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Error creating depth buffer" << std::endl;
		return false;
	}


	//3D texture for VoxelTexture
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &voxelTexture);
	glBindTexture(GL_TEXTURE_3D, voxelTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//init voxelTexture
	int numVoxels = voxelDimensions_ * voxelDimensions_ * voxelDimensions_;
	GLubyte* data = new GLubyte[numVoxels * 4];
	memset(data, 0, numVoxels * 4);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, voxelDimensions_, voxelDimensions_, voxelDimensions_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	delete[] data;	//release memory

	glGenerateMipmap(GL_TEXTURE_3D);


	//render from light source
	/*
	glm::mat4 viewMatrix = glm::lookAt(light->lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 projectionMatrix = glm::ortho(-120.f, 120.f, -120.f, 120.f, -100.f, 100.f);
	depthViewProjectionMatrix = projectionMatrix * viewMatrix;*/

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, light_near_plane, light_far_plane);
	depthViewProjectionMatrix.push_back(projectionMatrix * glm::lookAt(light->lightPos, light->lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	depthViewProjectionMatrix.push_back(projectionMatrix * glm::lookAt(light->lightPos, light->lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	depthViewProjectionMatrix.push_back(projectionMatrix * glm::lookAt(light->lightPos, light->lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	depthViewProjectionMatrix.push_back(projectionMatrix * glm::lookAt(light->lightPos, light->lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	depthViewProjectionMatrix.push_back(projectionMatrix * glm::lookAt(light->lightPos, light->lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	depthViewProjectionMatrix.push_back(projectionMatrix * glm::lookAt(light->lightPos, light->lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	//projection
	float size = GridWorldSize_;
	projectionMatrix = glm::ortho(-size * 0.5f, size * 0.5f, -size * 0.5f, size * 0.5f, size * 0.5f, size * 1.5f);
	projectionX = projectionMatrix * glm::lookAt(glm::vec3(size, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	projectionY = projectionMatrix * glm::lookAt(glm::vec3(0, size, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
	projectionZ = projectionMatrix * glm::lookAt(glm::vec3(0, 0, size), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	//FBO unbound
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//render depth texture and voxel texture
	CalcDepthTexture();
	CalcVoxelTexture();

	return true;
}

void VCTRenderer::render(float deltaTime)
{
	//updateKeyPress();
	
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GLAD_GL_NV_conservative_raster);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scrWidth, scrHeight);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable shader
	tracingShader->use();

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);

	tracingShader->setVec3("CameraPosition", camera->Position);
	//TODO: modify LightDirection to implement point light
	tracingShader->setVec3("LightDirection", light->lightPos);
	tracingShader->setFloat("VoxelGridWorldSize", GridWorldSize_);
	tracingShader->setInt("VoxelDimensions", voxelDimensions_);
	//tracingShader->setFloat("ambientFactor", ambientFactor_);
	tracingShader->setInt("shadowMapRes", shadowMapRes);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	tracingShader->setInt("ShadowMap", 5);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D, voxelTexture);
	tracingShader->setInt("VoxelTexture", 6);

	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));
	tracingShader->setMat4("ViewMatrix", viewMatrix);
	tracingShader->setMat4("ModelMatrix", modelMatrix);
	tracingShader->setMat4("ModelViewMatrix", viewMatrix * modelMatrix);
	tracingShader->setMat4("ProjectionMatrix", projectionMatrix);
	tracingShader->setFloat("far_plane", light_far_plane);
	tracingShader->setVec3("LightPos", light->lightPos);

	float coneTracingFactor = input->isConeTracing ? 1.f : 0.f;
	tracingShader->setFloat("coneTracingFactor", coneTracingFactor);
	float diffuseFactor = input->diffuse ? 1.f : 0.f;
	tracingShader->setFloat("diffuseFactor", diffuseFactor);
	float specularFactor = input->specular ? 1.f : 0.f;
	tracingShader->setFloat("specularFactor", specularFactor);
	float ambientFactor = input->ambient ? ambientFactor_ : 0.f;
	tracingShader->setFloat("ambientFactor", ambientFactor);

	model->Draw(*tracingShader);

	drawUI();
}

void VCTRenderer::depth_visualize(float deltaTime) {
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GLAD_GL_NV_conservative_raster);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, scrWidth, scrHeight);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	depthvisualizeShader->use();
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	depthvisualizeShader->setInt("ShadowMap", 5);

	depthvisualizeShader->setMat4("ModelMatrix", modelMatrix);
	depthvisualizeShader->setMat4("ModelViewMatrix", viewMatrix * modelMatrix);
	depthvisualizeShader->setMat4("ProjectionMatrix", projectionMatrix);
	depthvisualizeShader->setFloat("far_plane", light_far_plane);
	depthvisualizeShader->setVec3("lightPos", light->lightPos);

	model->Draw(*depthvisualizeShader);
}

void VCTRenderer::voxel_visualize(float deltaTime) {

	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glDisable(GLAD_GL_NV_conservative_raster);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//prepare matrices
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));
	
	//enable face culling and depth test
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	//render 3D Texture
	voxelvisualizeShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_3D, voxelTexture);
	voxelvisualizeShader->setInt("texture3D", 6);

	voxelvisualizeShader->setVec3("cameraPosition", camera->Position);

	voxelvisualizeShader->setMat4("M", modelMatrix);
	voxelvisualizeShader->setMat4("V", viewMatrix);
	voxelvisualizeShader->setMat4("P", projectionMatrix);

	glViewport(0, 0, scrWidth, scrHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model->Draw(*voxelvisualizeShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	//FBO unbound
	glViewport(0, 0, scrWidth, scrHeight);
}

void VCTRenderer::CalcDepthTexture() {
	//enable face culling and depth test in order to accelerate rendering
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFramebuffer);
	glViewport(0, 0, shadowMapRes, shadowMapRes);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Write Depth information into depthFramebuffer
	shadowShader->use();
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));
	shadowShader->setMat4("ModelMatrix", modelMatrix);
	for(int i = 0; i < 6; ++i)
		shadowShader->setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), depthViewProjectionMatrix[i]);
	shadowShader->setVec3("lightPos", light->lightPos);
	shadowShader->setFloat("far_plane", light_far_plane);
	model->Draw(*shadowShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	//FBO unbound
	glViewport(0, 0, scrWidth, scrHeight);
}

void VCTRenderer::CalcVoxelTexture() {
	//disable face culling and depth test to render all triangle faces
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GLAD_GL_NV_conservative_raster); //enable hardware conservative rasterization, supported from Maxwell architecture on NVIDIA GPUs
	
	//init
	glViewport(0, 0, voxelDimensions_, voxelDimensions_);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable voxelization shader
	voxelizationShader->use();

	//bind Projection matrixs
	voxelizationShader->setMat4("ProjectionX", projectionX);
	voxelizationShader->setMat4("ProjectionY", projectionY);
	voxelizationShader->setMat4("ProjectionZ", projectionZ);

	//bind Depth Texture
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	voxelizationShader->setInt("ShadowMap", 5);

	//set voxel resolution
	voxelizationShader->setInt("VoxelDimensions", voxelDimensions_);
	//bind voxelTexture
	glBindImageTexture(6, voxelTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
	voxelizationShader->setInt("VoxelTexture", 6);

	//model matrix
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));
	
	//Bind matrices
	voxelizationShader->setMat4("ModelMatrix", modelMatrix);
	voxelizationShader->setFloat("far_plane", light_far_plane);
	voxelizationShader->setVec3("lightPos", light->lightPos);
	voxelizationShader->setInt("shadowMapRes", shadowMapRes);

	//render voxel texture
	model->Draw(*voxelizationShader);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D, voxelTexture);
	glGenerateMipmap(GL_TEXTURE_3D);
	glViewport(0, 0, scrWidth, scrHeight);
}

void VCTRenderer::initUI(){
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 450");
}

void VCTRenderer::drawUI(){
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	//content
	ImGui::Begin("Renderer");
	ImGui::Checkbox("cone tracing(Z)", &(input->isConeTracing));
	ImGui::Checkbox("diffuse(X)", &(input->diffuse));
	ImGui::Checkbox("specular(C)", &(input->specular));
	ImGui::Checkbox("ambient(V)", &(input->ambient));
	ImGui::End();

	//rendering
	ImGui::Render();

	//draw
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}