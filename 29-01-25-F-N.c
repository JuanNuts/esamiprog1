#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 50

typedef struct {char input_filename[LEN], output_filename[LEN];} params;

struct Person
{
    char name[LEN], surname[LEN];
    unsigned short age;
    float height;
};


void assert(bool condition, int code, const char *format, ...)
{
    if (condition) return;

    va_list args;
    
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(code);
}

params decodeParameters(int argc, char *argv[])
{
    assert(argc == 3, EXIT_FAILURE, "Uso: %s <input> <output>\n", argv[0]);
    assert(!strcmp(argv[1] + strlen(argv[1]) - 4, ".txt"), EXIT_FAILURE, "Errore: estensione del file di input non supportata. Usare file con estensione \"txt\".\n");
    assert(!strcmp(argv[2] + strlen(argv[2]) - 4, ".txt"), EXIT_FAILURE, "Errore: estensione del file di output non supportata. Usare file con estensione \"txt\".\n");

    params args;

    strcpy(args.input_filename, argv[1]);
    strcpy(args.output_filename, argv[2]);

    return args;
}

unsigned readHeader(FILE *input)
{
    unsigned n = 0;

    fscanf(input, "%u", &n);
    return n;
}

struct Person *readFile(FILE *input, unsigned n)
{
    struct Person *persons = calloc(sizeof(struct Person), n);

    for (unsigned i = 0; i < n; i++) fscanf(input, "%s %s %hu %f", 
        persons[i].name,
        persons[i].surname,
        &persons[i].age,
        &persons[i].height
    );

    return persons;
}

unsigned short *extractAges(const struct Person *persons, unsigned n)
{
    unsigned short *ages = calloc(sizeof(unsigned short), n);

    for (unsigned i = 0; i < n; i++) ages[i] = persons[i].age;
    return ages;
}

void sortArray(unsigned short *z, unsigned n)
{
    for (unsigned i = 1; i < n; i++)
    {
        unsigned j = i - 1;
        unsigned short elem = z[i];

        while (j != UINT_MAX && z[j] > elem)
        z[j-- + 1] = z[j];

        z[j + 1] = elem;
    }
}

float calculateAverageHeight(struct Person *persons, unsigned n, unsigned short ageMedian)
{
    float tot = 0.0f;
    unsigned count = 0; 

    for (unsigned i = 0; i < n; i++) if (persons[i].age > ageMedian)
    {
        count++;
        tot += persons[i].height;
    }

    return (count == 0 ? 0.0f : tot / count);
}

void writeOutput(const char *output_filename, struct Person *persons, unsigned n, float averageHeight)
{
    FILE *output = fopen(output_filename, "w");

    for (unsigned i = 0; i < n; i++)
    if (persons[i].height > averageHeight)
    fprintf(output, "%s %s, Età: %hu, Altezza: %.2f\n", 
        persons[i].name, 
        persons[i].surname, 
        persons[i].age, 
        persons[i].height
    );

    fclose(output);
}

int main(int argc, char *argv[])
{
    params args = decodeParameters(argc, argv);
    
    puts("======A Stampa Parametri======");
    printf("input_filename = %s\noutput_filename = %s\n\n", args.input_filename, args.output_filename);

    FILE *input = fopen(args.input_filename, "r");

    assert(input, EXIT_FAILURE, "Errore: Il file \"%s\" non puo' essere aperto correttamente per la lettura.\n", args.input_filename);

    unsigned n = readHeader(input);

    puts("======B Valori letti dall’header======");
    printf("n = %u\n\n", n);

    struct Person *persons = readFile(input, n);

    fclose(input);
    puts("======B Contenuto dell’array di strutture======");
    for (unsigned i = 0; i < n; i++) printf("%u. Nome: %s, Cognome: %s, Età: %hu, Altezza: %.2f\n",
        i + 1,
        persons[i].name,
        persons[i].surname,
        persons[i].age,
        persons[i].height
    );

    unsigned short *z = extractAges(persons, n);

    puts("\n======C Array Z (età)======");
    for (unsigned i = 0; i < n; i++) printf("%hu ", z[i]);
    printf("\n\n");

    sortArray(z, n);
    puts("======C Array Z Ordinato======");
    for (unsigned i = 0; i < n; i++) printf("%hu ", z[i]);
    printf("\n\nMediana dell'età': %hu\n\n", z[n / 2]);

    float averageHeight = calculateAverageHeight(persons, n, z[n / 2]);

    puts("======D Altezza media delle persone con età superiore alla mediana======");
    printf("Media: %.2f\n\n", averageHeight);

    writeOutput(args.output_filename, persons, n, averageHeight);
    puts("======E Scrittura su file risultato.txt======\nFile scritto correttamente.");
    
    free(persons);
    free(z);
    putchar('\a');
    return EXIT_SUCCESS;
}