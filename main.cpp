#include "raylib.h"
#include <array>
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
bool debug = false;

class Player {
    public:

    // Top left corner
    int posX;
    int posY;

    Player (int posX, int posY) : posX(posX), posY(posY) {}
};

class Block {
    public:

    string name;
    Color color;
    bool placeMidAir;
    int width;
    int height;
    bool passableFromBelow;
    int id;
    int durability;

    Block (string name, Color color, bool placeMidAir, int width, int height, bool passableFromBelow, int id, int durability) {
        this -> name = name;
        this->color = color;
        this->placeMidAir = placeMidAir;
        this->height = height;
        this->width = width;
        this->passableFromBelow = passableFromBelow;
        this->id = id;
        this->durability = durability;
    }

    Block() {
        name = "Nothing";
        color= BLANK;
        placeMidAir = true;
        width = 0;
        height = 0;
        passableFromBelow = true;
        id = 0;
        durability = 0;
    }
};


class Entity {
    public:

    int posX;
    int posY;
    float velosityY;
    Block block;
    int life = 500;

    Entity (int posX, int posY, float velosityY, Block block) : posX(posX), posY(posY), velosityY(velosityY), block(block) {}

    Entity () {
        posX = 0;
        posY = 0;
        velosityY = 0.0;
        block = Block();
    }
};

const int blocksInARow = 40;
const int blocksInACol = 30;
const int blockWidth = 20;
const int screenWidht = blocksInARow * blockWidth;
const int screenHeight = blocksInACol * blockWidth;
const float gravity = 1;
const float jumpHeight = 17;
array<array<Block, 1000>, 1000> world {};
array<array<Block, 10>, 6> inventory {};

Color backGrowndColor = BLUE;

void FillWorldBlocks (vector<Block>& worldBlocks) {
    worldBlocks.push_back(Block("Nothing", BLANK, true, blockWidth, blockWidth, true, 0, 0));
    worldBlocks.push_back(Block("Player", ORANGE, true, blockWidth, blockWidth, true, 1, 0));

    worldBlocks.push_back(Block("Dirt", BROWN, false, blockWidth, blockWidth, false, 2, 0));
    worldBlocks.push_back(Block("Platform", BROWN, false, blockWidth, 4, true, 3, 0));
}

void createWorld (Player& player,vector<Block>& worldBlocks) {
    // i == y
    for (int i = 500; i < 1000; i ++) {

        // j = x
        for (int j = 0; j < 1000; j ++) {

            // world [y] [x]
            world.at(i).at(j) = worldBlocks.at(2);
        }
    }


    world.at(player.posY/blockWidth).at(player.posX/blockWidth + 3) = worldBlocks.at(2);
    world.at(player.posY/blockWidth - 1).at(player.posX/blockWidth + 3) = worldBlocks.at(2);
    world.at(player.posY/blockWidth + 1).at(player.posX/blockWidth + 3) = worldBlocks.at(2);
    world.at(player.posY/blockWidth - 8).at(player.posX/blockWidth + 3) = worldBlocks.at(2);
    world.at(player.posY/blockWidth - 8).at(player.posX/blockWidth - 3) = worldBlocks.at(2);
}

void DrawWorld (const Player& player, vector<Block>& worldBlocks, int8_t ofScreen, vector<Entity> brokenBlocks) {

    //Player pos / 20 is the player grid position
    int camX;
    int camY = player.posY;
    int playerOffcetX = player.posX % blockWidth;
    int playerOffcetY = player.posY % blockWidth;

    if (ofScreen == -1) {
        camX = (screenWidht / 2);
    }
    else if (ofScreen == 1) {
        camX = (world.at(0).size() * blockWidth - (screenWidht / 2) - blockWidth * 2);
    }
    else {
        camX = player.posX;
    }

    cout << "CamX: " << camX << "\n";
    cout << "CamY: " << camY << "\n";

    //j = y
    for (int j = 0; j < blocksInACol + 1; j ++) {

        // i = x
        for (int i = 0; i < blocksInARow + 1; i ++) {
            int blockPosX;

            if (ofScreen == -1) {
                blockPosX = i * blockWidth;
            }
            else if (ofScreen == 1) {
                blockPosX = i * blockWidth - blockWidth;
            }
            else {
                blockPosX = i * blockWidth - playerOffcetX;
            }

            int blockPosY = j * blockWidth - playerOffcetY;

            //Now take the blocks
            //That is X. Start at some before the player
            //40 blocks, 19 - player(2) - 19;
            int worldX = camX / blockWidth - 19 + i;

            //Same for Y
            //30 blocks, 14 - player(2) - 14
            int worldY = camY / blockWidth - 14 + j;


            Block block = world.at(worldY).at(worldX);
            DrawRectangle(blockPosX, blockPosY, block.width, block.height, block.color);

            if (block.id > 1 && debug) {
                DrawText(TextFormat("%d", block.durability), blockPosX, blockPosY + 1, 20, WHITE);
            }

            // cout << "Offscreen: " << (int)ofScreen << "\n";
            // cout << "Drawing block: " << i << "\n";
        }
    }

    for (Entity& brokenBlock : brokenBlocks) {
        cout << "Drawing entity: " << brokenBlock.block.name << "\nAt X: " << camX - brokenBlock.posX + screenWidht / 2 << "\nAt Y: " << camY - brokenBlock.posY + screenHeight / 2 << "\n\n";
        DrawRectangle(brokenBlock.posX - camX + screenWidht / 2 - blockWidth, brokenBlock.posY - camY + screenHeight / 2 - blockWidth, brokenBlock.block.width, brokenBlock.block.height, brokenBlock.block.color);
    }

    int playerX;

    if (ofScreen == -1) {
        playerX = player.posX - blockWidth;
    }
    else if (ofScreen == 1) {
        playerX = (player.posX - (world.at(0).size() * blockWidth)) + screenWidht;
    }
    else {
        playerX = screenWidht / 2 - blockWidth;
    }

    DrawRectangle(playerX, screenHeight / 2 - blockWidth * 1.5 + 10, blockWidth * 2, blockWidth * 3, BLACK);
}

void DrawInventory (bool isInventoryOpen, int selected) {
    int difference = blockWidth * 3;

    int posX = blockWidth;
    int posY = blockWidth;
    for (int i = 0; i < inventory.at(0).size(); i ++) {
        if (i == selected) {
            DrawRectangle(posX, posY, blockWidth * 2, blockWidth * 2, Fade(DARKBLUE, 0.5f));
            DrawRectangleLines(posX, posY, blockWidth * 2, blockWidth * 2, GOLD);
        }
        else {
            DrawRectangle(posX, posY, blockWidth * 2, blockWidth * 2, Fade(DARKBLUE, 0.5f));
            DrawRectangleLines(posX, posY, blockWidth * 2, blockWidth * 2, WHITE);
        }

        Block block = inventory.at(0).at(i);
        int offcet = ((blockWidth * 2) - block.height) / 2;

        // For platform = 40 - 4 = 36 / 2 = 18. Pos = blockWidth + 18
        DrawRectangle(posX + blockWidth / 2, posY + offcet, block.width, block.height, block.color);

        posX += difference;
    }

    if (isInventoryOpen) {
        for (int j = 1; j < inventory.size(); j ++) {
            posX = blockWidth;
            posY += difference;

            for (int i = 0; i < inventory.at(j).size(); i ++) {
                if (i == selected) {
                    DrawRectangle(posX, posY, blockWidth * 2, blockWidth * 2, Fade(DARKBLUE, 0.5f));
                    DrawRectangleLines(posX, posY, blockWidth * 2, blockWidth * 2, GOLD);
                }
                else {
                    DrawRectangle(posX, posY, blockWidth * 2, blockWidth * 2, Fade(DARKBLUE, 0.5f));
                    DrawRectangleLines(posX, posY, blockWidth * 2, blockWidth * 2, WHITE);
                }

                Block block = inventory.at(j).at(i);
                int offcet = ((blockWidth * 2) - block.height) / 2;

                // For platform = 40 - 4 = 36 / 2 = 18. Pos = 20 + 18
                DrawRectangle(posX + blockWidth / 2, posY + offcet, block.width, block.height, block.color);

                posX += difference;
            }
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
    vector<Block> worldBlocks;
    FillWorldBlocks(worldBlocks);

    vector<Entity> brokenBlocks;

    for (int i = 0; i < world.size(); i ++) {
        for (int j = 0; j < world.at(0).size(); j ++) {
            world.at(i).at(j) = Block();
        }
    }

    for (int i = 0; i < inventory.size(); i ++) {
        for (int j = 0; j < inventory.at(0).size(); j ++) {
            inventory.at(i).at(j) = Block();
        }
    }

    inventory.at(0).at(0) = worldBlocks.at(2);
    inventory.at(0).at(1) = worldBlocks.at(3);

    Player player (19500, 9900);

    createWorld(player, worldBlocks);

    char lastDirection = 'n';
    float verticalSpeed = 0;
    bool jumped = false;
    int jumpBufferFrames = 0;

    bool isInventoryOpen = false;
    int selectedInventorySlot = 0;

    int8_t ofScreen = 0;

    // Make a reset mechanic
    uint8_t pressedDown = 0;
    int8_t plasingBlocks = 0;

    InitWindow(blocksInARow * blockWidth, blocksInACol * blockWidth, "Terraria clone");
    SetTargetFPS(60);
    int playerSpeedMod = 3;
    SetExitKey(KEY_NULL);

    while (!WindowShouldClose()) {
        system("clear");

        verticalSpeed += gravity;

        int predictMoveX = player.posX;
        int predictMoveY = player.posY;

        if (pressedDown > 0) {
            pressedDown --;
        }

        if (jumpBufferFrames > 0) {
            jumpBufferFrames --;
        }

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_W)/* || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_W)*/) {
            pressedDown = 2;
        }

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


        if (verticalSpeed > 10) {
            verticalSpeed = 10;
        }
        else if (verticalSpeed < -jumpHeight) {
            verticalSpeed = -jumpHeight;
        }

        predictMoveY += verticalSpeed;

        // cout << "Player posX: " << player.posX << "\n";
        // cout << "Predicted player posX: " << predictMoveX << "\n";

        if (predictMoveX > player.posX) {
            // 1000 * 20 = 20000 - 1
            if (predictMoveX > world.at(0).size() * blockWidth - blockWidth * 2 - 1) {
                predictMoveX = world.at(0).size() * blockWidth - blockWidth * 2 - 1;
            }

            else {
                //Check X colision
                //Check 4 blocks Y and 3 blocks X
                int predictMoveXRight = predictMoveX + blockWidth * 2;
                for (int j = player.posY; j < player.posY + blockWidth * 3; j ++) {
                    int blockId = world.at(j / 20).at(predictMoveXRight / 20).id;
                    if (blockId != 0 && !worldBlocks.at(blockId).passableFromBelow) {
                        predictMoveX = (predictMoveXRight / blockWidth) * blockWidth - blockWidth * 2;
                    }
                }
            }
        }
        else if (predictMoveX < player.posX) {
            //Look left
            for (int j = player.posY; j < player.posY + blockWidth * 3; j ++) {
                int blockId = world.at(j / blockWidth).at(predictMoveX / blockWidth).id;
                if (blockId != 0 && !worldBlocks.at(blockId).passableFromBelow) {
                    predictMoveX = (predictMoveX / blockWidth) * blockWidth + blockWidth;
                }
                else if (predictMoveX - blockWidth < 0) {
                    predictMoveX = blockWidth;
                }
            }
        }

        if (predictMoveY > player.posY) {
            int predictMoveYDown = predictMoveY + blockWidth * 3;
            // 3 blocks. below the player, 1 right and 2 right
            for (int j = player.posX; j < player.posX + blockWidth * 2; j ++) {
                int blockId = world.at(predictMoveYDown / blockWidth).at(j / blockWidth).id;
                if (blockId != 0) {
                    if (!worldBlocks.at(blockId).passableFromBelow || pressedDown == 0) {
                        predictMoveY = (predictMoveYDown / blockWidth) * blockWidth - blockWidth * 3;
                        jumped = false;
                    }
                }
            }
        }
        else if (predictMoveY < player.posY) {
            //Look up
            // 3 blocks. below the player, 1 right and 2 right
            for (int j = player.posX; j < player.posX + blockWidth * 2; j ++) {
                int blockId = world.at(predictMoveY / blockWidth).at(j / blockWidth).id;
                // Makes pass from below
                // If ID is not 0, so smth and is not passable, we snap to the bottom of the block
                if (blockId != 0 && !worldBlocks.at(blockId).passableFromBelow) predictMoveY = (predictMoveY / 20) * 20 + 20;
            }
        }

        player.posX = predictMoveX;
        player.posY = predictMoveY;

        if (player.posX < screenWidht / 2) {
            ofScreen = -1;
        }
        else if (player.posX > world.at(0).size() * blockWidth - (screenWidht / 2) - blockWidth - 1) {
            ofScreen = 1;
        }
        else {
            ofScreen = 0;
        }

        if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && IsCursorOnScreen()) {
            int mouseGridX;

            // Get mouse pos
            if (ofScreen == -1) {
                // cout << "OfScreen is -1" << "\n";
                mouseGridX = (GetMouseX() + blockWidth) / blockWidth;
            }
            else if (ofScreen == 1) {
                // cout << "OfScreen is 1" << "\n";
                mouseGridX = (world.at(0).size() * blockWidth + GetMouseX() - screenWidht) / blockWidth;
                // cout << "Mouse at: " << mouseGridX << "\n";
            }
            else {
                // cout << "OfScreen is 0" << "\n";
                mouseGridX = ((GetMouseX() + player.posX - screenWidht / 2) + blockWidth) / blockWidth;
            }

            int mouseGridY = ((GetMouseY() + player.posY - screenHeight / 2) + blockWidth) / blockWidth;

            // cout << "\nmouseGridX: " << mouseGridX << "\nmouseGridY: " << mouseGridY;
            // cout << "\nplayerPosGridX: " << player.posX << "\nplayerPosGridY: " << player.posY << "\n";
            if (world.at(mouseGridY).at(mouseGridX).id != 0 && (plasingBlocks == -1 || plasingBlocks == 0)) {
                plasingBlocks = -1;
                if (world.at(mouseGridY).at(mouseGridX).durability > 1) {
                    world.at(mouseGridY).at(mouseGridX).durability --;
                }
                else {
                    Block block = world.at(mouseGridY).at(mouseGridX);
                    world.at(mouseGridY).at(mouseGridX) = Block();
                    brokenBlocks.push_back(Entity (mouseGridX * blockWidth, mouseGridY * blockWidth, -5, block));
                }
            }
            else if (plasingBlocks == 1 || plasingBlocks == 0){
                // world.at(mouseGridY).at(mouseGridX) = 1;
                plasingBlocks = 1;
                world.at(mouseGridY).at(mouseGridX) = inventory.at(0).at(selectedInventorySlot);
            }
        }
        else {
            plasingBlocks = 0;
        }

        for (Entity& brokenBlock : brokenBlocks) {
            // cout << "Broken block X: " << brokenBlock.posX << "\nBroken block Y: " << brokenBlock.posY << "\n";

            brokenBlock.velosityY += gravity;

            if (brokenBlock.velosityY > 5) {
                brokenBlock.velosityY = 5;
            }
            else if (brokenBlock.velosityY < -jumpHeight) {
                brokenBlock.velosityY = -jumpHeight;
            }

            int predicBlockY = brokenBlock.posY + brokenBlock.velosityY;

            if (predicBlockY > brokenBlock.posY) {
                int predictBlockYDown = predicBlockY + brokenBlock.block.height;
                // 3 blocks. below the player, 1 right and 2 right
                for (int j = brokenBlock.posX; j < brokenBlock.posX + brokenBlock.block.width; j ++) {
                    Block block = world.at(predictBlockYDown / blockWidth).at(brokenBlock.posX / blockWidth);
                    if (block.id != 0) {
                        // cout << "It is less" << "\n";
                        // int sub = brokenBlock.block.height;
                        // if (block.passableFromBelow) {
                        //     sub += 6;
                        // }
                        predicBlockY = (predictBlockYDown / blockWidth) * blockWidth - brokenBlock.block.height;
                        // predicBlockY = brokenBlock.posY;
                    }
                }
            }

            brokenBlock.posY = predicBlockY;
        }

        BeginDrawing();
        ClearBackground(backGrowndColor);

        DrawWorld(player, worldBlocks, ofScreen, brokenBlocks);
        DrawInventory(isInventoryOpen, selectedInventorySlot);

        EndDrawing();
    }

    return 0;
}
