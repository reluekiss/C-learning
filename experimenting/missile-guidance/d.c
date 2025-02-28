#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTEPS 1000
#define STATE_DIM 9
#define MEAS_DIM 3

// Box-Muller normal random generator
float rand_normal(void) {
    static int haveSpare = 0;
    static float spare;
    if(haveSpare) {
        haveSpare = 0;
        return spare;
    }
    haveSpare = 1;
    float u, v, s;
    do {
        u = (rand()/(float)RAND_MAX)*2.0f - 1.0f;
        v = (rand()/(float)RAND_MAX)*2.0f - 1.0f;
        s = u*u + v*v;
    } while(s >= 1.0f || s == 0.0f);
    s = sqrtf(-2.0f*logf(s)/s);
    spare = v * s;
    return u * s;
}

int invert3x3(float m[3][3], float invOut[3][3]) {
    float det = m[0][0]*(m[1][1]*m[2][2]-m[1][2]*m[2][1])
              - m[0][1]*(m[1][0]*m[2][2]-m[1][2]*m[2][0])
              + m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]);
    if(fabs(det) < 1e-6) return 0;
    float invDet = 1.0f/det;
    invOut[0][0] =  (m[1][1]*m[2][2]-m[1][2]*m[2][1])*invDet;
    invOut[0][1] = -(m[0][1]*m[2][2]-m[0][2]*m[2][1])*invDet;
    invOut[0][2] =  (m[0][1]*m[1][2]-m[0][2]*m[1][1])*invDet;
    invOut[1][0] = -(m[1][0]*m[2][2]-m[1][2]*m[2][0])*invDet;
    invOut[1][1] =  (m[0][0]*m[2][2]-m[0][2]*m[2][0])*invDet;
    invOut[1][2] = -(m[0][0]*m[1][2]-m[0][2]*m[1][0])*invDet;
    invOut[2][0] =  (m[1][0]*m[2][1]-m[1][1]*m[2][0])*invDet;
    invOut[2][1] = -(m[0][0]*m[2][1]-m[0][1]*m[2][0])*invDet;
    invOut[2][2] =  (m[0][0]*m[1][1]-m[0][1]*m[1][0])*invDet;
    return 1;
}

int main(void)
{
    const int screenWidth = 800, screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Missile Guidance with Kalman Filter (Accel Model)");

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, -150.0f, 100.0f };
    camera.target   = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up       = (Vector3){ 0.0f, 0.0f, 1.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    SetTargetFPS(60);

    float dt = 0.1f, omega_target = 0.1f;
    int timesteps = TIMESTEPS;

    // Target initial state (absolute)
    float x_target = 50.0f, y_target = 50.0f, z_target = 0.0f;
    float vx_target = 5.0f, vy_target = 2.0f, vz_target = 0.0f;

    // Missile remains at the origin.
    float x_missile = 0.0f, y_missile = 0.0f, z_missile = 0.0f;

    // Process noise (9x9 identity)
    float Q[STATE_DIM][STATE_DIM] = {0};
    for (int i = 0; i < STATE_DIM; i++) Q[i][i] = 1.0f;

    // Measurement noise (3x3 identity)
    float R_IR[MEAS_DIM][MEAS_DIM] = {0};
    for (int i = 0; i < MEAS_DIM; i++) R_IR[i][i] = 1.0f;

    // State transition matrix A (constant acceleration model)
    float A[STATE_DIM][STATE_DIM] = {0};
    for (int i = 0; i < STATE_DIM; i++) A[i][i] = 1.0f;
    A[0][3] = dt;              A[1][4] = dt;              A[2][5] = dt;
    A[0][6] = 0.5f * dt * dt;  A[1][7] = 0.5f * dt * dt;  A[2][8] = 0.5f * dt * dt;
    A[3][6] = dt;              A[4][7] = dt;              A[5][8] = dt;

    // Measurement matrix H (3x9) - measures position only
    float H[MEAS_DIM][STATE_DIM] = {0};
    for (int i = 0; i < MEAS_DIM; i++) H[i][i] = 1.0f;

    // Initial state estimate x_hat (target state relative to missile).
    // Since the missile is at (0,0,0), the relative state equals the absolute target state.
    float x_hat[STATE_DIM] = { x_target, y_target, z_target,
                               vx_target, vy_target, vz_target,
                               0.0f, 0.0f, 0.0f };

    // Initial covariance P (9x9 identity)
    float P[STATE_DIM][STATE_DIM] = {0};
    for (int i = 0; i < STATE_DIM; i++) P[i][i] = 1.0f;

    float estimated_state[TIMESTEPS][STATE_DIM] = {0};
    float target_state[TIMESTEPS][MEAS_DIM] = {0};

    float measurements[TIMESTEPS][MEAS_DIM] = {0};
    srand(time(NULL));

    for (int k = 0; k < timesteps; k++) {
        // Update target state (nonconstant acceleration dynamics for simulation)
        x_target += vx_target * dt + 5.0f * sinf(omega_target * k * dt);
        y_target += vy_target * dt + 5.0f * cosf(omega_target * k * dt);
        z_target += vz_target * dt;
        target_state[k][0] = x_target;
        target_state[k][1] = y_target;
        target_state[k][2] = z_target;

        // Measurement: target relative to missile + noise
        float measurement[MEAS_DIM] = {
            (x_target - x_missile) + rand_normal(),
            (y_target - y_missile) + rand_normal(),
            (z_target - z_missile) + rand_normal()
        };
        for (int i = 0; i < MEAS_DIM; i++) 
            measurements[k][i] = measurement[i];

        // Prediction: x_hat_minus = A * x_hat
        float x_hat_minus[STATE_DIM] = {0};
        for (int i = 0; i < STATE_DIM; i++)
            for (int j = 0; j < STATE_DIM; j++)
                x_hat_minus[i] += A[i][j] * x_hat[j];

        // Covariance prediction: P_minus = A * P * A^T + Q
        float P_minus[STATE_DIM][STATE_DIM] = {0};
        float AP[STATE_DIM][STATE_DIM] = {0};
        for (int i = 0; i < STATE_DIM; i++)
            for (int j = 0; j < STATE_DIM; j++)
                for (int k2 = 0; k2 < STATE_DIM; k2++)
                    AP[i][j] += A[i][k2] * P[k2][j];
        for (int i = 0; i < STATE_DIM; i++)
            for (int j = 0; j < STATE_DIM; j++) {
                float sum = 0.0f;
                for (int k2 = 0; k2 < STATE_DIM; k2++)
                    sum += AP[i][k2] * A[j][k2];
                P_minus[i][j] = sum + Q[i][j];
            }

        // Compute S = H * P_minus * H^T + R_IR (only the 3x3 block)
        float S[MEAS_DIM][MEAS_DIM] = {0};
        for (int i = 0; i < MEAS_DIM; i++)
            for (int j = 0; j < MEAS_DIM; j++)
                S[i][j] = P_minus[i][j] + R_IR[i][j];

        float S_inv[MEAS_DIM][MEAS_DIM];
        if(!invert3x3(S, S_inv))
            continue;

        // Kalman Gain K = P_minus * H^T * S_inv (9x3)
        float K_gain[STATE_DIM][MEAS_DIM] = {0};
        for (int i = 0; i < STATE_DIM; i++)
            for (int j = 0; j < MEAS_DIM; j++)
                for (int k2 = 0; k2 < MEAS_DIM; k2++)
                    K_gain[i][j] += P_minus[i][k2] * S_inv[k2][j];

        // Innovation = measurement - H*x_hat_minus
        float innovation[MEAS_DIM] = {0};
        for (int i = 0; i < MEAS_DIM; i++)
            innovation[i] = measurement[i] - x_hat_minus[i];

        // Update state estimate: x_hat = x_hat_minus + K_gain * innovation
        for (int i = 0; i < STATE_DIM; i++) {
            float sum = 0.0f;
            for (int j = 0; j < MEAS_DIM; j++)
                sum += K_gain[i][j] * innovation[j];
            x_hat[i] = x_hat_minus[i] + sum;
        }

        // Update covariance: P = (I - K*H)*P_minus
        float I_KH[STATE_DIM][STATE_DIM] = {0};
        for (int i = 0; i < STATE_DIM; i++) {
            for (int j = 0; j < STATE_DIM; j++) {
                I_KH[i][j] = (i == j) ? 1.0f : 0.0f;
                if(j < MEAS_DIM)
                    I_KH[i][j] -= K_gain[i][j];
            }
        }
        for (int i = 0; i < STATE_DIM; i++)
            for (int j = 0; j < STATE_DIM; j++) {
                float sum = 0.0f;
                for (int k2 = 0; k2 < STATE_DIM; k2++)
                    sum += I_KH[i][k2] * P_minus[k2][j];
                P[i][j] = sum;
            }

        for (int i = 0; i < STATE_DIM; i++)
            estimated_state[k][i] = x_hat[i];
    }

    int simIndex = 0;
    float timer = 0.0f;
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        timer += deltaTime;
        if(timer >= dt && simIndex < timesteps - 1) { timer = 0.0f; simIndex++; }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                DrawGrid(20, 5.0f);
                for (int i = 0; i < simIndex - 1; i++) {
                    Vector3 p1 = { estimated_state[i][0], estimated_state[i][1], estimated_state[i][2] };
                    Vector3 p2 = { estimated_state[i+1][0], estimated_state[i+1][1], estimated_state[i+1][2] };
                    DrawLine3D(p1, p2, RED);
                }
                // Draw true target state
                Vector3 targetPos = { target_state[simIndex][0], target_state[simIndex][1], target_state[simIndex][2] };
                DrawSphere(targetPos, 2.0f, BLUE);
                // Draw estimated target state
                Vector3 estPos = { estimated_state[simIndex][0], estimated_state[simIndex][1], estimated_state[simIndex][2] };
                camera.target = estPos;
                camera.position = (Vector3){ estPos.x - 250, estPos.y - 250, estPos.z + 130 };
                DrawSphere(estPos, 1.5f, RED);
            EndMode3D();
            DrawText("Missile Guidance with Kalman Filter (Acceleration Model)", 10, 10, 20, DARKGRAY);

            char stateText[256];
            sprintf(stateText, "State: [%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]",
                    estimated_state[simIndex][0], estimated_state[simIndex][1], estimated_state[simIndex][2],
                    estimated_state[simIndex][3], estimated_state[simIndex][4], estimated_state[simIndex][5],
                    estimated_state[simIndex][6], estimated_state[simIndex][7], estimated_state[simIndex][8]);
            DrawText(stateText, 10, 40, 20, DARKGRAY);
            
            char measText[128];
            sprintf(measText, "Measurement: [%.2f, %.2f, %.2f]",
                    measurements[simIndex][0], measurements[simIndex][1], measurements[simIndex][2]);
            DrawText(measText, 10, 70, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
