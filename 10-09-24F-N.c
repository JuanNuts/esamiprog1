#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXFLEN 50

struct Parameters
{ char in_filename[MAXFLEN], out_filename[MAXFLEN], c; };

struct Node
{
    char value;
    struct Node *next;
};

struct List
{
    struct Node *head, *tail;
};

void append(struct List *this, char value)
{
    struct Node *new = malloc(sizeof(struct Node));

    if (!new) return;
    new->value = value;
    new->next = NULL;

    if (!this->head) this->head = this->tail = new;
    else this->tail = this->tail->next = new;
}

void print(const struct List *this)
{
    for (struct Node *it = this->head; it; it = it->next)
    fprintf(stdout, "%c", it->value);
}

void removeNode(struct List *this, struct Node *node)
{
    if (!this->head || !node) return;
    
    if (node == this->head)
    {
        this->head = this->head->next;
        if (!this->head) this->tail = NULL;
    } 
    
    else 
    {
        struct Node *prev = this->head;

        while (prev && prev->next != node) prev = prev->next;

        if (!prev) return;

        prev->next = node->next;

        if (!prev->next) this->tail = prev;
    }

    free(node);
}

struct Parameters decodeParameters(int argc, const char *const argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: %s <in_filename> <out_filename> <c>.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strlen(argv[3]) != 1)
    {
        fprintf(stderr, "Il carattere c deve essere un simbolo valido.\n");
        exit(EXIT_FAILURE);
    }

    struct Parameters parameters = {0};

    strncpy(parameters.in_filename, argv[1], MAXFLEN - 1);
    strncpy(parameters.out_filename, argv[2], MAXFLEN - 1);
    parameters.c = argv[3][0];

    return parameters;
}

struct List readFile(const char *in_filename)
{
    FILE *fp = fopen(in_filename, "r");

    if (!fp)
    {
        fprintf(stderr, "Il file \"%s\" e' illegibile o inesistente.\n", in_filename);
        exit(EXIT_FAILURE);
    }

    struct List L = {0};
    char buffer = 0;

    while (fscanf(fp, "%c", &buffer) != EOF) append(&L, buffer);

    return L;
}

size_t countOccurrences(const struct List *this, char c)
{
    size_t count = 0;

    for (struct Node *it = this->head; it; it = it->next)
    if (it->value == c) count++;

    return count;
}

void processL(struct List *this, size_t o)
{
    struct Node *it = this->head;

    while (it) if (countOccurrences(this, it->value) < o) it = it->next;
    else
    {
        removeNode(this, it);
        it = this->head;
    }
}

void writeToFile(struct List *this, const char *out_filename)
{
    FILE *fp = fopen(out_filename, "w");

    if (!fp)
    {
        fprintf(stderr, "Il file \"%s\" non e' disponibile per la scrittura", out_filename);
        exit(EXIT_FAILURE);
    }

    while (this->head)
    {
        fprintf(fp, "%c", this->head->value);
        removeNode(this, this->head);
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    struct Parameters parameters = decodeParameters(argc, (const char**)(argv));

    puts("==========PUNTO A==========");
    printf("Nome del file di input: %s\nNome del file di output: %s\nValore di c: %c\n", 
        parameters.in_filename, parameters.out_filename, parameters.c
    );

    struct List L = readFile(parameters.in_filename);
    puts("\n==========PUNTO B==========");
    print(&L);

    size_t o = countOccurrences(&L, parameters.c);
    puts("\n\n==========PUNTO C==========");
    printf("Occorrence di %c in L: %lu", parameters.c, o);

    processL(&L, o);
    puts("\n==========PUNTO D==========");
    print(&L);

    writeToFile(&L, parameters.out_filename);
}