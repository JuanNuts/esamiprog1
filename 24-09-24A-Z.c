#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRLEN 256

typedef struct { char input_filename[MAXSTRLEN], output_filename[MAXSTRLEN]; } Parameters;
typedef struct Node { char data[MAXSTRLEN]; struct Node *next; } *List;

float average(const short *Y, size_t size)
{
    float total = 0.0f;
    
    for (size_t i = 0; i < size; i++) total += Y[i];
    
    return total / size;
}

void clear(List *head)
{
    while (*head)
    {
        struct Node *disposable = *head;
        
        *head = (*head)->next;
        free(disposable);
    }
}

size_t count(const List *head)
{
    size_t count = 0;
    
    for (const struct Node *node = *head; node; node = node->next) count++;
    
    return count;
}

short countVowels(const char *str)
{
    short count = 0;
    
    for (unsigned c = 0; str[c]; c++) switch (str[c])
    {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        count++;
    }
    
    return count;
}

Parameters decodeParameters(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <input_filename> <output_filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    Parameters parameters = {0};
    
    strncpy(parameters.input_filename, argv[1], MAXSTRLEN - 1);
    strncpy(parameters.output_filename, argv[2], MAXSTRLEN - 1);
    
    if (strcmp(parameters.input_filename + strlen(parameters.input_filename) - 5, ".text"))
    {
        fprintf(stderr, "Estensione non supportata, usare file con estensione \"*.text\"\n");
        exit(EXIT_FAILURE);
    }
    
    if (strcmp(parameters.output_filename + strlen(parameters.output_filename) - 7, ".output"))
    {
        fprintf(stderr, "Estensione non supportata, usare file con estensione \"*.output\"\n");
        exit(EXIT_FAILURE);
    }
    
    return parameters;
}

void insert(List *head, const char *data)
{
    struct Node *node = malloc(sizeof(struct Node));
    
    if (!node)
    {
        fprintf(stderr, "Allocazione fallita.\n");
        exit(ENOMEM);
    }
    
    strncpy(node->data, data, MAXSTRLEN - 1);
    
    struct Node *prev = NULL;
    
    while (*head && strlen((*head)->data) < strlen(node->data))
    {
        prev = *head;
        head = &(*head)->next;
    }
    
    node->next = *head;
    
    if (prev) prev->next = node;
    else *head = node;
}

void filterList(List *head, float avg)
{
    struct Node *prev = NULL;
    
    while (*head) if (countVowels((*head)->data) > avg)
    {
        struct Node *disposable = *head;

        if (prev) prev->next = (*head)->next;
        else *head = (*head)->next;
        
        free(disposable);
    } else {
        prev = *head;
        head = &(*head)->next;
    }
}

short *getArray(const List *head)
{
    short *Y = calloc(count(head), sizeof(short));
    size_t i = 0;
    
    if (!Y)
    {
        fprintf(stderr, "Allocazione fallita\n");
        exit(ENOMEM);
    }
    
    for (const struct Node *node = *head; node; node = node->next, i++)
    Y[i] = countVowels(node->data);
    
    return Y;
}

void print(const List *head, FILE *stream)
{
    for (const struct Node *node = *head; node; node = node->next)
    fprintf(stream, "%s\n", node->data);
}

List readFile(const char *input_filename)
{
    FILE *input = fopen(input_filename, "r");
    List X = NULL;
    char buffer[MAXSTRLEN];
    
    if (!input)
    {
        fprintf(stderr, "Impossibile aprire il file \"%s\"\n", input_filename);
        exit(ENOENT);
    }
    
    while (!feof(input))
    {
        fgets(buffer, MAXSTRLEN, input);
        if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = 0;
        insert(&X, buffer);
    }
    
    fclose(input);
    return X;
}

void writeToFile(const List *head, const char *output_filename)
{
    FILE *output = fopen(output_filename, "w");
    
    if (!output)
    {
        fprintf(stderr, "Impossibile aprire il file \"%s\"\n", output_filename);
        exit(ENOENT);
    }
    
    print(head, output);
    
    fclose(output);
}

int main(int argc, char *argv[])
{
    Parameters parameters = decodeParameters(argc, argv);
    
    puts("====== A Stampa Parametri ======");
    printf("input_filename = %s\noutput_filename = %s\n\n", parameters.input_filename, parameters.output_filename);
    
    List X = readFile(parameters.input_filename);
    
    puts("====== B Lista ordinata per lunghezza ======");
    print(&X, stdout);
    
    short *Y = getArray(&X);
    size_t listCount = count(&X);
    
    puts("\n====== C Array Y (numero di vocali) ======");
    for (size_t i = 0; i < listCount; i++) printf("%hd ", Y[i]);
    
    float avg = average(Y, listCount);
    
    filterList(&X, avg);
    
    puts("\n\n====== D Media di Y e lista dopo filtro ======");
    printf("Media di Y: %.1f\nContenuto di X:\n", avg);
    print(&X, stdout);
    
    writeToFile(&X, parameters.output_filename);
    
    clear(&X);
    free(Y);
}