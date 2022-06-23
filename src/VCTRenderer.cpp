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
	stdShader = new Shader("shader/naiveShader.vs", "shader/naiveShader.fs");

	//load model
	std::cout << "Loading models..." << std::endl;
	model = new Model("data/sponza.obj");
	std::cout << "Model loaded." << std::endl;

	return true;
}

void VCTRenderer::update(float deltaTime)
{
	//updateKeyPress();
	
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//enable shader
	stdShader->use();

	glm::mat4 viewMatrix = camera->GetViewMatrix();
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);

	stdShader->setMat4("projection", projectionMatrix);
	stdShader->setMat4("view", viewMatrix);

	model->Draw(*stdShader);
}