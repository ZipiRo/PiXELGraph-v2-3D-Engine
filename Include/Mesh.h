#pragma once

#include <fstream>
#include <sstream>

struct Triangle
{
    int indices[3];
    float normalDot;
    float lightIntensity; 
    Vector3 normal;
};

struct Mesh
{
    Vector3 position;
    Vector3 scale;
    Vector3 angle;

    std::vector<Vector3> points;
    std::vector<Vector3> transformedPoints;
    std::vector<Triangle> faces;

    Color color;

    void Transform()
    {
        transformedPoints.clear();
        transformedPoints.resize(points.size());
        angle = NormalizeAngle(angle);
        Matrix ModelMatrix = Model_Matrix(position, scale, angle);
        Matrix PVMatrix = PerspectiveMatrix * ViewMatrix;

        for (auto &triangle : faces)
        {
            std::vector<Vector3> worldPoints;
            for (const auto &index : triangle.indices)
            {
                Vector4 worldPoint = ModelMatrix * points[index];
                worldPoints.emplace_back(worldPoint);
            }

            Vector3 line1 = worldPoints[1] - worldPoints[0];
            Vector3 line2 = worldPoints[2] - worldPoints[0];
            Vector3 normal = Vector3::Normalize(Vector3::CrossProduct(line1, line2));

            triangle.normal = normal;
            triangle.normalDot = Vector3::DotProduct(normal, worldPoints[0] - cameraPosition);
            triangle.lightIntensity = std::max(0.0f, Vector3::DotProduct(lightDirection, normal));

            if(triangle.normalDot >= 0) continue;
            for(int i = 0; i < worldPoints.size(); i++)
            {
                Vector4 projectedPoint = PVMatrix * worldPoints[i];
                
                if (projectedPoint.w != 0)
                    projectedPoint /= projectedPoint.w;

                Vector3 screenPoint;
                screenPoint.x = (projectedPoint.x + 1.0f) * 0.5f * ScreenWidth;
                screenPoint.y = (1.0f - projectedPoint.y) * 0.5f * ScreenHeight;
                screenPoint.z = projectedPoint.z;

                transformedPoints[triangle.indices[i]] = screenPoint;
            }
        }
    }

    void Draw()
    {
        Transform();

        std::vector<Triangle> facesToDraw = faces;

        for(int i = 0; i < facesToDraw.size() - 1; i++)
        for(int j = i + 1; j < facesToDraw.size(); j++)
        {
            float z1 = (transformedPoints[faces[i].indices[0]].z + transformedPoints[faces[i].indices[1]].z + transformedPoints[faces[i].indices[2]].z) / 3;
            float z2 = (transformedPoints[faces[j].indices[0]].z + transformedPoints[faces[j].indices[1]].z + transformedPoints[faces[j].indices[2]].z) / 3;
            
            if(z1 < z2)
                std::swap(facesToDraw[i], facesToDraw[j]);
        }

        for (const auto &triangle : facesToDraw)
        {
            if (triangle.normalDot >= 0)
                continue;

            Vector3 P1 = transformedPoints[triangle.indices[0]];
            Vector3 P2 = transformedPoints[triangle.indices[1]];
            Vector3 P3 = transformedPoints[triangle.indices[2]];

            Color faceColor(color.r * triangle.lightIntensity, color.g * triangle.lightIntensity, color.b * triangle.lightIntensity);

            FillTriangle(P1, P2, P3, faceColor);
            // DrawTriangle(P1, P2, P3, Color::White);
        }
    }

    void LoadFromFile(const std::string &filename)
    {
        std::ifstream object(filename);
        if(!object.is_open()) return;
        
        while (!object.eof())
        {
            char line[128];
            object.getline(line, 128);

            std::stringstream stream;
            stream << line;

            char junk;
            if(line[0] == 'v')
            {
                Vector3 point;
                stream >> junk >> point.x >> point.y >> point.z;
                points.push_back(point);
            }

            if(line[0] == 'f')
            {
                Triangle triangle;
                stream >> junk >> triangle.indices[0] >> triangle.indices[1] >> triangle.indices[2];

                triangle.indices[0]--;
                triangle.indices[1]--;
                triangle.indices[2]--;
                
                faces.push_back(triangle);
            }
        }

        object.close();
    }
};