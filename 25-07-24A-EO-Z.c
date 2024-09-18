#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRLEN 16

typedef struct { char input[MAXSTRLEN], output[MAXSTRLEN]; unsigned n, m; } params;

void assert(bool condition, int code, const char *format, ...)
{
    if (condition) return;
    
    va_list args;
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    exit(code);
}

params readInput(int argc, char *argv[])
{
    assert(argc == 5, EXIT_FAILURE, "Usage: %s <input> <output> <n> <m>\n", argv[0]);
    
    char *endptr;
    params args;
    
    strncpy(args.input, argv[1], MAXSTRLEN - 1);
    strncpy(args.output, argv[2], MAXSTRLEN - 1);
    
    args.n = strtoul(argv[3], &endptr, 10);
    assert(!*endptr && args.n > 0, EXIT_FAILURE, "Il parametro n deve essere un parametro intero valido");
    
    args.m = strtoul(argv[4], &endptr, 10);
    assert(!*endptr && args.m > 0, EXIT_FAILURE, "Il parametro m deve essere un parametro intero valido");
    
    return args;
}

char ***allocMatrix(unsigned n, unsigned m)
{
    char ***matrix = malloc(n * sizeof(char**));
    assert(matrix, ENOMEM, "allocMatrix: allocazione fallita\n");
    
    for (size_t i = 0; i < n; i++)
    {
        matrix[i] = malloc(m * sizeof(char*));
        assert(matrix[i], ENOMEM, "allocMatrix: allocazione fallita\n");
        
        for (size_t j = 0; j < m; j++) 
        {
            matrix[i][j] = calloc(MAXSTRLEN, sizeof(char));
            assert(matrix[i][j], ENOMEM, "allocMatrix: allocazione fallita\n");
        }
    }
    
    return matrix;
}

void readMatrix(const char *input, unsigned n, unsigned m, char ***matrix)
{
    FILE *fp = fopen(input, "r");
    
    assert(fp, ENOENT, "Il file di input \"%s\" e' illegibile o inesistente.\n", input);
    
    for (size_t i = 0; i < n; i++) 
    for (size_t j = 0; j < m; j++)
    fscanf(fp, "%s", matrix[i][j]);
    
    fclose(fp);
}

void printMatrix(char ***matrix, unsigned n, unsigned m)
{
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++) printf("%s ", matrix[i][j]);
        putchar('\n');
    }
}

char ***transposeMatrix(char ***matrix, unsigned n, unsigned m)
{
    char ***transposed = malloc(m * sizeof(char**));
    assert(transposed, ENOMEM, "transposeMatrix: allocazione fallita\n");
    
    for (size_t j = 0; j < m; j++)
    {
        transposed[j] = malloc(n * sizeof(char*));
        assert(transposed[j], ENOMEM, "transposeMatrix: allocazione fallita\n");
        
        for (size_t i = 0; i < n; i++)
        {
            transposed[j][i] = calloc(MAXSTRLEN, sizeof(char));
            assert(transposed[j][i], ENOMEM, "transposeMatrix: allocazione fallita\n");
            strcpy(transposed[j][i], matrix[i][j]);
        }
    }
    
    printMatrix(transposed, m, n);
    
    return transposed;
}

void sortColMartix(char ***matrix, unsigned n, unsigned m)
{
    for (size_t j = 0; j < m; j++)for (size_t i = 0; i < n; i++)
    {
        size_t min = i;
        
        for (size_t k = i + 1; k < n; k++)
        if (strcmp(matrix[min][j], matrix[k][j]) > 0)
        min = k;
        
        char *temp = matrix[i][j];
        
        matrix[i][j] = matrix[min][j];
        matrix[min][j] = temp;
    }
}

void writeFile(const char *output, unsigned n, unsigned m, char ***matrix)
{
    FILE *fp = fopen(output, "w");
    
    assert(fp, ENOENT, "Il percorso del file \"%s\" non e' valido.\n", output);
    
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++) fprintf(fp, "%s ", matrix[i][j]);
        putc('\n', fp);
    }
    
    fclose(fp);
}

void deallocMatrix(char ***matrix, unsigned n, unsigned m)
{
    for (size_t i = 0; i < n; i++)
    for (size_t j = 0; j < m; j++) 
    free(matrix[i][j]);
}

int main(int argc, char *argv[])
{
    params args = readInput(argc, argv);
    
    printf("\a*** A ***\ninput: \"%s\"\noutput: \"%s\"\nn: %u\nm: %u\n", 
        args.input, 
        args.output, 
        args.n, 
        args.m
    );
    
    char ***matrix = allocMatrix(args.n, args.m);
    
    readMatrix(args.input, args.n, args.m, matrix);
    
    puts("\n*** B ***");
    printMatrix(matrix, args.n, args.m);
   
    puts("\n*** C ***");
    
    char ***transposed = transposeMatrix(matrix, args.n, args.m);
    
    sortColMartix(transposed, args.m, args.n);
    
    puts("\n*** D ***");
    printMatrix(transposed, args.m, args.n);
    writeFile(args.output, args.m, args.n, transposed);
    
    deallocMatrix(matrix, args.n, args.m);
    deallocMatrix(transposed, args.m, args.n);
    
    return EXIT_SUCCESS;
}