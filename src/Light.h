#pragma once
#include <glm/glm.hpp>

class Light
{
public:
	int light_type = 0;
	glm::vec3 lightColor = glm::vec3(0.0f);
	glm::vec3 lightPos = glm::vec3(0.0f, 8.0f, 0.0f);
	void setColor(glm::vec3 Color)
	{
		lightColor = Color;
	}
	void setPos(glm::vec3 Pos)
	{
		lightPos = Pos;
	}
};


