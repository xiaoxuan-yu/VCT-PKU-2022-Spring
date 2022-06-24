#include "VCTRenderer.h"
VCTRenderer::VCTRenderer(GLFWWindow* aWindow)
{
	window = aWindow->getGLFWwindow();
	std::cout << "Creating VCT Render" << std::endl;
	scrWidth = aWindow->getWidth();
	scrHeight = aWindow->getHeight();
}

VCTRenderer::~VCTRenderer()
{
	delete model;
}

bool VCTRenderer::init(Camera *mCamera) {
	std::cout << "Initializing VCT Render" << std::endl;
	//imgui init
	std::cout << "Loading UI ..." << std::endl;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 450");

	camera = mCamera;

	// load Shaders
	tracingShader = new Shader("shader/naiveShader.vs", "shader/naiveShader.fs");
	voxelizationShader = new Shader("shader/voxelization.vs", "shader/voxelization.gs", "shader/voxelization.fs");

	//load model
	std::cout << "Loading models..." << std::endl;
	model = new Model("data/sponza.obj");
	std::cout << "Model loaded." << std::endl;

	glEnable(GL_TEXTURE_3D);//enable 3D texture
	glBindTexture(GL_TEXTURE_3D, voxelTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//init voxelTexture
	int numVoxels = voxelDimensions_ * voxelDimensions_ * voxelDimensions_;
	GLubyte* data = new GLubyte[numVoxels * 4];
	memset(data, 0, numVoxels * 4);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, voxelDimensions_, voxelDimensions_, voxelDimensions_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	delete[] data;	//release memory

	return true;
}

void VCTRenderer::update(float deltaTime)
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

	tracingShader->setMat4("projection", projectionMatrix);
	tracingShader->setMat4("view", viewMatrix);

	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));
	tracingShader->setMat4("model", modelMatrix);

	model->Draw(*tracingShader);
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
	//set voxel resolution
	voxelizationShader->setInt("VoxelDimensions", voxelDimensions_);
	//bind voxelTexture
	glBindImageTexture(6, voxelTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
	voxelizationShader->setInt("VoxelTexture", 6);

	//model matrix
	glm::mat4 modelMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)), glm::vec3(0.0f, 0.0f, 0.0f));

}
