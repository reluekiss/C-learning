#include <immintrin.h>
#include <stdio.h>

float dotProd(__m512 a, __m512 b) {
    __m512 c = _mm512_mul_ps(a, b);

    return _mm512_reduce_add_ps(c);
}

inline float invSqrt(float a) {
    return _mm_cvtss_f32(
        _mm_sqrt_ss(_mm_set_ps1(a))
    );
}

inline static __m128 cross_product( __m128 const vec0, __m128 const vec1 ) {
    __m128 tmp0 = _mm_shuffle_ps(vec0,vec0,_MM_SHUFFLE(3,0,2,1));
    __m128 tmp1 = _mm_shuffle_ps(vec1,vec1,_MM_SHUFFLE(3,1,0,2));
    __m128 tmp2 = _mm_mul_ps(tmp0,vec1);
    __m128 tmp3 = _mm_mul_ps(tmp0,tmp1);
    __m128 tmp4 = _mm_shuffle_ps(tmp2,tmp2,_MM_SHUFFLE(3,0,2,1));
    return _mm_sub_ps(tmp3,tmp4);
}

__m512 outerProd(__m512 vec1, __m512 vec2) {
    return _mm512_mul_ps(_mm512_permute_ps(vec1, _MM_PERM_DDAB), vec2);
}

int main()
{
    __m512 vec = _mm512_set_ps(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    __m512 vec1 = _mm512_set_ps(16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);

    float sum = dotProd(vec1, vec);
    
    printf("dot: %f\n", sum);
    
    return 0;
}
