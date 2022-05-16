#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "texture.h"
#include "shader.h"
#include "circle2D.h"
#include "plane.h"

float vert[] = {
	// pos						// col				// normal			// uv
	0.5f,  0.5f, 0.f,			1.f,1.f,1.f,		0.f,0.f,1.f,		1.f, 1.f,
	-0.5f,-0.5f, 0.f,			1.f,1.f,1.f,		0.f,0.f,1.f,		0.f, 0.f,
	-0.5f, 0.5f, 0.f,			1.f,1.f,1.f,		0.f,0.f,1.f,		0.f, 1.f,
							
	0.5f,  0.5f, 0.f,			1.f,1.f,1.f,		0.f,0.f,1.f,		1.f, 1.f,
	-0.5f,-0.5f, 0.f,			1.f,1.f,1.f,		0.f,0.f,1.f,		0.f, 0.f,
	0.5f, -0.5f, 0.f,			1.f,1.f,1.f,		0.f,0.f,1.f,		1.f, 0.f,
};


class target : public plane {
public:
	glm::vec3 Right;
	glm::vec3 Up;
	unsigned int texture;
	vector<circle2D> bulletHoles;

	target() : plane() {
		Right = glm::normalize(glm::cross(normal, WorldUp)); 
		Up = glm::normalize(glm::cross(Right, normal));
		texture = 0;
		setUp();
	}

	target(glm::vec3 p) : plane(p) {
		Right = glm::normalize(glm::cross(normal, WorldUp));
		Up = glm::normalize(glm::cross(Right, normal));
		texture = 0;
		setUp();
	}

	target(glm::vec3 p, glm::vec3 n) : plane(p, n) {
		Right = glm::normalize(glm::cross(normal, WorldUp));
		Up = glm::normalize(glm::cross(Right, normal));
		texture = 0;
		setUp();
	}

	void setUp() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		this->texture = setup_texture("assets/target/target.bmp");
	}

	void draw(Shader shader, Camera camera) {
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		shader.setBool("material.isDiffuseMap", 1);
		glUniform1i(glGetUniformLocation(shader.ID, ("material.texture_diffuse")), 0);
		//glUniform1i(glGetUniformLocation(shader.ID, "isTex"), 1);
		calculateModelMatrix();
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		for (auto hole : bulletHoles) {
			hole.draw(shader);
		}
	}

	bool isHit(Camera camera) {
		glm::vec3 hitPos = hitPosition(camera);

		// From the model postion back to the origin
		glm::mat4 model_inverse = glm::inverse(model);
		glm::vec3 newPos = glm::vec3(model_inverse * glm::vec4(hitPos, 1.f));

		if (newPos.x < 0.5f && newPos.x > -0.5f) {
			if (newPos.y < 0.5f && newPos.y > -0.5f) {
				return true;
			}
		}
		return false;
	}

	void updateBulletHoles(Camera camera) {
		glm::vec3 hitPos = hitPosition(camera);

		if (isHit(camera)) {
			if (bulletHoles.size() == BULLET_HOLE_NUM)
				bulletHoles.erase(bulletHoles.begin());

			glm::vec3 drawPos = glm::vec3(hitPos.x + 0.01f * normal.x, 
				hitPos.y + 0.01f * normal.y, hitPos.z + 0.01f * normal.z);
			bulletHoles.push_back(circle2D(drawPos, normal, glm::vec3(1.f, 0.f, 0.f), 0.03f));
		}
	}

	void updatePosition(glm::vec3 p) {
		glm::vec3 delta = p - this->position;
		this->position = p;

		// Update the position of bullet holes
		for (int i = 0; i < bulletHoles.size();i++) {
			bulletHoles[i].updatePosition(bulletHoles[i].position + delta);
		}
	}
 };