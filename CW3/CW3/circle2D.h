#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <iostream>

#include "util.h"
#include "shader.h"
#include "plane.h"

class circle2D : public plane {
public:
	glm::vec3 colour;
	float radius;
	float* vertices;
	unsigned int VAO, VBO;

	circle2D() : plane() {}

	circle2D(glm::vec3 p, glm::vec3 colour, float radius) : plane(p) {
		this->colour = colour;
		this->radius = radius;

		CreateCircle(NUM_SEG, radius, colour);
		setUpCircle2D();
	}

	circle2D(glm::vec3 p, glm::vec3 n, glm::vec3 colour, float radius) : plane(p, n) {
		this->colour = colour;
		this->radius = radius;

		CreateCircle(NUM_SEG, radius, colour);
		setUpCircle2D();
	}

	void CreateCircle(int num_segments, float radius, glm::vec3 col)
	{
		int n_verts = num_segments * 3;
		int n_atts = n_verts * 3;
		vertices = (float*)malloc(sizeof(float) * (n_atts) * 2);

		float angle_offset = glm::radians(360.f) / num_segments;
		float current_angle = 0.f;

		for (int sc = 0; sc < num_segments; sc++)
		{
			// First pos
			vertices[sc * 3 * 6 + 0] = 0.f;
			vertices[sc * 3 * 6 + 1] = 0.f;
			vertices[sc * 3 * 6 + 2] = 0.f;
			// First col
			vertices[sc * 3 * 6 + 3] = col.x;
			vertices[sc * 3 * 6 + 4] = col.y;
			vertices[sc * 3 * 6 + 5] = col.z;

			// Second vert
			vertices[sc * 3 * 6 + 6] = sin(current_angle) * radius;
			vertices[sc * 3 * 6 + 7] = cos(current_angle) * radius;
			vertices[sc * 3 * 6 + 8] = 0.f;
			vertices[sc * 3 * 6 + 9] = col.x;
			vertices[sc * 3 * 6 + 10] = col.y;
			vertices[sc * 3 * 6 + 11] = col.z;

			// Third vert
			vertices[sc * 3 * 6 + 12] = sin(current_angle + angle_offset) * radius;
			vertices[sc * 3 * 6 + 13] = cos(current_angle + angle_offset) * radius;
			vertices[sc * 3 * 6 + 14] = 0.f;
			vertices[sc * 3 * 6 + 15] = col.x;
			vertices[sc * 3 * 6 + 16] = col.y;
			vertices[sc * 3 * 6 + 17] = col.z;

			current_angle += angle_offset;
		}
	}

	void setUpCircle2D() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUM_SEG * 3 * 6, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw(Shader shader) {
		shader.use();
		glBindVertexArray(VAO);
		glUniform1i(glGetUniformLocation(shader.ID, "isTex"), 0);
		calculateModelMatrix();
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, NUM_SEG * 3);
		glBindVertexArray(0);
	}

	void updatePosition(glm::vec3 p) {
		this->position = p;
	}
};