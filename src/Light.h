#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	glm::vec4 lightPos = glm::vec4(0.0f);
	glm::vec4 lightColor = glm::vec4(0.0f);
	void Light::setPos(glm::vec3 Pos)
	{
		lightPos = glm::vec4(Pos, 1);
	}
	void Light::setColor(glm::vec3 Color)
	{
		lightColor = glm::vec4(Color, 1);
	}
};


