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
Matrix CameraMatrix;
Matrix cameraRotationMatrix;
bool cameraUpdate;

Vector3 lightDirection;

Matrix Model_Matrix(const Vector3 &position, const Vector3 &scale, const Vector3 &angle)
{
    return Matrix::Translate(position) * RotateZYX_Matrix(angle) * Matrix::Scale(scale);
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
    Mesh xAxis;
    Mesh yAxis;
    Mesh zAxis;

    Text fps_text;
    Text cameraPos_text;

    void Start() override
    {
        fps_text.transform.SetPosition(0, Screen::Height() - 5);
        fps_text.transform.SetScale(4, 4);
        fps_text.SetColor(Color::Black);

        cameraPos_text.transform.SetPosition(0, 0);
        cameraPos_text.transform.SetScale(4, 4);
        cameraPos_text.SetColor(Color::Black);
        cameraPos_text.SetString("X:" + std::to_string((int)cameraPosition.x) + " Y:" + std::to_string((int)cameraPosition.y) + " Z:" + std::to_string((int)cameraPosition.z) + "\n" +
                    "PITCH:" + std::to_string((int)(cameraAngle.x * RAD_TO_DEG)) + " YAW:" + std::to_string((int)(cameraAngle.y * RAD_TO_DEG)) + " ROLL:" + std::to_string((int)(cameraAngle.z * RAD_TO_DEG)));

        ScreenWidth = Screen::Width();
        ScreenHeight = Screen::Height();

        fieldOfView = PI / 2;
        aspectRatio = ScreenWidth / ScreenHeight;

        cameraPosition = Vector3(0, 0, -20);
        targetPosition = Vector3::FORWARD;

        lightDirection = Vector3(-1, -1, -1);

        PerspectiveMatrix = Perspective_Matrix(fieldOfView, aspectRatio, 0.1f, 100.0f);

        cameraRotationMatrix = RotateZYX_Matrix(cameraAngle);
        lookDirection = cameraRotationMatrix * Vector3::FORWARD;
        targetPosition = cameraPosition + lookDirection;

        CameraMatrix = Camera_Matrix(cameraPosition, targetPosition, Vector3::UP);
        ViewMatrix = CameraMatrix.transpose();

        cube = MeshShapes::CreateCube();
        piram = MeshShapes::CreatePiramide();
        sfear = MeshShapes::CreateSfear();
        xAxis = yAxis = zAxis = MeshShapes::CreateCilinder();
        mesh.LoadFromFile("teapot.obj");

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
        sfear.color = Color::Green;
        
        mesh.position = Vector3(0, 0, 0);
        mesh.scale = Vector3(1, 1, 1);
        mesh.angle = Vector3::ZERO;
        mesh.color = Color(123, 23, 67);
        
        xAxis.position = Vector3(0, 0, 0);
        xAxis.scale = Vector3(1, 10, 1);
        xAxis.angle = Vector3(0, 0, PI / 2);
        xAxis.color = Color(255, 0, 0);

        yAxis.position = Vector3(0, 0, 0);
        yAxis.scale = Vector3(1, 10, 1);
        yAxis.angle = Vector3(0, 0, 0);
        yAxis.color = Color(0, 0, 255);

        zAxis.position = Vector3(0, 0, 0);
        zAxis.scale = Vector3(1, 10, 1);
        zAxis.angle = Vector3(PI / 2, 0, 0);
        zAxis.color = Color(0, 255, 0);
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
        int yawDirection = 0;
        int pitchDirection = 0;
        int rollDirection = 0;

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

        if (Input::IsKey(Key::Key_E))
            rollDirection += 1;

        if (Input::IsKey(Key::Key_Q))
            rollDirection -= 1;

        if(yawDirection != 0 || pitchDirection != 0 || rollDirection != 0)
        {
            cameraAngle += Vector3(pitchDirection * (PI / 6) * Time::deltaTime, yawDirection * (PI / 6) * Time::deltaTime, rollDirection * (PI / 6) * Time::deltaTime);
            cameraAngle = NormalizeAngle(cameraAngle);
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
            cameraPos_text.SetString("X:" + std::to_string((int)cameraPosition.x) + " Y:" + std::to_string((int)cameraPosition.y) + " Z:" + std::to_string((int)cameraPosition.z) + "\n" +
                    "PITCH:" + std::to_string((int)(cameraAngle.x * RAD_TO_DEG)) + " YAW:" + std::to_string((int)(cameraAngle.y * RAD_TO_DEG)) + " ROLL:" + std::to_string((int)(cameraAngle.z * RAD_TO_DEG)));

            targetPosition = cameraPosition + lookDirection;
            CameraMatrix = Camera_Matrix(cameraPosition, targetPosition, Vector3::UP);
            ViewMatrix = CameraMatrix.transpose();
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
        
        
        // mesh.angle.z += PI / 2 * Time::deltaTime;
        // mesh.angle.x += PI / 2 * Time::deltaTime;
        mesh.angle.y += PI / 2 * Time::deltaTime;

    }

    void Draw() override
    {
        // xAxis.Draw();
        // yAxis.Draw();
        // zAxis.Draw();

        cube.Draw();
        // piram.Draw();
        // sfear.Draw();
        // mesh.Draw(); 

        DrawTEXT(fps_text);
        DrawTEXT(cameraPos_text);
    }

public:
    EngineScene()
    {
        Screen::BacgroundColor = Color(0, 255, 255);
        MaxFPS = 999;
        Init(1240, 720, 1, L"3D Engine");
    }
};