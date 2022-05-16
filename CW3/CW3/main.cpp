
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
// User event
bool isWalking = false;
bool isFire = false;
bool isTorchUsed = false;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
// To define my FPS
float myFPStime = 0.f;
float lastFire = 0.f;
float PI = 3.14159265358979323846;

void displayGun(Shader shader, Model model, float shake, bool isWalking, float fire, bool isFire);
void displaySkybox(Shader shader, GLuint skyboxTexture, unsigned int VAO);
// The default setting of shader's uniform parameters
void setDefault(Shader shader);

// A cube
float vertices[] =
{
	//pos					//col				// Normal
	-0.5f, -0.5f, -0.5f,  	1.f, 0.f, 0.f,		0.f,0.f,-1.f,
	 0.5f, -0.5f, -0.5f,  	1.f, 0.f, 0.f,		0.f,0.f,-1.f,
	 0.5f,  0.5f, -0.5f,  	1.f, 0.f, 0.f,		0.f,0.f,-1.f,
	 0.5f,  0.5f, -0.5f,  	1.f, 0.f, 0.f,		0.f,0.f,-1.f,
	-0.5f,  0.5f, -0.5f,  	1.f, 0.f, 0.f,		0.f,0.f,-1.f,
	-0.5f, -0.5f, -0.5f,  	1.f, 0.f, 0.f,		0.f,0.f,-1.f,

	-0.5f, -0.5f,  0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,1.f,
	0.5f, -0.5f,  0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,1.f,
	0.5f,  0.5f,  0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,1.f,
	0.5f,  0.5f,  0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,1.f,
	-0.5f,  0.5f,  0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,1.f,
	-0.5f, -0.5f,  0.5f,  	0.f, 1.f, 0.f,		0.f,0.f,1.f,

	-0.5f,  0.5f,  0.5f,  	0.f, 0.f, 1.f,		-1.f,0.f,0.f,
	-0.5f,  0.5f, -0.5f,  	0.f, 0.f, 1.f,		-1.f,0.f,0.f,
	-0.5f, -0.5f, -0.5f,  	0.f, 0.f, 1.f,		-1.f,0.f,0.f,
	-0.5f, -0.5f, -0.5f,  	0.f, 0.f, 1.f,		-1.f,0.f,0.f,
	-0.5f, -0.5f,  0.5f,  	0.f, 0.f, 1.f,		-1.f,0.f,0.f,
	-0.5f,  0.5f,  0.5f,  	0.f, 0.f, 1.f,		-1.f,0.f,0.f,

	0.5f,  0.5f,  0.5f,  	1.f, 1.f, 0.f,		1.f,0.f,0.f,
	0.5f,  0.5f, -0.5f,  	1.f, 1.f, 0.f,		1.f,0.f,0.f,
	0.5f, -0.5f, -0.5f, 	1.f, 1.f, 0.f,		1.f,0.f,0.f,
	0.5f, -0.5f, -0.5f,  	1.f, 1.f, 0.f,		1.f,0.f,0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 1.f, 0.f,		1.f,0.f,0.f,
	0.5f,  0.5f,  0.5f,  	1.f, 1.f, 0.f,		1.f,0.f,0.f,

	-0.5f, -0.5f, -0.5f,  	1.f, 0.f, 1.f,		0.f,-1.f,0.f,
	0.5f, -0.5f, -0.5f,  	1.f, 0.f, 1.f,		0.f,-1.f,0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 0.f, 1.f,		0.f,-1.f,0.f,
	0.5f, -0.5f,  0.5f,  	1.f, 0.f, 1.f,		0.f,-1.f,0.f,
	-0.5f, -0.5f,  0.5f,  	1.f, 0.f, 1.f,		0.f,-1.f,0.f,
	-0.5f, -0.5f, -0.5f,  	1.f, 0.f, 1.f,		0.f,-1.f,0.f,

	-0.5f,  0.5f, -0.5f,  	0.f, 1.f, 1.f,		0.f,1.f,0.f,
	0.5f,  0.5f, -0.5f,  	0.f, 1.f, 1.f,		0.f,1.f,0.f,
	0.5f,  0.5f,  0.5f,  	0.f, 1.f, 1.f,		0.f,1.f,0.f,
	0.5f,  0.5f,  0.5f,  	0.f, 1.f, 1.f,		0.f,1.f,0.f,
	-0.5f,  0.5f,  0.5f,  	0.f, 1.f, 1.f,		0.f,1.f,0.f,
	-0.5f,  0.5f, -0.5f, 	0.f, 1.f, 1.f,		0.f,1.f,0.f,
};

float floor_celling[] = {
	// pos						// col					// normal			// texture
	-3.5f, -1.f,  3.5f, 		0.5f,0.5f,0.5f,			0.f,1.f,0.f,		0.f, 0.f,
	 3.5f, -1.f, -3.5f, 		0.5f,0.5f,0.5f,			0.f,1.f,0.f,		1.f, 1.f,
	 3.5f, -1.f,  3.5f,  		0.5f,0.5f,0.5f,			0.f,1.f,0.f,		1.f, 0.f,
																   
	-3.5f, -1.f,  3.5f, 		0.5f,0.5f,0.5f,			0.f,1.f,0.f,		0.f, 0.f,
	 3.5f, -1.f, -3.5f, 		0.5f,0.5f,0.5f,			0.f,1.f,0.f,		1.f, 1.f,
	-3.5f, -1.f, -3.5f, 		0.5f,0.5f,0.5f,			0.f,1.f,0.f,		0.f, 1.f,
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
glm::vec3 lightPos = glm::vec3(0.f, 3.f, 0.f);
glm::vec3 lightDir = glm::vec3(0.f, -1.f, 0.f);

void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)		
glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.moveCamera(FORWARD, deltaTime);
		isWalking = true;
	}			
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.moveCamera(BACKWARD, deltaTime);
		isWalking = true;
	}			
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.moveCamera(LEFT, deltaTime);
		isWalking = true;
	}			
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.moveCamera(RIGHT, deltaTime);
		isWalking = true;
	}			
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		isTorchUsed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		isTorchUsed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.MovementSpeed = MOVE_SPEED / 2;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE
		&& glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE 
		&& glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE
		&& glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE)
		isWalking = false;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
		camera.MovementSpeed = MOVE_SPEED;
	}
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
	}

	float dX = x - prevMouseX;
	float dY = y - prevMouseY;

	prevMouseX = x;
	prevMouseY = y;

	camera.orientCamera(dX, dY);
}

void processMouseClick(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseLeft_isClick = true;
		lastFire = static_cast<float>(glfwGetTime());
		isFire = true;
	}		
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

	// Shaders
	Shader simpleShader = Shader("mvp.vert", "col.frag");
	Shader skyboxShader = Shader("cubebox.vert", "cubebox.frag");
	Shader shader = Shader("shader.vert", "shader.frag");

	// textures - skybox and ground
	GLuint ground = setup_texture("assets/rock/rock.bmp");
	const char* files[11] = {
		"assets/skybox/right.bmp",
		"assets/skybox/left.bmp",
		"assets/skybox/top.bmp",
		"assets/skybox/bottom.bmp",
		"assets/skybox/back.bmp",
		"assets/skybox/front.bmp",
	};
	GLuint cubemapTexture = setup_cubebox(files);

	// models
	Model rock = Model("assets/rock/rock.obj", "rock");
	Model gun_ak47 = Model("assets/gun/ak/Kalashnikov.obj", "gun_ak47");
	Model lamp = Model("assets/Disc_125_wishnya/Disc_125_wishnya.obj", "lamp");
	
	target target1 = target(glm::vec3(0.f, 0.f, -3.5f));
	target target5 = target(glm::vec3(-2.f, 0.f, -3.5f));
	target target2 = target(glm::vec3(-3.5f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
	target target3 = target(glm::vec3(3.5f, 2.f, 0.f), glm::vec3(-1.f, -1.f, 0.f));
	target target4 = target(glm::vec3(2.5f, 2.f, -2.f), glm::vec3(0.f, -1.f, 0.f));
	sightBead sight = sightBead();

	float speed = 0.01f;

	unsigned int VAO[4];
	glGenVertexArrays(4, VAO);
	unsigned int VBO[4];
	glGenBuffers(4, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_celling), floor_celling, GL_STATIC_DRAW);
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

	
	// skybox
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

	while (!glfwWindowShouldClose(window))
	{
		lightDir = camera.Front;
		lightPos = camera.Position;

		// Timing
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// The frequency of gun animation
		float shake_frequency = sin((float)glfwGetTime() * 2 * PI);				// 1 second / a round
		float fire_frequency = abs(sin((float)glfwGetTime() * 10 * PI));		// 0.1 second / a round
		// Recover once the gun have shot
		if (currentFrame - lastFire > 0.1f)			isFire = false;

		processKeyboard(window);

		glClearColor(1.f, 1.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Sky box
		displaySkybox(skyboxShader, cubemapTexture, skyboxVAO);
		
		// cube
		shader.use();
		setDefault(shader);
		glm::vec3 cube_pos = glm::vec3(0.0f, 3.0f, 0.0f);

		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, cube_pos);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view = glm::mat4(1.f);
		view = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::mat4(1.f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 1.f, 10.f);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3fv(glGetUniformLocation(shader.ID, "cameraPos"), 1, &camera.Position.x);

		glBindVertexArray(VAO[0]);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Rocks
		shader.use();
		setDefault(shader);

		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-2.f, -1.f, -2.f));
		model = glm::rotate(model, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		shader.setMat4("model", model);
		rock.Draw(shader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.f, -1.f, 2.f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		shader.setMat4("model", model);
		rock.Draw(shader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.f, -1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		shader.setMat4("model", model);
		rock.Draw(shader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.f, -1.f, 0.f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		shader.setMat4("model", model);
		rock.Draw(shader);

		// Weapon
		displayGun(shader, gun_ak47, shake_frequency, isWalking, fire_frequency, isFire);

		// Floor
		shader.use();
		
		shader.setBool("material.isDiffuseMap", 1);
		glUniform1i(glGetUniformLocation(shader.ID, ("material.texture_diffuse")), 0);
		shader.setFloat("material.shininess", 2);
		model = glm::mat4(1.f);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(VAO[1]);
		glBindTexture(GL_TEXTURE_2D, ground);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw the targets
		shader.use();
		setDefault(shader);
		target1.draw(shader, camera);
		target5.draw(shader, camera);

		// Target 2 is set to be a moving target
		glm::vec3 newPos2 = target2.position +
			glm::vec3(target2.Right.x * speed, target2.Right.y * speed, target2.Right.z * speed);
		if (newPos2.z > 3.5f)
			speed = -speed;
		if (newPos2.z < -3.5f)
			speed = -speed;
		target2.updatePosition(newPos2);
		setDefault(shader);
		target2.draw(shader, camera);

		setDefault(shader);
		target3.draw(shader, camera);

		setDefault(shader);
		target4.draw(shader, camera);

		// Update the bulletholes if the player is shooting
		if (mouseLeft_isClick == true) {
			// The frequency of shooting is 0.1s
			// This means that 1s can shoot 10 times
			if (currentFrame - myFPStime > SHOOTING_SPEED) {
				myFPStime = currentFrame;
				target1.updateBulletHoles(camera);
				target2.updateBulletHoles(camera);
				target3.updateBulletHoles(camera);
				target4.updateBulletHoles(camera);
			}
		}

		// lamp
		glm::mat4 model_lamp = glm::mat4(1.0f);
		model_lamp = glm::translate(model_lamp, glm::vec3(0.f, 2.f, 0.f));
		model_lamp = glm::scale(model_lamp, glm::vec3(0.0025f, 0.0025f, 0.0025f));
		shader.setMat4("model", model_lamp);
		lamp.Draw(shader);
		
		// Draw at the end, close depth test
		// Ensure it will always show on the screen
		sight.draw(simpleShader, camera);
		
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

void displaySkybox(Shader shader, GLuint skyboxTexture, unsigned int VAO) {
	glDepthMask(GL_FALSE);
	shader.use();
	glm::mat4 view_sky = glm::mat4(glm::mat3(camera.GetViewMatrix()));
	glm::mat4 projection_sky = glm::mat4(1.f);
	projection_sky = glm::perspective(glm::radians(camera.Zoom), (float)800 / (float)600, 1.f, 10.f);
	shader.setMat4("view", view_sky);
	shader.setMat4("projection", projection_sky);
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
}

void displayGun(Shader shader, Model model, float shake, bool isWalking, float fire, bool isFire) {
	shader.use();
	setDefault(shader);
	//shader.setVec3("lightPos", glm::vec3(-5.f, 0.f, 0.f));

	glm::mat4 model_gun = glm::mat4(1.0f);
	glm::vec3 right_offset = glm::vec3(camera.Right.x * 0.40f, camera.Right.y * 0.40f, camera.Right.z * 0.40f);
	glm::vec3 front_offset = glm::vec3(camera.Front.x * 1.2f, camera.Front.y * 1.2f , camera.Front.z * 1.2f);
	glm::vec3 down_offset = glm::vec3(camera.Up.x * -0.4f, camera.Up.y * -0.4f, camera.Up.z * -0.4f);
	model_gun = glm::translate(model_gun, camera.Position + front_offset + right_offset + down_offset);
	model_gun = glm::rotate(model_gun, glm::radians(15.f), camera.Up);
	model_gun = glm::rotate(model_gun, glm::radians(20.f), camera.Right);
	model_gun = glm::rotate(model_gun, glm::radians(-camera.Yaw), glm::vec3(0.f, 1.f, 0.f));
	model_gun = glm::rotate(model_gun, glm::radians(camera.Pitch), glm::vec3(0.f, 0.f, 1.f));

	// Simulate holding a gun and walking
	if (isWalking) {
		model_gun = glm::translate(model_gun, glm::vec3(0.05f * shake, 0, 0));
	}

	// Simulate recoil force
	if (isFire) {
		model_gun = glm::rotate(model_gun, glm::radians(5.f) * fire, glm::vec3(0.f, 0.f, 1.f));
		model_gun = glm::translate(model_gun, glm::vec3(-0.05f * fire, 0, 0));
	}

	model_gun = glm::scale(model_gun, glm::vec3(0.015f, 0.015f, 0.015f));
	shader.setMat4("model", model_gun);

	glm::mat4 view_gun = glm::lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
	shader.setMat4("view", view_gun);
	glm::mat4 projection_gun = glm::mat4(1.f);
	projection_gun = glm::perspective(glm::radians(40.f), (float)800 / (float)600, 1.f, 10.f);
	shader.setMat4("projection", projection_gun);

	model.Draw(shader);
}

void setDefault(Shader shader) {
	shader.use();
	shader.setBool("material.isAmbientMap", 0);
	shader.setBool("material.isDiffuseMap", 0);
	shader.setBool("material.isSpecularMap", 0);
	shader.setBool("material.isBumpMap", 0);
	shader.setInt("material.texture_ambient;", 0);
	shader.setInt("material.texture_diffuse", 0);
	shader.setInt("material.texture_specular", 0);
	shader.setInt("material.texture_bump", 0);
	shader.setInt("material.shininess", 128);
	
	shader.setVec3("lightPos", lightPos);
	shader.setVec3("lamp.position", glm::vec3(0.0f, 5.0f, 0.0f));
	shader.setVec3("lamp.direction", glm::vec3(0.0f, -1.0f, 0.0f));
	shader.setVec3("lamp.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("lamp.diffuse", glm::vec3(1.0f, 1.f, 1.f));
	shader.setVec3("lamp.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	shader.setVec3("lightPos", lightPos);
	shader.setVec3("torch.position", lightPos);
	shader.setVec3("torch.direction", lightDir);
	shader.setVec3("torch.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	shader.setVec3("torch.diffuse", glm::vec3(1.f, 1.f, 1.f));
	shader.setVec3("torch.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	if(isTorchUsed)
		shader.setBool("isTorchUsed", 1);
	else
		shader.setBool("isTorchUsed", 0);
	shader.setVec3("viewPos", camera.Position);
	shader.setFloat("cutoff_cone", 15.f);
}
