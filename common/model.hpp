#pragma once

extern "C"
{
#include "glad/glad.h"
#include "GLFW/glfw3.h"
}

#include <string>
#include <vector>
#include <unordered_map>
#include "shader.hpp"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "spdlog/spdlog.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 tex_coords);
    void Print();
};

struct Texture
{
    GLuint id;
    aiTextureType type;
    aiString path;

    Texture(GLuint id, aiTextureType type, aiString path);
    void Print();
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Print();
    void Draw(Shader &shader);

private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;
    std::vector<Texture> textures_;

    GLuint VAO_, VBO_, EBO_;
    void SetupMesh();
};

class Model
{
public:
    Model(const std::string &path);
    void Print();
    void Draw(Shader &shader);

private:
    std::vector<Mesh> meshes_;
    std::string directory_;
    std::unordered_map<std::string, Texture> textures_loaded;

    void LoadModel(const std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> LoadMaterialTexutres(aiMaterial *material, aiTextureType type);
};