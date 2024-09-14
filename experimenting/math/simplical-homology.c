#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// TODO: add hashtables to simplex and simplicalcomplex that map names to vertexes and simplexes
struct Vertex {
    char *name;
    struct Vertex **adjacencies; // Array of pointers to Vertexes
    int dimension;
    float coordinates[];
};

struct Simplex {
    char *name;
    int num_vertexes;
    struct Vertex **vertexes;
};

struct SimplicalComplex {
    int num_simplexes;
    struct Simplex **simplexes;
};

// Pass in a the simplical complex to insert into, the vector of the names of the adjacent vertexes,
// it's length, the name of the vertex, its dimension and coordinate location.
struct Vertex *vertex_init(struct SimplicalComplex *complex, char *names[], int length, char *name, int dimension, float coordinates[])
{
    if(names || complex || name == NULL) {
        perror("variable in vertex_init is NULL");
        exit(-1);
    }
    
    struct Vertex *vertex = malloc(sizeof(struct Vertex));
    vertex->name = name;
    vertex->dimension = dimension;
    
    for(int i = 0; i < dimension; i++)
        vertex->coordinates[i] = coordinates[i];

    vertex->adjacencies = calloc(length, sizeof(struct Vertex *));

    for(int i = 0; i < length; i++)
        for(int j = 0; j < complex->num_simplexes; j++)
            for(int k = 0; k < complex->simplexes[j]->num_vertexes; k++)
                if(complex->simplexes[j]->vertexes[k]->name == names[i] && complex->simplexes[j]->vertexes[k]->name != name)
                    vertex->adjacencies[i] = complex->simplexes[j]->vertexes[k]; 

    return vertex;
}

struct Simplex *simplex_init()
{
    struct Simplex *simplex = malloc(sizeof(struct Simplex));
    return simplex;
}

struct SimplicalComplex *complex_init()
{
    struct SimplicalComplex *complex = malloc(sizeof(struct SimplicalComplex));
    return complex;
}

void simplex_insert(struct Simplex *simplex, struct Vertex *vertex)
{
    if(simplex || vertex == NULL) {
        perror("variable in simplex_insert is NULL");
        exit(-1);
    }

    simplex->num_vertexes++;
    simplex->vertexes = realloc(simplex->vertexes, sizeof(struct Vertex *) * simplex->num_vertexes);
    
    simplex->vertexes[simplex->num_vertexes] = vertex;
}

// d is the dimension of the boundary ie d_n(complex), if left as NULL, it will default to dimension - 1;
void boundary(struct SimplicalComplex *complex, int d)
{
    if(complex == NULL) {
        perror("variable in boundary is NULL");
        exit(-1);
    }
    if(d == NULL)
        d = complex->simplexes[0]->vertexes[0]->dimension - 1;

    for(int i = 0; i < complex->num_simplexes; i++) {
        char *names[complex->simplexes[i]->num_vertexes];
        for(int j = 0; j < complex->simplexes[i]->num_vertexes; j++) {
            names[j] = complex->simplexes[i]->vertexes[j]->name;
        }
        // TODO:: possibly broken
        for(int j = 0; j < complex->simplexes[i]->num_vertexes; j++) {
            char* concatNames[complex->simplexes[i]->num_vertexes * complex->simplexes[i]->num_vertexes - complex->simplexes[i]->num_vertexes];
            char tmp;
            for(int k = 0; k < complex->simplexes[i]->num_vertexes; k++) {
                if (k == j) {continue;}
                tmp = sprintf("%s%s", &tmp, names[k]);
            }
            concatNames[j] = &tmp;
        }
    }
}

int main()
{
    struct SimplicalComplex *complex = complex_init();
    complex->simplexes[0] = simplex_init();

    char *names[] = {"A", "B", "C", "D"};
    int length = sizeof(names) / sizeof(names[0]) - 1;
    
    float coordinates[][] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    int dimension = sizeof(coordinates) / sizeof(coordinates[0]);

    for(int i = 0; i < length; i++) {
        struct Vertex *vertex = vertex_init(complex, names, length, names[i], dimension, coordinates); 
        simplex_insert(complex->simplexes[0], vertex);
    }
    
cleanup:
    for(int i = 0; i < complex->simplexes[0]->num_vertexes; i++)
        free(complex->simplexes[0]->vertexes[i]);

    free(complex->simplexes[0]);
    free(complex);
    return 0;
}
