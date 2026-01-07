#include "raylib.h"
#include <array>
#include <iostream>

using namespace std;

const int blocksInARow = 40;
const int blocksInACol = 30;
const int blockWidth = 20;
const int screenWidht = blocksInARow * blockWidth;
const int screenHeight = blocksInACol * blockWidth;
const float gravity = 1;
const float jumpHeight = 17;

Color blackgrowndColor = BLUE;

class Player {
    public:

    // Top left corner
    int posX;
    int posY;

    Player (int posX, int posY) : posX(posX), posY(posY) {}
};

void FillBlockColors (array<Color, 10>& blocColors) {
    blocColors [0] = BLUE;
    blocColors [1] = BROWN;
    blocColors [2] = ORANGE;
}

void createWorld (array<array<int, 1000>, 1000>& world, Player& player) {
    // i == y
    for (int i = 500; i < 1000; i ++) {

        // j = x
        for (int j = 0; j < 1000; j ++) {

            // world [y] [x]
            world.at(i).at(j) = 1;
        }
    }

    // world.at(player.posY/20).at(player.posX/20) = 2;
    // world.at(player.posY/20 - 1).at(player.posX/20) = 2;
    // world.at(player.posY/20 + 1).at(player.posX/20) = 2;

    // world.at(player.posY/20).at(player.posX/20 + 1) = 2;
    // world.at(player.posY/20 - 1).at(player.posX/20 + 1) = 2;
    // world.at(player.posY/20 + 1).at(player.posX/20 + 1) = 2;


    world.at(player.posY/20).at(player.posX/20 + 3) = 1;
    world.at(player.posY/20 - 1).at(player.posX/20 + 3) = 1;
    world.at(player.posY/20 + 1).at(player.posX/20 + 3) = 1;

    world.at(player.posY/20 - 8).at(player.posX/20 + 3) = 1;


    world.at(player.posY/20 - 8).at(player.posX/20 - 3) = 1;
}

void DrawWorld (const array<array<int, 1000>, 1000>& world, const Player& player, array<Color, 10>& blockColors) {
    // int camX = player.posX;
    // int camY = player.posY;

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

            // cout << world.at(worldY).at(worldX) << " ";

            DrawRectangle(camX, camY, blockWidth, blockWidth, blockColors.at(world.at(worldY).at(worldX)));

            // if (world.at(worldY).at(worldX) == 1) {
            //     DrawRectangle(camX, camY, blockWidth, blockWidth, BROWN);
            // }
            // // Actual grid player pos
            // else if (world.at(worldY).at(worldX) == 2) {
            //     DrawRectangle(camX, camY, blockWidth, blockWidth, ORANGE);
            // }
        }

        // DrawRectangle(screenWidht / 2, screenHeight / 2, 2, 2, GREEN);

        // for (int j =  screenHeight / 2 - blockWidth * 1.5; j <  screenHeight / 2 - blockWidth * 1.5 + 55; j ++) {
        //     DrawRectangle(screenWidht / 2 - blockWidth + 40, j, 1, 1, GREEN);
        // }

        // cout << "\nRow: " << j << "\n";

        //Draw player on top
        DrawRectangle(screenWidht / 2 - blockWidth, screenHeight / 2 - (int)(blockWidth * 1.5 + 10), blockWidth * 2, blockWidth * 3, BLACK);
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
    array<Color, 10> blockColors;
    FillBlockColors(blockColors);

    Player player (10020, 9900);

    createWorld(world, player);

    char lastDirection = 'n';
    int playerSpeedMod = 3;
    float verticalSpeed = 0;
    bool jumped = false;

    // short jumpFrameBuffer = 0;

    InitWindow(blocksInARow * blockWidth, blocksInACol * blockWidth, "Terraria clone");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        verticalSpeed += gravity;
        // jumpFrameBuffer --;

        // world.at(player.posY/20).at(player.posX/20) = 0;
        // world.at(player.posY/20 - 1).at(player.posX/20) = 0;
        // world.at(player.posY/20 + 1).at(player.posX/20) = 0;

        // world.at(player.posY/20).at(player.posX/20 + 1) = 0;
        // world.at(player.posY/20 - 1).at(player.posX/20 + 1) = 0;
        // world.at(player.posY/20 + 1).at(player.posX/20 + 1) = 0;

        int predictMoveX = player.posX;
        int predictMoveY = player.posY;
        // bool colidingX = false;
        // bool colidingY = false;

        if (IsKeyPressed(KEY_UP)) {
            // if (jumped) {
            //     jumpFrameBuffer = 5;
            // }
            // else {
                verticalSpeed = -jumpHeight;
                jumped = true;
                // jumpFrameBuffer = 0;
            // }
            // lastDirection = 'u';
            // predictMoveY -= playerSpeedMod;
        }
        // else if (jumpFrameBuffer > 0 && !jumped) {
        //     verticalSpeed = -jumpHeight;
        //     jumped = true;
        //     jumpFrameBuffer = 0;
        // }

        // if (IsKeyDown(KEY_DOWN)) {
        //     lastDirection = 'd';
        //     predictMoveY += playerSpeedMod;
        // }
        if (IsKeyDown(KEY_RIGHT)) {
            lastDirection = 'r';
            predictMoveX += playerSpeedMod;
        }
        if (IsKeyDown(KEY_LEFT)) {
            lastDirection = 'l';
            predictMoveX -= playerSpeedMod;
        }
        // if (GetKeyPressed() == 0) {
        //     lastDirection = 'n';
        // }

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
            // cout << "player.posY row: " << player.posY << endl;
            // int predictMoveXGridRight = predictMoveXRight / 20;
            // cout << "player.posY / 20 row: " << player.posY / 20 << endl;
            for (int j = player.posY; j < player.posY + blockWidth * 3; j ++) {
                // cout << "Checking row: " << j << " col: " << predictMoveXGridRight << endl;

                if (world.at(j / 20).at(predictMoveXRight / 20) != 0) predictMoveX = (predictMoveXRight / 20) * 20 - 40;
            }
        }

        if (predictMoveX < player.posX) {
            //Look left
            for (int j = player.posY; j < player.posY + blockWidth * 3; j ++) {
                // cout << "Checking row: " << j << " col: " << predictMoveXGridRight << endl;
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


        // for (int j = player.posX / 20; j < player.posX / 20 + 2; j ++) {
        //     int predictMoveYGridDown = predictMoveY / 20 + 2;
        //     if (world.at(j).at(predictMoveYGridDown) != 0) predictMoveY = predictMoveYGridDown * 20 - 60;
        // }

        player.posX = predictMoveX;
        player.posY = predictMoveY;

        //Check X colision
        //Check 4 blocks Y and 3 blocks X
        // for (int i = predictMoveY; i < predictMoveX + 3; i ++) {
        //     for (int j = predictMoveX; j < predictMoveX + 2; j ++) {
        //         if (world.at(i).at(j) != 0) predictMoveX = player.posX;
        //     }
        // }

        // if (world.at(predictMoveY / 20).at(predictMoveX / 20) || world.at(predictMoveY / 20).at(predictMoveX / 20 + 1) || world.at(predictMoveY / 20).at(predictMoveX / 20 + 2)) {

        // }
        // world.at(player.posY/20).at(player.posX/20) = 2;
        // world.at(player.posY/20 - 1).at(player.posX/20) = 2;
        // world.at(player.posY/20 + 1).at(player.posX/20) = 2;

        // world.at(player.posY/20).at(player.posX/20 + 1) = 2;
        // world.at(player.posY/20 - 1).at(player.posX/20 + 1) = 2;
        // world.at(player.posY/20 + 1).at(player.posX/20 + 1) = 2;

        BeginDrawing();
        ClearBackground(blackgrowndColor);

        DrawWorld(world, player, blockColors);

        // for (int j = player.posY / 20; j < player.posY / 20 + 3; j ++) {
        //     DrawObjectWithPlayerOffcet(predictMoveXGridRight, j, 1, 20, GREEN, player);
        // }

        EndDrawing();
    }

    return 0;
}
