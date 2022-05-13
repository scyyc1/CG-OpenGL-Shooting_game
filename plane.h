#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "util.h"

class plane {
public:
	// Use a point in 3D and a norm to define a plane
	glm::vec3 position;
	glm::vec3 normal;
	// The model matrix
	glm::mat4 model;
	unsigned int VAO, VBO;

	plane() : position(DEFAULT_POSITION), normal(-DEFAULT_DIRECTION){
		calculateModelMatrix();
	}
	~plane(){}

	plane(glm::vec3 p) : position(p), normal(-DEFAULT_DIRECTION) {
		calculateModelMatrix();
	}

	plane(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {
		calculateModelMatrix();
	}

	// specify the intersect point of the plane and the camera
	glm::vec3 hitPosition(Camera camera) {
		float up = glm::dot(position - camera.Position, normal);
		float down = glm::dot(camera.Front, normal);
		float time = (float)up / (float)down;

		float x_hit = camera.Position.x + time * camera.Front.x;
		float y_hit = camera.Position.y + time * camera.Front.y;
		float z_hit = camera.Position.z + time * camera.Front.z;

		return glm::vec3(x_hit, y_hit, z_hit);
	}

	void calculateModelMatrix() {
		model = glm::mat4(1.f);
		model = glm::translate(model, position);

		glm::mat4 M_normal = getRotationMatrix(normal, -DEFAULT_DIRECTION);
		glm::mat4 M_tangent = glm::transpose(glm::inverse(M_normal));

		model = model * M_tangent;
	}
};
