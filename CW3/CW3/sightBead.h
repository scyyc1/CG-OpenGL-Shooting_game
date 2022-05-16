#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <iostream>

#include "util.h"
#include "shader.h"
#include "camera.h"

float sight[] = {
	 0.7f, 0.05f, 0.f,		1.f, 0.f, 0.f,		0.f,0.f,1.f,
	-0.7f,-0.05f, 0.f,		1.f, 0.f, 0.f,		0.f,0.f,1.f,
	 0.7f,-0.05f, 0.f,		1.f, 0.f, 0.f,		0.f,0.f,1.f,
					
	 0.7f, 0.05f, 0.f,		1.f, 0.f, 0.f,		0.f,0.f,1.f,
	-0.7f,-0.05f, 0.f,		1.f, 0.f, 0.f,		0.f,0.f,1.f,
	-0.7f, 0.05f, 0.f, 		1.f, 0.f, 0.f,		0.f,0.f,1.f,
};

class sightBead {
public:
	unsigned int VAO, VBO;

	sightBead() {
		setUpSightBead();
	}
	~sightBead() {}

	void setUpSightBead() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sight), sight, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void draw(Shader shader, Camera camera) {

		glDepthMask(GL_FALSE);
		shader.use();

		glm::mat4 view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(40.f), (float)800 / (float)600, 1.f, 10.f);
		
		// horizotal
		glUniform1i(glGetUniformLocation(shader.ID, "isTex"), 0);
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, camera.Position + glm::vec3(1.5f * camera.Front.x, 
			1.5f * camera.Front.y, 1.5f * camera.Front.z));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		model = glm::rotate(model, glm::radians(-camera.Yaw), glm::vec3(0.f, 1.f, 0.f));
		model = glm::rotate(model, glm::radians(-camera.Pitch), glm::vec3(1.f, 0.f, 0.f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// vertical
		glUniform1i(glGetUniformLocation(shader.ID, "isTex"), 0);
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
	}
};