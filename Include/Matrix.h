class Matrix
{
public:
    int cols, rows;
    float matrix[5][5];

    Matrix()
    {
        this->cols = 1;
        this->rows = 1;

        matrix[0][0] = 0;
    }

    Matrix(int rows, int cols)
    {
        this->cols = cols;
        this->rows = rows;

        for(int i = 0; i < rows; i++)
            for(int j = 0; j < cols; j++)
                matrix[i][j] = 0;
    }

    float &operator()(int row, int col)
    {
        if(row >= rows || col >= cols)
            throw Error("The index doesn't exit!");
        
        return matrix[row][col];
    }

    Matrix transpose()
    {
        Matrix new_matrix = *this;

        for(int i = 0; i < rows; i++)
            for(int j = i + 1; j < cols; j++)
                new_matrix.matrix[i][j] = matrix[j][i];
            
        return new_matrix;
    }

    Matrix operator*(const Matrix &other) const
    {
        if(this->cols != other.rows)
        return Matrix();

        Matrix result(this->rows, other.cols);

        for(int i = 0; i < this->rows; i++)
            for(int j = 0; j < other.cols; j++)
                for(int k = 0; k < this->cols; k++)
                    result.matrix[i][j] += this->matrix[i][k] * other.matrix[k][j];
    
        return result;
    }

    friend Vector4 operator*(const Matrix &matrix, const Vector4 &vector);
};

Vector4 operator*(const Matrix &matrix, const Vector4 &vector)
{
    Matrix vectorMatrix(4, 1);
    vectorMatrix(0, 0) = vector.x;
    vectorMatrix(1, 0) = vector.y;
    vectorMatrix(2, 0) = vector.z;
    vectorMatrix(3, 0) = vector.w; 

    Matrix resultMatrix = matrix * vectorMatrix;

    return Vector4(resultMatrix(0, 0), resultMatrix(1, 0), resultMatrix(2, 0), resultMatrix(3, 0));
}

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

Matrix RotateZYX_Matrix(const Vector3 &angle)
{
    return RotateZ_Matrix(angle.z) * RotateY_Matrix(angle.y) * RotateX_Matrix(angle.x);
}

Matrix RotateY_MatrixXZ_Matrix(const Vector3 &angle)
{
    return RotateY_Matrix(angle.y) * RotateX_Matrix(angle.x) * RotateZ_Matrix(angle.z);
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