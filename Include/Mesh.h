#pragma once

#include <algorithm>
#include <sstream>

struct Triangle
{
    int indices[3];
    float normalDot;
    float lightIntensity; 
    Vector3 normal;
};

void DrawTriangle(const Vector3 &P1, const Vector3 &P2, const Vector3 &P3, const Color &color)
{
    DrawLine(P1.x, P1.y, P2.x, P2.y, color);
    DrawLine(P2.x, P2.y, P3.x, P3.y, color);
    DrawLine(P3.x, P3.y, P1.x, P1.y, color);
}

float EdgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c)
{
    return float(c.x - a.x) * float(b.y - a.y) - float(c.y - a.y) * float(b.x - a.x);
}

void FillTriangle(const Vector3 &P1, const Vector3 &P2, const Vector3 &P3, Color color)
{
    // 1. Bounding box
    float minX = std::min(P1.x, std::min(P2.x, P3.x));
    float maxX = std::max(P1.x, std::max(P2.x, P3.x));
    float minY = std::min(P1.y, std::min(P2.y, P3.y));
    float maxY = std::max(P1.y, std::max(P2.y, P3.y));

    // Clamp to screen if needed
    minX = std::max(minX, 0.0f);
    minY = std::max(minY, 0.0f);
    maxX = std::min(maxX, (float)ScreenWidth - 1);
    maxY = std::min(maxY, (float)ScreenHeight - 1);

    // 2. Loop over pixels
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            Vector3 P(x, y, 0);

            float w0 = EdgeFunction(P2, P3, P);
            float w1 = EdgeFunction(P3, P1, P);
            float w2 = EdgeFunction(P1, P2, P);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                PlotPixel(x, y, color);
        }
    }
}

struct Mesh
{
    Vector3 position;
    Vector3 scale;
    Vector3 angle;

    std::vector<Vector3> points;
    std::vector<Vector3> transformedPoints;
    std::vector<Triangle> faces;

    Color color;
    Color outline = Color::Transparent;

    void Transform()
    {
        transformedPoints.clear();
        transformedPoints.resize(points.size());
        angle = NormalizeAngle(angle);
        Matrix ModelMatrix = Model_Matrix(position, scale, angle);
        Matrix PVMatrix = Camera::PerspectiveMatrix * Camera::ViewMatrix;

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
            triangle.normalDot = Vector3::DotProduct(normal, worldPoints[0] - Camera::GetPosition());
            triangle.lightIntensity = std::max(0.1f, Vector3::DotProduct(lightDirection, normal));

            if(triangle.normalDot >= 0) continue;
            for(int i = 0; i < worldPoints.size(); i++)
            {
                Vector4 projectedPoint = PVMatrix * worldPoints[i];
                
                if (projectedPoint.w != 0)
                    projectedPoint /= projectedPoint.w;

                Vector3 screenPoint;
                screenPoint.x = (projectedPoint.x + 1) * 0.5f * ScreenWidth;
                screenPoint.y = (1 - projectedPoint.y) * 0.5f * ScreenHeight;
                screenPoint.z = projectedPoint.z;

                transformedPoints[triangle.indices[i]] = screenPoint;
            }
        }
    }

    void Draw()
    {
        Transform();

        std::vector<Triangle> facesToDraw = faces;

        std::sort(facesToDraw.begin(), facesToDraw.end(), [this](const Triangle &T1, const Triangle &T2) {
            float z1 = (transformedPoints[T1.indices[0]].z + transformedPoints[T1.indices[1]].z + transformedPoints[T1.indices[2]].z) / 3;
            float z2 = (transformedPoints[T2.indices[0]].z + transformedPoints[T2.indices[1]].z + transformedPoints[T2.indices[2]].z) / 3;
            
            return z1 < z2;
        });

        for (const auto &triangle : facesToDraw)
        {
            if (triangle.normalDot >= 0)
                continue;

            Vector3 P1 = transformedPoints[triangle.indices[0]];
            Vector3 P2 = transformedPoints[triangle.indices[1]];
            Vector3 P3 = transformedPoints[triangle.indices[2]];

            Color faceColor(color.r * triangle.lightIntensity, color.g * triangle.lightIntensity, color.b * triangle.lightIntensity);

            FillTriangle(P1, P2, P3, faceColor);
            
            if(outline != Color::Transparent)
                DrawTriangle(P1, P2, P3, color);
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

namespace MeshShapes
{
    Mesh CreateCube()
    {
        Mesh mesh;

        mesh.points = {
            {-0.5f, 0.5f, 0.5f},  
            {0.5f, 0.5f, 0.5f},   
            {0.5f, -0.5f, 0.5f},  
            {-0.5f, -0.5f, 0.5f}, 
            {-0.5f, 0.5f, -0.5f}, 
            {0.5f, 0.5f, -0.5f},  
            {0.5f, -0.5f, -0.5f}, 
            {-0.5f, -0.5f, -0.5f} 
        };

        mesh.faces = {
            {0, 1, 2}, {0, 2, 3}, 
            {1, 5, 6},
            {1, 6, 2}, 
            {5, 4, 7},
            {5, 7, 6}, 
            {4, 0, 3},
            {4, 3, 7}, 
            {4, 5, 1},
            {4, 1, 0}, 
            {3, 2, 6},
            {3, 6, 7} 
        };

        return mesh;
    }

    Mesh CreatePiramide()
    {
        Mesh mesh;

        mesh.points = {
            {-0.5f, 0.0f, -0.5f}, 
            {0.5f, 0.0f, -0.5f},  
            {0.5f, 0.0f, 0.5f},   
            {-0.5f, 0.0f, 0.5f},  
            {0.0f, 1.0f, 0.0f}    
        };

        mesh.faces = {
            {2, 1, 0}, {3, 2, 0}, 
            {0, 1, 4},            
            {1, 2, 4},            
            {2, 3, 4},            
            {3, 0, 4}             
        };

        return mesh;
    }

    Mesh CreateSfear(int rings = 15, int segments = 15)
    {
        Mesh mesh;

        for (int ring = 0; ring <= rings; ++ring)
        {
            float theta = ring * PI / rings;
            float y = cos(theta);
            float r = sin(theta);

            for (int seg = 0; seg <= segments; ++seg)
            {
                float phi = seg * 2 * PI / segments;
                float x = r * cos(phi);
                float z = r * sin(phi);
                mesh.points.push_back({x, y, z});
            }
        }

        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = ring * (segments + 1) + seg;
                int next = current + segments + 1;

                mesh.faces.push_back({current, next, current + 1});
                mesh.faces.push_back({current + 1, next, next + 1});
            }
        }

        return mesh;
    }

    Mesh CreateCilinder(int segments = 15)
    {
        Mesh mesh;

        float halfHeight = 1.0f * 0.5f;
        float angleStep = (2.0f * PI) / segments;

        for (int i = 0; i < segments; i++)
        {
            float angle = i * angleStep;
            float x = cos(angle) * 0.5f;
            float z = sin(angle) * 0.5f;
            mesh.points.push_back(Vector3(x, halfHeight, z));
        }

        for (int i = 0; i < segments; i++)
        {
            float angle = i * angleStep;
            float x = cos(angle) * 0.5f;
            float z = sin(angle) * 0.5f;
            mesh.points.push_back(Vector3(x, -halfHeight, z));
        }

        int topCenterIndex = (int)mesh.points.size();
        mesh.points.push_back(Vector3(0, halfHeight, 0));

        int bottomCenterIndex = (int)mesh.points.size();
        mesh.points.push_back(Vector3(0, -halfHeight, 0));

        for (int i = 0; i < segments; i++)
        {
            int next = (i + 1) % segments;

            int top1 = i;
            int top2 = next;
            int bottom1 = i + segments;
            int bottom2 = next + segments;

            mesh.faces.push_back({ top1, bottom1, bottom2 });
            mesh.faces.push_back({ top1, bottom2, top2 });
        }

        for (int i = 0; i < segments; i++)
        {
            int next = (i + 1) % segments;
            mesh.faces.push_back({ topCenterIndex, i, next });
            mesh.faces.push_back({ bottomCenterIndex, next + segments, i + segments });
        }

        return mesh;
    }
}