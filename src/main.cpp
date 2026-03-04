#include <raylib.h>
#include <math.h>

constexpr int screenWidth = 160;
constexpr int screenHeight = 120;
constexpr int scale = 4;

typedef struct {
    float cos[360];
    float sin[360];
}math; math M;

typedef struct {
    int x, y, z;
    int a; //   Rotation angle
    int l; //   Look up and down
}player; player P;

void rad_to_degrees() {
    for (int x=0; x<360; x++) {
        M.cos[x]=cos(x/180.0*PI);
        M.sin[x]=sin(x/180.0*PI);
    }
}

void init() {
    P.x=70;
    P.y=110;
    P.z=20;
    P.a=0;
    P.l=0;

    rad_to_degrees();
}

void movePlayer() {
    // Atualização de input 
    if (IsKeyDown(KEY_W)) { 
        P.a-=4; 
        if (P.a < 0) {
            P.a += 360;
        }
    } 
    if (IsKeyDown(KEY_S)) { 
        P.a+=4; 
        if (P.a > 359) {
            P.a -= 360;
        } 
    }

    int dx=M.sin[P.a]*10; //    Forward/back
    int dy=M.cos[P.a]*10;

    if (IsKeyDown(KEY_A)) { 
        P.x += dx;
        P.y += dy;
    } 
    if (IsKeyDown(KEY_D)) { 
        P.x -= dx;
        P.y -= dy;
    }
}

void clipBehindPlayer(int *x1,int *y1,int *z1, int x2,int y2,int z2) {
    float da=*y1; //distance plane -> point a
    float db= y2; //distance plane -> point b
    float d=da-db; 
    
    if(d==0){
        d=1;
    }

    float s = da/(da-db); //intersection factor (between 0 and 1)
    *x1 = *x1 + s*(x2-(*x1));
    *y1 = *y1 + s*(y2-(*y1)); 
    
    //prevent dividion by zero
    if(*y1==0){ 
        *y1=1;
    }

    *z1 = *z1 + s*(z2-(*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2) {
    int x;
    // Hold difference in distance
    int dyb = b2 - b1; // y distance of bottom line
    int dyt = t2 - t1; // y distance of top    line
    int dx = x2 - x1;
    if (dx == 0) {
        dx = 1;
    }
    int xs = x1; // hold initial x1 starting position

    // CLIP X
    if (x1 < 1) {
        x1 = 1; // clip left
    } 
    if (x2 < 1) {
        x2 = 1; // clip left
    }
    if (x1 > screenWidth - 1) {
        x1 = screenWidth - 1; // clip right
    }
    if (x2 > screenWidth - 1) {
        x2 = screenWidth - 1; // clip right
    }

    // draw x verticle lines
    for (x = x1; x < x2; x++) {
        int y1 = dyb * (x - xs + 0.5) / dx + b1; // y bottom point
        int y2 = dyt * (x - xs + 0.5) / dx + t1; // y top point

        // Clip Y
        if (y1 < 1) {
            y1 = 1;
        }
        if (y2 < 1) {
            y2 = 1;
        }
        if (y1 > screenHeight - 1) {
            y1 = screenHeight - 1;
        }
        if (y2 > screenHeight - 1) {
            y2 = screenHeight - 1;
        }

        for (int y = y1; y < y2; y++) {
            DrawRectangle(x * scale, y * scale, scale, scale, RED);
        }
    }
}

int main() {
    InitWindow(screenWidth * scale, screenHeight * scale, "Myslivcovi-Vzpominky");

    SetTargetFPS(24);

    init();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            //BeginMode3D(camera);
            //EndMode3D();

            movePlayer();

            int wx[4], wy[4], wz[4];
            float CS=M.cos[P.a], SN=M.sin[P.a];

            int x1=40-P.x, y1=10-P.y;
            int x2=40-P.x, y2=290-P.y;

            //  World x position
            wx[0]=x1*CS-y1*SN;
            wx[1]=x2*CS-y2*SN;
            wx[2]=wx[0];
            wx[3]=wx[1];


            //  World y position
            wy[0]=y1*CS-x1*SN;
            wy[1]=y2*CS-x2*SN;
            wy[2]=wy[0];
            wy[3]=wy[1];

            //  World z height
            wz[0]=0-P.z+((P.l*wy[0])/32);
            wz[1]=0-P.z+((P.l*wy[1])/32);
            wz[2]=wz[0]+40;
            wz[3]=wz[1]+40;

            //  Dont draw wall behind player
            if(wy[0]<1 && wy[1]<1){
                continue;
            }

            //  Still, PART of the wall could be behind the player:
            //point 1 behind player, clip
            if(wy[0]<1) { 
                clipBehindPlayer(&wx[0],&wy[0],&wz[0], wx[1],wy[1],wz[1]); //bottom line
                clipBehindPlayer(&wx[2],&wy[2],&wz[2], wx[3],wy[3],wz[3]); //top line
            }
            //point 2 behind player, clip
            if(wy[1]<1) { 
                clipBehindPlayer(&wx[1],&wy[1],&wz[1], wx[0],wy[0],wz[0]); //bottom line
                clipBehindPlayer(&wx[3],&wy[3],&wz[3], wx[2],wy[2],wz[2]); //top line
            }

            //  Screen x and y position
            wx[0]=wx[0]*200/wy[0]+screenWidth/2;
            wy[0]=wz[0]*200/wy[0]+screenHeight/2;

            wx[1]=wx[1]*200/wy[1]+screenWidth/2;
            wy[1]=wz[1]*200/wy[1]+screenHeight/2;

            wx[2]=wx[2]*200/wy[2]+screenWidth/2;
            wy[2]=wz[2]*200/wy[2]+screenHeight/2;  
            wx[3]=wx[3]*200/wy[3]+screenWidth/2; 
            wy[3]=wz[3]*200/wy[3]+screenHeight/2;

            //  Draw points
            /*
            if (wx[0] > 0 && wx[0] < screenWidth && wy[0] > 0 && wy[0] < screenHeight) {
                DrawRectangle(wx[0] * scale, wy[0] * scale, scale, scale, RED);
            }

            if (wx[1] > 0 && wx[1] < screenWidth && wy[1] > 0 && wy[1] < screenHeight) {
                DrawRectangle(wx[1] * scale, wy[1] * scale, scale, scale, RED);
            }
                */
            drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3]);

            DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RED);
        EndDrawing();
    }
    
    CloseWindow();
}