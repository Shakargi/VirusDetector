#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Virus structure defention

typedef struct virus {
unsigned short SigSize;
unsigned char* VirusName;
unsigned char* Sig;
} virus;


int littleEndien;



/* -----------------  1A - read & print viruses ----------------- */

virus* readVirus(FILE* file){
    virus *v = (virus*)malloc(sizeof(virus));
    if (!v) return NULL;
 
    if (fread(&v->SigSize, sizeof(unsigned short), 1, file) != 1) {
        free(v);
        return NULL;
    }
 
    if (!littleEndien) {
        unsigned char *b = (unsigned char *)&v->SigSize;
        unsigned char tmp = b[0];
        b[0] = b[1];
        b[1] = tmp;
    }
 
    v->VirusName = (unsigned char*)malloc(16);
    if (!v->VirusName) {
        free(v);
        return NULL;
    }
 
    if (fread(v->VirusName, 1, 16, file) != 16) {
        free(v->VirusName);
        free(v);
        return NULL;
    }
 
    v->Sig = (unsigned char*)malloc(v->SigSize);
    if (!v->Sig) {
        free(v->VirusName); 
        free(v); 
        return NULL;
    }
 
    if (fread(v->Sig, 1, v->SigSize, file) != v->SigSize) {
        free(v->Sig);
        free(v->VirusName);
        free(v);
        return NULL;
    }
 
    return v;
    
}


void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus Name: %s\n", virus->VirusName);
    fprintf(output, "Virus size: %hu\n", virus->SigSize);
    fprintf(output, "signature:\n");
    for (unsigned short i = 0; i < virus->SigSize; i++)
        fprintf(output, "%02X ", virus->Sig[i]);
    fprintf(output, "\n\n");
}

int magicNumber(FILE* file) {
    char magic[4];
    if (fread(magic, 1, 4, file) != 4) {
        fprintf(stderr, "Error: file doesn't contain a magic number\n");
        return 0;
    }
    if (strncmp(magic, "VIRL", 4) == 0)
        littleEndien = 1;
    else if (strncmp(magic, "VIRB", 4) == 0)
        littleEndien = 0;
    else {
        fprintf(stderr, "Error: invalid magic number\n");
        return 0;
    }
    return 1;
}


/* ----------------- 1B - Linked List implementation -----------------*/

typedef struct link
{
    struct link* nextVirus;
    virus *vir;
} link;




link* deleteLink(link* node){
    link* next = node->nextVirus;
    free(node->vir->Sig);
    free(node->vir->VirusName);
    free(node->vir);
    free(node);

    return next;

}
void list_print(link* virus_list, FILE* file){
    link* node = virus_list;
    while (node != NULL)
    {
        printVirus(node->vir, file);
        node = node->nextVirus;
    }
}

link* list_append(link* virus_list, virus* data){
    link* firstLink = (link*)malloc(sizeof(link));
    if(!firstLink) return virus_list;
    firstLink->nextVirus = virus_list;
    firstLink->vir = data;

    return firstLink;
    
}

void list_free(link* virus_list){
    while (virus_list != NULL){
        virus_list = deleteLink(virus_list);
    }
}


int main() {
    link* virus_list = NULL;
    char buffer[10000];
    char fileName[256];
    char choice[10];

    while (1) {
        printf("<L>oad signatures\n");
        printf("<P>rint signatures\n");
        printf("<S>elect file to inspect\n");
        printf("<D>etect viruses\n");
        printf("<F>ix file\n");
        printf("<Q>uit\n");
        printf("Choose: ");

        fgets(choice, sizeof(choice), stdin);
        char c;
        sscanf(choice, " %c", &c);

        if (c == 'L') {
            char filename[256];
            printf("Enter signatures file name: ");
            fgets(filename, sizeof(filename), stdin);
            filename[strcspn(filename, "\n")] = '\0';  // strip newline
            FILE* f = fopen(filename, "rb");
            if (!f) { fprintf(stderr, "Cannot open file\n"); continue; }

            if (!magicNumber(f)) {
               fclose(f);
                continue;
            }

            if (virus_list != NULL) list_free(virus_list);
            virus_list = NULL;

            virus* v;
            while ((v = readVirus(f)) != NULL)
                virus_list = list_append(virus_list, v);

            fclose(f);
            printf("Signatures loaded.\n\n");

        } else if (c == 'P') {
            list_print(virus_list, stdout);

        } else if (c == 'S') {
            printf("Enter File Name: ");
            fgets(fileName, sizeof(fileName), stdin);
            fileName[strcspn(fileName, "\n")] = '\0';
            printf("File selected\n\n");

        } else if (c == 'D') {
            if (!fileName) fprintf(stderr, "No file has been selected");
            FILE* f = fopen(fileName, "r");
            if (!f) printf(stderr, "Cannot read the file");
            fread(buffer, 1, sizeof(buffer), f);
            
            

        } else if (c == 'F') {
            printf("Not implemented\n");

        } else if (c == 'Q') {
            list_free(virus_list);
             // if (inspectedFile) fclose(inspectedFile);
            break;
        }
    }

    return 0;
}
