#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define STATE_DIM 12
#define MEAS_DIM 6
#define CTRL_DIM 6
#define NUM_SIGMA (2 * STATE_DIM + 1)

#define ALPHA 1e-3
#define BETA 2.0
#define KAPPA 0.0

#define SENSOR_BASE_ADDR ((volatile double*)0x40000000)
#define ACTUATOR_BASE_ADDR ((volatile double*)0x50000000)

typedef struct {
    double x[STATE_DIM];
    double P[STATE_DIM][STATE_DIM];
} UKFState;

/* --- Dynamics and Measurement Models --- */
void f(double t, const double *x, const double *u, double dt, double *x_next) {
    double mass = 100.0;
    double Ixx = 10.0, Iyy = 10.0, Izz = 10.0;
    double g = 9.81;
    int i;
    /* Position update: pos_dot = velocity */
    for (i = 0; i < 3; i++)
        x_next[i] = x[i] + dt * x[i+3];
    /* Velocity update: vel_dot = force/m (gravity in z) */
    for (i = 0; i < 3; i++) {
        double accel = u[i] / mass;
        if (i == 2) accel -= g;
        x_next[i+3] = x[i+3] + dt * accel;
    }
    /* Euler angles update: angles_dot = angular velocities */
    for (i = 0; i < 3; i++)
        x_next[i+6] = x[i+6] + dt * x[i+9];
    /* Angular velocities update: ang_vel_dot = moment/inertia */
    x_next[9]  = x[9]  + dt * (u[3] / Ixx);
    x_next[10] = x[10] + dt * (u[4] / Iyy);
    x_next[11] = x[11] + dt * (u[5] / Izz);
}

void h(double t, const double *x, double *z) {
    int i;
    /* Measurement: [pos; vel] */
    for (i = 0; i < MEAS_DIM; i++)
        z[i] = x[i];
}

/* --- Matrix Utilities --- */
int cholesky(int n, double A[n][n], double L[n][n]) {
    int i, j, k;
    for (i = 0; i < n; i++) {
        for (j = 0; j <= i; j++) {
            double sum = A[i][j];
            for (k = 0; k < j; k++)
                sum -= L[i][k] * L[j][k];
            if (i == j) {
                if (sum <= 0) return -1;
                L[i][j] = sqrt(sum);
            } else {
                L[i][j] = sum / L[j][j];
            }
        }
        for (j = i+1; j < n; j++)
            L[i][j] = 0;
    }
    return 0;
}

void matrix_invert(int n, double A[n][n], double A_inv[n][n]) {
    int i, j, k;
    double aug[n][2*n];
    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++)
            aug[i][j] = A[i][j];
        for (j = n; j < 2*n; j++)
            aug[i][j] = (i == (j - n)) ? 1.0 : 0.0;
    }
    for (i = 0; i < n; i++){
        double pivot = aug[i][i];
        if (fabs(pivot) < 1e-9) {
            for (int r = i+1; r < n; r++){
                if (fabs(aug[r][i]) > 1e-9){
                    for (j = 0; j < 2*n; j++){
                        double temp = aug[i][j];
                        aug[i][j] = aug[r][j];
                        aug[r][j] = temp;
                    }
                    pivot = aug[i][i];
                    break;
                }
            }
        }
        for (j = 0; j < 2*n; j++)
            aug[i][j] /= pivot;
        for (int r = 0; r < n; r++){
            if (r != i) {
                double factor = aug[r][i];
                for (j = 0; j < 2*n; j++)
                    aug[r][j] -= factor * aug[i][j];
            }
        }
    }
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            A_inv[i][j] = aug[i][j+n];
}

/* --- Unscented Kalman Filter --- */
void ukf_predict(UKFState *ukf, const double *u, double dt, double t) {
    double lambda = ALPHA*ALPHA*(STATE_DIM+KAPPA) - STATE_DIM;
    double scaling = STATE_DIM + lambda;
    double weights_mean[NUM_SIGMA], weights_cov[NUM_SIGMA];
    weights_mean[0] = lambda / scaling;
    weights_cov[0]  = lambda / scaling + (1 - ALPHA*ALPHA + BETA);
    for (int i = 1; i < NUM_SIGMA; i++){
        weights_mean[i] = 1.0/(2*scaling);
        weights_cov[i]  = 1.0/(2*scaling);
    }
    double sigma_points[NUM_SIGMA][STATE_DIM];
    for (int i = 0; i < STATE_DIM; i++)
        sigma_points[0][i] = ukf->x[i];
    double A[STATE_DIM][STATE_DIM];
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < STATE_DIM; j++)
            A[i][j] = ukf->P[i][j] * scaling;
    double L[STATE_DIM][STATE_DIM];
    cholesky(STATE_DIM, A, L);
    for (int i = 0; i < STATE_DIM; i++){
        for (int j = 0; j < STATE_DIM; j++){
            sigma_points[i+1][j]           = ukf->x[j] + L[j][i];
            sigma_points[i+1+STATE_DIM][j] = ukf->x[j] - L[j][i];
        }
    }
    double sigma_pred[NUM_SIGMA][STATE_DIM];
    for (int i = 0; i < NUM_SIGMA; i++)
        f(t, sigma_points[i], u, dt, sigma_pred[i]);
    double x_pred[STATE_DIM] = {0};
    for (int i = 0; i < NUM_SIGMA; i++)
        for (int j = 0; j < STATE_DIM; j++)
            x_pred[j] += weights_mean[i] * sigma_pred[i][j];
    double P_pred[STATE_DIM][STATE_DIM] = {0};
    for (int i = 0; i < NUM_SIGMA; i++) {
        double diff[STATE_DIM];
        for (int j = 0; j < STATE_DIM; j++)
            diff[j] = sigma_pred[i][j] - x_pred[j];
        for (int j = 0; j < STATE_DIM; j++)
            for (int k = 0; k < STATE_DIM; k++)
                P_pred[j][k] += weights_cov[i] * diff[j] * diff[k];
    }
    for (int i = 0; i < STATE_DIM; i++){
        ukf->x[i] = x_pred[i];
        for (int j = 0; j < STATE_DIM; j++)
            ukf->P[i][j] = P_pred[i][j];
    }
}

void ukf_update(UKFState *ukf, const double *z, double t) {
    double lambda = ALPHA*ALPHA*(STATE_DIM+KAPPA) - STATE_DIM;
    double scaling = STATE_DIM + lambda;
    double weights_mean[NUM_SIGMA], weights_cov[NUM_SIGMA];
    weights_mean[0] = lambda / scaling;
    weights_cov[0]  = lambda / scaling + (1 - ALPHA*ALPHA + BETA);
    for (int i = 1; i < NUM_SIGMA; i++){
        weights_mean[i] = 1.0/(2*scaling);
        weights_cov[i]  = 1.0/(2*scaling);
    }
    double sigma_points[NUM_SIGMA][STATE_DIM];
    for (int i = 0; i < STATE_DIM; i++)
        sigma_points[0][i] = ukf->x[i];
    double A[STATE_DIM][STATE_DIM];
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < STATE_DIM; j++)
            A[i][j] = ukf->P[i][j] * scaling;
    double L[STATE_DIM][STATE_DIM];
    cholesky(STATE_DIM, A, L);
    for (int i = 0; i < STATE_DIM; i++){
        for (int j = 0; j < STATE_DIM; j++){
            sigma_points[i+1][j]           = ukf->x[j] + L[j][i];
            sigma_points[i+1+STATE_DIM][j] = ukf->x[j] - L[j][i];
        }
    }
    double sigma_meas[NUM_SIGMA][MEAS_DIM];
    for (int i = 0; i < NUM_SIGMA; i++)
        h(t, sigma_points[i], sigma_meas[i]);
    double z_pred[MEAS_DIM] = {0};
    for (int i = 0; i < NUM_SIGMA; i++)
        for (int j = 0; j < MEAS_DIM; j++)
            z_pred[j] += weights_mean[i] * sigma_meas[i][j];
    double S[MEAS_DIM][MEAS_DIM] = {0};
    double cross_cov[STATE_DIM][MEAS_DIM] = {0};
    for (int i = 0; i < NUM_SIGMA; i++) {
        double z_diff[MEAS_DIM], x_diff[STATE_DIM];
        for (int j = 0; j < MEAS_DIM; j++)
            z_diff[j] = sigma_meas[i][j] - z_pred[j];
        for (int j = 0; j < STATE_DIM; j++)
            x_diff[j] = sigma_points[i][j] - ukf->x[j];
        for (int j = 0; j < MEAS_DIM; j++) {
            for (int k = 0; k < MEAS_DIM; k++)
                S[j][k] += weights_cov[i] * z_diff[j] * z_diff[k];
            for (int k = 0; k < STATE_DIM; k++)
                cross_cov[k][j] += weights_cov[i] * x_diff[k] * z_diff[j];
        }
    }
    double R[MEAS_DIM][MEAS_DIM] = {0};
    double meas_noise = 0.1;
    for (int i = 0; i < MEAS_DIM; i++) {
        R[i][i] = meas_noise;
        S[i][i] += meas_noise;
    }
    double S_inv[MEAS_DIM][MEAS_DIM];
    matrix_invert(MEAS_DIM, S, S_inv);
    double K[STATE_DIM][MEAS_DIM] = {0};
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < MEAS_DIM; j++)
            for (int k = 0; k < MEAS_DIM; k++)
                K[i][j] += cross_cov[i][k] * S_inv[k][j];
    double z_diff[MEAS_DIM];
    for (int i = 0; i < MEAS_DIM; i++)
        z_diff[i] = z[i] - z_pred[i];
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < MEAS_DIM; j++)
            ukf->x[i] += K[i][j] * z_diff[j];
    double KS[STATE_DIM][MEAS_DIM] = {0};
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < MEAS_DIM; j++)
            for (int k = 0; k < MEAS_DIM; k++)
                KS[i][j] += K[i][k] * S[k][j];
    double KSKT[STATE_DIM][STATE_DIM] = {0};
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < STATE_DIM; j++)
            for (int k = 0; k < MEAS_DIM; k++)
                KSKT[i][j] += KS[i][k] * K[j][k];
    for (int i = 0; i < STATE_DIM; i++)
        for (int j = 0; j < STATE_DIM; j++)
            ukf->P[i][j] -= KSKT[i][j];
}

/* --- Sliding Mode Control --- */
void sliding_mode_control(const double *x_est, const double *x_des, double *u) {
    int i;
    double lambda_pos = 1.0, k_pos = 10.0;
    double s[3];
    for (i = 0; i < 3; i++) {
        double err_pos = x_est[i] - x_des[i];
        double err_vel = x_est[i+3] - x_des[i+3];
        s[i] = err_vel + lambda_pos * err_pos;
        u[i] = -k_pos * (s[i] >= 0 ? 1.0 : -1.0);
    }
    double lambda_ang = 1.0, k_ang = 5.0;
    double s_ang[3];
    for (i = 0; i < 3; i++) {
        double err_ang = x_est[i+6] - x_des[i+6];
        double err_angvel = x_est[i+9] - x_des[i+9];
        s_ang[i] = err_angvel + lambda_ang * err_ang;
        u[i+3] = -k_ang * (s_ang[i] >= 0 ? 1.0 : -1.0);
    }
}

/* --- Memory-Mapped I/O --- */
/* void read_sensors(double *z) {
    volatile double *sensor_ptr = SENSOR_BASE_ADDR;
    for (int i = 0; i < MEAS_DIM; i++)
        z[i] = sensor_ptr[i];
}

void write_actuators(const double *u) {
    volatile double *actuator_ptr = ACTUATOR_BASE_ADDR;
    for (int i = 0; i < CTRL_DIM; i++)
        actuator_ptr[i] = u[i];
} */

// Function to generate Gaussian random numbers (for noise)
double gauss_rand(double mu, double sigma) {
    static double buffer;
    static int initialized = 0;

    // Seed random number generator for reproducibility
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }

    // Box-Muller transform
    double x1, x2, rand_num;
    do {
        x1 = (double)rand() / RAND_MAX * 2 - 1;
        x2 = (double)rand() / RAND_MAX * 2 - 1;
    } while (x1 <= -1 || x1 >= 1);

    rand_num = mu + sigma * sqrt(-2.0 * log(x1)) * cos(2 * M_PI * x2);
    return rand_num;
}

/* --- Modified Sensor Reading Function --- */
void read_sensors(double *z) {
    static double noise_offset = 0;
    static const char* labels[MEAS_DIM] = {"Position X", "Velocity X", "Position Y", 
                                           "Velocity Y", "Position Z", "Velocity Z"};
    srand(time(NULL));
    
    printf("\nSensor Reading at t = %.3fs:\n", (double)clock() / CLOCKS_PER_SEC);
    
    for (int i = 0; i < MEAS_DIM; i++) {
        // Generate synthetic readings with some noise
        z[i] = sin((i + 1) * clock() / (double)CLOCKS_PER_SEC) + 
               gauss_rand(0, 0.1) + ((i % 2 == 0) ? 100 : 50);
        
        printf("%s: %.3lf\n", labels[i], z[i]);
    }
    
    printf("\n");
}

/* --- Modified Actuator Writing Function --- */
void write_actuators(UKFState *ukf, double *u) {
    static const char* acctuator_labels[CTRL_DIM] = {"Thrust", "Roll", "Pitch", "Yaw"};
    
    printf("Actuator Outputs:\n");
    for (int i = 0; i < CTRL_DIM; i++) {
        // Generate synthetic control outputs
        u[i] = sin((i + 1) * clock() / (double)CLOCKS_PER_SEC) * 
               ((i == 0) ? 50 : 20);
        
        printf("%s: %.3lf\n", acctuator_labels[i], u[i]);
    }

    printf("\nMissile State:\n");
    for (int i = 0; i < STATE_DIM; ++i) {
        printf("%d: %.3f\n", i, ukf->x[i]);
    }
    printf("\n");
    for (int i = 0; i < STATE_DIM; ++i) {
        printf("%d:\n %.3f %.3f %.3f\n%.3f %.3f %.3f\n%.3f %.3f %.3f\n%.3f %.3f %.3f\n", i,
               ukf->P[i][0], ukf->P[i][1], ukf->P[i][2],
               ukf->P[i][3], ukf->P[i][4], ukf->P[i][5],
               ukf->P[i][6], ukf->P[i][7], ukf->P[i][8],
               ukf->P[i][9], ukf->P[i][10], ukf->P[i][11]);
    }
    printf("\n");
}

/* --- Main Loop --- */
int main() {
    UKFState ukf;
    int i, j;
    for (i = 0; i < STATE_DIM; i++) {
        ukf.x[i] = 0.0;
        for (j = 0; j < STATE_DIM; j++)
            ukf.P[i][j] = (i == j) ? 0.1 : 0.0;
    }
    double dt = 0.01, t = 0.0;
    double u[CTRL_DIM] = {0};
    double x_des[STATE_DIM] = {0};
    /* Desired position: e.g., x=1000, y=0, z=500; velocity: 300,0,0; rest zero */
    x_des[0] = 1000.0; x_des[1] = 0.0;   x_des[2] = 500.0;
    x_des[3] = 300.0;  x_des[4] = 0.0;   x_des[5] = 0.0;

    while (1) {
        double z[MEAS_DIM];
        read_sensors(z);
        ukf_predict(&ukf, u, dt, t);
        ukf_update(&ukf, z, t);
        sliding_mode_control(ukf.x, x_des, u);
        write_actuators(&ukf, u);
        t += dt;
        usleep((int)(dt * 1e6));
    }
    return 0;
}
