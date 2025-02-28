#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define state vector size (12 states)
#define STATE_SIZE 12

// Define input vector size (6 inputs)
#define INPUT_SIZE 6

// State transition matrix (A)
static const double A[STATE_SIZE][STATE_SIZE] = {
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

// Control input matrix (B)
static const double B[STATE_SIZE][INPUT_SIZE] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
};

// Measurement matrix (C)
static const double C[9][STATE_SIZE] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

// Process noise covariance (Q)
static const double Q[STATE_SIZE][STATE_SIZE] = {
    {0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0.1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0.1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0.1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.1}
};

// Measurement noise covariance (R)
static const double R[9][9] = {
    {1e-4, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1e-4, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1e-4, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1e-4, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1e-4, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1e-4, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1e-4, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1e-4, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1e-4}
};

// Initial state vector (x0)
static const double x0[STATE_SIZE] = {
    0,  // position (x)
    0,  // velocity (vx)
    0,  // acceleration (ax)
    0,  // position (y)
    0,  // velocity (vy)
    0,  // acceleration (ay)
    0,  // position (z)
    0,  // velocity (vz)
    0,  // acceleration (az)
    0,  // angular velocity (roll)
    0,  // angular velocity (pitch)
    0   // angular velocity (yaw)
};

// Initial state covariance (P0)
static const double P0[STATE_SIZE][STATE_SIZE] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};

// Function to perform Kalman filter prediction step
void kalman_predict(double x[STATE_SIZE], double P[STATE_SIZE][STATE_SIZE]) {
    // State transition matrix (F)
    static const double F[STATE_SIZE][STATE_SIZE] = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
    };

    // Control input matrix (B)
    static const double B[STATE_SIZE][STATE_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    // Process noise covariance (Q)
    static const double Q[STATE_SIZE][STATE_SIZE] = {
        {1e-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1e-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1e-4, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1e-4, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1e-4, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1e-4, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1e-4, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1e-4, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1e-4, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1e-4, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1e-4, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1e-4}
    };

    // Compute F * x
    double Fx[STATE_SIZE];
    for (int i = 0; i < STATE_SIZE; ++i) {
        Fx[i] = 0;
        for (int j = 0; j < STATE_SIZE; ++j) {
            Fx[i] += F[i][j] * x[j];
        }
    }

    // Compute B * u
    double Bu[STATE_SIZE];
    for (int i = 0; i < STATE_SIZE; ++i) {
        Bu[i] = 0;
        for (int j = 0; j < STATE_SIZE; ++j) {
            Bu[i] += B[i][j] * u[j];
        }
    }

    // Update x
    for (int i = 0; i < STATE_SIZE; ++i) {
        x[i] = Fx[i] + Bu[i];
    }

    // Compute new covariance P
    double new_P[STATE_SIZE][STATE_SIZE];
    for (int i = 0; i < STATE_SIZE; ++i) {
        for (int j = 0; j < STATE_SIZE; ++j) {
            new_P[i][j] = 0;
            for (int k = 0; k < STATE_SIZE; ++k) {
                new_P[i][j] += F[i][k] * P[k][i] * F[j][k];
            }
        }
    }

    // Add process noise covariance Q
    for (int i = 0; i < STATE_SIZE; ++i) {
        for (int j = 0; j < STATE_SIZE; ++j) {
            new_P[i][j] += Q[i][j];
        }
    }

    // Update P with new_P
    memcpy(P, new_P, sizeof(new_P));
}

