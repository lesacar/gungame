#include <stdlib.h>
#include "raylib.h"
#define RCAMERA_IMPLEMENTATION
#include "rcamera.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    bool paused;
    bool borderless;
    bool exitWindow;
} L_KEYPRESSES;

typedef struct {
    int32_t width;
    int32_t height;
} W_info; // Window width and height


#define MAX_COLUMNS 12

void custom_keypress_controls(L_KEYPRESSES *lkeys, W_info *w_info) // handles inputs for fullscreen toggle, pause menu, window exit, etc...
{
    if (IsKeyPressed(KEY_ESCAPE))
        {
            lkeys->paused = !lkeys->paused;
        }
        
        if (IsKeyPressed(KEY_F11))
 		{
            if (!lkeys->borderless)
            {
                // SetWindowSize(GetMonitorWidth(GetCurrentMonitor()),GetMonitorHeight(GetCurrentMonitor()));
                SetWindowSize(GetMonitorWidth(GetCurrentMonitor())-1, GetMonitorHeight(GetCurrentMonitor())-1);
                SetWindowPosition(0,0);
                lkeys->borderless = !lkeys->borderless;
            } else {
                SetWindowSize(w_info->width, w_info->height);
                lkeys->borderless = !lkeys->borderless;
            }
 		}
}

void Game(Camera *camera, float heights[MAX_COLUMNS], Vector3 positions[MAX_COLUMNS], Color colors[MAX_COLUMNS], int *cameraMode,
    Texture2D *ye, Mesh mesh, Model model, Color* mapPixels)
{

    // Define the target update rate for the camera (60 times per second)
    const double targetUpdateRate = 1.0 / 60.0;
    static double timeAccumulator = 0.0;

    // Accumulate time
    timeAccumulator += GetFrameTime();

    // Check if it's time to update the camera
    while (timeAccumulator >= targetUpdateRate)
    {
        // Reset the time accumulator
        timeAccumulator -= targetUpdateRate;
        
        UpdateCameraPro(camera,
            (Vector3){
                (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) * 0.1f -      // Move forward-backward
                (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * 0.1f,
                (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * 0.1f -   // Move right-left
                (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * 0.1f,
                0.0f                                                // Move up-down
            },
            (Vector3){0},
            0);                              // Move to target (zoom)
    }
    UpdateCameraPro(camera,
            (Vector3){0},
            (Vector3){
                GetMouseDelta().x * 0.05f,                            // Rotation: yaw
                GetMouseDelta().y * 0.05f,                            // Rotation: pitch
                0.0f                                                // Rotation: roll
            },
            GetMouseWheelMove() * 2.0f);  

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(*camera);

    DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
    DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
    DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
    DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

    ///////////////////////////

DrawTexturePro(*ye,
    (Rectangle){ 0.0f, 0.0f, ye->width, ye->height }, // source rectangle
    (Rectangle){ 2.0f, 2.0f, -ye->width / 25.0f, ye->height / 25.0f }, // destination rectangle scaled down by 100x
    (Vector2){ ye->width / 200.0f, ye->height / 200.0f }, // origin
    0.0f, // rotation angle
    WHITE // tint
);


    //////////////////////////

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        DrawCube(positions[i], 2.0f, heights[i], 2.0f, colors[i]);
        // DrawCubeWires(positions[i], 2.0f, heights[i], 2.0f, MAROON);
    }

    // Draw player cube
    if (*cameraMode == CAMERA_THIRD_PERSON)
    {
        DrawCube(camera->target, 0.5f, 0.5f, 0.5f, PURPLE);
        DrawCubeWires(camera->target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
    }

    EndMode3D();

    // Draw info boxes
    DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(5, 5, 330, 100, BLUE);

    DrawText("Camera controls:", 15, 15, 10, BLACK);
    DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
    DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
    DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
    DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
    DrawText("- Camera projection key: P", 15, 90, 10, BLACK);
    DrawText(TextFormat("%d FPS", GetFPS()), 15, 110, 32, BLACK);

    DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
    DrawRectangleLines(600, 5, 195, 100, BLUE);

    DrawText("Camera status:", 610, 15, 10, BLACK);
    DrawText(TextFormat("- Mode: %s", (*cameraMode == CAMERA_FREE) ? "FREE" :
                                      (*cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                      (*cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                      (*cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
    DrawText(TextFormat("- Projection: %s", (camera->projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
                                            (camera->projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
    DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera->position.x, camera->position.y, camera->position.z), 610, 60, 10, BLACK);
    DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera->target.x, camera->target.y, camera->target.z), 610, 75, 10, BLACK);
    DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera->up.x, camera->up.y, camera->up.z), 610, 90, 10, BLACK);

    EndDrawing();
}






//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    W_info w_info = {
        .width = 1366,
        .height = 768
    };
    SetConfigFlags(FLAG_MSAA_4X_HINT|FLAG_WINDOW_UNDECORATED);
    
    InitWindow(w_info.width,w_info.height, "OpenGL Window");
    SetExitKey(KEY_NULL);
    L_KEYPRESSES lkeys = {
        .exitWindow = false,
        .borderless = false,
        .paused = false
    }; // struct with all values for handling custom window events
    Image yeImg = LoadImage("ye.png");
    if (!IsImageReady(yeImg))
    {
        printf("YE ERROR 111;");
        exit(0);
    }
    ImageFlipVertical(&yeImg);
    
    Texture2D ye = LoadTextureFromImage(yeImg);
    SetTextureFilter(ye, TEXTURE_FILTER_TRILINEAR);
    Mesh mesh = GenMeshCubicmap(yeImg, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model model = LoadModelFromMesh(mesh);
    Color *mapPixels = LoadImageColors(yeImg);
    UnloadImage(yeImg);

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 90.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;

    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)-14+i*2, heights[i]/2.0f, (float)-10 };
        colors[i] = (Color){ GetRandomValue(0, 255), GetRandomValue(0, 255), GetRandomValue(0,255), GetRandomValue(0,255) };
    }

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(0);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!lkeys.exitWindow)
    {
        custom_keypress_controls(&lkeys, &w_info);
        if (!lkeys.paused) {
            Game(&camera, heights, positions, colors, &cameraMode, &ye, mesh, model, mapPixels);
        } 
        if (lkeys.paused) {
            BeginDrawing();
            EndDrawing();
        }
        
        if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_Q)) || WindowShouldClose()) lkeys.exitWindow = true;
        
    }
    EnableCursor();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
