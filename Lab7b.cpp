
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>

#include "camera.h"
#include "shader.h"
#include "window.h"
#include "texture.h"
#include "Model.h"
#include "mesh.h"
#include "target.h"
#include "circle2D.h"
#include "sightBead.h"


// Mouse event
bool mouseLeft_isClick = false;
bool mouseRight_isClick = false;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float myFPStime = 0.f;

void displayGun(Shader loadOBJShader, Model model);

float vertices[] =
{
	//pos					//col			
	-0.5f, -0.5f, -0.5f,  	0.f, 0.f, -1.f,
	0.5f, -0.5f, -0.5f,  	0.f, 0.f, -1.f, 
	0.5f,  0.5f, -0.5f,  	0.f, 0.f, -1.f,
	0.5f,  0.5f, -0.5f,  	0.f, 0.f, -1.f,
	-0.5f,  0.5f, -0.5f,  	0.f, 0.f, -1.f,
	-0.5f, -0.5f, -0.5f,  	0.f, 0.f, -1.f,

	-0.5f, -0.5f,  0.5f,  	0.f, 0.f, 1.f,
	0.5f, -0.5f,  0.5f,  	0.f, 0.f, 1.f,
	0.5f,  0.5f,  0.5f,  	0.f, 0.f, 1.f,
	0.5f,  0.5f,  0.5f,  	0.f, 0.f, 1.f,
	-0.5f,  0.5f,  0.5f,  	0.f, 0.f, 1.f,
	-0.5f, -0.5f,  0.5f,  	0.f, 0.f, 1.f,

	-0.5f,  0.5f,  0.5f,  	-1.f, 0.f, 0.f,
	-0.5f,  0.5f, -0.5f,  	-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	-1.f, 0.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	-1.f, 0.f, 0.f,
	-0.5f, -0.5f,  0.5f,  	-1.f, 0.f, 0.f,
	-0.5f,  0.5f,  0.5f,  	-1.f, 0.f, 0.f,

	0.5f,  0.5f,  0.5f,  	1.f, 0.f, 0.f,
	0.5f,  0.5f, -0.5f,  	1.f, 0.f, 0.f,
	0.5f, -0.5f, -0.5f, 	1.f, 0.f, 0.f,
	0.5f, -0.5f, -0.5f,  	1.f, 0.f, 0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 0.f, 0.f,
	0.5f,  0.5f,  0.5f,  	1.f, 0.f, 0.f,

	-0.5f, -0.5f, -0.5f,  	0.f, -1.f, 0.f,
	0.5f, -0.5f, -0.5f,  	0.f, -1.f, 0.f,
	0.5f, -0.5f,  0.5f,  	0.f, -1.f, 0.f,
	0.5f, -0.5f,  0.5f,  	0.f, -1.f, 0.f,
	-0.5f, -0.5f,  0.5f,  	0.f, -1.f, 0.f,
	-0.5f, -0.5f, -0.5f,  	0.f, -1.f, 0.f,

	-0.5f,  0.5f, -0.5f,  	0.f, 1.f, 0.f,
	0.5f,  0.5f, -0.5f,  	0.f, 1.f, 0.f,
	0.5f,  0.5f,  0.5f,  	0.f, 1.f, 0.f,
	0.5f,  0.5f,  0.5f,  	0.f, 1.f, 0.f,
	-0.5f,  0.5f,  0.5f,  	0.f, 1.f, 0.f,
	-0.5f,  0.5f, -0.5f, 	0.f, 1.f, 0.f,
};

float floor_celling[] = {
	// pos						// col					// texture
	-3.5f, -1.f,  3.5f, 		0.5f,0.5f,0.5f,			
	 3.5f, -1.f, -3.5f, 		0.5f,0.5f,0.5f,
	 3.5f, -1.f,  3.5f,  		0.5f,0.5f,0.5f,

	-3.5f, -1.f,  3.5f, 		0.5f,0.5f,0.5f,
	 3.5f, -1.f, -3.5f, 		0.5f,0.5f,0.5f,
	-3.5f, -1.f, -3.5f, 		0.5f,0.5f,0.5f,
};

float walls[] = {
	-3.5f, -1.f,  3.5f,			0.88f,0.88f,0.88f,		0.f, 0.f,
	-3.5f,  3.f, -3.5f,			0.88f,0.88f,0.88f,		4.f, 7.f,
	-3.5f, -1.f, -3.5f,			0.88f,0.88f,0.88f,		0.f, 7.f,

	-3.5f, -1.f,  3.5f,			0.88f,0.88f,0.88f,		0.f, 0.f,
	-3.5f,  3.f, -3.5f,			0.88f,0.88f,0.88f,		4.f, 7.f,
	-3.5f,  3.f,  3.5f,			0.88f,0.88f,0.88f,		4.f, 0.f,
};

float skyBox[] = {
	//pos				
	-5.f, -5.f, -5.f,
	 5.f, -5.f, -5.f,
	 5.f,  5.f, -5.f,
	 5.f,  5.f, -5.f,
	-5.f,  5.f, -5.f,
	-5.f, -5.f, -5.f,
	 			 
	-5.f, -5.f,  5.f,
	 5.f, -5.f,  5.f,
	 5.f,  5.f,  5.f,
	 5.f,  5.f,  5.f,
	-5.f,  5.f,  5.f,
	-5.f, -5.f,  5.f,
	 	 		 
	-5.f,  5.f,  5.f,
	-5.f,  5.f, -5.f,
	-5.f, -5.f, -5.f,
	-5.f, -5.f, -5.f,
	-5.f, -5.f,  5.f,
	-5.f,  5.f,  5.f,
	 	 	 	 
	 5.f,  5.f,  5.f,
	 5.f,  5.f, -5.f,
	 5.f, -5.f, -5.f,
	 5.f, -5.f, -5.f,
	 5.f, -5.f,  5.f,
	 5.f,  5.f,  5.f,
	 		 	 
	-5.f, -5.f, -5.f,
	 5.f, -5.f, -5.f,
	 5.f, -5.f,  5.f,
	 5.f, -5.f,  5.f,
	-5.f, -5.f,  5.f,
	-5.f, -5.f, -5.f,
	 	   	 
	-5.f,  5.f, -5.f,
	 5.f,  5.f, -5.f,
	 5.f,  5.f,  5.f,
	 5.f,  5.f,  5.f,
	-5.f,  5.f,  5.f,
	-5.f,  5.f, -5.f,
};


Camera camera;

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)			camera.moveCamera(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)			camera.moveCamera(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)			camera.moveCamera(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)			camera.moveCamera(RIGHT, deltaTime);
}

bool firstMouse = true;
float prevMouseX;
float prevMouseY;

void processScoll(GLFWwindow* window, double x, double y)
{
	camera.scrollCamera(y);
}

void processMouse(GLFWwindow* window, double x, double y)
{
	if (firstMouse)
	{
		prevMouseX = x;
		prevMouseY = y;
		firstMouse = false;

		//std::cout << prevMouseX << " " << prevMouseY << std::endl;
	}

	float dX = x - prevMouseX;
	float dY = y - prevMouseY;

	prevMouseX = x;
	prevMouseY = y;

	camera.orientCamera(dX, dY);
}

void processMouseClick(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)		mouseLeft_isClick = true;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)		mouseRight_isClick = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)		mouseLeft_isClick = false;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)	mouseRight_isClick = false;
	
}

int main(int argc, char** argv)
{
	GLFWwindow* window = createWindow(windowWidth, windowHeight, "Fly Through Camera");
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, processMouse);
	glfwSetScrollCallback(window, processScoll);
	glfwSetMouseButtonCallback(window, processMouseClick);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Shader shader = Shader("mvp.vert", "col.frag");
	Shader skyboxShader = Shader("cubebox.vert", "cubebox.frag");
	Shader reflectShader = Shader("reflect.vert", "reflect.frag");
	Shader loadOBJShader = Shader("loadOBJ.vert", "loadOBJ.frag");
	Shader textureShader = Shader("texture.vert", "texture.frag");

	GLuint texture = setup_texture("assets/target/target.bmp");


	Model rock = Model("assets/rock/rock.obj", "rock");
	Model cube = Model("assets/cube/cube.obj", "cube");
	Model gun_ak47 = Model("assets/gun/ak/Kalashnikov.obj", "gun_ak47");
	
	//Model gun_pistol = Model("assets/gun/test/qiangxie7.obj", "gun_pistol");
	
	target target1 = target(glm::vec3(0.f, 0.f, -3.5f));
	target target2 = target(glm::vec3(-3.5f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
	target target3 = target(glm::vec3(3.5f, 2.f, 0.f), glm::vec3(-1.f, -1.f, 0.f));
	target target4 = target(glm::vec3(2.5f, 2.f, -2.f), glm::vec3(0.f, -1.f, 0.f));
	sightBead sight = sightBead();

	const char* files[11] = {
		"assets/skybox/right.bmp",
		"assets/skybox/left.bmp",
		"assets/skybox/top.bmp",
		"assets/skybox/bottom.bmp",
		"assets/skybox/back.bmp",
		"assets/skybox/front.bmp",
	};
	GLuint cubemapTexture = setup_cubebox(files);
	GLuint t = setup_texture("assets/target/target.bmp");

	unsigned int VAO[4];
	glGenVertexArrays(4, VAO);
	unsigned int VBO[4];
	glGenBuffers(4, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_celling), floor_celling, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(walls), walls, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//glBindVertexArray(VAO[3]);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	/*******************skybox***********************/
	unsigned int skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	unsigned int skyboxVBO;
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyBox), skyBox, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	int count = 0;

	while (!glfwWindowShouldClose(window))
	{
		// Timing
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboard(window);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Sky box
		glDepthMask(GL_FALSE);
		skyboxShader.use();
		glm::mat4 view_sky = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view_sky);
		glm::mat4 projection_sky = glm::mat4(1.f);
		projection_sky = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 1.f, 10.f);
		skyboxShader.setMat4("projection", projection_sky);
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		
		/********************Obj*******************/
		//reflectShader.use();
		//glm::vec3 cube_pos = glm::vec3(0.0f, 2.0f, 0.0f);
		//// glUniform1i(glGetUniformLocation(shaderProgram.ID, "isTex"), 0);

		glm::mat4 model = glm::mat4(1.f);
		//model = glm::translate(model, cube_pos);
		//glUniformMatrix4fv(glGetUniformLocation(reflectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		//glUniformMatrix4fv(glGetUniformLocation(reflectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 1.f, 10.f);
		//glUniformMatrix4fv(glGetUniformLocation(reflectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		//glUniform3fv(glGetUniformLocation(reflectShader.ID, "cameraPos"), 1, &camera.Position.x);

		//glBindVertexArray(VAO[0]);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		///*************rock******************/
		loadOBJShader.use();

		loadOBJShader.setMat4("projection", projection);
		loadOBJShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.f, -1.f, -2.f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		loadOBJShader.setMat4("model", model);
		rock.Draw(loadOBJShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, -1.f, -2.f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		loadOBJShader.setMat4("model", model);
		rock.Draw(loadOBJShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, -1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		loadOBJShader.setMat4("model", model);
		rock.Draw(loadOBJShader);

		///*************cube******************/
		//loadOBJShader.use();

		//loadOBJShader.setMat4("projection", projection);
		//loadOBJShader.setMat4("view", view);

		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f)); 
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//loadOBJShader.setMat4("model", model);
		//cube.Draw(loadOBJShader);

		/*************gun******************/
		displayGun(loadOBJShader, gun_ak47);

		/********************celling & floor*******************/
		shader.use();
		glUniform1i(glGetUniformLocation(shader.ID, "isTex"), 0);
		model = glm::mat4(1.f);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/*model = glm::mat4(1.f);
		model = glm::rotate(model, glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
		model = glm::translate(model, glm::vec3(0.f, -2.f, 0.f));
		glUniform1i(glGetUniformLocation(shader.ID, "isTex"), 0);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO[1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);*/

		// test
		target1.draw(shader, camera);
		target2.draw(shader, camera);
		target3.draw(shader, camera);
		target4.draw(shader, camera);

		if (mouseLeft_isClick == true) {
			if (currentFrame - myFPStime > SHOOTING_SPEED) {
				myFPStime = currentFrame;
				count++;
				target1.updateBulletHoles(camera);
				target2.updateBulletHoles(camera);
				target3.updateBulletHoles(camera);
				target4.updateBulletHoles(camera);
			}
		}
		
		// Draw at the end, close depth test
		// Ensure it will always show on the screen
		sight.draw(shader, camera);
			
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void displayGun(Shader loadOBJShader, Model model) {
	loadOBJShader.use();
	//loadOBJShader.setVec3("light.position", glm::vec3(0.0f, 5.0f, 0.0f));
	loadOBJShader.setVec3("lightPos", glm::vec3(-5.0f, 0.0f, 0.0f));

	glm::mat4 model_gun = glm::mat4(1.0f);
	glm::vec3 right_offset = glm::vec3(camera.Right.x * 0.40f, camera.Right.y * 0.40f, camera.Right.z * 0.40f);
	glm::vec3 front_offset = glm::vec3(camera.Front.x * 1.2f, camera.Front.y * 1.2f, camera.Front.z * 1.2f);
	glm::vec3 down_offset = glm::vec3(camera.Up.x * -0.4f, camera.Up.y * -0.4f, camera.Up.z * -0.4f);
	model_gun = glm::translate(model_gun, camera.Position + front_offset + right_offset + down_offset);
	model_gun = glm::rotate(model_gun, glm::radians(15.f), camera.Up);
	model_gun = glm::rotate(model_gun, glm::radians(20.f), camera.Right);
	model_gun = glm::rotate(model_gun, glm::radians(-camera.Yaw), glm::vec3(0.f, 1.f, 0.f));
	model_gun = glm::rotate(model_gun, glm::radians(camera.Pitch), glm::vec3(0.f, 0.f, 1.f));
	model_gun = glm::scale(model_gun, glm::vec3(0.015f, 0.015f, 0.015f));
	loadOBJShader.setMat4("model", model_gun);

	glm::mat4 view_gun = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
	loadOBJShader.setMat4("view", view_gun);
	glm::mat4 projection_gun = glm::mat4(1.f);
	projection_gun = glm::perspective(glm::radians(40.f), (float)800 / (float)600, 1.f, 10.f);
	loadOBJShader.setMat4("projection", projection_gun);

	model.Draw(loadOBJShader);
}
