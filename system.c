#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1024
#define MAX_INSTITUTIONS 1000

typedef struct
{
    char name[100];
    char address[100];
    char postalCode[20];
    char phoneNumber[20];
    char institutionsCategory[50];
    char institutionsType[50];
} Institution;

Institution institutions[MAX_INSTITUTIONS];
int institutionCount = 0;

void trimNewline(char *str)
{
    int len = strlen(str);
    if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
    {
        str[len - 1] = '\0';
    }
}

void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

void readCSV(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Unable to open file for reading");
        return;
    }

    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, MAX_LINE_LENGTH, file) && institutionCount < MAX_INSTITUTIONS)
    {
        trimNewline(buffer);
        Institution inst;
        if (sscanf(buffer, "%99[^,],%99[^,],%19[^,],%19[^,],%49[^,],%49[^\n]",
                   inst.name, inst.address, inst.postalCode, inst.phoneNumber,
                   inst.institutionsCategory, inst.institutionsType) == 6)
        {
            institutions[institutionCount++] = inst;
        }
    }

    fclose(file);
}

void writeCSV(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Unable to open file for writing");
        return;
    }

    for (int i = 0; i < institutionCount; i++)
    {
        fprintf(file, "%s,%s,%s,%s,%s,%s\n",
                institutions[i].name, institutions[i].address,
                institutions[i].postalCode, institutions[i].phoneNumber,
                institutions[i].institutionsCategory, institutions[i].institutionsType);
    }

    fclose(file);
}

void printInstitution(const Institution *inst, FILE *file)
{
    fprintf(file, "Name: %s\n", inst->name);
    fprintf(file, "Address: %s\n", inst->address);
    fprintf(file, "Postal Code: %s\n", inst->postalCode);
    fprintf(file, "Phone Number: %s\n", inst->phoneNumber);
    fprintf(file, "Category: %s\n", inst->institutionsCategory);
    fprintf(file, "Type: %s\n\n", inst->institutionsType);
}

void listAllInstitutions(FILE *file)
{
    for (int i = 0; i < institutionCount; i++)
    {
        printInstitution(&institutions[i], file);
    }
}

void findInstitutionByName(FILE *file)
{
    char nameToFind[100];
    printf("Enter the name of the institution to find: ");
    fgets(nameToFind, sizeof(nameToFind), stdin);
    trimNewline(nameToFind);

    bool found = false;
    for (int i = 0; i < institutionCount; i++)
    {
        if (strcmp(institutions[i].name, nameToFind) == 0)
        {
            printInstitution(&institutions[i], file);
            found = true;
        }
    }

    if (!found)
    {
        fprintf(file, "Institution with name '%s' not found.\n", nameToFind);
    }
}

void addInstitution(FILE *file)
{
    if (institutionCount >= MAX_INSTITUTIONS)
    {
        fprintf(file, "Maximum number of institutions reached.\n");
        return;
    }

    Institution newInst;

    printf("Enter Institution Name: ");
    fgets(newInst.name, sizeof(newInst.name), stdin);
    trimNewline(newInst.name);

    printf("Enter Address: ");
    fgets(newInst.address, sizeof(newInst.address), stdin);
    trimNewline(newInst.address);

    printf("Enter Postal Code: ");
    fgets(newInst.postalCode, sizeof(newInst.postalCode), stdin);
    trimNewline(newInst.postalCode);

    printf("Enter Phone Number: ");
    fgets(newInst.phoneNumber, sizeof(newInst.phoneNumber), stdin);
    trimNewline(newInst.phoneNumber);

    printf("Enter Institution Category: ");
    fgets(newInst.institutionsCategory, sizeof(newInst.institutionsCategory), stdin);
    trimNewline(newInst.institutionsCategory);

    printf("Enter Institution Type: ");
    fgets(newInst.institutionsType, sizeof(newInst.institutionsType), stdin);
    trimNewline(newInst.institutionsType);

    institutions[institutionCount++] = newInst;
    fprintf(file, "Institution '%s' added successfully.\n", newInst.name);
}

void removeInstitution(FILE *file)
{
    char name[100];
    printf("Enter the name of the institution to remove: ");
    fgets(name, sizeof(name), stdin);
    trimNewline(name);

    for (int i = 0; i < institutionCount; i++)
    {
        if (strcmp(institutions[i].name, name) == 0)
        {
            for (int j = i; j < institutionCount - 1; j++)
            {
                institutions[j] = institutions[j + 1];
            }
            institutionCount--;
            fprintf(file, "Institution '%s' removed successfully.\n", name);
            return;
        }
    }
    fprintf(file, "Institution '%s' not found.\n", name);
}

void printTotalInstitutions(FILE *file)
{
    fprintf(file, "Total number of institutions: %d\n", institutionCount);
}

void printTotalsByType(FILE *file)
{
    char typeToFind[50];
    printf("Enter the institution type to count: ");
    fgets(typeToFind, sizeof(typeToFind), stdin);
    trimNewline(typeToFind);

    int count = 0;
    for (int i = 0; i < institutionCount; i++)
    {
        if (strcmp(institutions[i].institutionsType, typeToFind) == 0)
        {
            count++;
        }
    }

    fprintf(file, "Total number of institutions of type '%s': %d\n", typeToFind, count);
}

void offerFileOutput(void (*operation)(FILE *), FILE *defaultFile)
{
    int writeToFiles;
    printf("Do you want to write these details to a file? (1 for yes, 0 for no): ");
    scanf("%d", &writeToFiles);
    clearInputBuffer();

    FILE *tempFile = tmpfile();
    if (!tempFile)
    {
        perror("Error creating temporary file");
        return;
    }

    operation(tempFile);

    fseek(tempFile, 0, SEEK_SET);
    char buffer[MAX_LINE_LENGTH];

    if (writeToFiles)
    {
        char filename[100];
        printf("Enter filename to write output: ");
        fgets(filename, sizeof(filename), stdin);
        trimNewline(filename);

        FILE *file = fopen(filename, "w");
        if (!file)
        {
            perror("Error opening file");
            return;
        }

        while (fgets(buffer, MAX_LINE_LENGTH, tempFile))
        {
            fprintf(file, "%s", buffer);
            fprintf(defaultFile, "%s", buffer);
        }
        fclose(file);
        printf("Output written to %s\n", filename);
    }
    else
    {
        while (fgets(buffer, MAX_LINE_LENGTH, tempFile))
        {
            fprintf(defaultFile, "%s", buffer);
        }
    }

    fclose(tempFile);
}

int main()
{
    readCSV("Institutions.csv");

    bool running = true;
    char choice;

    while (running)
    {
        printf("\nMenu:\n");
        printf("1. Add New Institution\n");
        printf("2. Remove an Institution\n");
        printf("3. Find Institution By Name\n");
        printf("4. List All Institutions\n");
        printf("5. Total Number of Institutions\n");
        printf("6. Total Number of Institutions by Type\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);
        clearInputBuffer();

        switch (choice)
        {
        case '1':
            offerFileOutput(addInstitution, stdout);
            break;
        case '2':
            offerFileOutput(removeInstitution, stdout);
            break;
        case '3':
            offerFileOutput(findInstitutionByName, stdout);
            break;
        case '4':
            offerFileOutput(listAllInstitutions, stdout);
            break;
        case '5':
            offerFileOutput(printTotalInstitutions, stdout);
            break;
        case '6':
            offerFileOutput(printTotalsByType, stdout);
            break;
        case '7':
            writeCSV("Institutions.csv");
            running = false;
            break;
        default:
            printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}
