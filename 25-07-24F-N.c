#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nullptr ((void*)0)
#define new(T) malloc(sizeof(T))
#define MAXSTRLEN 256

typedef FILE *stream;
typedef struct { char inputFilename[MAXSTRLEN]; } params;

FILE *(*const open)(const char*, const char*) = fopen;
int (*const close)(FILE*) = fclose;
void (*const delete)(void*) = free;

void assert(bool condition, int code, const char *format, ...);
void bubbleSort(short *Y, unsigned n);
params decodeParameters(int argc, const char *argv[]);
short *getArray(short ***X, unsigned n, unsigned m);
void processX(short ***X, const short *Y, unsigned n, unsigned m);
short ***readFile(stream input, unsigned n, unsigned m);
stream readHeader(const char *inputFilename, unsigned *n, unsigned *m);

int main(int argc, const char *argv[])
{
    params args = decodeParameters(argc, argv);
    unsigned n, m;
    stream input = readHeader(args.inputFilename, &n, &m);
    short ***X = readFile(input, n, m), *Y = getArray(X, n, m);
    
    puts("\a\n======A Stampa Parametri======");
    printf("inputFilename = \"%s\"\n", args.inputFilename);
    
    puts("\n======B Valori n e m======");
    printf("n = %u, m = %u\n", n, m);
    
    puts("\n======B Matrice X======");
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++) printf("  %hu", *X[i][j]);
        putchar('\n');
    }
    
    puts("\n======C Array Y======");
    for (size_t i = 0; i < n; i++) printf("%hu ", Y[i]);
    
    bubbleSort(Y, n);
    
    puts("\n\n======C Array Y Ordinato======");
    for (size_t i = 0; i < n; i++) printf("%hu ", Y[i]);
    
    processX(X, Y, n, m);
    
    puts("\n\n======D Matrice X modificata======");
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++) if (X[i][j]) printf("  %hu", *X[i][j]);
        else printf("   *");
        putchar('\n');
    }
    
    fputs("\n======E Stampa su standard error======\n", stderr);
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++) if (X[i][j]) fprintf(stderr, "  %hu", *X[i][j]);
        else fprintf(stderr, "   -");
        putc('\n', stderr);
    }
    
    delete(Y);
    
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++) if (X[i][j]) delete(X[i][j]);
        delete(X[i]);
    }
    
    delete(X);
    
    return EXIT_SUCCESS;
}

void assert(bool condition, int code, const char *format, ...)
{
    if (condition) return;
    
    va_list args;
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    exit(code);
}

void bubbleSort(short *Y, unsigned n)
{
    for (size_t i = 0; i < n; i++) for (size_t j = i + 1; j < n; j++) if (Y[i] < Y[j])
    {
        short temp = Y[i];
        Y[i] = Y[j];
        Y[j] = temp;
    }
}

params decodeParameters(int argc, const char *argv[])
{
    assert(argc == 2, EXIT_FAILURE, "Uso: %s <inputFilename>\n", argv[0]);
    
    assert(
        !strcmp(argv[1] + (strlen(argv[1]) - 4), ".csv"),
        EXIT_FAILURE,
        "Il file fornito in input non ha estensione \"csv\".\n"
    );
    
    params args;
    
    strncpy(args.inputFilename, argv[1], MAXSTRLEN - 1);
    
    return args;
}

short *getArray(short ***X, unsigned n, unsigned m)
{
    short *Y = new(short[n]);
    assert(Y, ENOMEM, "`getArray': allocazione fallita.\n");
    
    for (size_t i = 0; i < n; i++)
    {
        short max = *X[i][0];
        
        for (size_t j = 1; j < m; j++) if (max < *X[i][j]) max = *X[i][j];
        
        Y[i] = max;
    }
    
    return Y;
}

void processX(short ***X, const short *Y, unsigned n, unsigned m)
{
    for (size_t i = 0; i < n; i++) 
    for (size_t j = 0; j < m; j++)
    if (*X[i][j] > Y[i] / 2 + 1)
    {
        delete(X[i][j]);
        X[i][j] = nullptr;
    }
}

short ***readFile(stream input, unsigned n, unsigned m)
{
    short ***X = new(short**[n]);
    assert(X, ENOMEM, "`readFile': allocazione fallita.\n");
    
    for (size_t i = 0; i < n; i++)
    {
        X[i] = new(short*[m]);
        assert(X[i], ENOMEM, "`readFile': allocazione fallita.\n");
        
        for (size_t j = 0; j < m; j++)
        {
            X[i][j] = new(short);
            assert(X[i][j], ENOMEM, "`readFile': allocazione fallita.\n");
            fscanf(input, "%hd,", X[i][j]);
        }
    }
    
    close(input);
    return X;
}

stream readHeader(const char *inputFilename, unsigned *n, unsigned *m)
{
    stream input = open(inputFilename, "r");
    
    assert(input, ENOENT, "Il file \"%s\" e' illegibile o inesistente.\n", inputFilename);
    
    fscanf(input, "%u,%u", n, m);
    
    return input;
}