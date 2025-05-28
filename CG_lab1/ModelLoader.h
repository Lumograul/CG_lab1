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

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            // Чтение вершин
            if (type == "v") {
                float x, y, z;
                iss >> x >> y >> z;
                tempPositions.push_back({ x, y, z, 1.0f });
            }


            // Чтение граней
            if (type == "f") {
                std::vector<std::string> faceVertices;
                std::string vertex;
                // Читаем все вершины грани
                while (iss >> vertex) {
                    faceVertices.push_back(vertex);
                }

                auto getIndex = [](const std::string& s) {
                    return static_cast<uint32_t>(std::stoi(s.substr(0, s.find('/'))) - 1);
                    };

                // Обработка треугольника (3 вершины)
                if (faceVertices.size() == 3) {
                    mesh.indices.push_back(getIndex(faceVertices[0]));
                    mesh.indices.push_back(getIndex(faceVertices[1]));
                    mesh.indices.push_back(getIndex(faceVertices[2]));
                }
                // Обработка квадрата (4 вершины) — разбиваем на два треугольника
                else if (faceVertices.size() == 4) {
                    // Первый треугольник: v0, v1, v2
                    mesh.indices.push_back(getIndex(faceVertices[0]));
                    mesh.indices.push_back(getIndex(faceVertices[1]));
                    mesh.indices.push_back(getIndex(faceVertices[2]));
                    // Второй треугольник: v0, v2, v3
                    mesh.indices.push_back(getIndex(faceVertices[0]));
                    mesh.indices.push_back(getIndex(faceVertices[2]));
                    mesh.indices.push_back(getIndex(faceVertices[3]));
                }
            }
        }

        // Заполнение вершин с цветом
        for (const auto& pos : tempPositions) {
            mesh.vertices.push_back({ pos, color });
        }

        return mesh;
    }
};