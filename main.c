#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define GRAPHICS_API_OPENGL_33

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>

#define max(a, b) (a) > (b) ? (a) : (b)
#define min(a, b) (a) < (b) ? (a) : (b)
#define clamp(x, low, high) min(max(x, low), high)

#define RAD_TO_DEG(x) ((x / PI) * 180)
#define DEG_TO_RAD(x) ((x / 180.f) * PI)

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define TEXT_PADDING 20

#define MOUSE_SPEED 50.f
#define ROTATE_SPEED 100.f
#define MOVE_SPEED 50.f

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "polar-coords");
    SetTargetFPS(60);

    Texture2D default_texture = { rlGetTextureIdDefault(), 1, 1, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 };

    bool debug_mode = false;
    float theta = 0.f;
    float radius = 5.f;
    size_t count = (size_t)(2 * PI * 12 / 0.1f) + 1;
    Vector2 *points = malloc(sizeof *points * count);
    int j = 0;
    for (float r = 0; r < 2 * PI * 12; r+=0.1f) {
        points[j++] = (Vector2){
            .x = cos(r) * r * 5,
            .y = sin(r) * r * 5,
        };
    }

    Shader curve_shader = LoadShader(NULL, "smooth_curve.fs");

    RenderTexture2D graph = LoadRenderTexture(1000, 1000);
    {
    BeginTextureMode(graph);
    Camera2D camera = { 0 };
    camera.zoom = 1.f;
    camera.offset = (Vector2){500.f,  500.f };
    BeginMode2D(camera);
    BeginShaderMode(curve_shader);
    for (float r = 0; r < 2 * PI * 10; r+=0.01f/(r*0.05f + 0.01f)) {
        Rectangle rect = {
            .x = cos(r) * r * 5,
            .y = sin(r) * r * 5,
            .width = 4,
            .height = 4,
        };
        DrawTexturePro(default_texture, (Rectangle) { 0.f, 0.f, 1.f, 1.f }, rect, (Vector2){ 2.f, 2.f }, RAD_TO_DEG(r), WHITE);
    }
    EndShaderMode();
    EndMode2D();
    EndTextureMode();
    }
    free(points);

    Image img = LoadImageFromTexture(graph.texture);
    ExportImage(img, "polar_curve.png");
    UnloadImage(img);

    Camera2D camera = { 0 };
    camera.zoom = 2.f;
    camera.offset = (Vector2){ WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 };

    float graph_rot = 0.f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        camera.zoom = clamp(camera.zoom + GetMouseWheelMove() * dt * MOUSE_SPEED, 0.1f, 15);
        if (IsKeyPressed(KEY_F)) {
            debug_mode = !debug_mode;
        }

        if (IsKeyDown(KEY_A)) {
            theta -= dt * ROTATE_SPEED / radius;
        } else if (IsKeyDown(KEY_D)) {
            theta += dt * ROTATE_SPEED / radius;
        }

        if (IsKeyDown(KEY_W)) {
            radius += dt * MOVE_SPEED;
        } else if (IsKeyDown(KEY_S)) {
            radius -= dt * MOVE_SPEED;
        }
        radius = clamp(radius, 0.1f, 500.f);

        if (IsKeyDown(KEY_R)) {
            graph_rot += 100 * dt;
            if (graph_rot > 360.f) graph_rot = 0.f;
        } else {
            graph_rot = Lerp(graph_rot, 0.f, dt * 5.f);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        if (debug_mode) {
            DrawTexturePro(
                graph.texture,
                (Rectangle){
                    .x = 0,
                    .y = 0,
                    .width = graph.texture.width,
                    .height = -graph.texture.height,
                },
                (Rectangle){
                    .x = 0,
                    .y = 0,
                    .width = graph.texture.width,
                    .height = graph.texture.height,
                },
                (Vector2) { .x = 500.f, .y = 500.f },
                graph_rot,
                WHITE
            );
        } else {
            BeginShaderMode(curve_shader);
            for (float r = 0; r < 2 * PI * 10; r+=0.01f/(r*0.05f + 0.01f)) {
                Rectangle rect = {
                    .x = cos(r + DEG_TO_RAD(graph_rot)) * r * 5,
                    .y = sin(r + DEG_TO_RAD(graph_rot)) * r * 5,
                    .width = 4,
                    .height = 4,
                };
                DrawTexturePro(default_texture, (Rectangle) { 0.f, 0.f, 1.f, 1.f }, rect, (Vector2){ 2.f, 2.f }, RAD_TO_DEG(r), WHITE);
            }
            EndShaderMode();
        }
        Rectangle rect = {
            .x = cos(theta) * radius,
            .y = sin(theta) * radius,
            .width = 5,
            .height = 3,
        };
        DrawRectanglePro(rect, (Vector2){ 2.5f, 1.5f }, RAD_TO_DEG(theta), BLUE);
        if (debug_mode) {
            DrawLine(-10000, 0, 10000, 0, WHITE);
            DrawLine(0, -10000, 0, 10000, WHITE);
        }
        EndMode2D();
        if (debug_mode) {
            DrawText("y", WINDOW_WIDTH / 2.f + TEXT_PADDING, 5, 20, WHITE);
            DrawText("x", WINDOW_WIDTH - 25, WINDOW_HEIGHT / 2.f - TEXT_PADDING - 20, 20, WHITE);
            DrawLineV((Vector2){ WINDOW_WIDTH / 2.f, 0 }, (Vector2){ WINDOW_WIDTH / 2.f - 15, 15 }, WHITE);
            DrawLineV((Vector2){ WINDOW_WIDTH / 2.f, 0 }, (Vector2){ WINDOW_WIDTH / 2.f + 15, 15 }, WHITE);
            DrawLineV((Vector2){ WINDOW_WIDTH, WINDOW_HEIGHT / 2.f }, (Vector2){ WINDOW_WIDTH - 15, WINDOW_HEIGHT / 2.f + 15 }, WHITE);
            DrawLineV((Vector2){ WINDOW_WIDTH, WINDOW_HEIGHT / 2.f }, (Vector2){ WINDOW_WIDTH - 15, WINDOW_HEIGHT / 2.f - 15 }, WHITE);
        }
        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
}
