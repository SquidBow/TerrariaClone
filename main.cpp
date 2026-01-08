#include "raylib.h"
#include <algorithm>
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

struct Player {
    public:

    // Top left corner
    int posX;
    int posY;

    Player (int posX, int posY) : posX(posX), posY(posY) {}
};

struct Block {
    public:

    string name;
    Color color;
    bool placeMidAir;
    int width;
    int height;
    bool passableFromBelow;
    int id;

    Block (string name, Color color, bool placeMidAir, int width, int height, bool passableFromBelow, int id) {
        this -> name = name;
        this->color = color;
        this->placeMidAir = placeMidAir;
        this->height = height;
        this->width = width;
        this->passableFromBelow = passableFromBelow;
        this->id = id;
    }

    Block() {}
};

void FillWorldBlocks (array<Block, 10>& blocColors) {
    blocColors.at(0) = Block("Nothing", BLUE, true, 20, 20, true, 0);
    blocColors.at(1) = Block("Dirt", BROWN, false, 20, 20, false, 1);
    blocColors.at(2) = Block("Player", ORANGE, true, 20, 20, true, 2);
    blocColors.at(3) = Block("Platform", BROWN, false, 20, 4, true, 3);
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

            Block block = worldBlocks.at(world.at(worldY).at(worldX));
            DrawRectangle(camX, camY, block.width, block.height, block.color);

        }

        //Draw player on top
        DrawRectangle(screenWidht / 2 - blockWidth, screenHeight / 2 - blockWidth * 1.5 + 10, blockWidth * 2, blockWidth * 3, BLACK);
    }
}

void DrawInventory (array<array<Block, 10>, 6>& inventory, bool isInventoryOpen, int selected) {
    int difference = blockWidth * 3;

    if (!isInventoryOpen) {
        int posX = blockWidth;
        for (int i = 0; i < inventory.at(0).size(); i ++) {
            if (i == selected) {
                DrawRectangle(posX, blockWidth, blockWidth * 2, blockWidth * 2, Fade(DARKBLUE, 0.5f));
                DrawRectangleLines(posX, blockWidth, blockWidth * 2, blockWidth * 2, GOLD);
            }
            else {
                DrawRectangle(posX, blockWidth, blockWidth * 2, blockWidth * 2, Fade(DARKBLUE, 0.5f));
                DrawRectangleLines(posX, blockWidth, blockWidth * 2, blockWidth * 2, WHITE);
            }

            Block block = inventory.at(0).at(i);
            int offcet = ((blockWidth * 2) - block.height) / 2;

            // For platform = 40 - 4 = 36 / 2 = 18. Pos = 20 + 18
            DrawRectangle(posX + blockWidth / 2, blockWidth + offcet, block.width, block.height, block.color);

            posX += difference;
        }
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

    array<array<Block, 10>, 6> inventory;

    inventory.at(0).at(0) = Block("Dirt", BROWN, false, 20, 20, false, 1);
    inventory.at(0).at(1) = Block("Platform", BROWN, false, 20, 4, true, 3);

    Player player (10020, 9900);

    createWorld(world, player);

    char lastDirection = 'n';
    int playerSpeedMod = 3;
    float verticalSpeed = 0;
    bool jumped = false;
    int jumpBufferFrames = 0;

    bool isInventoryOpen = false;
    int selectedInventorySlot = 0;

    InitWindow(blocksInARow * blockWidth, blocksInACol * blockWidth, "Terraria clone");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        verticalSpeed += gravity;

        if (jumpBufferFrames > 0) {
            jumpBufferFrames --;
        }

        int predictMoveX = player.posX;
        int predictMoveY = player.posY;

        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) /*&& !jumped*/) {
            // verticalSpeed = -jumpHeight;
            jumpBufferFrames = 7;
            jumped = true;
        }

        if (jumpBufferFrames > 0 && !jumped) {
            verticalSpeed = -jumpHeight;
            jumpBufferFrames = 0;
        }

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            lastDirection = 'r';
            predictMoveX += playerSpeedMod;
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            lastDirection = 'l';
            predictMoveX -= playerSpeedMod;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            isInventoryOpen = !isInventoryOpen;
        }

        if (IsKeyPressed(KEY_ONE)) {
            selectedInventorySlot = 0;
        }
        else if (IsKeyPressed(KEY_TWO)) {
            selectedInventorySlot = 1;
        }
        else if (IsKeyPressed(KEY_THREE)) {
            selectedInventorySlot = 2;
        }
        else if (IsKeyPressed(KEY_FOUR)) {
            selectedInventorySlot = 3;
        }
        else if (IsKeyPressed(KEY_FIVE)) {
            selectedInventorySlot = 4;
        }
        else if (IsKeyPressed(KEY_SIX)) {
            selectedInventorySlot = 5;
        }
        else if (IsKeyPressed(KEY_SEVEN)) {
            selectedInventorySlot = 6;
        }
        else if (IsKeyPressed(KEY_EIGHT)) {
            selectedInventorySlot = 7;
        }
        else if (IsKeyPressed(KEY_NINE)) {
            selectedInventorySlot = 8;
        }
        else if (IsKeyPressed(KEY_ZERO)) {
            selectedInventorySlot = 9;
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
                // world.at(mouseGridY).at(mouseGridX) = 1;
                world.at(mouseGridY).at(mouseGridX) = inventory.at(0).at(selectedInventorySlot).id;
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
        else if (predictMoveX < player.posX) {
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
        else if (predictMoveY < player.posY) {
            //Look up
            // 3 blocks. below the player, 1 right and 2 right
            for (int j = player.posX; j < player.posX + blockWidth * 2; j ++) {
                int blockId = world.at(predictMoveY / 20).at(j / 20);
                if (blockId != 0 && !worldBlocks.at(blockId).passableFromBelow) predictMoveY = (predictMoveY / 20) * 20 + 20;
            }
        }

        player.posX = predictMoveX;
        player.posY = predictMoveY;

        BeginDrawing();
        ClearBackground(blackgrowndColor);

        DrawWorld(world, player, worldBlocks);
        DrawInventory(inventory, isInventoryOpen, selectedInventorySlot);

        EndDrawing();
    }

    return 0;
}
