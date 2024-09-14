#include <stdio.h>
#include <math.h>
#include <assert.h>

#define EPSILON 1e-9

typedef struct {
  float x;
  float y;
  float z;
} vec_3d;

typedef struct {
  float w;
  union {
    vec_3d dual;
    struct { 
        float q1, q2, q3;
    };
  };
} quaternion;

quaternion quaternion_multiply (const quaternion *quat_a, const quaternion *quat_b)
{
  quaternion result_quat = {0};
  if (!quat_a || !quat_b) {
    fprintf (stderr, "%s: Invalid input.\n", __func__);
    return result_quat;
  }

  float w_a = quat_a->w, q1_a = quat_a->q1, q2_a = quat_a->q2, q3_a = quat_a->q3;
  float w_b = quat_b->w, q1_b = quat_b->q1, q2_b = quat_b->q2, q3_b = quat_b->q3;

  result_quat.w = w_a * w_b - q1_a * q1_b - q2_a * q2_b - q3_a * q3_b;
  result_quat.q1 = w_a * q1_b + q1_a * w_b + q2_a * q3_b - q3_a * q2_b;
  result_quat.q2 = w_a * q2_b - q1_a * q3_b + q2_a * w_b + q3_a * q1_b;
  result_quat.q3 = w_a * q3_b + q1_a * q2_b - q2_a * q1_b + q3_a * w_b;

  return result_quat;
}

quaternion quaternion_conjugate (const quaternion *quat)
{
  quaternion conjugate = {0};
  conjugate.w = quat->w;
  conjugate.q1 = -quat->q1;
  conjugate.q2 = -quat->q2;
  conjugate.q3 = -quat->q3;
  return conjugate;
}

vec_3d rotate_vector (const vec_3d *vector, const quaternion *rotation_quat)
{
  quaternion vector_quat = {0, .q1 = vector->x, .q2 = vector->y, .q3 = vector->z};
  quaternion quat_conj = quaternion_conjugate (rotation_quat);
  quaternion temp = quaternion_multiply (rotation_quat, &vector_quat);
  quaternion rotated_quat = quaternion_multiply (&temp, &quat_conj);
  vec_3d rotated_vector = {rotated_quat.q1, rotated_quat.q2, rotated_quat.q3};
  return rotated_vector;
}

static void test ()
{
  quaternion rotation_quat = {0.7071f, 0.0f, 0.7071f, 0.0f}; // Rotation by 90 degrees about y-axis

  vec_3d axes[3] = {
    {1.0f, 0.0f, 0.0f}, // X-axis
    {0.0f, 1.0f, 0.0f}, // Y-axis
    {0.0f, 0.0f, 1.0f}	// Z-axis
  };

  printf ("Before Rotation:\n");
  for (int i = 0; i < 3; i++) {
    printf ("Axis %c: (%.2f, %.2f, %.2f)\n", 'X' + i, axes[i].x, axes[i].y,
        axes[i].z);
  }

  printf ("\nAfter Rotation (90 degrees around Y-axis):\n");
  for (int i = 0; i < 3; i++) {
    vec_3d rotated_axis = rotate_vector (&axes[i], &rotation_quat);
    printf ("Axis %c: (%.2f, %.2f, %.2f)\n", 'X' + i, rotated_axis.x,
        rotated_axis.y, rotated_axis.z);
  }
}

int main ()
{
  test ();
  return 0;
}
