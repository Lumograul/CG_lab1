//#include "ModelLoader.h"
//#include "Logger.h"
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//
//ModelLoader::ModelLoader() : texturePath("") {
//}
//
//bool ModelLoader::LoadModel(const std::string& filePath) {
//
//    Assimp::Importer importer;
//    logger << "[ModelLoader] ������� ��������� ����: " << filePath << std::endl;
//
//    const aiScene* scene = importer.ReadFile(filePath,
//        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
//
//    if (!scene) {
//        //logger << "[ModelLoader] ������: �� ������� ��������� �����: " << importer.GetErrorString() << std::endl;
//        return false;
//    }
//
//    /*if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
//        logger << "[ModelLoader] ������: ����� ��������: " << importer.GetErrorString() << std::endl;
//        return false;
//    }*/
//
//  /*  if (!scene->mRootNode) {
//        logger << "[ModelLoader] ������: ����������� �������� ����" << std::endl;
//        return false;
//    }*/
//
//    /*if (!scene->HasMeshes()) {
//        logger << "[ModelLoader] ������: ���� ����������� � ������" << std::endl;
//        return false;
//    }*/
//
//    //logger << "[ModelLoader] ������ ������� ���������, ���������� ���: " << scene->mNumMeshes << std::endl;
//
//    aiMesh* mesh = scene->mMeshes[0];
//    //logger << "[ModelLoader] ��������� ����: �������=" << mesh->mNumVertices << ", �����=" << mesh->mNumFaces << std::endl;
//
//    vertices.clear();
//    indices.clear();
//
//    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
//        aiVector3D pos = mesh->mVertices[i];
//        aiVector3D norm = mesh->mNormals[i];
//        vertices.push_back(pos.x);
//        vertices.push_back(pos.y);
//        vertices.push_back(pos.z);
//        vertices.push_back(norm.x);
//        vertices.push_back(norm.y);
//        vertices.push_back(norm.z);
//        if (mesh->HasTextureCoords(0)) {
//            aiVector3D texCoord = mesh->mTextureCoords[0][i];
//            vertices.push_back(texCoord.x);
//            vertices.push_back(texCoord.y);
//        }
//        else {
//            vertices.push_back(0.0f);
//            vertices.push_back(0.0f);
//            logger << "[ModelLoader] ���������� ���������� ����������� ��� ������� " << i << ", ����������� � (0, 0)" << std::endl;
//        }
//    }
//    logger << "[ModelLoader] ������� ����������, ����� ���������� float: " << vertices.size() << std::endl;
//
//    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
//        aiFace face = mesh->mFaces[i];
//        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
//            indices.push_back(face.mIndices[j]);
//        }
//    }
//    logger << "[ModelLoader] ������� ����������, ����� ����������: " << indices.size() << std::endl;
//
//    if (scene->HasMaterials()) {
//        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//        aiString path;
//        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
//            texturePath = path.C_Str();
//            logger << "[ModelLoader] �������� ������� � ������: " << texturePath << std::endl;
//        }
//        else {
//            logger << "[ModelLoader] �������� � ��������� �� �������" << std::endl;
//        }
//    }
//    else {
//        logger << "[ModelLoader] ��������� ����������� � ������" << std::endl;
//    }
//
//    logger << "[ModelLoader] �������� ������ ��������� �������" << std::endl;
//    return true;
//}
//
//const std::vector<float>& ModelLoader::GetVertices() const {
//    logger << "[ModelLoader] ������ ������, ������: " << vertices.size() << std::endl;
//    return vertices;
//}
//
//const std::vector<unsigned int>& ModelLoader::GetIndices() const {
//    logger << "[ModelLoader] ������ ��������, ������: " << indices.size() << std::endl;
//    return indices;
//}
//
//const std::string& ModelLoader::GetTexturePath() const {
//    logger << "[ModelLoader] ������ ���� � ��������: " << texturePath << std::endl;
//    return texturePath;
//}