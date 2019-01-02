/*
    search.c
    saku
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tnode{
    char term[21];
    struct tnode *left;
    struct tnode *right;
    struct lnode *pos;
} tnode;

typedef struct lnode{
    int id;
    struct lnode *next;
} lnode;

typedef struct snode {
    lnode *pos;
    struct snode *next;
} snode;

tnode *createTNode(int id, char *term);
tnode *insertTermAndList(tnode **node, int id, char *term);
tnode *search(tnode **node, char *term);
void freeTree(tnode **node);

lnode *createLNode(int id);
lnode *insertLNode(lnode **node, int id);
int searchLNode(lnode **node, int id);
void printList(lnode **node);
void freeList(lnode **node);

void push(snode **first, lnode *pos);
lnode *pop(snode **first);
lnode *operationAnd(lnode **first, lnode **second);
lnode *operationOr(lnode **first, lnode **second);
lnode *operationNot(lnode **first, lnode **second);

int main(int argc, char **argv) {
    char *filename;
    FILE *fp;

    int id;
    char *query;
    char buf[1005];
    tnode *root = NULL;
    tnode *hit;
    snode *first = NULL;

    if (argc != 2) {
        fprintf(stderr,"Usage: %s prob_file\n", argv[0]);
        return 1;
    }
    filename = argv[1];

    if((fp = fopen(filename, "r")) == NULL){
        fprintf(stderr, "%s: file cannot open.\n", filename);
        return 1;
    }

    while (fscanf(fp, "%1004[^\n]%*c", buf) != EOF) {
        char *tmp;
        tmp = strtok(buf, "\t");
        id = (int) strtol(tmp, NULL, 10);
        while (tmp != NULL) {
            tmp = strtok(NULL, " ");
            if (tmp != NULL) {
                root = insertTermAndList(&root, id, tmp);
            }
        }
    }

    scanf("%400[^\n]%*c", buf);
    query = strtok(buf, " ");
    while (query != NULL) {
        if (strcmp(query, "AND") == 0) {
            lnode *latter = pop(&first);
            lnode *former = pop(&first);
            push(&first, operationAnd(&former, &latter));
        } else if (strcmp(query, "OR") == 0) {
            lnode *latter = pop(&first);
            lnode *former = pop(&first);
            push(&first, operationOr(&former, &latter));
        } else if (strcmp(query, "NOT") == 0) {
            lnode *latter = pop(&first);
            lnode *former = pop(&first);
            push(&first, operationNot(&former, &latter));
        } else {
            hit = search(&root, query);
            if (hit != NULL) push(&first, hit->pos);
            else push(&first, NULL);
        }
        query = strtok(NULL, " ");
    }
    if (first->pos != NULL) printList(&(first->pos));
    else printf("Not found\n");

    while (first != NULL) pop(&first);
    
    freeTree(&root);

    fclose(fp);

    return 0;

}

tnode *createTNode(int id, char *term){
    tnode *node;
    node = (tnode*)malloc(sizeof(tnode));
    if (node == NULL) {
        printf("Cannot allocate memory.\n");
        exit(2);
    }
    node->pos = createLNode(id);
    strcpy(node->term, term);
    node -> left = NULL;
    node -> right = NULL;
    return node;
}

tnode *insertTermAndList(tnode **node, int id, char *term){
    if (*node == NULL) {
        *node = createTNode(id, term);
    } else if (strcmp(term, (*node)->term) > 0) {
        (*node)->right = insertTermAndList(&((*node)->right), id, term);
    } else if (strcmp(term, (*node)->term) < 0) {
        (*node)->left = insertTermAndList(&((*node)->left), id, term);
    } else if (strcmp(term, (*node)->term) == 0) {
        (*node)->pos = insertLNode(&((*node)->pos), id);
    }
    return *node;
}

tnode *search(tnode **node, char *term){
    if (*node == NULL) return NULL;
    else if (strcmp(term, (*node)->term) > 0) return search(&((*node)->right), term);
    else if (strcmp(term, (*node)->term) < 0) return search(&((*node)->left), term);
    return *node;
}

void freeTree(tnode **node){
    if ((*node)->left != NULL) freeTree(&((*node)->left));
    if ((*node)->right != NULL) freeTree(&((*node)->right));
    if ((*node)->pos != NULL) freeList(&((*node)->pos));
    free(*node);
}

lnode *createLNode(int id){
    lnode *new = (lnode*)malloc(sizeof(lnode));
    if (new == NULL) {
        printf("Cannot allocate memory.\n");
        exit(1);
    }
    new->id = id;
    new->next = NULL;
    return new;
}

lnode *insertLNode(lnode **node, int id) {
    if (*node == NULL) {
        *node = createLNode(id);
    } else if ((*node)->next != NULL && (*node)->id < id) {
        if ((*node)->next->id < id) {
            (*node)->next = insertLNode(&((*node)->next), id);
        } else if ((*node)->next->id > id) {
            lnode *tmp = createLNode(id);
            tmp->next = (*node)->next;
            (*node)->next = tmp;
        }
    } else if ((*node)->id < id) {
            (*node)->next = createLNode(id);
    } else if ((*node)->id > id) {
            lnode *tmp = createLNode(id);
            tmp->next = *node;
            return tmp;
    } 
    return *node;
} 

void printList(lnode **node) {
    lnode *tmp = *node;
    while (tmp != NULL) {
        printf("%d\n", tmp->id);
        tmp = tmp->next;
    }
}

void freeList(lnode **node) {
    lnode *tmp = *node;
    while (tmp != NULL) {
        free(tmp); 
        tmp = tmp->next;
    }
}

void push(snode **first, lnode *pos) {
    snode *new = (snode*)malloc(sizeof(snode));
    if (new == NULL) {
        printf("Cannot allocate memory.\n");
        exit(1);
    }

    new->pos = pos;
    new->next = *first;
    *first = new;
} 

//スタックからpopしてその値を返す関数
lnode *pop(snode **first) {
    lnode *pos;
    snode *tmp = *first;
    if (tmp == NULL) exit(2);

    pos = tmp->pos;
    *first = tmp->next;
    free(tmp);
    return pos; 
}

lnode *operationAnd(lnode **first, lnode **second){
    if (*first == NULL || *second == NULL) {
        return NULL;
    } else {
        lnode *new = NULL;
        while (*second != NULL) {     
            if (searchLNode(first, (*second)->id)) {
                new = insertLNode(&new, (*second)->id);
            }
            *second = (*second)->next;
        }
        return new;
    }
}

int searchLNode(lnode **node, int id) {
    lnode *tmp = *node;
    while (tmp != NULL) {
        if (id == tmp->id) return 1;  
        tmp = tmp->next;
    }
    return 0;
}

lnode *operationOr(lnode **first, lnode **second){
    if (*first == NULL && *second == NULL) {
        return NULL;
    } else if (*first == NULL) {
        return *second;
    } else {
        while (*second != NULL) {
            *first = insertLNode(first, (*second)->id);
            *second = (*second)->next;
        }
        return *first;
    }
}

lnode *operationNot(lnode **first, lnode **second){
    lnode *new = NULL;
    while (*first != NULL) {     
        if (searchLNode(second, (*first)->id) == 0) {
            new = insertLNode(&new, (*first)->id);
        }
        *first = (*first)->next;
    }
    return new;
}
