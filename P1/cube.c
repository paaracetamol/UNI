#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define width 80
#define height 40
#define size 7

typedef struct {
    float x, y, z;
} Vec3;


void rotx(Vec3 *v, float a) {
    float y = v->y;
    float z = v->z;
    v->y = y * cos(a) - z * sin(a);
    v->z = y * sin(a) + z * cos(a);
}

void roty(Vec3 *v, float a) {
    float x = v->x;
    float z = v->z;
    v->x =  x * cos(a) + z * sin(a);
    v->z = -x * sin(a) + z * cos(a);
}

void rotz(Vec3 *v, float a) {
    float x = v->x;
    float y = v->y;
    v->x = x * cos(a) - y * sin(a);
    v->y = x * sin(a) + y * cos(a);
}


void project(Vec3 v, int *x, int *y) {
    float d = 30;
    float scale = d / (d + v.z);
    float ratio = 2.0; 
    *x = (int)(width / 2 + v.x * scale * ratio);
    *y = (int)(height / 2 - v.y * scale);
}


float dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}


Vec3 normalize(Vec3 v) {
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len > 0) {
        v.x /= len;
        v.y /= len;
        v.z /= len;
    }
    return v;
}


void shade(float bri, char *out) {
    
    const char shades[] = ".':*?#&%@$";
    int nshades = strlen(shades);
    
    int idx = (int)(bri * (nshades - 1));
    if (idx < 0) idx = 0;
    if (idx >= nshades) idx = nshades - 1;
    
    
    if (bri > 0.75) {
        sprintf(out, "\033[1m%c\033[0m", shades[idx]);
    } else {
        sprintf(out, "%c", shades[idx]);
    }
}

int main() {
    char buffer[height][width * 10];
    float zbuffer[height][width];
    float bbuffer[height][width];
    float ax = 0, ay = 0, az = 0;
    
    Vec3 light = normalize((Vec3){0.5, -0.7, -0.8});
    
    
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    
    
    
    printf("\033[2J");
    //printf("\033[?25l");
    
    while (1) {
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                buffer[i][j * 10] = '\0';
                zbuffer[i][j] = 1000;
                bbuffer[i][j] = 0;
            }
        }
        
        
        for (float cx = -size; cx < size; cx += 0.2) { 
            for (float cy = -size; cy < size; cy += 0.4) {
                
                Vec3 faces[6] = {
                    { cx,   cy,  -size},  // front
                    { cx,   cy,   size},  // back
                    { cx,  -size, cy  },  // bot
                    { cx,   size, cy  },  // top
                    {-size, cx,   cy  },  // left
                    { size, cx,   cy  }   // right
                };
                
                Vec3 normals[6] = {
                    { 0,  0, -1},  // front
                    { 0,  0,  1},   // back
                    { 0, -1,  0},  // bot
                    { 0,  1,  0},   // top
                    {-1,  0,  0},  // left
                    { 1,  0,  0}    // right
                };
                
                for (int f = 0; f < 6; f++) {
                    Vec3 p = faces[f];
                    Vec3 n = normals[f];
                    
                    
                    rotx(&p, ax);
                    roty(&p, ay);
                    rotz(&p, az);
                    
                    rotx(&n, ax);
                    roty(&n, ay);
                    rotz(&n, az);
                    
                    
                    float diff = dot(normalize(n), light);
                    
                    
                    float amb = 0.05;
                    float cnt = 1.5;
                    
                    
                    float bri = amb + (1.0 - amb) * fmax(0, diff);
                    bri = pow(bri, 1.0 / cnt);
                    
                    
                    if (bri > 1.0) bri = 1.0;
                    if (bri < 0.0) bri = 0.0;
                    
                    
                    Vec3 viewDir = {0, 0, -1};
                    if (dot(n, viewDir) > 0) {
                        int x, y;
                        project(p, &x, &y);
                        
                        if (x >= 0 && x < width && 
                            y >= 0 && y < height) {
                            if (p.z < zbuffer[y][x]) {
                                zbuffer[y][x] = p.z;
                                bbuffer[y][x] = bri;
                            }
                        }
                    }
                }
            }
        }
        
        
        for (int i = 0; i < height; i++) {
            int bufpos = 0;
            for (int j = 0; j < width; j++) {
                if (zbuffer[i][j] < 1000) {
                    char sh[20];
                    shade(bbuffer[i][j], sh);
                    int l = strlen(sh);
                    memcpy(&buffer[i][bufpos], sh, l);
                    bufpos += l;
                } else {
                    buffer[i][bufpos++] = ' ';
                }
            }
            buffer[i][bufpos] = '\0';
        }
        
        
        printf("\033[H");
        for (int i = 0; i < height; i++) {
            printf("%s\n", buffer[i]);
        }
        
        
        ax += 0.04;
        ay += 0.06;
        az += 0.02;
        
        usleep(30000);
    }
    
    printf("\033[?25h");
    return 0;
}
