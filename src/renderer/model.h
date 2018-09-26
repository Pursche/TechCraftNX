#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

struct CModel
{
	std::string name;
	std::string path;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	static CModel* LoadFromOBJ(std::string name, std::string path);
};