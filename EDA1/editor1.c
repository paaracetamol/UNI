#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 81



struct no {
    char txt[MAX];
    struct no *ant;
    struct no *seg;
};




struct no *ini = NULL;
struct no *end = NULL;
struct no *cur = NULL;




struct no *node(char *t) {

    struct no *n = malloc(sizeof(struct no));

    int i = 0;
    while (t[i] != '\0') {
        n->txt[i] = t[i];
        i++;
    }
    n->txt[i] = '\0';

    n->ant = NULL;
    n->seg = NULL;
    return n;
}





int count() {
    int c = 0;
    struct no *p = ini;

    while (p != NULL) {
        c = c + 1;
        p = p->seg;

    }

    return c;
}






// node -> pos i
struct no *nodeTo(int i) {
    int c = 1;
    struct no *p = ini;

    while (p != NULL) {
        if (c == i) {
            return p;
        }

        c++;
        p = p->seg;
    }

    return NULL;

}







// p depois x, p NULL -> ini
void _insert(struct no *p, struct no *x) {
    struct no *s;

    if (ini == NULL) {
        ini = x;
        end = x;
        x->ant = NULL;
        x->seg = NULL;
        return;
    }


    if (p == NULL) {
        x->ant = NULL;
        x->seg = ini;
        ini->ant = x;
        ini = x;
        return;
    }


    s = p->seg;
    x->ant = p;
    x->seg = s;
    p->seg = x;

    if (s != NULL) {
        s->ant = x;
    }
    else {
        end = x;
    }
}






void tira(struct no *p) {
    if (p == NULL) return;

    if (p->ant != NULL) p->ant->seg = p->seg;
    else ini = p->seg;



    if (p->seg != NULL) p->seg->ant = p->ant;
    else end = p->ant;

}





// l > 80 -> newline
void wrap(struct no *p) {
    char resto[1000];
    struct no *x;

    while (p != NULL && strlen(p->txt) > 80) {
        strcpy(resto, p->txt + 80);
        p->txt[80] = '\0';
        x = node(resto);
        _insert(p, x);
        // printf("wrap %s\n", resto);

        p = x;

    }

}




/*
void wrap(struct no *p) {
    char resto[1000];
    struct no *x;
    if (p == NULL) return;
    if (strlen(p->txt) <= 80) return;
    strcpy(resto, p->txt + 80);
    p->txt[80] = '\0';
    x = node(resto);
    _insert(p, x);
    wrap(x);
}
*/





void insertInCursor(char *t) {


    struct no *x = node(t);
    if (cur == NULL) {
        _insert(end, x);

    } else {
        _insert(cur->ant, x);
    }


    cur = x;
    wrap(cur);


}



void insertInLine(int n, char *t) {
    struct no *p = nodeTo(n);
    struct no *x = node(t);


    if (p == NULL) {
        _insert(end, x);

    } else {

        _insert(p->ant, x);
    }



    cur = x;
    wrap(cur);

    // printf("%d\n", n);

}





void insertEnd(char *t) {
    struct no *x = node(t);
    _insert(end, x);
    cur = x;
    wrap(cur);
}





void deleteLine() {
    struct no *p = cur;
    
    if (p == NULL) return;

    if (p->seg != NULL) cur = p->seg;
    else cur = p->ant;


    tira(p);

}







void deleteCursor(int n) {
    
    struct no *p = nodeTo(n);
    if (p == NULL) return;


    if (p == cur) {

        if (p->seg != NULL) cur = p->seg;
        else  cur = p->ant;
    }

    tira(p);
}




void editCursor(char *t) {
    if (cur == NULL) return;
    strcpy(cur->txt, t);
    wrap(cur);
}



void editLine(int n, char *t) {
    struct no *p = nodeTo(n);
    if (p == NULL) return;
    strcpy(p->txt, t);
    wrap(p);
}


void goUp() {
    if (cur != NULL && cur->ant != NULL) {
        cur = cur->ant;
    }
}

void goDown() {
    if (cur != NULL && cur->seg != NULL) {
        cur = cur->seg;
    }
}






void print() {
    int i = 1;
    struct no *p = ini;

    printf("------------------------------------ Editor ------------------------------------\n");
    while (p != NULL) {
        if (p == cur) printf("%d > %s\n", i, p->txt);
        else          printf("%d : %s\n", i, p->txt);
        i++;
        p = p->seg;
    }
    printf("--------------------------------------------------------------------------------\n");
}


void printCursor() {
    if (cur != NULL) {
        printf("%s\n", cur->txt);
    }
}



void search(char *t) {
    int i = 1;
    struct no *p = ini;
    while (p != NULL) {
        if (strstr(p->txt, t) != NULL) {
            printf("%d : %s\n", i, p->txt);
            return;
        }
        i++;
        p = p->seg;
    }
    printf("not found\n");
}




/*
void foo() {
    insertEnd("line 1");
    insertEnd("line 2");
    insertEnd("line 3");
    print();
}
*/


// txt entre ""
void aspas(char *line, char *dest) {
    int i;
    int j = 0;
    int open = 0;

    for (i = 0; line[i] != '\0'; i++) {

        if (line[i] == '"') {
            if (open == 0) {
                open = 1;
            } else {
                break;
            }
        }
        else if (open == 1) {
            dest[j] = line[i];
            j++;
        }
    }

    dest[j] = '\0';
}











int main() {
    char line[1000];
    char cmd[50];
    char txt[1000];
    int n;

    while (fgets(line, 1000, stdin) != NULL) {
        int L = strlen(line);
        if (L > 0 && line[L-1] == '\n') line[L-1] = '\0';

        sscanf(line, "%s", cmd);
        //printf("cmd=%s\n", cmd);

        if (strcmp(cmd, "InsertInCursor") == 0) {
            aspas(line, txt);
            insertInCursor(txt);
        }


        else if (strcmp(cmd, "InsertInLine") == 0) {
            sscanf(line, "%s %d", cmd, &n);
            aspas(line, txt);
            insertInLine(n, txt);
        }

        else if (strcmp(cmd, "InsertEnd") == 0) {
            aspas(line, txt);
            insertEnd(txt);
        }

        else if (strcmp(cmd, "DeleteLine") == 0) {
            deleteLine();
        }


        else if (strcmp(cmd, "DeleteCursor") == 0) {
            sscanf(line, "%s %d", cmd, &n);
            deleteCursor(n);
        }


        else if (strcmp(cmd, "EditCursor") == 0) {
            aspas(line, txt);
            editCursor(txt);
        }

        else if (strcmp(cmd, "EditLine") == 0) {
            sscanf(line, "%s %d", cmd, &n);
            aspas(line, txt);
            editLine(n, txt);
        }

        else if (strcmp(cmd, "GoUp") == 0) {
            goUp();
        }

        else if (strcmp(cmd, "GoDown") == 0) {
            goDown();
        }


        else if (strcmp(cmd, "Print") == 0) {
            print();
        }

        else if (strcmp(cmd, "PrintCursor") == 0) {
            printCursor();
        }



        else if (strcmp(cmd, "Search") == 0) {
            aspas(line, txt);
            search(txt);
        }



        else if (strcmp(cmd, "Exit") == 0) {
            break;
        }
    }




    return 0;
}















