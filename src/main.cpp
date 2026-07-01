#include <raylib.h>
#include <math.h>
#include <stdio.h>

#include "../textures/T_00.h"
#include "../textures/T_VIEW2D.h"

#include "game.h"
#include "levelEditor.h"

int numSect = 4;
int numWalls = 16;

math M;
player P;
walls W[256];
sectors S[128];
TextureMaps Textures[64];
Texture2D texturesRaylib[64];

void rad_to_degrees() {
    for (int x=0; x<360; x++) {
        M.cos[x]=cos(x/180.0*PI);
        M.sin[x]=sin(x/180.0*PI);
    }
}

void load() {
    FILE *fp = fopen("level.h","r");

    if (fp == NULL) { 
        printf("Error opening level.h"); 
        return;
    }
    int s;

    fscanf(fp,"%i",&numSect);   //number of sectors
    //load all sectors
    for(s=0; s < numSect; s++) {
        fscanf(fp,"%i",&S[s].ws);  
        fscanf(fp,"%i",&S[s].we); 
        fscanf(fp,"%i",&S[s].z1);  
        fscanf(fp,"%i",&S[s].z2); 
        fscanf(fp,"%i",&S[s].st); 
        fscanf(fp,"%i",&S[s].ss);  
    }

    fscanf(fp,"%i",&numWalls);   //number of walls
    //load all walls
    for(s=0; s < numWalls; s++) {
        fscanf(fp,"%i",&W[s].x1);  
        fscanf(fp,"%i",&W[s].y1); 
        fscanf(fp,"%i",&W[s].x2);  
        fscanf(fp,"%i",&W[s].y2); 
        fscanf(fp,"%i",&W[s].wt);
        fscanf(fp,"%i",&W[s].u); 
        fscanf(fp,"%i",&W[s].v);  
        fscanf(fp,"%i",&W[s].shade);  
    }
    fscanf(fp,"%i %i %i %i %i", &P.x, &P.y, &P.z, &P.a, &P.l); //player position, angle, look direction 
    fclose(fp); 

    printf("numSect=%d\n", numSect);
    printf("numWalls=%d\n", numWalls);
    printf("Player: %d %d %d %d %d\n", P.x, P.y, P.z, P.a, P.l);
}

void loadImage(const unsigned char* tex, int texWidth, int texHeight, int slot) {
    //define textures
    Image img;
    img.data = (void*)tex; // pointer to the raw bytes
    img.width = texWidth;
    img.height = texHeight;
    img.mipmaps = 1;
    img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;

    Texture2D tex00 = LoadTextureFromImage(img);
    texturesRaylib[slot] = tex00;

    // auxiliary structure
    Textures[slot].w = texWidth;
    Textures[slot].h = texWidth;
    Textures[slot].name = tex; // pointer for raw bytes
}

/*
sectors loadSectors[]={
    //wall start, wall end, z1 height, z2 height, y distances, bottom color, top color - surf and surface are not initialized
    {0,  4, 0, 40, 0, ORANGE, YELLOW},//sector 1
    {4,  8, 0, 40, 0, DARKGREEN, GREEN}, //sector 2
    {8, 12, 0, 40, 0, Color{190, 33, 55, 105}, Color{230, 41, 55, 105}},  //sector 3
    {12, 16, 0, 40, 0, DARKPURPLE, PURPLE}, //sector 4
};

walls loadWalls[]={
    //x1,y1, x2,y2, color
    0, 0, 32, 0, YELLOW,
    32, 0, 32, 32, ORANGE,
    32, 32, 0, 32, YELLOW,
    0, 32, 0, 0, ORANGE,

    64, 0, 96, 0, GREEN,
    96, 0, 96,32, DARKGREEN,
    96, 32, 64,32, GREEN,
    64, 32, 64, 0, DARKGREEN,

    64, 64, 96, 64, Color{230, 41, 55, 105},
    96, 64, 96, 96, Color{190, 33, 55, 105},
    96, 96, 64, 96, Color{230, 41, 55, 105}, // Transparent red
    64, 96, 64, 64, Color{190, 33, 55, 105}, // Transparent maroon

    0, 64, 32, 64, PURPLE,
    32, 64, 32, 96, DARKPURPLE,
    32, 96,  0, 96, PURPLE,
    0, 96,  0, 64, DARKPURPLE,
};
*/

void init() {
    /*
    P.x=-32;
    P.y=307;
    P.z=20;
    P.a=162;
    P.l=0;
    */

    rad_to_degrees();

    DisableCursor();
    /*
    //load sectors
    int s, w, v2=0;
    for(s=0; s<numSect; s++) {
        S[s].ws = loadSectors[s].ws;                   //wall start number
        S[s].we = loadSectors[s].we;                   //wall end number
        S[s].z1 = loadSectors[s].z1;                   //sector bottom height
        S[s].z2 = loadSectors[s].z2 - loadSectors[s].z1; //sector top height
        S[s].c1 = loadSectors[s].c1;                   //sector top color
        S[s].c2 = loadSectors[s].c2;                   //sector bottom color

        for(w=S[s].ws; w<S[s].we; w++) {
            W[w] = loadWalls[v2];
            v2++;
        }
    }
    */

    load(); // load level

    loadImage(T_00, T_00_WIDTH, T_00_HEIGHT, 0);
    loadImage(T_VIEW2D, T_VIEW2D_WIDTH, T_VIEW2D_HEIGHT, 1);
}

void movePlayer() {
    int dx=M.sin[P.a]*10; //    Forward/back
    int dy=M.cos[P.a]*10;

    // Input update
    if (IsKeyDown(KEY_W)) {
        P.x += dx;
        P.y += dy;
    } 
    if (IsKeyDown(KEY_S)) { 
        P.x -= dx;
        P.y -= dy;
    }

    //  left
    if (IsKeyDown(KEY_A)) { 
        P.x -= dy;
        P.y += dx;
    }
    //  right
    if (IsKeyDown(KEY_D)) {
        P.x += dy;
        P.y -= dx;
    }

    //  Go up
    if (IsKeyDown(KEY_SPACE)) {
        P.z -= 1;
    }
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
        P.z += 1;
    }

    // Mouse rotation
    Vector2 mouseDelta = GetMouseDelta(); // difference since the last frame
    P.a += int(mouseDelta.x * 0.1f);    // adjusts sensitivity by multiplying
    if (P.a < 0) {
        P.a += 360;
    }
    if (P.a > 359) {
        P.a -= 360;
    }

    // Look up/down with vertical mouse movement.
    P.l += int(mouseDelta.y * -0.1f);
}

void clipBehindPlayer(int *x1,int *y1,int *z1, float *u1, float *v1, int x2, int y2, int z2, float u2, float v2) {
    float da = *y1; //distance plane -> point a
    float db = y2; //distance plane -> point b
    float d = da-db; 
    
    if(d == 0){
        d=1;
    }

    float s = da/(da-db); //intersection factor (between 0 and 1)
    *x1 = *x1 + s*(x2-(*x1));
    *y1 = *y1 + s*(y2-(*y1)); 
    
    //prevent dividion by zero
    if(*y1 == 0){ 
        *y1=1;
    }

    *z1 = *z1 + s*(z2-(*z1));

    // Interpolate texture coordinates (prevent them from being compressed instead of clipped when they are not entirely within the field of view)
    *u1 = *u1 + s * (u2 - (*u1));
    *v1 = *v1 + s * (v2 - (*v1));
}

Color GetTexturePixel(int texId, int u, int v) {
    // ensures that u and v are within the limits.
    u = u % Textures[texId].w;
    v = v % Textures[texId].h;

    // each pixel has 3 bytes (RGB)
    int index = (v * Textures[texId].w + u) * 3;
    unsigned char r = Textures[texId].name[index + 0];
    unsigned char g = Textures[texId].name[index + 1];
    unsigned char b = Textures[texId].name[index + 2];

    return (Color){ r, g, b, 255 };
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, float u0, float u1, int wallIdx) {
    // Hold difference in distance
    int dx = x2 - x1;

    if (dx == 0) {
        dx = 1;
    }
    int xs = x1;
    int xe = x2;

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
    for (int x = x1; x < x2; x++) {
        float tx = (float)(x-xs) / (float)(xe-xs);

        float u = u0 + tx*(u1-u0);

        int y1 = (b2 - b1) * (x - xs + 0.5f) / dx + b1;
        int y2 = (t2 - t1) * (x - xs + 0.5f) / dx + t1;

        int y1Original = y1;
        int y2Original = y2;

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

        /*
        //surface
        if (S[s].surface == 1) { 
            S[s].surfBottom[x] = y1; //save bottom points
            continue;
        }
        if (S[s].surface == 2) { 
            S[s].surfTop[x] = y2; //save top points
            continue;
        }
        if (S[s].surface == 3) {
            // save top and bottom points
            S[s].surfBottom[x] = y1;
            S[s].surfTop[x] = y2;
            continue;
        }
        */

        /*
        if (S[s].surface == -1) { 
            for (y = S[s].surfBottom[x]; y < y1; y++){
                //  This function draws a pixel: x posittion, y position, width, height, color
                //DrawRectangle(x * scale, y * scale, scale, scale, S[s].c1); //bottom
            }
        }
        if (S[s].surface == -2) { 
            for (y = y2; y < S[s].surfTop[x]; y++){ 
                DrawRectangle(x * scale, y * scale, scale, scale, S[s].c2); //top
            }
        }
        if (S[s].surface == -3) {
            for (y = S[s].surfBottom[x]; y < y1; y++) {
                DrawRectangle(x * scale, y * scale, scale, scale, S[s].c1); // bottom
            }
            for (y = y2; y < S[s].surfTop[x]; y++) {
                DrawRectangle(x * scale, y * scale, scale, scale, S[s].c2); // top
            }
        }
        */
       
        for (int y = y1; y < y2; y++) {
            float ty = (float)(y-y1Original) / (float)(y2Original-y1Original);

            // interpolate between top and bottom of the column
            float v = ty * Textures[W[wallIdx].wt].h;

            int texU = (int)(u * Textures[W[wallIdx].wt].w);
            int texV = (int)v;

            if (texU < 0) {
                texU += Textures[W[wallIdx].wt].w;
            }
            if (texV < 0) {
                texV += Textures[W[wallIdx].wt].h;
            }

            Color texColor = GetTexturePixel(W[wallIdx].wt, texU, texV);

            DrawRectangle(x * scale, y * scale, scale, scale, texColor);
        }
    }
}

int dist(int x1, int y1, int x2, int y2) {
    int distance = sqrt( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
    return distance;
}

void draw3D() {
    int s, w;
    int wx[4], wy[4], wz[4];
    int sx[4], sy[4]; // screen coords

    float CS = M.cos[P.a], SN = M.sin[P.a];

    //order sectors by distance
    for(s=0; s < numSect-1; s++) {    
        for(w = 0; w < numSect-s-1; w++) {
            if(S[w].d < S[w+1].d) { 
                sectors st = S[w]; 
                S[w] = S[w+1]; 
                S[w+1] = st; 
            }
        }
    }

    //draw sectors
    for(s = 0; s < numSect; s++) { 
        S[s].d=0; //clear distance

        bool sectorTransparent = false;

        //  First draw faces that are turned back and also save vertical limits (surfTop[x] and surfBottom[x])
        for(int loop = 0; loop < 2; loop++) {

            //  cycle trough walls
            for(w = S[s].ws; w < S[s].we; w++) {
                /*
                    if (W[w].c.a < 255) {
                        sectorTransparent = true;
                    }
                */

                //offset bottom 2 points by player
                int x1 = W[w].x1 - P.x;
                int y1 = W[w].y1 - P.y;
                int x2 = W[w].x2 - P.x;
                int y2 = W[w].y2 - P.y;

                // associating texture coordinates (u,v) to the four vertices of the wall
                float tu[4], tv[4];

                tu[0] = 0; 
                tu[1] = W[w].u; // 0–1
                tu[2] = tu[0];
                tu[3] = tu[1];

                tv[0] = 0;
                tv[1] = 0;
                tv[2] = W[w].v; 
                tv[3] = W[w].v;

                /*
                W[w].tu1 = tu[0];
                W[w].tv1 = tv[0];
                W[w].tu2 = tu[1];
                W[w].tv2 = tv[2];
                */

                //  swap for surface (first draw flipped faces)
                if(loop == 0) { 
                    int swp = x1; 
                    x1 = x2; 
                    x2 = swp; 
                    swp = y1; 
                    y1 = y2; 
                    y2 = swp;
                }

                //  World x position
                wx[0] = x1 * CS - y1 * SN;
                wx[1] = x2 * CS - y2 * SN;
                wx[2] = wx[0];
                wx[3] = wx[1];

                //  World y position
                wy[0] = y1 * CS + x1 * SN;
                wy[1] = y2 * CS + x2 * SN;
                wy[2] = wy[0];
                wy[3] = wy[1];

                S[s].d += dist(0, 0, (wx[0] + wx[1]) / 2, (wy[0] + wy[1]) / 2);  //store wall distance

                //  World z height
                wz[0] = S[s].z1 - P.z + ((P.l * wy[0]) / 32);
                wz[1] = S[s].z1 - P.z + ((P.l * wy[1]) / 32);
                wz[2] = wz[0] + S[s].z2; //top line has new z 
                wz[3] = wz[1] + S[s].z2;
                /*
                wz[2]=S[s].z2-P.z+((P.l*wy[0])/32.0);
                wz[3]=S[s].z2-P.z+((P.l*wy[1])/32.0);
                */

                //  Dont draw wall behind player
                if (wy[0] < 1 && wy[1] < 1) {
                    continue;
                }

                //  Still, PART of the wall could be behind the player:
                // point 1 behind player, clip
                if (wy[0] < 1) {
                    clipBehindPlayer(&wx[0], &wy[0], &wz[0], &tu[0], &tv[0], wx[1], wy[1], wz[1], tu[1], tv[1]); // bottom line
                    clipBehindPlayer(&wx[2], &wy[2], &wz[2], &tu[2], &tv[2], wx[3], wy[3], wz[3], tu[3], tv[3]); // top line
                }
                // point 2 behind player, clip
                if (wy[1] < 1) {
                    clipBehindPlayer(&wx[1], &wy[1], &wz[1], &tu[1], &tv[1], wx[0], wy[0], wz[0], tu[0], tv[0]); // bottom line
                    clipBehindPlayer(&wx[3], &wy[3], &wz[3], &tu[3], &tv[3], wx[2], wy[2], wz[2], tu[2], tv[2]); // top line
                }

                //  Screen x and y position
                sx[0] = wx[0] * 200 / wy[0] + screenWidth / 2;
                sy[0] = wz[0] * 200 / wy[0] + screenHeight / 2;

                sx[1] = wx[1] * 200 / wy[1] + screenWidth / 2;
                sy[1] = wz[1] * 200 / wy[1] + screenHeight / 2;

                sx[2] = wx[2] * 200 / wy[2] + screenWidth / 2;
                sy[2] = wz[2] * 200 / wy[2] + screenHeight / 2;
                sx[3] = wx[3] * 200 / wy[3] + screenWidth / 2;
                sy[3] = wz[3] * 200 / wy[3] + screenHeight / 2;

                drawWall(sx[0], sx[1], 
                        sy[0], sy[1],   // Y position of the bottom of the wall in x1 and x2
                        sy[2], sy[3],   // Y position of the top of the wall in x1 and x2

                        tu[0], tu[1],   // bottom left
                        w);
            }
            S[s].d /= (S[s].we - S[s].ws); //find average sector distance
            S[s].surface *= -1; //flip to negative to draw surface (first time the loop that draws x verstical lines runs, it saves the top and bottom values, the next time it uses them)
        }

        if (sectorTransparent) {
            S[s].surface = 3; // draws top and bottom
        }
        else if (P.z < S[s].z1) {
            S[s].surface = 1; // bottom
        }
        else if (P.z > S[s].z2) {
            S[s].surface = 2; // top
        }
        else {
            S[s].surface = 0; // none
        }
    }
}

bool editorMode = false;

int main() {
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth * scale, screenHeight * scale, "Utulno");

    SetTargetFPS(24);

    init();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground((Color){25, 25, 112, 255});

            if (editorMode) {
                draw2D();   // editor
                if (IsKeyPressed(KEY_F5)) {
                    save();
                }
                if (IsKeyPressed(KEY_TAB)) {
                    editorMode = false;
                }
            }
            else {
                movePlayer();
                draw3D();

                if (IsKeyPressed(KEY_TAB)) {
                    editorMode = true;
                }
            }

            DrawText(TextFormat("Player Position: %i, %i, %i", P.x, P.y, P.z), 10, 10, 20, GREEN);
            DrawText(TextFormat("FPS: %i", GetFPS()), 10, 30, 20, GREEN);
        EndDrawing();
    }
    
    CloseWindow();
}