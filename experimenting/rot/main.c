#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define __USE_GNU
#include <sched.h>
#include <pthread.h>

#define M_PI 3.14159265358979323846
#define TOTAL_RUNS 20000000
#define BATCH_SIZE 100000

typedef struct {
    double mean;
    double std;
} TimingResult;

typedef struct {
    double w, x, y, z;
} Quaternion;

typedef struct {
    double scalar;
    double b23, b31, b12;
} Rotor;

static inline double sqrt(double x) {
    int i = *(int *)&x;
    i = 0x5f3759df - (i >> 1);
    float y = *(float *)&i;
    y = y * (1.5f - 0.5f * x * y * y);
    return x * y;
}

static inline Quaternion QuaternionRotation(double angle, double ax, double ay, double az) {
    Quaternion q;
    double norm = sqrt(ax*ax + ay*ay + az*az);
    if (norm == 0) norm = 1;
    ax /= norm; ay /= norm; az /= norm;
    q.w = 1 - angle*angle/8;
    double s = angle/2 - angle*angle*angle/48;
    q.x = s * ax;
    q.y = s * ay;
    q.z = s * az;
    return q;
}

static inline Rotor RotorFromBivector(double angle, double ax, double ay, double az) {
    Rotor r;
    double norm = sqrt(ax*ax + ay*ay + az*az);
    if (norm == 0) norm = 1;
    ax /= norm; ay /= norm; az /= norm;
    r.scalar = 1 - angle*angle/8;
    double s = angle/2 - angle*angle*angle/48;
    r.b23 = -ax * s;
    r.b31 = -ay * s;
    r.b12 = -az * s;
    return r;
}

static inline void MatrixRotation(double angle, double ax, double ay, double az, double R[3][3]) {
    double norm = sqrt(ax*ax + ay*ay + az*az);
    if (norm == 0) norm = 1;
    ax /= norm; ay /= norm; az /= norm;
    double c = 1 - angle*angle/2;
    double s = angle - angle*angle*angle/6;
    double t = 1 - c;
    R[0][0] = t*ax*ax + c;
    R[0][1] = t*ax*ay - s*az;
    R[0][2] = t*ax*az + s*ay;
    R[1][0] = t*ax*ay + s*az;
    R[1][1] = t*ay*ay + c;
    R[1][2] = t*ay*az - s*ax;
    R[2][0] = t*ax*az - s*ay;
    R[2][1] = t*ay*az + s*ax;
    R[2][2] = t*az*az + c;
}

static void *quat_thread(void *arg) {
    TimingResult *res = (TimingResult *)arg;
    double angle = M_PI / 4, ax = 0, ay = 0, az = 1;
    int num_batches = (TOTAL_RUNS + BATCH_SIZE - 1) / BATCH_SIZE;
    double sum = 0, sumsq = 0;
    clock_t start, end;
    for (int b = 0; b < num_batches; b++) {
        int runs = (b == num_batches - 1) ? (TOTAL_RUNS - b * BATCH_SIZE) : BATCH_SIZE;
        for (int i = 0; i < runs; i++) {
            start = clock();
            Quaternion q = QuaternionRotation(angle, ax, ay, az);
            end = clock();
            double t = (double)(end - start) / CLOCKS_PER_SEC;
            sum += t;
            sumsq += t * t;
        }
    }
    res->mean = sum / TOTAL_RUNS;
    res->std  = sqrt((sumsq / TOTAL_RUNS) - (res->mean * res->mean));
    pthread_exit(arg);
}

static void *matrix_thread(void *arg) {
    TimingResult *res = (TimingResult *)arg;
    double angle = M_PI / 4, ax = 0, ay = 0, az = 1;
    int num_batches = (TOTAL_RUNS + BATCH_SIZE - 1) / BATCH_SIZE;
    double sum = 0, sumsq = 0;
    clock_t start, end;
    for (int b = 0; b < num_batches; b++) {
        int runs = (b == num_batches - 1) ? (TOTAL_RUNS - b * BATCH_SIZE) : BATCH_SIZE;
        for (int i = 0; i < runs; i++) {
            double R[3][3];
            start = clock();
            MatrixRotation(angle, ax, ay, az, R);
            end = clock();
            double t = (double)(end - start) / CLOCKS_PER_SEC;
            sum += t;
            sumsq += t * t;
        }
    }
    res->mean = sum / TOTAL_RUNS;
    res->std  = sqrt((sumsq / TOTAL_RUNS) - (res->mean * res->mean));
    pthread_exit(arg);
}

static void *rotor_thread(void *arg) {
    TimingResult *res = (TimingResult *)arg;
    double angle = M_PI / 4, ax = 0, ay = 0, az = 1;
    int num_batches = (TOTAL_RUNS + BATCH_SIZE - 1) / BATCH_SIZE;
    double sum = 0, sumsq = 0;
    clock_t start, end;
    for (int b = 0; b < num_batches; b++) {
        int runs = (b == num_batches - 1) ? (TOTAL_RUNS - b * BATCH_SIZE) : BATCH_SIZE;
        for (int i = 0; i < runs; i++) {
            start = clock();
            Rotor r = RotorFromBivector(angle, ax, ay, az);
            end = clock();
            double t = (double)(end - start) / CLOCKS_PER_SEC;
            sum += t;
            sumsq += t * t;
        }
    }
    res->mean = sum / TOTAL_RUNS;
    res->std  = sqrt((sumsq / TOTAL_RUNS) - (res->mean * res->mean));
    pthread_exit(arg);
}

int main(void) {
    pthread_t t_quat, t_matrix, t_rotor;
    TimingResult res_quat, res_matrix, res_rotor;
    pthread_attr_t attr;
    cpu_set_t cpuset;
    
    pthread_attr_init(&attr);
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
    pthread_create(&t_quat, &attr, quat_thread, (void *)&res_quat);
    pthread_attr_destroy(&attr);
    
    pthread_attr_init(&attr);
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
    pthread_create(&t_matrix, &attr, matrix_thread, (void *)&res_matrix);
    pthread_attr_destroy(&attr);
    
    pthread_attr_init(&attr);
    CPU_ZERO(&cpuset);
    CPU_SET(2, &cpuset);
    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);
    pthread_create(&t_rotor, &attr, rotor_thread, (void *)&res_rotor);
    pthread_attr_destroy(&attr);
    
    pthread_join(t_quat, NULL);
    pthread_join(t_matrix, NULL);
    pthread_join(t_rotor, NULL);

    printf("Quaternion creation: mean = %e s, std = %e s\n", res_quat.mean, res_quat.std);
    printf("Matrix rotation creation: mean = %e s, std = %e s\n", res_matrix.mean, res_matrix.std);
    printf("Rotor creation: mean = %e s, std = %e s\n", res_rotor.mean, res_rotor.std);

    return 0;
}

