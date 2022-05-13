#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "util.h"


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	glm::vec3 Position;		
	
	glm::vec3 Front;		// viewpoint
	glm::vec3 Up;			// viewpoint
	glm::vec3 Right;		// viewpoint

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	// camera options
	float Zoom;				
	float MovementSpeed;	// How fast the camera move
	float MouseSensitivity;	// How fast the viewpoint move

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f), 
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float yaw = -90.f, float pitch = 0.f)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(MOVE_SPEED), MouseSensitivity(MOUSE_SENSITIVE), Zoom(INITIAL_ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateViewPoint();
	}

	void moveCamera(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;

		if (Position.x > ROOM_EDGE - 0.1f)
			Position.x = ROOM_EDGE - 0.1f;
		if (Position.x < -ROOM_EDGE + 0.1f)
			Position.x = -ROOM_EDGE + 0.1f;
		if (Position.z > ROOM_EDGE - 0.1f)
			Position.z = ROOM_EDGE - 0.1;
		if (Position.z < -ROOM_EDGE + 0.1f)
			Position.z = -ROOM_EDGE + 0.1;
		Position.y = 0.0f;
	}

	void orientCamera(float xoffset, float yoffset) 
	{
		Yaw		+=	xoffset	* MouseSensitivity;
		Pitch	-=	yoffset	* MouseSensitivity;

		if (Pitch > 89.f)	Pitch = 89.f;
		if (Pitch < -89.f)	Pitch = -89.f;

		updateViewPoint();
	}

	void scrollCamera(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < MIN_ZOOM)
			Zoom = MIN_ZOOM;
		if (Zoom > MAX_ZOOM)
			Zoom = MAX_ZOOM;
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

private:
	void updateViewPoint()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
