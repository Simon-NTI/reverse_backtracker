#include <raylib.h>
#include <raymath.h>
#include "stdlib.h"
#include "stdio.h"
#include "time.h"

#define TILEMAP_HEIGHT 25
#define TILEMAP_WIDTH 25

#define IMAGE_WIDTH 600
#define IMAGE_HEIGHT 600

int steps = 0;

#define TARGET_FRAMERATE 45

#define TILE_WIDTH IMAGE_WIDTH / TILEMAP_WIDTH
#define TILE_HEIGHT IMAGE_HEIGHT / TILEMAP_HEIGHT

int tilemap[TILEMAP_HEIGHT][TILEMAP_WIDTH];
int tile_relations[TILEMAP_HEIGHT][TILEMAP_WIDTH];

Image maze_image = {0};

typedef enum
{
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_WEST,
    DIRECTION_CONSUMED
} Direction_e;

typedef enum
{
    TILE_UNVISITED,
    TILE_VISITED,
    TILE_ACTIVE_STEP
} Tile_State_e;

typedef struct
{
    int x;
    int y;
} Vector2Int_t;

void shuffle_array_int(unsigned char *array, int element_count)
{
    for (int i = element_count - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        unsigned char temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void draw_maze()
{
    for (int y = 0; y < TILEMAP_HEIGHT; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH; x++)
        {
            if (tile_relations[y][x] == 0)
            {
                continue;
            }

            // Check all four directions, if the tile does not connect to that side, close it off
            if (!(tile_relations[y][x] & (1 << DIRECTION_NORTH)))
            {
                ImageDrawLine(&maze_image, x * TILE_WIDTH, y * TILE_HEIGHT, (x + 1) * TILE_WIDTH, y * TILE_HEIGHT, BLACK);
            }

            if (!(tile_relations[y][x] & (1 << DIRECTION_EAST)))
            {
                ImageDrawLine(&maze_image, (x + 1) * TILE_WIDTH, y * TILE_HEIGHT, (x + 1) * TILE_WIDTH, (y + 1) * TILE_HEIGHT, BLACK);
            }

            if (!(tile_relations[y][x] & (1 << DIRECTION_SOUTH)))
            {
                ImageDrawLine(&maze_image, x * TILE_WIDTH, (y + 1) * TILE_HEIGHT, (x + 1) * TILE_WIDTH, (y + 1) * TILE_HEIGHT, BLACK);
            }

            if (!(tile_relations[y][x] & (1 << DIRECTION_WEST)))
            {
                ImageDrawLine(&maze_image, x * TILE_WIDTH, y * TILE_HEIGHT, x * TILE_WIDTH, (y + 1) * TILE_HEIGHT, BLACK);
            }
        }
    }
}

void draw_tiles()
{
    for (int y = 0; y < TILEMAP_HEIGHT; y++)
    {
        for (int x = 0; x < TILEMAP_WIDTH; x++)
        {
            switch (tilemap[y][x])
            {
            case TILE_UNVISITED:
            {
                continue;
            }
            break;

            case TILE_VISITED:
            {
                DrawRectangle(x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, (Color){.a = 160, .r = DARKGRAY.r, .g = DARKGRAY.g, .b = DARKGRAY.b});
            }
            break;

            case TILE_ACTIVE_STEP:
            {
                DrawRectangle(x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, (Color){.a = 160, .r = PINK.r, .g = PINK.g, .b = PINK.b});
            }
            break;

            default:
                break;
            }
        }
    }
}

void step(Vector2Int_t position)
{
    tilemap[position.y][position.x] = TILE_ACTIVE_STEP;

    steps++;

    int x_next = position.x;
    int y_next = position.y;

    unsigned char directions[4] = {
        DIRECTION_NORTH,
        DIRECTION_EAST,
        DIRECTION_SOUTH,
        DIRECTION_WEST};

    shuffle_array_int(directions, 4);

    for (unsigned char direction_index = 0; direction_index < 4; direction_index++) // Find an unvisited tile
    {
        // Move marker
        switch (directions[direction_index])
        {
        case DIRECTION_NORTH:
        {
            y_next--;
        }
        break;

        case DIRECTION_EAST:
        {
            x_next++;
        }
        break;

        case DIRECTION_SOUTH:
        {
            y_next++;
        }
        break;

        case DIRECTION_WEST:
        {
            x_next--;
        }
        break;

        default:
        {
            printf("Fatal Error: Invalid direction\n");
            printf("Direction %d\n", directions[direction_index]);
            exit(1);
        }
        break;
        }

        // Check if out of bounds
        if (y_next < 0 || y_next > (TILEMAP_HEIGHT - 1) || x_next < 0 || x_next > (TILEMAP_WIDTH - 1))
        {
            directions[direction_index] = DIRECTION_CONSUMED;

            y_next = position.y;
            x_next = position.x;

            continue;
        }

        if (tilemap[y_next][x_next] != TILE_UNVISITED)
        {
            directions[direction_index] = DIRECTION_CONSUMED;

            y_next = position.y;
            x_next = position.x;

            continue;
        }

        // Write down relations for the next and current tile
        tile_relations[position.y][position.x] |= (1 << directions[direction_index]);

        {
            int opposite_direction = directions[direction_index] - 2;
            if (opposite_direction < 0)
            {
                opposite_direction += 4;
            }

            tile_relations[y_next][x_next] |= (1 << opposite_direction);
        }

        directions[direction_index] = DIRECTION_CONSUMED;

        BeginDrawing();
        ClearBackground(SKYBLUE);
        draw_tiles();

        DrawRectangle(x_next * TILE_WIDTH, y_next * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, DARKPURPLE);

        EndDrawing();

        if (WindowShouldClose())
        {
            CloseWindow();
            exit(1);
        }

        step((Vector2Int_t){.x = x_next, .y = y_next});

        y_next = position.y;
        x_next = position.x;
    }

    BeginDrawing();
    ClearBackground(SKYBLUE);
    draw_tiles();

    DrawRectangle(x_next * TILE_WIDTH, y_next * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, DARKPURPLE);

    EndDrawing();

    if (WindowShouldClose())
    {
        CloseWindow();
        exit(1);
    }

    tilemap[position.y][position.x] = TILE_VISITED;
}

int main()
{
    maze_image = GenImageColor(IMAGE_WIDTH, IMAGE_HEIGHT, SKYBLUE);
    long seed = time(NULL);
    srand(seed);

    InitWindow(IMAGE_WIDTH, IMAGE_HEIGHT, "Reverse Backtracker");
    SetTargetFPS(TARGET_FRAMERATE);

    step((Vector2Int_t){.x = rand() % TILEMAP_WIDTH, .y = rand() % TILEMAP_HEIGHT});

    // printf("Step count %d\n", steps);
    printf("Seed: %lu\n", seed);

    draw_maze();
    ExportImage(maze_image, "maze_output.png");

    Texture2D maze_texture = LoadTexture("maze_output.png");

    SetTargetFPS(5);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawTexture(maze_texture, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(maze_texture);
}