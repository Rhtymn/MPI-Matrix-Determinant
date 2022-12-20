#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

void print2dVec(vector<vector<int>>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        for (int j = 0; j < vec.at(i).size(); j++) {
            cout << ' ' << vec.at(i)[j];
        }
        cout << endl;
    }
}

vector<vector<int>> generateRandomMatrix(int N) {
    // Seed the random number generator with the current time
    // srand(time(nullptr));

    // Create a 2D vector of integers
    vector<vector<int>> vec;

    // Generate N x N matrix in vector
    for (int i = 0; i < N; i++) {
        vector<int> innerVec;
        for (int j = 0; j < N; j++) {
            // Generate a random number in the range [0,5]
            int num = rand() % 5;

            // Push num to the innerVec
            innerVec.push_back(num);
        }
        // Push innerVec to vec
        vec.push_back(innerVec);
    }

    return vec;
}

vector<vector<int>> minor(vector<vector<int>>& vec, int row, int col) {
    vector<vector<int>> result;
    for (int i = 0; i < vec.size(); i++) {
        if (i == row) continue;
        vector<int> temp;
        for (int j = 0; j < vec.at(i).size(); j++) {
            if (j == col) continue;
            temp.push_back(vec.at(i)[j]);
        }
        result.push_back(temp);
    }
    return result;
}

int determinant(vector<vector<int>>& vec) {
    if (vec.size() == 2) {
        return (vec.at(0)[0] * vec.at(1)[1]) - (vec.at(0)[1] * vec.at(1)[0]);
    }
    else {
        int result = 0;
        for (int i = 0; i < vec.at(0).size(); i++) {
            if (vec.at(0)[i] == 0) continue;
            vector<vector<int>> cof = minor(vec, 0, i);
            if ((i + 1) % 2 != 0) {
                result += vec.at(0)[i] * determinant(cof);
            }
            else {
                result -= vec.at(0)[i] * determinant(cof);
            }
        }
        return result;
    }
}

int main(int argc, char** argv) {
    // get N from an argument pass in cmd
    int N = atoi(argv[1]);

    // Generate random matrix N x N
    vector <vector<int>> mat = generateRandomMatrix(N);

    // Initialization of parallel using MPI
    MPI_Init(NULL, NULL);

    // Get num of processor 
    int world_size; MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get rank of processor
    int world_rank; MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Task decomposition
    int start_pos = 0;
    int end_pos = 0; 
    int task_per_process = N / world_size == 0 ? 1 : N / world_size;
    if (world_size >= N) {
        if (world_rank < N) {
            start_pos = task_per_process * world_rank; // inclusive
            end_pos = start_pos + task_per_process; // exclusive
        }
    }
    else {
        start_pos = task_per_process * world_rank; // inclusive
        end_pos = start_pos + task_per_process; // exclusive

        if (world_rank == world_size - 1) {
            if (N % world_size > 0 && N > world_size) {
                end_pos += (N % world_size);
            }
            else if (end_pos == start_pos + 1 && N < world_size) {
                end_pos = start_pos;
            }
        }
    }
    // cout << "Process " << world_rank << " start on " << start_pos << " end on " << end_pos;
    
    int curr_rank_result = 0;
    int global_result = 0;

    // start timer
    double start_time = MPI_Wtime();

    // Parallel execution to find determinant of matrix
    if (mat.size() == 1) {
        curr_rank_result = mat.at(0)[0];
    }
    else if (mat.size() == 2) {
        curr_rank_result = (mat.at(0)[0] * mat.at(1)[1]) - (mat.at(0)[1] * mat.at(1)[0]);
    }
    else {
            for (int i = start_pos; i < end_pos; i++) {
                if (mat.at(0)[i] == 0) continue;
                vector<vector<int>> cof = minor(mat, 0, i);
                // Make cofactor
                if ((i + 1) % 2 != 0) {
                    curr_rank_result += mat.at(0)[i] * determinant(cof);
                }
                else {
                    curr_rank_result -= mat.at(0)[i] * determinant(cof);
                }
            }
    }

    if (mat.size() > 2) {
        MPI_Barrier(MPI_COMM_WORLD);
        // MPI_Allreduce(&curr_rank_result, &global_result, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);
        MPI_Reduce(&curr_rank_result, &global_result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    // Stop timer
    double end_time = MPI_Wtime();

    // Show result
    if (world_rank == 0) {
        int determinant = mat.size() > 2 ? global_result : curr_rank_result;
        cout << "Matrix : \n";
        print2dVec(mat);
        cout << "Determinant : " << determinant << "\n";
        cout << "Time elapsed : " << end_time - start_time << "seconds\n";
    }
    MPI_Finalize();
    return 0;
}
