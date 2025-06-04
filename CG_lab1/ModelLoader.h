#pragma once
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <DirectXMath.h>

struct Vertex {
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color; // Если нужно цвет извне
    DirectX::XMFLOAT2 texCoord;
    DirectX::XMFLOAT3 normal;

};
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};
class ModelLoader {
public:
    static MeshData LoadModel(const std::string& path, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f }) {
        MeshData mesh;
        std::ifstream file(path);
        std::string line;
        std::vector<DirectX::XMFLOAT4> tempPositions;
        std::vector<DirectX::XMFLOAT2> tempTexCoords;
        std::vector<DirectX::XMFLOAT3> tempNormals;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                tempPositions.push_back({ x, y, z, 1.0f });
            }
            else if (type == "vt") {
                float u, v;
                iss >> u >> v;
                tempTexCoords.emplace_back(u, -v);
            }
            else if (type == "vn") {
                float nx, ny, nz;
                iss >> nx >> ny >> nz;
                tempNormals.emplace_back(nx, ny, nz);
            }
            else if (type == "f") {
                std::vector<std::string> faceVertices;
                std::string vertex;
                while (iss >> vertex) {
                    faceVertices.push_back(vertex);
                }

                auto parseIndices = [](const std::string& part) {
                    std::istringstream viss(part);
                    std::string v, vt, vn;
                    getline(viss, v, '/');
                    getline(viss, vt, '/');
                    getline(viss, vn, '/');
                    return std::make_tuple(
                        stoi(v) - 1,
                        vt.empty() ? -1 : stoi(vt) - 1,
                        vn.empty() ? -1 : stoi(vn) - 1
                    );
                    };

                // Обработка треугольников и квадратов
                if (faceVertices.size() >= 3) {
                    // Первый треугольник (0, 1, 2)
                    for (size_t i = 0; i < 3; ++i) {
                        auto indices = parseIndices(faceVertices[i]);
                        int vIdx = std::get<0>(indices);
                        int vtIdx = std::get<1>(indices);
                        int vnIdx = std::get<2>(indices);
                        Vertex vert;
                        vert.position = tempPositions[vIdx];
                        vert.color = color;
                        vert.texCoord = (vtIdx != -1) ? tempTexCoords[vtIdx] : DirectX::XMFLOAT2(0, 0);
                        vert.normal = (vnIdx != -1) ? tempNormals[vnIdx] : DirectX::XMFLOAT3(0, 0, 0);

                        mesh.vertices.push_back(vert);
                        mesh.indices.push_back(static_cast<uint32_t>(mesh.vertices.size() - 1));
                    }

                    // Второй треугольник для квадрата (0, 2, 3)
                    if (faceVertices.size() == 4) {
                        for (size_t i : {0, 2, 3}) {
                            auto indices = parseIndices(faceVertices[i]);
                            int vIdx = std::get<0>(indices);
                            int vtIdx = std::get<1>(indices);
                            int vnIdx = std::get<2>(indices);
                            Vertex vert;
                            vert.position = tempPositions[vIdx];
                            vert.color = color;
                            vert.texCoord = (vtIdx != -1) ? tempTexCoords[vtIdx] : DirectX::XMFLOAT2(0, 0);
                            vert.normal = (vnIdx != -1) ? tempNormals[vnIdx] : DirectX::XMFLOAT3(0, 0, 0);

                            mesh.vertices.push_back(vert);
                            mesh.indices.push_back(static_cast<uint32_t>(mesh.vertices.size() - 1));
                        }
                    }
                }
            }
        }
        return mesh;
    }
};