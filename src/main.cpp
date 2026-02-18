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

    int dx=M.cos[P.a]*10;
    int dy=M.sin[P.a]*10;

    if (IsKeyDown(KEY_A)) { 
        P.x += dx;
        P.y += dy;
    } 
    if (IsKeyDown(KEY_D)) { 
        P.x -= dx;
        P.y -= dy;
    }
}

/*
void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, Color c, int s) {
    int x, y;
    // Hold difference in distance
    int dyb = b2 - b1; // y distance of bottom line
    int dyt = t2 - t1; // y distance of top    line
    int dx = x2 - x1;
    if (dx == 0) {
        dx = 1;
    }            // x distance
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
        // The Y start and end point
        int y1 = dyb * (x - xs + 0.5) / dx + b1; // y bottom point
        int y2 = dyt * (x - xs + 0.5) / dx + t1; // y bottom point
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
        // surface
        if (S[s].surface == 1) {
            S[s].surf[x] = y1;
            continue;
        } // save bottom points
        if (S[s].surface == 2) {
            S[s].surf[x] = y2;
            continue;
        } // save top    points
        if (S[s].surface == -1) {
            for (y = S[s].surf[x]; y < y1; y++) {
                DrawRectangle(x * scale, y * scale, scale, scale, S[s].c1); // bottom
            }
        }
        if (S[s].surface == -2) {
            for (y = y2; y < S[s].surf[x]; y++) {
                DrawRectangle(x * scale, y * scale, scale, scale, S[s].c2); // top
            }
        }
        for (y = y1; y < y2; y++) {
            DrawRectangle(x * scale, y * scale, scale, scale, c); // normal wall
        }
    }
}
*/

int main() {
    InitWindow(screenWidth * scale, screenHeight * scale, "Myslivcovi-Vzpominky");

    SetTargetFPS(60);

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

            //  World y position
            wy[0]=y1*CS-x1*SN;
            wy[1]=y2*CS-x2*SN;

            //  World z height
            wz[0]=0-P.z+((P.l*wy[0])/32);
            wz[1]=0-P.z+((P.l*wy[1])/32);

            //  Screen x and y position
            wx[0]=wx[0]*200/wy[0]+screenWidth/2;
            wy[0]=wz[0]*200/wy[0]+screenHeight/2;

            wx[1]=wx[1]*200/wy[1]+screenWidth/2;
            wy[1]=wz[1]*200/wy[1]+screenHeight/2;

            //  Draw points
            if (wx[0] > 0 && wx[0] < screenWidth && wy[0] > 0 && wy[0] < screenHeight) {
                DrawRectangle(wx[0] * scale, wy[0] * scale, scale, scale, RED);
            }

            if (wx[1] > 0 && wx[1] < screenWidth && wy[1] > 0 && wy[1] < screenHeight) {
                DrawRectangle(wx[1] * scale, wy[1] * scale, scale, scale, RED);
            }

            DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, RED);
        EndDrawing();
    }
    
    CloseWindow();
}