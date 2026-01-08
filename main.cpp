#include "raylib.h"
#include <array>
#include <iostream>
#include <stdexcept>

using namespace std;

const int blocksInARow = 40;
const int blocksInACol = 30;
const int blockWidth = 20;
const int screenWidht = blocksInARow * blockWidth;
const int screenHeight = blocksInACol * blockWidth;
const float gravity = 1;
const float jumpHeight = 17;

Color blackgrowndColor = BLUE;

struct Player {
    public:

    // Top left corner
    int posX;
    int posY;

    Player (int posX, int posY) : posX(posX), posY(posY) {}
};

struct Block {
    public:

    int id;
    string name;
    Color color;
    bool placeMidAir;

    Block (string name, Color color, bool placeMidAir, int id) {
        this -> name = name;
        this->color = color;
        this->placeMidAir = placeMidAir;
        this-> id = id;
    }

    Block() {}
};

void FillWorldBlocks (array<Block, 10>& blocColors) {
    blocColors.at(0) = Block("Nothing", BLUE, true, 1);
    blocColors.at(1) = Block("Dirt", BROWN, false, 2);
    blocColors.at(2) = Block("Player", ORANGE, true, 3);
}

// void FillBlockColors (array<Color, 10>& blocColors) {
//     blocColors [0] = BLUE;
//     blocColors [1] = BROWN;
//     blocColors [2] = ORANGE;
// }

void createWorld (array<array<int, 1000>, 1000>& world, Player& player) {
    // i == y
    for (int i = 500; i < 1000; i ++) {

        // j = x
        for (int j = 0; j < 1000; j ++) {

            // world [y] [x]
            world.at(i).at(j) = 1;
        }
    }


    world.at(player.posY/20).at(player.posX/20 + 3) = 1;
    world.at(player.posY/20 - 1).at(player.posX/20 + 3) = 1;
    world.at(player.posY/20 + 1).at(player.posX/20 + 3) = 1;

    world.at(player.posY/20 - 8).at(player.posX/20 + 3) = 1;


    world.at(player.posY/20 - 8).at(player.posX/20 - 3) = 1;
}

void DrawWorld (const array<array<int, 1000>, 1000>& world, const Player& player, array<Block, 10>& worldBlocks) {

    //Player pos / 20 is the player grid position
    int playerGridX = player.posX / 20;
    int playerGridY = player.posY / 20;
    int playerOffcetX = player.posX % 20;
    int playerOffcetY = player.posY % 20;


    //j = y
    for (int j = 0; j < blocksInACol + 1; j ++) {

        // i = x
        for (int i = 0; i < blocksInARow + 1; i ++) {
            //Now take the blocks
            //That is X. Start at some before the player
            //40 blocks, 19 - player(2) - 19;
            int worldX = playerGridX - 19 + i;

            //Same for Y
            //30 blocks, 14 - player(2) - 14
            int worldY = playerGridY - 14 + j;

            int camX = i * blockWidth - playerOffcetX;
            int camY = j * blockWidth - playerOffcetY;


            DrawRectangle(camX, camY, blockWidth, blockWidth, worldBlocks.at(world.at(worldY).at(worldX)).color);

        }

        //Draw player on top
        DrawRectangle(screenWidht / 2 - blockWidth, screenHeight / 2 - blockWidth * 1.5 + 10, blockWidth * 2, blockWidth * 3, BLACK);
    }
}

//Player Offcet = object pos - player pos
void DrawObjectWithPlayerOffcet (int objectPosX, int objectPosY, int width, int height, Color color, Player& player) {
    int playerOffcerX = objectPosX - player.posX;
    int playerOffcetY = objectPosY - player.posY;

    int playerScreenX = screenWidht / 2 - blockWidth;
    int playerScreenY = screenHeight / 2 - blockWidth * 1.5;

    DrawRectangle(playerScreenX + playerOffcerX, playerScreenY + playerOffcetY, width, height, color);
}

int main () {
    array<array<int, 1000>, 1000> world {0};
    // array<Color, 10> blockColors;
    // FillBlockColors(blockColors);

    array<Block, 10> worldBlocks;
    FillWorldBlocks(worldBlocks);

    Player player (10020, 9900);

    createWorld(world, player);

    char lastDirection = 'n';
    int playerSpeedMod = 3;
    float verticalSpeed = 0;
    bool jumped = false;

    InitWindow(blocksInARow * blockWidth, blocksInACol * blockWidth, "Terraria clone");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(blackgrowndColor);

        verticalSpeed += gravity;

        int predictMoveX = player.posX;
        int predictMoveY = player.posY;

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) {
                verticalSpeed = -jumpHeight;
                jumped = true;
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            lastDirection = 'r';
            predictMoveX += playerSpeedMod;
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            lastDirection = 'l';
            predictMoveX -= playerSpeedMod;
        }

        // Get mouse pos
        int mouseGridX = ((GetMouseX() + player.posX - screenWidht / 2) + blockWidth) / 20;
        int mouseGridY = ((GetMouseY() + player.posY - screenHeight / 2) + blockWidth) / 20;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsCursorOnScreen()) {

            // cout << "\nmouseGridX: " << mouseGridX << "\nmouseGridY: " << mouseGridY;
            // cout << "\nplayerPosGridX: " << player.posX << "\nplayerPosGridY: " << player.posY << "\n";
            if (world.at(mouseGridY).at(mouseGridX) != 0) {
                world.at(mouseGridY).at(mouseGridX) = 0;
            }
            else {
                world.at(mouseGridY).at(mouseGridX) = 1;
            }
        }


        if (verticalSpeed > 10) {
            verticalSpeed = 10;
        }
        else if (verticalSpeed < -jumpHeight) {
            verticalSpeed = -jumpHeight;
        }

        predictMoveY += verticalSpeed;

        if (predictMoveX > player.posX) {

            //Check X colision
            //Check 4 blocks Y and 3 blocks X
            int predictMoveXRight = predictMoveX + 40;
            for (int j = player.posY; j < player.posY + blockWidth * 3; j ++) {
                if (world.at(j / 20).at(predictMoveXRight / 20) != 0) predictMoveX = (predictMoveXRight / 20) * 20 - 40;
            }
        }
        if (predictMoveX < player.posX) {
            //Look left
            for (int j = player.posY; j < player.posY + blockWidth * 3; j ++) {
                if (world.at(j / 20).at(predictMoveX / 20) != 0) predictMoveX = (predictMoveX / 20) * 20 + 20;
            }
        }

        if (predictMoveY > player.posY) {
            int predictMoveYDown = predictMoveY + blockWidth * 3;
            // 3 blocks. below the player, 1 right and 2 right
            for (int j = player.posX; j < player.posX + blockWidth * 2; j ++) {
                if (world.at(predictMoveYDown / 20).at(j / 20) != 0) {
                    predictMoveY = (predictMoveYDown / 20) * 20 - 60;
                    jumped = false;
                }
            }
        }
        if (predictMoveY < player.posY) {
            //Look up
            // 3 blocks. below the player, 1 right and 2 right
            for (int j = player.posX; j < player.posX + blockWidth * 2; j ++) {
                if (world.at(predictMoveY / 20).at(j / 20) != 0) predictMoveY = (predictMoveY / 20) * 20 + 20;
            }
        }

        player.posX = predictMoveX;
        player.posY = predictMoveY;

        DrawWorld(world, player, worldBlocks);

        EndDrawing();
    }

    return 0;
}
