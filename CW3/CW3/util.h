#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

#define	windowHeight	600
#define	windowWidth		800
#define	center_X		windowWidth/2	
#define	center_Y		windowHeight/2
#define DEG2RAD(n)		n*(M_PI/180)

#define MIN_ZOOM		30.f
#define	MAX_ZOOM		45.f
#define	INITIAL_ZOOM	45.f

#define MOUSE_SENSITIVE 0.1f
#define MOVE_SPEED		2.5f

#define ROOM_EDGE		3.5f

#define NUM_SEG         32

#define SHOOTING_SPEED  0.1f
const int BULLET_HOLE_NUM = 10;


const float STAND = 0.f;
const glm::vec3 DEFAULT_POSITION = glm::vec3(0.f, 0.f, 0.f);
const glm::vec3 DEFAULT_DIRECTION = glm::vec3(0.f, 0.f, -1.f);
const glm::vec3 WorldUp = glm::vec3(0.f, 1.f, 0.f);

using namespace std;

char* read_file(const char* filename)
{
	FILE* f;
	fopen_s(&f, filename, "rb");
	if (f == NULL)
		return NULL;
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	rewind(f);
	char* bfr = (char*)malloc(sizeof(char) * (size + 1));
	if (bfr == NULL)
		return NULL;
	long ret = fread(bfr, 1, size, f);
	if (ret != size)
		return NULL;
	bfr[size] = '\0';
	return bfr;
}

inline void split(const string& in, vector<std::string>& out, string token)
{
    out.clear();
    std::string buffer;
    for (int i = 0; i < int(in.size()); i++){
        std::string test = in.substr(i, token.size());
        if (test == token) {
            if (!buffer.empty()){
                out.push_back(buffer);
                buffer.clear();
                i += (int)token.size() - 1;
            }else{
                out.push_back("");
            }
        }
        else if (i + token.size() >= in.size()){
            buffer += in.substr(i, token.size());
            out.push_back(buffer);
            break;
        }else{
            buffer += in[i];
        }
    }
}

// Obtain the first "word" in a string
// Like "v" for vertex or "vn" for vertex norm
std::string firstToken(const std::string& in)
{
    if (!in.empty())
    {
        size_t token_start = in.find_first_not_of(" \t");
        size_t token_end = in.find_first_of(" \t", token_start);
        if (token_start != std::string::npos && token_end != std::string::npos){
            return in.substr(token_start, token_end - token_start);
        }else if (token_start != std::string::npos){
            return in.substr(token_start);
        }
    }
    return "";
}

// Get tail of string after first token and possibly following spaces
inline std::string tail(const std::string& in)
{
    size_t token_start = in.find_first_not_of(" \t");
    size_t space_start = in.find_first_of(" \t", token_start);
    size_t tail_start = in.find_first_not_of(" \t", space_start);
    size_t tail_end = in.find_last_not_of(" \t");
    if (tail_start != std::string::npos && tail_end != std::string::npos){
        return in.substr(tail_start, tail_end - tail_start + 1);
    }
    else if (tail_start != std::string::npos){
        return in.substr(tail_start);
    }
    return "";
}

// Get element at given index position
template <class T>
inline const T& getElement(const std::vector<T>& elements, std::string& index)
{
    int idx = stoi(index);
    if (idx < 0)
        idx = int(elements.size()) + idx;
    else
        idx--;
    return elements[idx];
}

// Angle two vector
inline float angleBetween(const glm::vec3 a, const glm::vec3 b)
{
    float angle = glm::dot(a, b);
    angle /= (glm::length(a) * glm::length(b));
    return angle = acosf(angle);
}

// Obtain ratation matrix (from a to b)
glm::mat4 getRotationMatrix(const glm::vec3 a, const glm::vec3 b) {
    glm::mat4 M = glm::mat4(1.f);

    if (a.x == b.x && a.y == b.y && a.z == b.z)
        return M;
    
    // Obtain rotate angle
    float angle = angleBetween(a, b);
    // Obtain rotate axis
    glm::vec3 rotation_axis = glm::normalize(-glm::cross(a, b));
    // Build rotation matrix
    glm::mat4 rotation_mat = glm::mat4(1.f);
    rotation_mat = glm::rotate(rotation_mat, angle, rotation_axis);

    M = glm::inverse(glm::transpose(rotation_mat));

    return M;
}
