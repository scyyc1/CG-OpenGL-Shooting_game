#pragma once
#include "util.h"
#include "mesh.h"

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

class Model
{
public:
    vector<Mesh>            LoadedMeshes;
    vector<Vertex>          LoadedVertices;
    vector<unsigned int>    LoadedIndices;
    vector<Material>        LoadedMaterials;
    string                  path;
    string                  name;

    Model(string path, string name){
        this->path = path;
        this->name = name;
        bool success = LoadFile(path);
        if (success)
            std::cout << "Successfully load " << name << "!" << std::endl;
        else
            std::cout << "Fail to load " << name << "!" << std::endl;
    }

    ~Model()
    {
        LoadedMeshes.clear();
    }

    void Draw(Shader& shader)
    {
        for (auto mesh : LoadedMeshes)
            mesh.Draw(shader);
    }

    bool LoadFile(string Path)
    {
        if (Path.substr(Path.size() - 4, 4) != ".obj")
            return false;

        ifstream file(Path);

        if (!file.is_open())
            return false;

        LoadedMeshes.clear();
        LoadedVertices.clear();
        LoadedIndices.clear();

        vector<glm::vec3> Positions;
        vector<glm::vec2> TCoords;
        vector<glm::vec3> Normals;

        vector<Vertex> Vertices;
        vector<unsigned int> Indices;

        vector<string> MeshMatNames;

        bool listening = false;
        string meshname;

        Mesh tempMesh;

        string curline;
        while (getline(file, curline))
        {
            // Generate a Mesh Object or Prepare for an object to be created
            if (firstToken(curline) == "o" || firstToken(curline) == "g" || curline[0] == 'g')
            {
                if (!listening){
                    listening = true;
                    if (firstToken(curline) == "o" || firstToken(curline) == "g")
                        meshname = tail(curline);
                    else
                        meshname = "unnamed";
                } else {
                    if (!Indices.empty() && !Vertices.empty())
                    {
                        // Create Mesh
                        tempMesh = Mesh(Vertices, Indices);
                        tempMesh.name = meshname;
                        tempMesh.directory = Path.substr(0, Path.find_last_of('/'));
                        LoadedMeshes.push_back(tempMesh);

                        // Cleanup
                        Vertices.clear();
                        Indices.clear();
                        meshname.clear();

                        meshname = tail(curline);
                    }
                    else
                    {
                        if (firstToken(curline) == "o" || firstToken(curline) == "g")
                        {
                            meshname = tail(curline);
                        }
                        else
                        {
                            meshname = "unnamed";
                        }
                    }
                }
            }
            // Generate a Vertex Position
            if (firstToken(curline) == "v")
            {
                vector<string> spos;
                glm::vec3 vpos;
                split(tail(curline), spos, " ");

                vpos.x = stof(spos[0]);
                vpos.y = stof(spos[1]);
                vpos.z = stof(spos[2]);

                Positions.push_back(vpos);
            }
            // Generate a Vertex Texture Coordinate
            if (firstToken(curline) == "vt")
            {
                vector<string> stex;
                glm::vec2 vtex;
                split(tail(curline), stex, " ");

                vtex.x = stof(stex[0]);
                vtex.y = stof(stex[1]);

                TCoords.push_back(vtex);
            }
            // Generate a Vertex Normal;
            if (firstToken(curline) == "vn")
            {
                vector<string> snor;
                glm::vec3 vnor;
                split(tail(curline), snor, " ");

                vnor.x = stof(snor[0]);
                vnor.y = stof(snor[1]);
                vnor.z = stof(snor[2]);

                Normals.push_back(vnor);
            }
            // Generate a Face (vertices & indices)
            if (firstToken(curline) == "f")
            {
                vector<Vertex> vVerts;
                GenVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

                // Add Vertices
                for (int i = 0; i < int(vVerts.size()); i++){
                    Vertices.push_back(vVerts[i]);
                    LoadedVertices.push_back(vVerts[i]);
                }

                vector<unsigned int> iIndices;
                VertexTriangluation(iIndices, vVerts);

                // Add Indices
                for (int i = 0; i < int(iIndices.size()); i++)
                {
                    unsigned int indnum = (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
                    Indices.push_back(indnum);

                    indnum = (unsigned int)((LoadedVertices.size()) - vVerts.size()) + iIndices[i];
                    LoadedIndices.push_back(indnum);

                }
            }

            // Get Mesh Material Name
            if (firstToken(curline) == "usemtl")
            {
                MeshMatNames.push_back(tail(curline));

                // Create new Mesh, if Material changes within a group
                if (!Indices.empty() && !Vertices.empty())
                {
                    // Create Mesh
                    tempMesh = Mesh(Vertices, Indices);
                    tempMesh.name = meshname;
                    tempMesh.directory = Path.substr(0, Path.find_last_of('/'));
                    int i = 2;
                    while (1) {
                        tempMesh.name = meshname + "_" + to_string(i);

                        for (auto& m : LoadedMeshes)
                            if (m.name == tempMesh.name)
                                continue;
                        break;
                    }

                    // Insert Mesh
                    LoadedMeshes.push_back(tempMesh);

                    // Cleanup
                    Vertices.clear();
                    Indices.clear();
                }
            }
 
            // Load Materials
            if (firstToken(curline) == "mtllib")
            {
                // Obtain the location of .mtl file from the path of .obj
                vector<string> temp;
                split(Path, temp, "/");
                string pathtomat = "";
                if (temp.size() != 1)
                    for (int i = 0; i < temp.size() - 1; i++)
                        pathtomat += temp[i] + "/";

                pathtomat += tail(curline);

                LoadMaterials(pathtomat);
            }
        }

        // Deal with last mesh
        if (!Indices.empty() && !Vertices.empty())
        {
            // Create Mesh
            tempMesh = Mesh(Vertices, Indices);
            tempMesh.name = meshname;
            tempMesh.directory = Path.substr(0, Path.find_last_of('/'));

            // Insert Mesh
            LoadedMeshes.push_back(tempMesh);
        }        

        // Set Materials for each Mesh
        for (int i = 0; i < MeshMatNames.size(); i++)
        {
            std::string matname = MeshMatNames[i];
            for (int j = 0; j < LoadedMaterials.size(); j++) {
                if (LoadedMaterials[j].name == matname) {
                    LoadedMeshes[i].setMaterials(LoadedMaterials[j]);
                    break;
                }
            }
        }

        if (LoadedMeshes.empty() && LoadedVertices.empty() && LoadedIndices.empty())
            return false;
        else
            return true;
    }

private:
    // Generate vertices from a list of positions,
    //	tcoords, normals and a face line
    void GenVerticesFromRawOBJ(vector<Vertex>& oVerts,
        const vector<glm::vec3>& iPositions,
        const vector<glm::vec2>& iTCoords,
        const vector<glm::vec3>& iNormals,
        string icurline)
    {
        vector<string> sface, svert;
        Vertex vVert;
        split(tail(icurline), sface, " ");

        bool noNormal = false;

        // For every given vertex do this
        for (int i = 0; i < int(sface.size()); i++)
        {
            int vtype;                                              // Indicate different style of "f" line
            split(sface[i], svert, "/");

            if (svert.size() == 1)          vtype = 1;              // Only position v
            if (svert.size() == 2)          vtype = 2;              // "v/vt"
            if (svert.size() == 3)                                  // "v/vt/vn" or "v//vn"
                if (svert[1] != "")         vtype = 4;              // "v/vt/vn"
                else                        vtype = 3;              // "v//vn"

            // Put the right data to right place v, vt & vn
            switch (vtype)
            {
                case 1:                                             // "v"
                {
                    vVert.Position = getElement(iPositions, svert[0]);
                    vVert.TexCoords = glm::vec2(0, 0);
                    noNormal = true;
                    oVerts.push_back(vVert);
                    break;
                }
                case 2:                                              // "v/vt"
                {
                    vVert.Position = getElement(iPositions, svert[0]);
                    vVert.TexCoords = getElement(iTCoords, svert[1]);
                    noNormal = true;
                    oVerts.push_back(vVert);
                    break;
                }
                case 3:                                              // "v//vn"
                {
                    vVert.Position = getElement(iPositions, svert[0]);
                    vVert.TexCoords = glm::vec2(0, 0);
                    vVert.Normal = getElement(iNormals, svert[2]);
                    oVerts.push_back(vVert);
                    break;
                }
                case 4:                                              // "v/vt/vn"
                {
                    vVert.Position = getElement(iPositions, svert[0]);
                    vVert.TexCoords = getElement(iTCoords, svert[1]);
                    vVert.Normal = getElement(iNormals, svert[2]);
                    oVerts.push_back(vVert);
                    break;
                }
                default:
                {
                    break;
                }
            }
        }
        
        // Calculate the normal of vertices that are not provided
        if (noNormal) {
            // Use two edges to calculate the norm
            glm::vec3 A = oVerts[0].Position - oVerts[1].Position;
            glm::vec3 B = oVerts[2].Position - oVerts[1].Position;
            glm::vec3 normal = glm::cross(A, B);

            for (int i = 0; i < int(oVerts.size()); i++)
                oVerts[i].Normal = normal;
            
        }
    }

    // Triangulate a list of vertices into a face by printing
    //	inducies corresponding with triangles within it
    void VertexTriangluation(vector<unsigned int>& oIndices,
        const vector<Vertex>& iVerts)
    {
        // If there are 2 or less verts,
        // no triangle can be created,
        // so exit
        if (iVerts.size() < 3)
            return;
        
        // If it is a triangle no need to calculate it
        if (iVerts.size() == 3)
        {
            oIndices.push_back(0);
            oIndices.push_back(1);
            oIndices.push_back(2);
            return;
        }

        // Create a list of vertices
        vector<Vertex> tVerts = iVerts;

        while (true)
        {
            // For every vertex
            for (int i = 0; i < int(tVerts.size()); i++)
            {
                // pPrev = the previous vertex in the list
                Vertex pPrev;
                if (i == 0)
                    pPrev = tVerts[tVerts.size() - 1];
                else
                    pPrev = tVerts[i - 1];

                // pCur = the current vertex;
                Vertex pCur = tVerts[i];

                // pNext = the next vertex in the list
                Vertex pNext;
                if (i == tVerts.size() - 1)
                    pNext = tVerts[0];
                else
                    pNext = tVerts[i + 1];

                // Check to see if there are only 3 verts left
                // if so this is the last triangle
                if (tVerts.size() == 3)
                {
                    // Create a triangle from pCur, pPrev, pNext
                    for (int j = 0; j < int(tVerts.size()); j++)
                    {
                        if (iVerts[j].Position == pCur.Position)
                            oIndices.push_back(j);
                        if (iVerts[j].Position == pPrev.Position)
                            oIndices.push_back(j);
                        if (iVerts[j].Position == pNext.Position)
                            oIndices.push_back(j);
                    }

                    tVerts.clear();
                    break;
                }
                if (tVerts.size() == 4)
                {
                    // Create a triangle from pCur, pPrev, pNext
                    for (int j = 0; j < int(iVerts.size()); j++)
                    {
                        if (iVerts[j].Position == pCur.Position)
                            oIndices.push_back(j);
                        if (iVerts[j].Position == pPrev.Position)
                            oIndices.push_back(j);
                        if (iVerts[j].Position == pNext.Position)
                            oIndices.push_back(j);
                    }

                    glm::vec3 tempVec;
                    for (int j = 0; j < int(tVerts.size()); j++)
                    {
                        if (tVerts[j].Position != pCur.Position
                            && tVerts[j].Position != pPrev.Position
                            && tVerts[j].Position != pNext.Position)
                        {
                            tempVec = tVerts[j].Position;
                            break;
                        }
                    }

                    // Create a triangle from pCur, pPrev, pNext
                    for (int j = 0; j < int(iVerts.size()); j++)
                    {
                        if (iVerts[j].Position == pPrev.Position)
                            oIndices.push_back(j);
                        if (iVerts[j].Position == pNext.Position)
                            oIndices.push_back(j);
                        if (iVerts[j].Position == tempVec)
                            oIndices.push_back(j);
                    }

                    tVerts.clear();
                    break;
                }

                // If Vertex is not an interior vertex
                float angle = angleBetween(pPrev.Position - pCur.Position, pNext.Position - pCur.Position) * (180 / 3.14159265359);
                if (angle <= 0 && angle >= 180)
                    continue;

                // If any vertices are within this triangle
                bool inTri = false;
                for (int j = 0; j < int(iVerts.size()); j++)
                {
                    if (inTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
                        && iVerts[j].Position != pPrev.Position
                        && iVerts[j].Position != pCur.Position
                        && iVerts[j].Position != pNext.Position)
                    {
                        inTri = true;
                        break;
                    }
                }
                if (inTri)
                    continue;

                // Create a triangle from pCur, pPrev, pNext
                for (int j = 0; j < int(iVerts.size()); j++)
                {
                    if (iVerts[j].Position == pCur.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pPrev.Position)
                        oIndices.push_back(j);
                    if (iVerts[j].Position == pNext.Position)
                        oIndices.push_back(j);
                }

                // Delete pCur from the list
                for (int j = 0; j < int(tVerts.size()); j++)
                {
                    if (tVerts[j].Position == pCur.Position)
                    {
                        tVerts.erase(tVerts.begin() + j);
                        break;
                    }
                }

                // reset i to the start
                // -1 since loop will add 1 to it
                i = -1;
            }

            // if no triangles were created
            if (oIndices.size() == 0)
                break;

            // if no more vertices
            if (tVerts.size() == 0)
                break;
        }
    }

    // Load Materials from .mtl file
    bool LoadMaterials(string path)
    {
        if (path.substr(path.size() - 4, path.size()) != ".mtl")
            return false;
        ifstream file(path);
        if (!file.is_open())
            return false;

        Material tempMaterial;

        bool listening = false;

        // Go through each line looking for material variables
        string curline;
        while (getline(file, curline))
        {
            if (firstToken(curline) == "newmtl")
            {
                if (!listening)
                {
                    listening = true;

                    if (curline.size() > 7)
                    {
                        tempMaterial.name = tail(curline);
                    }
                    else
                    {
                        tempMaterial.name = "none";
                    }
                }
                else
                {
                    // Generate the material

                    // Push Back loaded Material
                    LoadedMaterials.push_back(tempMaterial);

                    // Clear Loaded Material
                    tempMaterial = Material();

                    if (curline.size() > 7)
                    {
                        tempMaterial.name = tail(curline);
                    }
                    else
                    {
                        tempMaterial.name = "none";
                    }
                }
            }
            // Ambient Color
            if (firstToken(curline) == "Ka")
            {
                vector<string> temp;
                split(tail(curline), temp, " ");

                if (temp.size() != 3)
                    continue;

                tempMaterial.Ka.x = stof(temp[0]);
                tempMaterial.Ka.y = stof(temp[1]);
                tempMaterial.Ka.z = stof(temp[2]);
            }
            // Diffuse Color
            if (firstToken(curline) == "Kd")
            {
                vector<string> temp;
                split(tail(curline), temp, " ");

                if (temp.size() != 3)
                    continue;

                tempMaterial.Kd.x = stof(temp[0]);
                tempMaterial.Kd.y = stof(temp[1]);
                tempMaterial.Kd.z = stof(temp[2]);
            }
            // Specular Color
            if (firstToken(curline) == "Ks")
            {
                vector<string> temp;
                split(tail(curline), temp, " ");

                if (temp.size() != 3)
                    continue;

                tempMaterial.Ks.x = stof(temp[0]);
                tempMaterial.Ks.y = stof(temp[1]);
                tempMaterial.Ks.z = stof(temp[2]);
            }

            // Read elements in the file
            if (firstToken(curline) == "Ns")
                tempMaterial.Ns = stof(tail(curline));
            if (firstToken(curline) == "Ni")
                tempMaterial.Ni = stof(tail(curline));
            if (firstToken(curline) == "d")
                tempMaterial.d = stof(tail(curline));
            if (firstToken(curline) == "illum")
                tempMaterial.illum = stoi(tail(curline));
            if (firstToken(curline) == "map_Ka")
                tempMaterial.map_Ka = tail(curline);
            if (firstToken(curline) == "map_Kd")
                tempMaterial.map_Kd = tail(curline);
            if (firstToken(curline) == "map_Ks")
                tempMaterial.map_Ks = tail(curline);
            if (firstToken(curline) == "map_Ns")
                tempMaterial.map_Ns = tail(curline);
            if (firstToken(curline) == "map_d")
                tempMaterial.map_d = tail(curline);
            if (firstToken(curline) == "map_Bump" || firstToken(curline) == "map_bump" || firstToken(curline) == "bump")
                tempMaterial.map_bump = tail(curline);
        }

        LoadedMaterials.push_back(tempMaterial);

        if (LoadedMaterials.empty())
            return false;
        else
            return true;
    }


    bool inTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
    {
        // Test to see if it is within an infinite prism that the triangle outlines.
        bool within_tri_prisim = SameSide(point, tri1, tri2, tri3) && SameSide(point, tri2, tri1, tri3)
            && SameSide(point, tri3, tri1, tri2);

        // If it isn't it will never be on the triangle
        if (!within_tri_prisim)
            return false;

        // Calulate Triangle's Normal
        glm::vec3 n = GenTriNormal(tri1, tri2, tri3);

        // Project the point onto this normal
        glm::vec3 proj = Project(point, n);

        // If the distance from the triangle to the point is 0
        //	it lies on the triangle
        if (glm::length(proj) == 0)
            return true;
        else
            return false;
    }

    // A test to see if P1 is on the same side as P2 of a line segment ab
    bool SameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b)
    {
        glm::vec3 cp1 = glm::cross(b - a, p1 - a);
        glm::vec3 cp2 = glm::cross(b - a, p2 - a);

        if (glm::dot(cp1, cp2) >= 0)
            return true;
        else
            return false;
    }

    // Generate a cross produect normal for a triangle
    inline glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
    {
        glm::vec3 u = t2 - t1;
        glm::vec3 v = t3 - t1;

        glm::vec3 normal = glm::cross(u, v);
        return normal;
    }

    // Projection Calculation of a vector to another
    glm::vec3 Project(const glm::vec3 a, const glm::vec3 b)
    {
        glm::vec3 bn = b / glm::length(b);
        return bn * glm::dot(a, bn);
    }

};
