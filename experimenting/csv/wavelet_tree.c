#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct wavelet_node {
    int lo, hi, size;
    int *b; /* prefix sum array */
    struct wavelet_node *left, *right;
} wavelet_node_t;

wavelet_node_t* build_wavelet_tree(int *arr, int size, int lo, int hi) {
    if (lo == hi || size == 0)
        return NULL;
    wavelet_node_t *node = malloc(sizeof(wavelet_node_t));
    node->lo = lo;
    node->hi = hi;
    node->size = size;
    node->b = malloc((size + 1) * sizeof(int));
    node->b[0] = 0;
    int mid = (lo + hi) >> 1;
    int *left_arr = malloc(size * sizeof(int));
    int *right_arr = malloc(size * sizeof(int));
    int left_count = 0, right_count = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] <= mid) {
            left_arr[left_count++] = arr[i];
            node->b[i+1] = node->b[i] + 1;
        } else {
            right_arr[right_count++] = arr[i];
            node->b[i+1] = node->b[i];
        }
    }
    node->left = build_wavelet_tree(left_arr, left_count, lo, mid);
    node->right = build_wavelet_tree(right_arr, right_count, mid+1, hi);
    free(left_arr);
    free(right_arr);
    return node;
}

int rank_query(wavelet_node_t *node, int c, int pos) {
    if (!node) return 0;
    if (node->lo == node->hi)
        return pos;
    int mid = (node->lo + node->hi) >> 1;
    if (c <= mid)
        return rank_query(node->left, c, node->b[pos]);
    else
        return rank_query(node->right, c, pos - node->b[pos]);
}

void free_wavelet_tree(wavelet_node_t *node) {
    if (!node) return;
    free_wavelet_tree(node->left);
    free_wavelet_tree(node->right);
    free(node->b);
    free(node);
}

size_t compute_wavelet_tree_memory(wavelet_node_t *node) {
    if (!node) return 0;
    size_t mem = sizeof(wavelet_node_t) + (node->size + 1) * sizeof(int);
    mem += compute_wavelet_tree_memory(node->left);
    mem += compute_wavelet_tree_memory(node->right);
    return mem;
}

int cmp_str(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void process_column(const char *col_name, char **col_data, int num_rows, const char *query_val) {
    /* Build unique mapping for this column */
    char **unique = malloc(num_rows * sizeof(char*));
    int unique_count = 0;
    for (int i = 0; i < num_rows; i++) {
        int found = 0;
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(col_data[i], unique[j]) == 0) { found = 1; break; }
        }
        if (!found)
            unique[unique_count++] = col_data[i];
    }
    qsort(unique, unique_count, sizeof(char*), cmp_str);
    
    /* Map each entry to its integer code */
    int *mapped = malloc(num_rows * sizeof(int));
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < unique_count; j++) {
            if (strcmp(col_data[i], unique[j]) == 0) {
                mapped[i] = j;
                break;
            }
        }
    }
    
    /* Build wavelet tree */
    wavelet_node_t *wt = build_wavelet_tree(mapped, num_rows, 0, unique_count - 1);
    size_t mem_usage = compute_wavelet_tree_memory(wt);
    
    /* Determine query code */
    int query_code = -1;
    for (int j = 0; j < unique_count; j++) {
        if (strcmp(query_val, unique[j]) == 0) {
            query_code = j;
            break;
        }
    }
    
    int occurrences = 0;
    double time_taken = 0.0;
    if (query_code != -1) {
        clock_t start = clock();
        occurrences = rank_query(wt, query_code, num_rows);
        clock_t end = clock();
        time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    }
    
    printf("Column \"%s\": Unique=%d, Memory=%zu bytes, Query \"%s\" => %d occurrences in %f sec\n",
           col_name, unique_count, mem_usage, query_val, occurrences, time_taken);
    
    free_wavelet_tree(wt);
    free(mapped);
    free(unique);
}

int main() {
    const char *csv_data =
"dayOfTheWeek,wokeBeforeNoon,hasEaten,mealPlanned\n"
"Sunday,no,no,Bread\n"
"Sunday,no,no,Tofu\n"
"Monday,no,no,Yams\n"
"Tuesday,no,yes,Steak\n"
"Thursday,yes,yes,Yams\n"
"Saturday,yes,yes,Tofu\n"
"Monday,yes,yes,Eggs\n"
"Monday,yes,yes,Eggs\n"
"Tuesday,no,yes,Bread\n"
"Wednesday,no,yes,Banana\n"
"Thursday,yes,yes,Eggs\n"
"Friday,no,no,Yams\n"
"Friday,yes,yes,Tofu\n"
"Saturday,no,yes,Tufu\n"  /* Assuming typo meant "Tofu" */
"Monday,no,no,Banana\n"
"Friday,no,yes,Yams\n"
"Saturday,yes,yes,Steak\n"
"Tuesday,yes,yes,Eggs\n"
"Sunday,yes,yes,Banana\n"
"Saturday,yes,yes,Steak\n"
"Thursday,no,no,Steak\n"
"Friday,yes,no,Yams\n"
"Monday,yes,no,Banana\n"
"Tuesday,yes,yes,Waffle\n"
"Sunday,no,no,Bread\n"
"Wednesday,yes,no,Bread\n"
"Thursday,no,yes,Banana\n"
"Saturday,yes,no,Yams\n"
"Tuesday,yes,no,Waffle\n"
"Saturday,no,no,Eggs\n"
"Saturday,yes,yes,Steak\n"
"Sunday,yes,yes,Bread\n";
    
    /* Parse CSV into columns */
    char *data = strdup(csv_data);
    char *line = strtok(data, "\n");  /* Header */
    char *header = strdup(line);
    
    /* Count rows */
    int row_count = 0;
    char *data_copy = strdup(csv_data);
    char *temp_line = strtok(data_copy, "\n");
    temp_line = strtok(NULL, "\n");
    while (temp_line) {
        row_count++;
        temp_line = strtok(NULL, "\n");
    }
    free(data_copy);
    
    /* Allocate arrays for each column */
    char **col0 = malloc(row_count * sizeof(char*));
    char **col1 = malloc(row_count * sizeof(char*));
    char **col2 = malloc(row_count * sizeof(char*));
    char **col3 = malloc(row_count * sizeof(char*));
    
    int idx = 0;
    while ((line = strtok(NULL, "\n")) != NULL) {
        char *line_dup = strdup(line);
        char *token = strtok(line_dup, ",");
        int col = 0;
        while (token) {
            if (col == 0)
                col0[idx] = strdup(token);
            else if (col == 1)
                col1[idx] = strdup(token);
            else if (col == 2)
                col2[idx] = strdup(token);
            else if (col == 3)
                col3[idx] = strdup(token);
            token = strtok(NULL, ",");
            col++;
        }
        free(line_dup);
        idx++;
    }
    free(data);
    free(header);
    
    /* Process each column:
       dayOfTheWeek: query "Monday"
       wokeBeforeNoon: query "yes"
       hasEaten: query "yes"
       mealPlanned: query "Bread" */
    process_column("dayOfTheWeek", col0, row_count, "Monday");
    process_column("wokeBeforeNoon", col1, row_count, "yes");
    process_column("hasEaten", col2, row_count, "yes");
    process_column("mealPlanned", col3, row_count, "Bread");
    
    /* Free column data */
    for (int i = 0; i < row_count; i++) {
        free(col0[i]);
        free(col1[i]);
        free(col2[i]);
        free(col3[i]);
    }
    free(col0);
    free(col1);
    free(col2);
    free(col3);
    
    return 0;
}
