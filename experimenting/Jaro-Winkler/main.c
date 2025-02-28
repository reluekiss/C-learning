#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int match_distance(int len) {
    return (len / 2) - 1;
}

static double jaro_distance(const char *s1, const char *s2) {
    int s1_len = (int)strlen(s1);
    int s2_len = (int)strlen(s2);
    if (!s1_len || !s2_len) return 0.0;

    int dist = match_distance((s1_len > s2_len) ? s1_len : s2_len);
    int *s1_match = calloc(s1_len, sizeof(int));
    int *s2_match = calloc(s2_len, sizeof(int));

    int matches = 0;
    for (int i = 0; i < s1_len; i++) {
        int start = i > dist ? i - dist : 0;
        int end = (i + dist + 1 < s2_len) ? i + dist + 1 : s2_len;
        for (int j = start; j < end; j++) {
            if (!s2_match[j] && s1[i] == s2[j]) {
                s1_match[i] = 1;
                s2_match[j] = 1;
                matches++;
                break;
            }
        }
    }
    if (!matches) {
        free(s1_match);
        free(s2_match);
        return 0.0;
    }

    int t = 0, k = 0;
    for (int i = 0; i < s1_len; i++) {
        if (s1_match[i]) {
            while (!s2_match[k]) k++;
            if (s1[i] != s2[k]) t++;
            k++;
        }
    }
    free(s1_match);
    free(s2_match);

    double m = (double)matches;
    return (1.0 / 3.0) * ((m / s1_len) + (m / s2_len) + ((m - (t / 2.0)) / m));
}

double jaro_winkler(const char *s1, const char *s2, double prefix_scale) {
    double jaro = jaro_distance(s1, s2);
    int prefix = 0;
    for (int i = 0; i < (int)fmin(strlen(s1), strlen(s2)); i++) {
        if (s1[i] == s2[i]) prefix++;
        else break;
    }
    if (prefix > 4) prefix = 4;
    return jaro + prefix * prefix_scale * (1.0 - jaro);
}

int main() {
    const char *s1 = "martha";
    const char *s2 = "marhta";
    printf("Jaro-Winkler distance: %f\n", jaro_winkler(s1, s2, 0.1));
    return 0;
}
