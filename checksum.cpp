#include <cstdio>
#include <mpi.h>
#include <tuple>
#include <cmath>
#include <string>

/*
    You can modify the following namespace.
*/
namespace attention {
    struct Matrix {
        int row, col;
        double **data;

        Matrix(int row, int col) : row(row), col(col) {
            data = new double*[row];
            for (int i = 0; i < row; ++i) {
                data[i] = new double[col];
            }
        }

        ~Matrix() {
            for (int i = 0; i < row; ++i) {
                delete[] data[i];
            }
            delete[] data;
        }
    };



}



int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    // Prepare for matrices and the ans.
    attention::Matrix *q, *k, *v;
    double ans;
    if (rank == 0) {
        printf("Matrix size: %d x %d\n", q->row, q->col);
        printf("Ans: %.10lf\n", ans);
    }

    // Start attention.
    auto start = MPI_Wtime();
    auto qkv = attention::attention(q, k, v);
    auto end = MPI_Wtime();

    // Reduce the answer
    double qkv_ans = reduce_the_sum(qkv);
    if (rank == 0) {
        printf("Your answer: %.10lf\n", qkv_ans);

        // Check the answer.
        bool correct = check(qkv_ans, ans);

        // Output the result.
        if (correct) {
            printf("Correct! Time: %.10lf\n", end - start);
        } else {
            printf("Wrong!\n");
        }
    }
    MPI_Finalize();

    return 0;
}