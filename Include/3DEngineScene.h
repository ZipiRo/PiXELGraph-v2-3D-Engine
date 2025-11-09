#include "Core/PiXELGraph.h"

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"

float ScreenWidth;
float ScreenHeight;

Vector3 lightDirection;

#include "Camera.h"
#include "Mesh.h"

class EngineScene : public PiXELGraph
{
private:
    float timer = 1;

    Mesh cube;
    Mesh piram;
    Mesh sfear;
    Mesh mesh;
    Mesh xAxis;
    Mesh yAxis;
    Mesh zAxis;

    Text fps_text;

    void Start() override
    {
        ScreenWidth = Screen::Width();
        ScreenHeight = Screen::Height();

        float fov = PI / 2;
        float aspect = ScreenWidth / ScreenHeight;
        
        Camera::GetInstance().Init(fov, aspect);

        lightDirection = Vector3(0, 0, -1);

        fps_text.transform.SetPosition(0, Screen::Height() - 5);
        fps_text.transform.SetScale(4, 4);
        fps_text.SetColor(Color::Black);

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
            Vector3 cameraAngle = Vector3(pitchDirection * (PI / 6) * Time::deltaTime, 
                            yawDirection * (PI / 6) * Time::deltaTime, 
                            rollDirection * (PI / 6) * Time::deltaTime);

            Camera::Rotate(NormalizeAngle(cameraAngle));
        }

        if(cameraDirection.x != 0 || cameraDirection.y != 0 || cameraDirection.z != 0)
        {
            Vector3 cameraPosition = Vector3::Normalize(cameraDirection) * 8.0f * Time::deltaTime;
            Camera::Move(cameraPosition);
        }

        Camera::Update();

        cube.angle.z -= PI / 2 * Time::deltaTime;
        cube.angle.x -= PI / 2 * Time::deltaTime;
        cube.angle.y += PI / 2 * Time::deltaTime;

        piram.angle.z += PI / 2 * Time::deltaTime;
        piram.angle.x += PI / 2 * Time::deltaTime;
        piram.angle.y += PI / 2 * Time::deltaTime;

        sfear.angle.z += PI / 2 * Time::deltaTime;
        sfear.angle.x += PI / 2 * Time::deltaTime;
        sfear.angle.y += PI / 2 * Time::deltaTime;
        
        mesh.angle.z += PI / 2 * Time::deltaTime;
        mesh.angle.x += PI / 2 * Time::deltaTime;
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
    }

public:
    EngineScene()
    {
        Screen::BacgroundColor = Color(0, 255, 255);
        MaxFPS = 999;
        Init(1240, 720, 1, L"3D Engine");
    }
};