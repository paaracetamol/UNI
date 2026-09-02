#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int w, h;
char **img;

int sign(int x) {
    if(x > 0) return 1;
    if(x < 0) return -1;
    return 0;
}

int abs_val(int x) {
    if(x < 0) return -x;
    return x;
}

void point(int x, int y) {
    if(x >= 0 && x < w && y >= 0 && y < h) {
        img[y][x] = '#';
    }
}

void line(int x1, int y1, int x2, int y2) {
    int dx = abs_val(x2 - x1);
    int dy = -abs_val(y2 - y1);
    int sx = sign(x2 - x1);
    int sy = sign(y2 - y1);
    int e = dx + dy;
    int e2;

    while(1) {
        point(x1, y1);

        if(x1 == x2 && y1 == y2) break;

        e2 = 2 * e;

        if(e2 >= dy) {
            if(x1 == x2) break;
            e = e + dy;
            x1 = x1 + sx;
        }

        if(e2 <= dx) {
            if(y1 == y2) break;
            e = e + dx;
            y1 = y1 + sy;
        }
    }
}

void rect(int x1, int y1, int x2, int y2) {
    line(x1, y1, x2, y1);
    line(x2, y1, x2, y2);
    line(x2, y2, x1, y2);
    line(x1, y2, x1, y1);
}

void init_img() {
    int i, j;
    img = (char**)malloc(h * sizeof(char*));
    for(i = 0; i < h; i++) {
        img[i] = (char*)malloc(w * sizeof(char));
        for(j = 0; j < w; j++) {
            img[i][j] = ' ';
        }
    }
}

void readf(char *fn) {
    FILE *f = fopen(fn, "r");
    if(!f) {
        printf("erro ao abrir ficheiro\n");
        exit(1);
    }

    char buf[256];
    char cmd;
    int x, y, x1, y1, x2, y2;

    fgets(buf, 256, f);
    sscanf(buf, "%d %d", &w, &h);

    init_img();

    while(fgets(buf, 256, f)) {
        if(buf[0] == '#' || buf[0] == '\n') continue;

        if(sscanf(buf, " p %d %d", &x, &y) == 2) {
            point(x, y);
        }
        else if(sscanf(buf, " l %d %d %d %d", &x1, &y1, &x2, &y2) == 4) {
            line(x1, y1, x2, y2);
        }
        else if(sscanf(buf, " r %d %d %d %d", &x1, &y1, &x2, &y2) == 4) {
            rect(x1, y1, x2, y2);
        }
    }

    fclose(f);
}

void save(char *fn) {
    FILE *f = fopen(fn, "w");
    int i, j;

    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            fputc(img[i][j], f);
        }
        fputc('\n', f);
    }

    fclose(f);
}

int main() {
    char fname[100];

    printf("input myvg: ");
    scanf("%s", fname);

    readf(fname);

    save("out.txt");

    printf("done\n");

    return 0;
}
