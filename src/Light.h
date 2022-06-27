#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	glm::vec3 lightPos = glm::vec3(1.0f, 4.0f, 1.0f);
	glm::vec3 lightColor = glm::vec3(0.0f);
	void Light::setPos(glm::vec3 Pos)
	{
		lightPos = Pos;
	}
	void Light::setColor(glm::vec3 Color)
	{
		lightColor = Color;
	}
};


