#include "Core/PiXELGraph.h"

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

float ScreenWidth;
float ScreenHeight;
Matrix PerspectiveMatrix;
Matrix ViewMatrix;

Vector3 cameraPosition;
Vector3 targetPosition;
Vector3 lookDirection;
Vector3 cameraAngle;
Matrix cameraRotationMatrix;
bool cameraUpdate;

Vector3 lightDirection;

Matrix RotateX_Matrix(float angle)
{
    Matrix matrix(4, 4);

    float sinX = sin(angle);
    float cosX = cos(angle);

    matrix(0, 0) = 1;
    matrix(1, 1) = cosX;
    matrix(1, 2) = -sinX;
    matrix(2, 1) = sinX;
    matrix(2, 2) = cosX;
    matrix(3, 3) = 1;

    return matrix;
}
Matrix RotateY_Matrix(float angle)
{
    Matrix matrix(4, 4);

    float sinY = sin(angle);
    float cosY = cos(angle);

    matrix(0, 0) = cosY;
    matrix(0, 2) = sinY;
    matrix(1, 1) = 1;
    matrix(2, 0) = -sinY;
    matrix(2, 2) = cosY;
    matrix(3, 3) = 1;

    return matrix;
}
Matrix RotateZ_Matrix(float angle)
{
    Matrix matrix(4, 4);

    float sinZ = sin(angle);
    float cosZ = cos(angle);

    matrix(0, 0) = cosZ;
    matrix(0, 1) = -sinZ;
    matrix(1, 0) = sinZ;
    matrix(1, 1) = cosZ;
    matrix(2, 2) = 1;
    matrix(3, 3) = 1;

    return matrix;
}

Matrix RotateZYX_Matrix(const Vector3 &angle)
{
    return RotateZ_Matrix(angle.z) * RotateY_Matrix(angle.y) * RotateX_Matrix(angle.x);
}
Matrix RotateYXZ_Matrix(const Vector3 &angle)
{
    return RotateY_Matrix(angle.y) * RotateX_Matrix(angle.x) * RotateZ_Matrix(angle.z);
}
Matrix Translate_Matrix(const Vector3 &position)
{
    Matrix matrix(4, 4);

    matrix(0, 0) = 1;
    matrix(1, 1) = 1;
    matrix(2, 2) = 1;
    matrix(3, 3) = 1;
    matrix(0, 3) = position.x;
    matrix(1, 3) = position.y;
    matrix(2, 3) = position.z;

    return matrix;
}
Matrix Scale_Matrix(const Vector3 &scale)
{
    Matrix matrix(4, 4);

    matrix(0, 0) = scale.x;
    matrix(1, 1) = scale.y;
    matrix(2, 2) = scale.z;
    matrix(3, 3) = 1;

    return matrix;
}

Matrix Model_Matrix(const Vector3 &position, const Vector3 &scale, const Vector3 &angle)
{
    return Translate_Matrix(position) * RotateZYX_Matrix(angle) * Scale_Matrix(scale);
}

Matrix Camera_Matrix(const Vector3 &cameraPosition, const Vector3 &targetPosition, const Vector3 &upVector = Vector3::UP)
{
    Vector3 forward = Vector3::Normalize(cameraPosition - targetPosition);
    Vector3 right = Vector3::Normalize(Vector3::CrossProduct(upVector, forward));
    Vector3 up = Vector3::CrossProduct(forward, right);

    Matrix matrix(4, 4);

    matrix(0, 0) = right.x;
    matrix(0, 1) = up.x;
    matrix(0, 2) = -forward.x;

    matrix(1, 0) = right.y;
    matrix(1, 1) = up.y;
    matrix(1, 2) = -forward.y;

    matrix(2, 0) = right.z;
    matrix(2, 1) = up.z;
    matrix(2, 2) = -forward.z;

    matrix(3, 0) = -Vector3::DotProduct(right, cameraPosition);
    matrix(3, 1) = -Vector3::DotProduct(up, cameraPosition);
    matrix(3, 2) = -Vector3::DotProduct(forward, cameraPosition);

    matrix(3, 3) = 1;

    return matrix;
}

Matrix Perspective_Matrix(float fov, float aspect, float nearPlane, float farPlane)
{
    float tan_half_FOV = tanf(fov * 0.5f);
    Matrix matrix(4, 4);

    matrix(0, 0) = 1.0f / (aspect * tan_half_FOV);
    matrix(1, 1) = 1.0f / tan_half_FOV;
    matrix(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
    matrix(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    matrix(3, 2) = -1.0f;
    matrix(3, 3) = 0.0f;

    return matrix;
}

Vector3 NormalizeAngle(Vector3 angle)
{
    return Vector3(NormalizeAngle(angle.x), NormalizeAngle(angle.y), NormalizeAngle(angle.z));
}

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
    int minX = std::min(P1.x, std::min(P2.x, P3.x));
    int maxX = std::max(P1.x, std::max(P2.x, P3.x));
    int minY = std::min(P1.y, std::min(P2.y, P3.y));
    int maxY = std::max(P1.y, std::max(P2.y, P3.y));

    // Clamp to screen if needed
    minX = std::max(minX, 0);
    minY = std::max(minY, 0);
    maxX = std::min(maxX, (int)ScreenWidth - 1);
    maxY = std::min(maxY, (int)ScreenHeight - 1);

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

#include "Mesh.h"

class EngineScene : public PiXELGraph
{
private:
    float timer = 1;

    float fieldOfView;
    float aspectRatio;

    Mesh cube;
    Mesh piram;
    Mesh sfear;
    Mesh mesh;

    Text fps_text;

    Matrix CameraMatrix;

    void Start() override
    {
        fps_text.transform.SetPosition(0, 0);
        fps_text.transform.SetScale(7, 7);
        fps_text.SetColor(Color::White);

        ScreenWidth = Screen::Width();
        ScreenHeight = Screen::Height();

        fieldOfView = PI / 2;
        aspectRatio = ScreenWidth / ScreenHeight;

        cameraPosition = Vector3(0, 0, -30);
        targetPosition = Vector3::FORWARD;

        lightDirection = Vector3(0, 0, -1);

        PerspectiveMatrix = Perspective_Matrix(fieldOfView, aspectRatio, 0.1f, 100.0f);

        cameraRotationMatrix = RotateZYX_Matrix(cameraAngle);
        lookDirection = cameraRotationMatrix * Vector3::FORWARD;
        targetPosition = cameraPosition + lookDirection;

        CameraMatrix = Camera_Matrix(cameraPosition, targetPosition, Vector3::UP);
        ViewMatrix = CameraMatrix.Transpose();

        cube.points = {
            {-0.5f, 0.5f, 0.5f},  // 0
            {0.5f, 0.5f, 0.5f},   // 1
            {0.5f, -0.5f, 0.5f},  // 2
            {-0.5f, -0.5f, 0.5f}, // 3
            {-0.5f, 0.5f, -0.5f}, // 4
            {0.5f, 0.5f, -0.5f},  // 5
            {0.5f, -0.5f, -0.5f}, // 6
            {-0.5f, -0.5f, -0.5f} // 7
        };

        cube.faces = {
            {0, 1, 2}, {0, 2, 3}, // front
            {1, 5, 6},
            {1, 6, 2}, // left
            {5, 4, 7},
            {5, 7, 6}, // back
            {4, 0, 3},
            {4, 3, 7}, // right
            {4, 5, 1},
            {4, 1, 0}, // top
            {3, 2, 6},
            {3, 6, 7} // bottom
        };

        piram.points = {
            {-0.5f, 0.0f, -0.5f}, // 0
            {0.5f, 0.0f, -0.5f},  // 1
            {0.5f, 0.0f, 0.5f},   // 2
            {-0.5f, 0.0f, 0.5f},  // 3
            {0.0f, 1.0f, 0.0f}    // 4
        };

        piram.faces = {
            {2, 1, 0}, {3, 2, 0}, // bottom
            {0, 1, 4},            // front
            {1, 2, 4},            // right
            {2, 3, 4},            // back
            {3, 0, 4}             // left
        };

        int rings = 15;
        int segments = 15;
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
                sfear.points.push_back({x, y, z});
            }
        }

        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int current = ring * (segments + 1) + seg;
                int next = current + segments + 1;

                sfear.faces.push_back({current, next, current + 1});
                sfear.faces.push_back({current + 1, next, next + 1});
            }
        }

        mesh.LoadFromFile("dir.obj");

        cube.position = Vector3(0, 0, 0);
        cube.scale = Vector3(10, 10, 10);
        cube.angle = Vector3::ZERO;
        cube.color = Color(123, 44, 213);

        piram.position = Vector3(0, 0, 0);
        piram.scale = Vector3(10, 10, 10);
        piram.angle = Vector3::ZERO;
        piram.color = Color::Blue;

        sfear.position = Vector3(0, 0, 0);
        sfear.scale = Vector3(10, 10, 10);
        sfear.angle = Vector3::ZERO;
        sfear.color = Color::White;
        
        mesh.position = Vector3(0, 0, 0);
        mesh.scale = Vector3(1, 1, 1);
        mesh.angle = Vector3::ZERO;
        mesh.color = Color::White;
    }

    void Update() override
    {
        timer += Time::deltaTime;
        if (timer >= 1)
        {
            Debug::Log("FPS: " + std::to_string(1 / Time::deltaTime));
            fps_text.SetString("FPS:" + std::to_string((int)(1 / Time::deltaTime)));
            timer = 0;
        }

        Vector3 cameraDirection;
        float yawDirection = 0;
        float pitchDirection = 0;

        if (Input::IsKey(Key::Key_W))
            cameraDirection.z += 1;

        if (Input::IsKey(Key::Key_S))
            cameraDirection.z -= 1;

        if (Input::IsKey(Key::Key_A))
            cameraDirection.x += 1;

        if (Input::IsKey(Key::Key_D))
            cameraDirection.x -= 1;

        if (Input::IsKey(Key::Key_LeftShift))
            cameraDirection.y -= 1;

        if (Input::IsKey(Key::Key_Space))
            cameraDirection.y += 1;

        if (Input::IsKey(Key::Key_LeftArrow))
            yawDirection += 1;

        if (Input::IsKey(Key::Key_RightArrow))
            yawDirection -= 1;

        if (Input::IsKey(Key::Key_DownArrow))
            pitchDirection += 1;

        if (Input::IsKey(Key::Key_UpArrow))
            pitchDirection -= 1;

        if(yawDirection != 0 || pitchDirection != 0)
        {
            cameraAngle += NormalizeAngle(Vector3(pitchDirection * 1.0f * Time::deltaTime, yawDirection * 1.0f * Time::deltaTime, 0));
            cameraRotationMatrix = RotateZYX_Matrix(cameraAngle);
        
            lookDirection = cameraRotationMatrix * Vector3::FORWARD;
            cameraUpdate = true;
        }

        if(cameraDirection.x != 0 || cameraDirection.y != 0 || cameraDirection.z != 0)
        {
            cameraPosition += Vector3::Normalize(cameraDirection) * 8.0f * Time::deltaTime;
            cameraUpdate = true;
        }
        
        if(cameraUpdate)
        {
            targetPosition = cameraPosition + lookDirection;
            CameraMatrix = Camera_Matrix(cameraPosition, targetPosition, Vector3::UP);
            ViewMatrix = CameraMatrix.Transpose();
            cameraUpdate = false;
        }

        cube.angle.z -= PI / 2 * Time::deltaTime;
        cube.angle.x -= PI / 2 * Time::deltaTime;
        cube.angle.y += PI / 2 * Time::deltaTime;

        piram.angle.z += PI / 2 * Time::deltaTime;
        piram.angle.x += PI / 2 * Time::deltaTime;
        piram.angle.y += PI / 2 * Time::deltaTime;

        // sfear.angle.z += PI / 2 * Time::deltaTime;
        // sfear.angle.x += PI / 2 * Time::deltaTime;
        sfear.angle.y += PI / 2 * Time::deltaTime;
    }

    void Draw() override
    {
        // cube.Draw();
        // piram.Draw();
        // sfear.Draw();

        mesh.Draw();

        DrawTEXT(fps_text);
    }

public:
    EngineScene()
    {
        Screen::BacgroundColor = Color::Black;
        MaxFPS = 60;
        Init(1280, 720, 2, L"3D Engine");
    }
};