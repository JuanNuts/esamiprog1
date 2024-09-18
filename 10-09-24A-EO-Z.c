#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRLEN 30

struct Parameters
{
    char fileName[MAXSTRLEN];
    int x, y;
};

struct Record
{
    char name[MAXSTRLEN], surname[MAXSTRLEN];
    int age;
};

typedef struct Node
{
    struct Record data;
    struct Node *next;
} *List;

struct Parameters readInput(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: <nomefile> <x> <y>.\n");
        exit(-1);
    }

    struct Parameters parameters = {0};

    strncpy(parameters.fileName, argv[1], MAXSTRLEN - 1);
    parameters.x = atoi(argv[2]);
    parameters.y = atoi(argv[3]);

    if (parameters.x < 10 || parameters.x > 60)
    {
        fprintf(stderr, "Il parametro x deve appartenere all'intervallo [10, 60].\n");
        exit(-1);
    }

    if (parameters.y < 10 || parameters.y > 60)
    {
        fprintf(stderr, "Il parametro y deve appartenere all'intervallo [10, 60].\n");
        exit(-1);
    }

    if (parameters.x >= parameters.y)
    {
        fprintf(stderr, "I parametri x e y devono essere tali che 10 <= x < y <= 60.\n");
        exit(-1);
    }

    return parameters;
}

void insert(List *head, struct Record data)
{
    struct Node *node = malloc(sizeof(struct Node));

    if (!node)
    {
        fprintf(stderr, "Allocazione fallita.");
        exit(-1);
    }

    node->data = data;
    
    struct Node *prev = NULL;

    while (*head && strcmp((*head)->data.surname, data.surname) < 0)
    {
        prev = *head;
        head = &(*head)->next;
    }

    node->next = *head;

    if (prev) prev->next = node;
    else *head = node;
}

List buildList(const char *filename)
{
    FILE *fp = fopen(filename, "r");

    if (!fp)
    {
        fprintf(stderr, "File \"%s\" inesistente", filename);
        exit(-1);
    }

    List records = NULL;
    struct Record data = {0};
    
    while (fscanf(fp, "%s %s %d", data.name, data.surname, &data.age) != EOF)
    insert(&records, data);

    fclose(fp);
    return records;
}

void printList(const List *head)
{
    for (struct Node *node = *head; node; node = node->next)
    printf("%.7s\t%.7s\t%d\n", node->data.name, node->data.surname, node->data.age);
}

void removeByAge(List *head, int x, int y)
{
    struct Node *prev = NULL;

    while (*head) if ((*head)->data.age >= x && (*head)->data.age <= y)
    {
        struct Node *disposable = *head;

        if (prev) prev->next = (*head)->next;
        else *head = (*head)->next;
        
        //head = &disposable->next;
        free(disposable);
    } else {
        prev = *head;
        head = &(*head)->next;
    }
}

int main(int argc, char *argv[])
{
    struct Parameters parameters = readInput(argc, argv);
    List records = buildList(parameters.fileName);
    
    printList(&records);
    puts("--------------------");
    removeByAge(&records, parameters.x, parameters.y);
    printList(&records);
}