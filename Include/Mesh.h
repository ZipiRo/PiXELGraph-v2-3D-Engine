#pragma once

#include <algorithm>

struct Triangle
{
    int indices[3];
    Vector3 normal;
    float normalDot;
    float lightIntensity; 
};

struct Mesh
{
    Vector3 position;
    Vector3 scale;
    Vector3 angle;

    std::vector<Vector3> points;
    std::vector<Triangle> faces;

    std::vector<Vector3> transformedPoints;

    Color color;

    void Transform()
    {
        transformedPoints.reserve(points.size());
        angle = NormalizeAngle(angle);
        Matrix ModelMatrix = Model_Matrix(position, scale, angle);
        Matrix PVMatrix = PerspectiveMatrix * ViewMatrix;

        for (auto &triangle : faces)
        {
            std::vector<Vector3> worldPoints;
            for (const auto &index : triangle.indices)
            {
                Matrix WorldMatrix = ModelMatrix * points[index];
                worldPoints.emplace_back(WorldMatrix(0, 0), WorldMatrix(1, 0), WorldMatrix(2, 0));
            }

            for(int i = 0; i < worldPoints.size(); i++)
            {
                Matrix ProjectedMatrix = PVMatrix * worldPoints[i];

                float w = ProjectedMatrix(3, 0);
                if (w != 0)
                {
                    ProjectedMatrix(0, 0) /= w;
                    ProjectedMatrix(1, 0) /= w;
                    ProjectedMatrix(2, 0) /= w;
                }

                Vector3 screenPoint;
                screenPoint.x = (ProjectedMatrix(0, 0) + 1.0f) * 0.5f * ScreenWidth;
                screenPoint.y = (1.0f - ProjectedMatrix(1, 0)) * 0.5f * ScreenHeight;
                screenPoint.z = ProjectedMatrix(2, 0);

                transformedPoints[triangle.indices[i]] = screenPoint;
            }

            Vector3 line1 = worldPoints[1] - worldPoints[0];
            Vector3 line2 = worldPoints[2] - worldPoints[0];

            triangle.normal = Vector3::Normalize(Vector3::CrossProduct(line1, line2));
            triangle.normalDot = Vector3::DotProduct(triangle.normal, worldPoints[0] - cameraPosition);
            triangle.lightIntensity = std::max(0.0f, Vector3::DotProduct(lightDirection, triangle.normal));
        }

        // transformedPoints.clear();
        // for(const auto& point : points)
        // {
        //     Vector3 screenPoint = TransformPoint(point, position, scale, angle);
        //     screenPoint.x = (screenPoint.x + 1.0f) * 0.5f * ScreenWidth;
        //     screenPoint.y = (1.0f - screenPoint.y) * 0.5f * ScreenHeight;
        //     transformedPoints.push_back(screenPoint);
        // }
    }

    void Draw()
    {
        Transform();

        std::vector<Triangle> facesToDraw = faces;

        for(int i = 0; i < facesToDraw.size() - 1; i++)
            for(int j = i + 1; j < facesToDraw.size(); j++)
            {
                float z1 = transformedPoints[faces[i].indices[0]].z + transformedPoints[faces[i].indices[1]].z + transformedPoints[faces[i].indices[2]].z / 3;
                float z2 = transformedPoints[faces[j].indices[0]].z + transformedPoints[faces[j].indices[1]].z + transformedPoints[faces[j].indices[2]].z / 3;
                
                if(z1 < z2)
                    std::swap(facesToDraw[i], facesToDraw[j]);
            }

        for (const auto &triangle : faces)
        {
            if (triangle.normalDot >= 0.0f)
                continue;

            Vector3 P1 = transformedPoints[triangle.indices[0]];
            Vector3 P2 = transformedPoints[triangle.indices[1]];
            Vector3 P3 = transformedPoints[triangle.indices[2]];

            Color faceColor(color.r * triangle.lightIntensity, color.g * triangle.lightIntensity, color.b * triangle.lightIntensity);

            FillTriangle(P1, P2, P3, faceColor);
            // DrawTriangle(P1, P2, P3, Color::White);
        }
    }
};