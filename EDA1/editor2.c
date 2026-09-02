#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 81
#define HSIZE 211



struct no {
    char txt[MAX];
    struct no *ant;
    struct no *seg;
};




struct no *ini = NULL;
struct no *end = NULL;
struct no *cur = NULL;




// spellcheck 
// chained hash tale 
struct word {
    char w[MAX];
    struct word *next;
};

struct word *dict[HSIZE];




// undo
struct snap {
    char *lines;
    int n;
    int curpos;
    struct snap *next;
};

struct snap *undoTop = NULL;
struct snap *redoTop = NULL;




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







void freeList() {
    struct no *p = ini;
    struct no *n;

    while (p != NULL) {
        n = p->seg;
        free(p);
        p = n;
    }

    ini = NULL;
    end = NULL;
    cur = NULL;
}








// snapshot atual 
// line + cur pos
struct snap *makeSnap() {
    struct snap *s = malloc(sizeof(struct snap));
    int c = count();
    struct no *p = ini;
    int i = 0;

    s->lines = malloc(c * MAX);
    s->n = c;
    s->curpos = 0;

    while (p != NULL) {
        strcpy(s->lines + i * MAX, p->txt);
        i++;
        if (p == cur) s->curpos = i;
        p = p->seg;
    }

    s->next = NULL;
    return s;
}





// restore snap
void restoreSnap(struct snap *s) {
    int i;

    freeList();

    for (i = 0; i < s->n; i++) {
        struct no *x = node(s->lines + i * MAX);
        _insert(end, x);
        if (i + 1 == s->curpos) cur = x;
    }

    free(s->lines);
    free(s);
}







void clearRedo() {
    struct snap *s = redoTop;
    struct snap *n;

    while (s != NULL) {
        n = s->next;
        free(s->lines);
        free(s);
        s = n;
    }

    redoTop = NULL;
}







void pushUndo() {
    struct snap *s = makeSnap();

    s->next = undoTop;
    undoTop = s;

    clearRedo();
}





void undo() {
    struct snap *s;

    if (undoTop == NULL) {
        printf("nothing to undo\n");
        return;
    }

    s = makeSnap();
    s->next = redoTop;
    redoTop = s;

    s = undoTop;
    undoTop = undoTop->next;

    restoreSnap(s);
}







void redo() {
    struct snap *s;

    if (redoTop == NULL) {
        printf("nothing to redo\n");
        return;
    }

    s = makeSnap();
    s->next = undoTop;
    undoTop = s;

    s = redoTop;
    redoTop = redoTop->next;

    restoreSnap(s);
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

    pushUndo();

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
    pushUndo();

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
    pushUndo();

    struct no *x = node(t);
    _insert(end, x);
    cur = x;
    wrap(cur);
}





void deleteLine() {
    struct no *p = cur;
    
    if (p == NULL) return;

    pushUndo();

    if (p->seg != NULL) cur = p->seg;
    else cur = p->ant;


    tira(p);

}







void deleteCursor(int n) {
    
    struct no *p = nodeTo(n);
    if (p == NULL) return;

    pushUndo();

    if (p == cur) {

        if (p->seg != NULL) cur = p->seg;
        else  cur = p->ant;
    }

    tira(p);
}




void editCursor(char *t) {
    if (cur == NULL) return;
    pushUndo();
    strcpy(cur->txt, t);
    wrap(cur);
}



void editLine(int n, char *t) {
    struct no *p = nodeTo(n);
    if (p == NULL) return;
    pushUndo();
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




int hashWord(char *s) {
    int h = 0;
    int i;

    for (i = 0; s[i] != '\0'; i++) {
        h = h + s[i];
    }

    return h % HSIZE;
}


void addWord(char *s) {
    int h = hashWord(s);
    struct word *w = malloc(sizeof(struct word));

    strcpy(w->w, s);
    w->next = dict[h];
    dict[h] = w;
}


int findWord(char *s) {
    int h = hashWord(s);
    struct word *p = dict[h];

    while (p != NULL) {
        if (strcmp(p->w, s) == 0) return 1;
        p = p->next;
    }

    return 0;
}


void loadDict() {
    FILE *f = fopen("portuguese.txt", "r");
    char buf[MAX];
    int i;

    if (f == NULL) {
        printf("dictionary not found\n");
        return;
    }

    while (fgets(buf, MAX, f) != NULL) {
        int l = strlen(buf);
        if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';

        for (i = 0; buf[i] != '\0'; i++) {
            buf[i] = tolower(buf[i]);
        }

        addWord(buf);
    }

    fclose(f);
}




// so considera a-z como letra, acentos partem a palavra (simplificacao)
int isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}


void spell() {
    struct no *p = ini;
    char buf[MAX];
    int ln = 1;

    while (p != NULL) {
        int len = strlen(p->txt);
        int i = 0;
        int j = 0;

        while (i <= len) {
            if (i < len && isLetter(p->txt[i])) {
                buf[j] = tolower(p->txt[i]);
                j++;
            } else {
                if (j > 0) {
                    buf[j] = '\0';
                    if (!findWord(buf)) {
                        printf("%d : %s\n", ln, buf);
                    }
                    j = 0;
                }
            }
            i++;
        }

        ln++;
        p = p->seg;
    }
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

    loadDict();

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


        else if (strcmp(cmd, "spell") == 0) {
            spell();
        }


        else if (strcmp(cmd, "undo") == 0) {
            undo();
        }


        else if (strcmp(cmd, "redo") == 0) {
            redo();
        }



        else if (strcmp(cmd, "Exit") == 0) {
            break;
        }
    }




    return 0;
}