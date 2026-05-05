#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Screen dimensions and cell size
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int CELL_SIZE = SCREEN_WIDTH / 3;

// Colors
SDL_Color WHITE = { 255, 255, 255, 255 };
SDL_Color BLACK = { 0, 0, 0, 255 };
SDL_Color LIGHT_BLUE = { 173, 216, 230, 255 };
SDL_Color DARK_BLUE = { 0, 0, 139, 255 };
SDL_Color RED = { 255, 0, 0, 255 };

// Global SDL variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* appleTexture = NULL;
SDL_Texture* bananaTexture = NULL;
TTF_Font* font = NULL;

// Game state
int board[3][3];  // 0 = empty, 1 = apple, 2 = banana
int currentPlayer = 1;  // 1 = apple, 2 = banana
bool gameOver = false;

// Load a texture from file
SDL_Texture* LoadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Failed to load image %s: %s\n", path, IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Initialize SDL
bool InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }
    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    // Load textures
    appleTexture = LoadTexture("D:\\Projects\\GL\\SDLx32-0\\Debug\\apple.png");
    bananaTexture = LoadTexture("D:\\Projects\\GL\\SDLx32-0\\Debug\\banana.png");
    if (!appleTexture || !bananaTexture) return false;

    // Load font
    font = TTF_OpenFont("D:\\Projects\\GL\\SDLx32-0\\Debug\\Roboto-Black.ttf", 30);
    if (!font) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    return true;
}

// Render text
void RenderText(const char* text, SDL_Color color, SDL_Rect rect) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, WHITE);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    rect.w = textSurface->w;
    rect.h = textSurface->h;
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &rect);
    SDL_DestroyTexture(textTexture);
}

// Draw grid
void DrawGrid() {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    for (int i = 1; i < 3; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);  // Vertical
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);  // Horizontal
    }
}

// Draw board state
void DrawBoard() {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int x = col * CELL_SIZE;
            int y = row * CELL_SIZE;
            SDL_Rect rect = { x, y, CELL_SIZE, CELL_SIZE };
            if (board[row][col] == 1) {
                SDL_RenderCopy(renderer, appleTexture, NULL, &rect);
            }
            else if (board[row][col] == 2) {
                SDL_RenderCopy(renderer, bananaTexture, NULL, &rect);
            }
        }
    }
}

// Animate placement
void AnimatePlacement(SDL_Texture* texture, int x, int y) {
    for (int size = 0; size <= CELL_SIZE; size += 10) {
        SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderClear(renderer);
        DrawGrid();
        DrawBoard();
        SDL_Rect rect = { x + (CELL_SIZE - size) / 2, y + (CELL_SIZE - size) / 2, size, size };
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }
}

// Check if someone won
int CheckWinner() {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0) return board[i][0];
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0) return board[0][i];
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0) return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != 0) return board[0][2];
    return 0;
}

// Check if the board is full
bool IsBoardFull() {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row][col] == 0) return false;
        }
    }
    return true;
}

// Display result (winner or draw)
void DisplayResult(int winner) {
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderClear(renderer);

    char resultText[50];
    if (winner == 1) {
        snprintf(resultText, sizeof(resultText), "Apple Won!");
    }
    else if (winner == 2) {
        snprintf(resultText, sizeof(resultText), "Banana Won!");
    }
    else {
        snprintf(resultText, sizeof(resultText), "It's a Draw!");
    }

    SDL_Rect rect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 1, SCREEN_HEIGHT / 6 };
    RenderText(resultText, WHITE, rect);

    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
}

// Menu screen
void ShowMenu() {
    SDL_SetRenderDrawColor(renderer, LIGHT_BLUE.r, LIGHT_BLUE.g, LIGHT_BLUE.b, LIGHT_BLUE.a);
    SDL_RenderClear(renderer);

    // Draw "Player vs Player"
    SDL_Rect rectPvP = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6 };
    SDL_SetRenderDrawColor(renderer, DARK_BLUE.r, DARK_BLUE.g, DARK_BLUE.b, DARK_BLUE.a);
    SDL_RenderFillRect(renderer, &rectPvP);

    // Center text
    SDL_Rect textRect = rectPvP;
    textRect.x += 10;
    textRect.w -= 20;
    RenderText("Player 1 vs Player 2", WHITE, textRect);

    SDL_RenderPresent(renderer);

    SDL_Event e;
    bool inMenu = true;
    while (inMenu) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                gameOver = true;
                inMenu = false;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                inMenu = false;  // Start PvP game
            }
        }
    }
}

// Clean up resources
void CleanUp() {
    SDL_DestroyTexture(appleTexture);
    SDL_DestroyTexture(bananaTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

// Main game loop
void GameLoop() {
    SDL_Event e;

    while (!gameOver) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                gameOver = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if (!gameOver) {
                    int row = e.button.y / CELL_SIZE;
                    int col = e.button.x / CELL_SIZE;
                    if (board[row][col] == 0) {
                        board[row][col] = currentPlayer;
                        AnimatePlacement((currentPlayer == 1) ? appleTexture : bananaTexture, col * CELL_SIZE, row * CELL_SIZE);

                        int winner = CheckWinner();
                        if (winner != 0 || IsBoardFull()) {
                            gameOver = true;
                            DisplayResult(winner);
                        }
                        else {
                            currentPlayer = (currentPlayer == 1) ? 2 : 1;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderClear(renderer);
        DrawGrid();
        DrawBoard();
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char* args[]) {
    if (!InitSDL()) {
        printf("Failed to initialize!\n");
        return 1;
    }

    ShowMenu();
    memset(board, 0, sizeof(board));
    currentPlayer = 1;
    gameOver = false;

    GameLoop();
    CleanUp();

    return 0;
}
