#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define EVER ;;
#define WIDTH 1669
#define HEIGHT 1045
#define SQSIZE 5
#define COLCELLS 278
#define ROWCELLS 174

struct cell{
    int alive;
    SDL_Rect rect;
};


int main (int argc, char *argv[]){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("Failed to init SDL: %s \n", SDL_GetError());
        return 1;
    }
    if(argc != 3){
        fprintf(stderr, "Program needs 2 arguments.\nFirst is refresh rate in ms.\nSecond is %% change to fill cell.\nExample: ./conway 100 30\n");
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("conway", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL ){
        printf("Window could not be created!: %s \n", SDL_GetError());
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL ){
        printf("Renderer could not be created!: %s \n", SDL_GetError());
        return 1;
    }

    struct cell cells[ROWCELLS][COLCELLS];
    int sleepms = atoi(argv[1]);
    int pct = atoi(argv[2]);
    srand(time(0));
    Uint32 fps_frames = 0;
    SDL_Event event;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255,255,255,255);
    int cellsAliveTmp[ROWCELLS][COLCELLS];
    float frameTime = 0;
    int prevTime = 0;
    int currentTime = 0;
    float deltaTime = 0;

    for(int i = 0, y = 1; i < ROWCELLS; i++, y+=6){
        for(int j = 0, x = 1; j < COLCELLS; j++, x+=6){
            cells[i][j].rect.x = x;
            cells[i][j].rect.y = y;
            cells[i][j].rect.w = cells[i][j].rect.h = SQSIZE;

            int n = rand() % 100;
            if(n < pct) {
                cells[i][j].alive = 1;
            }
        }
    }

    while(1){
        prevTime = currentTime;
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - prevTime);
        if(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                break;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN){
                cells[event.button.y/6-1][event.button.x/6-1].alive = 1;
            }
        }

        frameTime += deltaTime;

        if ( frameTime >= sleepms){
            frameTime = 0;
            for(int i = 0; i < ROWCELLS; i++){
                for(int j = 0; j < COLCELLS; j++){
                    if(cells[i][j].alive == 0){
                        SDL_SetRenderDrawColor(renderer, 0,0,0,0);
                        SDL_RenderFillRect(renderer, &cells[i][j].rect);
                    } else {
                        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                        SDL_RenderFillRect(renderer, &cells[i][j].rect);
                    }
                }
            }
            // change cellsAliveTmp state
            for (int i = 0; i < ROWCELLS; ++i) {
                for (int j = 0; j < COLCELLS; ++j) {
                    // get number of neighbors around
                    int nb = 0;
                    for (int y = -1; y < 2 ; ++y) {
                        for (int x = -1; x < 2 ; ++x) {
                            if(y == 0 && x == 0) {// skip self
                                continue;
                            }
                            if(i+y < 0 || j+x < 0 || i+y >= ROWCELLS || j+x >= COLCELLS)
                                continue;
                            if(cells[i+y][j+x].alive == 1){
                                nb++;
                            }
                        }
                    }
                    // conway's rules:
                    if(cells[i][j].alive == 1) {
                        if(nb < 2 || nb > 3) {
                            cellsAliveTmp[i][j] = 0;
                        } else {
                            cellsAliveTmp[i][j] = 1;
                        }
                    }else if (nb == 3) {
                        cellsAliveTmp[i][j] = 1;
                    }
                }
            }
            // change world state
            for (int i = 0; i < ROWCELLS; ++i) {
                for (int j = 0; j < COLCELLS; ++j) {
                    cells[i][j].alive = cellsAliveTmp[i][j];
                }
            }
            fps_frames++;
        }

        SDL_RenderPresent(renderer);
        if(currentTime > 30000){
            break;
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    printf("%d", fps_frames);
    return 0;
}

