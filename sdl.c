#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <unistd.h>
#include <time.h>


const int WIDTH = 1669;
const int HEIGHT = 1045;
const int SQSIZE = 5;
const int COLCELLS = 278;
const int ROWCELLS = 174;


int main (int argc, char *argv[]){
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "Failed to init SDL: %s \n", SDL_GetError());
        return 1;
    }
    if(argc != 3){
        printf("Program needs 2 arguments.\nFirst is refresh rate in ms.\nSecond is %% change to fill cell.\nExample: ./conway 100 30\n");
        return 1;
    }
    int sleepms = atoi(argv[1]) * 1000;
    int pct = atoi(argv[2]);
    srand(time(0));

    SDL_Window *window = SDL_CreateWindow("conway", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);

    struct cell{
        int alive;
        SDL_Rect rect;
    };

    int worldTmp[ROWCELLS][COLCELLS];
    struct cell cells[ROWCELLS][COLCELLS];

    for(int i = 0, y = 1; i < ROWCELLS; i++, y+=6){
        for(int j = 0, x = 1; j < COLCELLS; j++, x+=6){
            cells[i][j].rect.x = x;
            cells[i][j].rect.y = y;

            cells[i][j].rect.w = SQSIZE;
            cells[i][j].rect.h = SQSIZE;

            int n = rand() % 100;
            if(n < pct) {
                cells[i][j].alive = 1;
                SDL_RenderFillRect(renderer, &cells[i][j].rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
    SDL_Event event;

    do{
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
        SDL_RenderPresent(renderer);
        // change worldTmp state
        for (int i = 0; i < ROWCELLS; ++i) {
            for (int j = 0; j < COLCELLS; ++j) {
                // get number of neighbors around
                int nb = 0;
                for (int x = -1; x < 2 ; ++x) {
                    for (int y = -1; y < 2 ; ++y) {
                        if(y == 0 && x == 0) {// skip self
                            continue;
                        }
                        if(cells[i+x][j+y].alive == 1){
                            nb++;
                        }
                    }
                }
                // conway's rules:
                if(cells[i][j].alive == 1) {
                    if(nb < 2 || nb > 3) {
                        worldTmp[i][j] = 0;
                    } else {
                        worldTmp[i][j] = 1;
                    }
                }else if (nb == 3) {
                    worldTmp[i][j] = 1;
                }
            }
        }
        // change world state
        for (int i = 1; i < ROWCELLS; ++i) {
            for (int j = 1; j < COLCELLS; ++j) {
                cells[i][j].alive = worldTmp[i][j];
            }
        }
        usleep(sleepms);
    }while(1);
    return 0;
}

