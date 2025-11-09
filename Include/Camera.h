#pragma once

class Camera
{
private:
    Vector3 targetPosition;
    Vector3 lookDirection;
    
    Vector3 position;
    Vector3 angle;

    float fov;
    float aspect;

    bool cameraUpdate;
    Camera()
    {
        const Matrix &RotationMatrix = RotateZYX_Matrix(angle);
        lookDirection = RotationMatrix * Vector3::FORWARD;
        targetPosition = position + lookDirection;

        Matrix CameraMatrix = Camera_Matrix(position, targetPosition, Vector3::UP);
        ViewMatrix = CameraMatrix.transpose();
        cameraUpdate = true;
    }

public:
    Camera(const Camera &) = delete;
    void operator=(const Camera &) = delete;

    static Matrix ViewMatrix;
    static Matrix PerspectiveMatrix;

    void Init(float fov, float aspect)
    {
        this->fov = fov;
        this->aspect = aspect;

        PerspectiveMatrix = Perspective_Matrix(this->fov, this->aspect, 0.1f, 100.0f);
    }

    static Camera &GetInstance()
    {
        static Camera instance;
        return instance;
    }

    static Vector3 GetPosition()
    {
        auto &instance = Camera::GetInstance();
        return instance.position;
    } 
    
    static Vector3 GetAnlge()
    {
        auto &instance = Camera::GetInstance();
        return instance.angle;
    } 

    static void Rotate(const Vector3 &amount)
    {
        auto &instance = Camera::GetInstance();

        instance.angle += amount;
        
        const Matrix &RotationMatrix = RotateZYX_Matrix(instance.angle);
        instance.lookDirection = RotationMatrix * Vector3::FORWARD;
        instance.cameraUpdate = true;
    }
    
    static void SetAngle(const Vector3 &angle)
    {
        auto &instance = Camera::GetInstance();

        instance.angle = angle;
        
        const Matrix &RotationMatrix = RotateZYX_Matrix(instance.angle);
        instance.lookDirection = RotationMatrix * Vector3::FORWARD;
        instance.cameraUpdate = true;
    }

    static void Move(const Vector3 &amount)
    {
        auto &instance = Camera::GetInstance();

        instance.position += amount;
        instance.cameraUpdate = true;
    }
    
    static void SetPosition(const Vector3 &position)
    {
        auto &instance = Camera::GetInstance();

        instance.position = position;
        instance.cameraUpdate = true;
    }

    static void Update()
    {
        auto &instance = Camera::GetInstance();

        if(instance.cameraUpdate)
        {
            instance.targetPosition = instance.position + instance.lookDirection;
            Matrix CameraMatrix = Camera_Matrix(instance.position, instance.targetPosition, Vector3::UP);
            ViewMatrix = CameraMatrix.transpose();
            
            instance.cameraUpdate = false;
        }
    }
};

Matrix Camera::ViewMatrix;
Matrix Camera::PerspectiveMatrix;