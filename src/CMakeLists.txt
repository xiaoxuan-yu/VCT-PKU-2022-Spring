# CMakeList.txt: ParticleSystem �� CMake ��Ŀ���ڴ˴�����Դ���벢����
# ��Ŀ�ض����߼���
#
cmake_minimum_required (VERSION 3.8)

# ��Դ�������ӵ�����Ŀ�Ŀ�ִ���ļ���
add_executable (VCT "main.cpp" "Camera.h" "GLFWInput.h" "Light.h" "Mesh.h" "Model.h" "Shader.h" "Timer.h" "Window.h" "VCTRenderer.h" "VCTRenderer.cpp")
target_link_libraries(VCT PRIVATE assimp::assimp glm::glm stb glfw glad imgui)
target_include_directories(VCT PRIVATE ../third_party/glad/include ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(VCT PRIVATE cxx_std_17)

# TODO: ������Ҫ�������Ӳ��Բ���װĿ�ꡣ
