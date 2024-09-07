#include "model.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 tex_coords)
    : position(position), normal(normal), tex_coords(tex_coords)
{
}

void Vertex::Print()
{
    SPDLOG_INFO("({:.2}, {:.2}, {:.2})\t({:.2}, {:.2}, {:.2})\t({:.2}, {:.2})",
        position.x, position.y, position.z,
        normal.x, normal.y, normal.z,
        tex_coords.x, tex_coords.y);
}

Texture::Texture(GLuint id, aiTextureType type, aiString path)
    : id(id), type(type), path(path)
{
}

void Texture::Print()
{
    SPDLOG_INFO("{}\t{}\t{}", id, static_cast<int>(type), path.C_Str());
}

GLuint TextureFromFile(const std::string &path, const std::string &directory, bool gamma)
{
    std::string filename = directory + "/" + path;

    int width, height, channels;
    stbi_uc *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        SPDLOG_ERROR("stbi_load {} failed", filename);
        return 0;
    }

    GLenum format;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture_id;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices_(vertices), indices_(indices), textures_(textures)
{
    SetupMesh();
}

void Mesh::Print()
{
    SPDLOG_INFO("verteices: {}", vertices_.size());
    // for (auto &vertex : vertices_)
    // {
    //     vertex.Print();
    // }

    SPDLOG_INFO("indices: {}", indices_.size());
    // for (auto &index : indices_)
    // {
    //     SPDLOG_INFO("{}", index);
    // }

    SPDLOG_INFO("textures: {}", textures_.size());
    // for (auto &texture : textures_)
    // {
    //     texture.Print();
    // }
}

void Mesh::Draw(Shader &shader)
{
    int diffuse_index = 1;
    int specular_index = 1;
    int normal_index = 1;
    int height_index = 1;
    for (int i = 0; i < textures_.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string uniform_name{"texture_"};
        switch (textures_[i].type)
        {
        case aiTextureType_DIFFUSE:
            uniform_name += "diffuse" + std::to_string(diffuse_index++);
            break;
        case aiTextureType_SPECULAR:
            uniform_name += "specular" + std::to_string(specular_index++);
            break;
        case aiTextureType_NORMALS:
            uniform_name += "normal" + std::to_string(normal_index++);
            break;
        case aiTextureType_HEIGHT:
            uniform_name += "height" + std::to_string(height_index++);
            break;
        default:
            SPDLOG_ERROR("invalid texture type");
            return;
        }

        shader.SetUniform(uniform_name, i);
        glBindTexture(GL_TEXTURE_2D, textures_[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(GL_NONE);
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);

    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, position)));
    // vertex normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, tex_coords)));

    glBindVertexArray(GL_NONE);
}

Model::Model(const std::string &path)
{
    LoadModel(path);
}

void Model::Print()
{
    for (auto &mesh : meshes_)
    {
        mesh.Print();
    }
    SPDLOG_INFO("directory: {}", directory_);
}

void Model::Draw(Shader &shader)
{
    // SPDLOG_INFO("---------------");
    for (auto &mesh : meshes_)
        mesh.Draw(shader);
}

void Model::LoadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("Assimp::Importer Readfile failed: {}", importer.GetErrorString());
        return;
    }
    directory_ = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (int i = 0; i < node->mNumMeshes; ++i)
    {
        meshes_.push_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
    }

    for (int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
        vertices.emplace_back(
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
            glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
            mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f));
    }
    // indices
    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
        {
            indices.push_back(mesh->mFaces[i].mIndices[j]);
        }
    }

    // material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuse_maps = LoadMaterialTexutres(material, aiTextureType_DIFFUSE);
        std::vector<Texture> specular_maps = LoadMaterialTexutres(material, aiTextureType_SPECULAR);
        std::vector<Texture> normal_maps = LoadMaterialTexutres(material, aiTextureType_NORMALS);
        std::vector<Texture> height_maps = LoadMaterialTexutres(material, aiTextureType_HEIGHT);

        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
        textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
        textures.insert(textures.end(), height_maps.begin(), height_maps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTexutres(aiMaterial *material, aiTextureType type)
{
    std::vector<Texture> textures;
    for (int i = 0; i < material->GetTextureCount(type); ++i)
    {
        aiString rel_path;
        material->GetTexture(type, i, &rel_path);
        if (!textures_loaded.contains(rel_path.C_Str()))
        {
            Texture texture(TextureFromFile(rel_path.C_Str(), directory_, false), type, rel_path);
            textures_loaded.insert(std::make_pair(rel_path.C_Str(), texture));
            textures.push_back(texture);
        }
        else
        {
            textures.push_back(textures_loaded.at(rel_path.C_Str()));
        }
    }
    return textures;
}
