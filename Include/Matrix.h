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

    Matrix Transpose()
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
};