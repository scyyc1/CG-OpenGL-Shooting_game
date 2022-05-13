#pragma once
#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    // for bump(normal) mapping
    // Calculate TBN
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

struct Material
{
    Material()
    {
        name;
        Ns = 0.0f;
        Ni = 0.0f;
        d = 0.0f;
        illum = 0;
    }

    string name;
    
    glm::vec3 Ka;               // Ambient Color
    glm::vec3 Kd;               // Diffuse Color
    glm::vec3 Ks;               // Specular Color

    // Not use in my demo
    float Ns, Ni, d, Tr, Sharpness, illum;
    glm::vec3 Ke, Tf;
    
    string map_Ka;              // Ambient Texture Map
    string map_Kd;              // Diffuse Texture Map
    string map_Ks;              // Specular Texture Map
    string map_bump;            // Bump/normal Texture Map
    string map_Ns;              // Specular Hightlight Map--not used
    string map_d;               // Alpha Texture Map--not used
    
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    Material             MeshMaterial;
    string               directory;
    string               name;
    unsigned int VAO;

    Mesh(){}

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
        calculateTBNs();
        setupMesh();
    }

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->MeshMaterial = material;
        calculateTBNs();
        setupMesh();
        setupTextureWithMaterial();
    }

    void setMaterials(Material material) {
        this->MeshMaterial = material;
        setupTextureWithMaterial();
    }

    string getName() {
        return this->name;
    }

    void setName(string str) {
        this->name = str;
    }

    // render the mesh
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int ambientNr = 1;
        unsigned int bumpNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse")
                number = to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = to_string(specularNr++);
            else if (name == "texture_ambient")
                number = to_string(ambientNr++);
            else if (name == "texture_bump")
                number = to_string(bumpNr++);
            
            glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);
            shader.setVec3("light.ambient", MeshMaterial.Ka);
            shader.setVec3("light.diffuse", MeshMaterial.Kd);
            shader.setVec3("light.specular", MeshMaterial.Ks);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        glBindVertexArray(0);
    }

    // Generate texture with maps
    void setupTextureWithMaterial() {
        if (MeshMaterial.map_Ka != "") {
            string path = this->directory + "/" + MeshMaterial.map_Ka;
            GLuint ID = setup_texture(path.c_str());

            Texture temp;
            temp.id = ID;
            temp.type = "texture_ambient";
            temp.path = MeshMaterial.map_Ka.c_str();

            textures.push_back(temp);
        }
        if (MeshMaterial.map_Kd != "") {
            string path = this->directory + "/" + MeshMaterial.map_Kd;
            GLuint ID = setup_texture(path.c_str());

            Texture temp;
            temp.id = ID;
            temp.type = "texture_diffuse";
            temp.path = MeshMaterial.map_Kd.c_str();

            textures.push_back(temp);
        }
        if (MeshMaterial.map_Ks != "") {
            string path = this->directory + "/" + MeshMaterial.map_Ks;
            GLuint ID = setup_texture(path.c_str());

            Texture temp;
            temp.id = ID;
            temp.type = "texture_specular";
            temp.path = MeshMaterial.map_Ks.c_str();

            textures.push_back(temp);
        }
        if (MeshMaterial.map_bump != "") {
            string path = this->directory + "/" + MeshMaterial.map_bump;
            GLuint ID = setup_texture(path.c_str());

            Texture temp;
            temp.id = ID;
            temp.type = "texture_bump";
            temp.path = MeshMaterial.map_bump.c_str();

            textures.push_back(temp);
        }
    }

    void calculateTBNs() {
        for (int i = 0; i + 2 < indices.size(); i+=3) {
            calculateTBN(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]);
        }

        // Take the average of tangent and bitangent in each vertex for triangles which shares the same vertex
        for (auto v : vertices) {
            v.Bitangent = glm::normalize(v.Bitangent);
            v.Tangent = glm::normalize(v.Tangent);
        }
    }

    // Calculate TBN matrix for single triangle
    void calculateTBN(Vertex& v1, Vertex& v2, Vertex& v3) {
        glm::vec3 tangent, bitangent;

        glm::vec3 edge1 = v2.Position - v1.Position;
        glm::vec3 edge2 = v3.Position - v1.Position;
        glm::vec2 deltaUV1 = v2.TexCoords - v1.TexCoords;
        glm::vec2 deltaUV2 = v3.TexCoords - v1.TexCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // Take the average of tangent and bitangent
        // Since vertices may be used in multiple triangle
        v1.Tangent += glm::normalize(tangent);
        v2.Tangent += glm::normalize(tangent);
        v3.Tangent += glm::normalize(tangent);

        v1.Bitangent += glm::normalize(bitangent);
        v2.Bitangent += glm::normalize(bitangent);
        v3.Bitangent += glm::normalize(bitangent);
    }
};