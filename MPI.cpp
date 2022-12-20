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

vector<vector<int>> cofactor(vector<vector<int>>& vec, int row, int col) {
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
            vector<vector<int>> cof = cofactor(vec, 0, i);
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
                vector<vector<int>> cof = cofactor(mat, 0, i);
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

//////////////////////////////////////////////////////////////////////////////////////
// MPI_SEND & MPI_RECV 
/*// Initialize the MPI environment
MPI_Init(NULL, NULL);

// Get the number of processes
int world_size;
MPI_Comm_size(MPI_COMM_WORLD, &world_size);

// Get the rank of the process
int world_rank;
MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

double start_time = MPI_Wtime();
double end_time;
int number;
if (world_rank == 0) {
    number = -1;
    MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
}
else if (world_rank == 1) {
    MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    end_time = MPI_Wtime();
    printf("Process 1 received number %d from process 0\n", number);
    std::cout << "Waktu " << (end_time - start_time);
}

// Finalize the MPI environment
MPI_Finalize();*/

//////////////////////////////////////////////////////////////////////////////////////
// MPI_Status
/*MPI_Init(NULL, NULL);

int world_size; MPI_Comm_size(MPI_COMM_WORLD, &world_size);
int world_rank; MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
const int MAX_NUMBERS = 100;
int numbers[MAX_NUMBERS];
int number_amount;

if (world_rank == 0) {
    // Pick a random amount of integers to send to process one
    srand((unsigned int)time(NULL));
    number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;

    // Send the amount of integers to process one
    MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
    printf("0 sent %d numbers to 1\n", number_amount);
}
else if (world_rank == 1) {
    MPI_Status status;
    // Receive at most MAX_NUMBERS from process zero
    MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    // After receiving the message, check the status to determince
    // how many numbers were actually received
    MPI_Get_count(&status, MPI_INT, &number_amount);

    // Print off the amount of numbers, and also print additional
    // information in the status object
    printf("1 received %d numbers from 0. Message source = %d, tag = %d\n",
        number_amount, status.MPI_SOURCE, status.MPI_TAG);
}

MPI_Finalize();*/


////////////////////////////////////////////////////////////////////////////////////////
// MPI_Prob
/*MPI_Init(NULL, NULL);
int world_size; MPI_Comm_size(MPI_COMM_WORLD, &world_size);
int world_rank; MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

int number_amount;
if (world_rank == 0) {
    const int MAX_NUMBERS = 100;
    int numbers[MAX_NUMBERS];

    // Pick a random amount of integers to send to process one
    srand(time(NULL));
    number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;

    // Send the random amount of integers to process one
    MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
    printf("0 sent %d numbers to 1\n", number_amount);
}
else if (world_rank == 1) {
    MPI_Status status;

    // Probe for an incoming message from process zero
    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

    // When probe returns, the status object has the size and other
    // attributes of the incoming message. Get the message size
    MPI_Get_count(&status, MPI_INT, &number_amount);

    // Allocate a buffer to hold the incoming numbers
    int* number_buf = (int*)malloc(sizeof(int) * number_amount);

    // Now receive the message with the allocated buffer
    MPI_Recv(number_buf, number_amount, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("1 dynamically received %d numbers from 0.\n", number_amount);
    free(number_buf);
}

MPI_Finalize();*/

//////////////////////////////////////////////////////////////////////////////////
//
/*typedef struct {
    int location;
    int num_steps_left_in_walk;
} Walker;

void decompose_domain(int domain_size, int world_rank,
    int world_size, int* subdomain_start,
    int* subdomain_size) {
    if (world_size > domain_size) {
        // Don't worry about this special case. Assume the domain
        // size is greater than the world size.
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    *subdomain_start = domain_size / world_size * world_rank;
    *subdomain_size = domain_size / world_size;
    if (world_rank == world_size - 1) {
        // Give remainder to last process
        *subdomain_size += domain_size % world_size;
    }
}

void initialize_walkers(int num_walkers_per_proc, int max_walk_size,
    int subdomain_start, int subdomain_size,
    vector<Walker>* incoming_walkers) {
    Walker walker;
    for (int i = 0; i < num_walkers_per_proc; i++) {
        // Initialize walkers in the middle of the subdomain
        walker.location = subdomain_start;
        walker.num_steps_left_in_walk = (rand() / (float)RAND_MAX) * max_walk_size;
        incoming_walkers->push_back(walker);
    }
}

void walk(Walker* walker, int subdomain_start, int subdomain_size,
    int domain_size, vector<Walker>* outgoing_walkers) {
    while (walker->num_steps_left_in_walk > 0) {
        if (walker->location == subdomain_start + subdomain_size) {
            // Take care of the case when the walker is at the end
            // of the domain by wrapping it around to the beginning
            if (walker->location == domain_size) {
                walker->location = 0;
            }
            outgoing_walkers->push_back(*walker);
            break;
        }
        else {
            walker->num_steps_left_in_walk--;
            walker->location++;
        }
    }
}

void send_outgoing_walkers(vector<Walker>* outgoing_walkers,
    int world_rank, int world_size) {
    // Send the data as an array of MPI_BYTEs to the next process.
    // The last process sends to process zero.
    MPI_Send((void*)outgoing_walkers->data(),
        outgoing_walkers->size() * sizeof(Walker), MPI_BYTE,
        (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);

    // Clear the outgoing walkers
    outgoing_walkers->clear();
}

void receive_incoming_walkers(vector<Walker>* incoming_walkers,
    int world_rank, int world_size) {
    MPI_Status status;

    // Receive from the process before you. If you process zero,
    // receive from the last process
    int incoming_rank = (world_rank == 0) ? world_size - 1 : world_rank - 1;
    MPI_Probe(incoming_rank, 0, MPI_COMM_WORLD, &status);

    // Resize your incoming walker buffer based on how much data is
    // being received
    int incoming_walkers_size;
    MPI_Get_count(&status, MPI_BYTE, &incoming_walkers_size);
    incoming_walkers->resize(incoming_walkers_size / sizeof(Walker));

    MPI_Recv((void*)incoming_walkers->data(), incoming_walkers_size, MPI_BYTE,
        incoming_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}*/
