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

    static Matrix RotateX(float angle);
    static Matrix RotateY(float angle);
    static Matrix RotateZ(float angle);
    static Matrix Translate(const Vector3 &position); 
    static Matrix Scale(const Vector3 &Scale); 

    friend Vector4 operator*(const Matrix &matrix, const Vector4 &vector);
};

Matrix Matrix::RotateX(float angle)
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
Matrix Matrix::RotateY(float angle)
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
Matrix Matrix::RotateZ(float angle)
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
Matrix Matrix::Translate(const Vector3 &position)
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
Matrix Matrix::Scale(const Vector3 &scale)
{
    Matrix matrix(4, 4);

    matrix(0, 0) = scale.x;
    matrix(1, 1) = scale.y;
    matrix(2, 2) = scale.z;
    matrix(3, 3) = 1;

    return matrix;
}

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

Matrix RotateZYX_Matrix(const Vector3 &angle)
{
    return Matrix::RotateZ(angle.z) * Matrix::RotateY(angle.y) * Matrix::RotateX(angle.x);
}

Matrix RotateYXZ_Matrix(const Vector3 &angle)
{
    return Matrix::RotateY(angle.y) * Matrix::RotateX(angle.x) * Matrix::RotateZ(angle.z);
}