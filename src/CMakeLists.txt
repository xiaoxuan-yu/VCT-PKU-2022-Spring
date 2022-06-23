# CMakeList.txt: ParticleSystem 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源代码添加到此项目的可执行文件。
add_executable (VCT "main.cpp" "Camera.h" "GLFWInput.h" "Light.h" "Mesh.h" "Model.h" "Shader.h" "Timer.h" "Window.h" "VCTRenderer.h" "VCTRenderer.cpp")
target_link_libraries(VCT PRIVATE assimp::assimp glm::glm stb glfw glad imgui)
target_include_directories(VCT PRIVATE ../third_party/glad/include ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(VCT PRIVATE cxx_std_17)

# TODO: 如有需要，请添加测试并安装目标。
