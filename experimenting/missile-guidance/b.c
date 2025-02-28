#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define MM_SENSOR_ADDR 0x12345678 // Simulated memory mapped address for sensors
#define MM_ACTUATOR_ADDR 0x9ABCDEF0 // Simulated memory mapped address for actuators

// Missile state vector (position, velocity, orientation, angular rates)
typedef struct {
    double x;   // Position in X
    double y;   // Position in Y
    double z;   // Position in Z
    double vx;  // Velocity in X
    double vy;  // Velocity in Y
    double vz;  // Velocity in Z
    double yaw; // Yaw angle
    double pitch;// Pitch angle
    double roll;// Roll angle
    double dyaw;// Angular rate of yaw
    double dpitch;// Angular rate of pitch
    double droll;// Angular rate of roll
} missile_state_t;

// Process noise covariance matrix (Q)
static const double Q[12][12] = {
    {0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.01, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.01}
};

// Measurement covariance matrix (R)
static const double R[9][9] = {
    {0.1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0.1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0.1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0.1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0.1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0.1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0.1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0.1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0.1}
};

// State transition function
void state_transition(double *x, double *u) {
    // Simulates the missile dynamics
    x[0] += u[0];   // X position update
    x[1] += u[1];   // Y position update
    x[2] += u[2];   // Z position update
    x[3] += u[3];   // Velocity in X
    x[4] += u[4];   // Velocity in Y
    x[5] += u[5];   // Velocity in Z
    x[6] += u[6];   // Yaw angle update
    x[7] += u[7];   // Pitch angle update
    x[8] += u[8];   // Roll angle update
    x[9] += u[9];   // Angular rate of yaw
    x[10] += u[10]; // Angular rate of pitch
    x[11] += u[11]; // Angular rate of roll
}

// Measurement function (returns simulated sensor measurements)
void measurement(double *z, const double *x) {
    // Simulates the missile's sensors
    z[0] = x[0];   // X position
    z[1] = x[1];   // Y position
    z[2] = x[2];   // Z position
    z[3] = x[3];   // Velocity in X
    z[4] = x[4];   // Velocity in Y
    z[5] = x[5];   // Velocity in Z
    z[6] = x[6];   // Yaw angle
    z[7] = x[7];   // Pitch angle
    z[8] = x[8];   // Roll angle
}

// Sliding Mode Control (SMC) function
void sliding_mode_control(double *u, const double *x, double *disturbance) {
    // Design the sliding surface: s = x - r (reference)
    double s[3];
    s[0] = x[0];   // Position error in X
    s[1] = x[1];   // Position error in Y
    s[2] = x[2];   // Position error in Z

    // Compute the control input
    u[0] = -2 * s[0] - 4 * s[1];
    u[1] = -2 * s[1] - 4 * s[2];
    u[2] = -2 * s[2]; 

    // Account for disturbance
    u[0] += disturbance[0];
    u[1] += disturbance[1];
    u[2] += disturbance[2];

    // Angular rates control
    u[3] = x[6];   // Yaw rate
    u[4] = x[7];   // Pitch rate
    u[5] = x[8];   // Roll rate

    u[6] = x[9];   // Angular rate of yaw
    u[7] = x[10];  // Angular rate of pitch
    u[8] = x[11];  // Angular rate of roll

    // Disturbance rejection (example: add some noise)
    u[0] += (rand() / RAND_MAX) * 0.1;
    u[1] += (rand() / RAND_MAX) * 0.1;
    u[2] += (rand() / RAND_MAX) * 0.1;
}

int main() {
    // Initialize the state vector
    double x[12];
    for(int i = 0; i < 12; ++i) {
        x[i] = 0.0;
    }

    // Simulate disturbances
    double disturbance[3];
    for(int i = 0; i < 3; ++i) {
        disturbance[i] = (rand() / RAND_MAX) * 0.5 - 0.25;
    }

    // Main loop
    while(1) {
        // Apply control input
        double u[12];
        sliding_mode_control(u, x, disturbance);

        // Update the state
        state_transition(x, u);

        // Generate measurements
        double z[9];
        measurement(z, x);

        // Simulate sensor noise
        for(int i = 0; i < 9; ++i) {
            z[i] += (rand() / RAND_MAX) * 0.05;
        }

        // Print the state estimates (for demonstration)
        printf("State Estimates:\n");
        for(int i = 0; i < 12; ++i) {
            printf("x[%d] = %.3f\n", i, x[i]);
        }
    }

    return 0;
}
