#include <iostream>
#include <vector>

using namespace std;

// Matrix operations helper functions (prototypes)
void multiplyMatrix(double A[], double B[], int sizeA, int sizeB, double result[]);
void addMatrices(double A[], double B[], int size, double result[]);
void identityMatrix(double I[], int size);
void matrixTranspose(double transposed[], double original[], int size);

int main() {
    // Example implementation of Kalman Filter

    const int STATE_SIZE = 2; // x and dx (velocity)
    const int MEASUREMENTS = 1; // Only position is measured

    vector<double> x(STATE_SIZE);   // State vector: x[0] = position, x[1] = velocity
    vector<double> y;               // Measurement vector
    vector<vector<double>> A(STATE_SIZE, vector<double>(STATE_SIZE)); // State transition matrix
    vector<vector<double>> H(MEASUREMENTS, vector<double>(STATE_SIZE)); // Measurement matrix
    vector<vector<double>> P(STATE_SIZE, vector<double>(STATE_SIZE));  // Covariance matrix
    vector<vector<double>> K(STATE_SIZE, vector<double>(MEASUREMENTS)); // Kalman gain

    // Initialize matrices with zeros
    fill(A.begin(), A.end(), vector<double>(A[0].size(), 0.0));
    fill(H.begin(), H.end(), vector<double>(H[0].size(), 0.0));
    fill(P.begin(), P.end(), vector<double>(P[0].size(), 0.0));
    fill(K.begin(), K.end(), vector<double>(K[0].size(), 0.0));

    // Define state transition matrix A
    A[0][0] = 1.0;
    A[0][1] = 1.0;  // dt (delta time) assumed to be 1 for simplicity
    A[1][0] = 0.0;
    A[1][1] = 1.0;

    // Define measurement matrix H: only position is measured
    H[0][0] = 1.0;
    H[0][1] = 0.0;

    // Initialize state vector with initial guess (position)
    x[0] = 0.0;  // Initial position at time t=0
    x[1] = 0.0;  // Initial velocity

    // Initial covariance matrix setup
    fill(P.begin(), P.end(), vector<double>(P[0].size(), 1e-4));  // Small initial uncertainty

    // Simulate measurements (for demonstration purposes)
    double dt = 1.0;
    double process_noise = 0.2;
    double measurement_noise = 1.0;

    // Generate simulated state and measurements
    vector<double> x_true(STATE_SIZE);
    vector<double> z(MEASUREMENTS); // Noisy measurements

    // Simulate true states (for demonstration)
    x_true[0] = dt;  // True position at t=1
    x_true[1] = 0.0; // Zero velocity
    z[0] = x_true[0] + measurement_noise * rand(); // Noisy measurement

    // Time Update (Prediction)
    vector<double> x_pred(STATE_SIZE);
    multiplyMatrix(&A[0][0], &x[0], STATE_SIZE, 1, &x_pred[0]); // x = A * x
    vector<vector<double>> P_pred(STATE_SIZE, vector<double>(STATE_SIZE));
    multiplyMatrix(&A[0][0], &P[0][0], STATE_SIZE, STATE_SIZE); // temp = A * P
    multiplyMatrix(temp, A, STATE_SIZE, STATE_SIZE, &P_pred[0][0]); // P = A*P*A^T + Q (assuming process noise)

    // Measurement Update (Correction)
    vector<vector<double>> HTH(MEASUREMENTS, vector<double>(MEASUREMENTS));
    multiplyMatrix(&H[0][0], &H[0][0], MEASUREMENTS, STATE_SIZE); // H * H^T
    addMatrices(&HTH[0][0], &R[0][0], MEASUREMENTS, &HTH[0][0]); // R is measurement noise covariance

    vector<vector<double>> S(MEASUREMENTS, vector<double>(MEASUREMENTS));
    multiplyMatrix(&H[0][0], &P_pred[0][0], MEASUREMENTS, STATE_SIZE);
    multiplyMatrix(temp, H, MEASUREMENTS, STATE_SIZE); // temp = H * P
    addMatrices(&S[0][0], &temp[0][0], MEASUREMENTS, &S[0][0]); // S = H*P*H^T + R

    vector<vector<double>> inv_S(MEASUREMENTS, vector<double>(MEASUREMENTS));
    invertMatrix(S, MEASUREMENTS); // Invert the covariance matrix

    multiplyMatrix(&P_pred[0][0], &H[0][0], STATE_SIZE, STATE_SIZE);
    multiplyMatrix(temp, inv_S, STATE_SIZE, MEASUREMENTS); // K = P * H^T * S^{-1}

    vector<double> y(STATE_SIZE); // Innovation
    for (int i = 0; i < STATE_SIZE; ++i) {
        for (int j = 0; j < MEASUREMENTS; ++j) {
            y[i] += K[i][j] * (z[j] - H[j][i] * x_pred[i]);
        }
    }

    // Update state vector
    addMatrices(&x[0], &y[0], STATE_SIZE, &x[0]);

    // Update covariance matrix
    multiplyMatrix(&K[0][0], S, STATE_SIZE, MEASUREMENTS);
    transpose(K); // K^T
    subtractMatrices(&P_pred[0][0], &K[0][0], STATE_SIZE, P);

}

