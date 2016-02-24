#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <cstdlib>

class BackGround{
public:
    BackGround(){
        tiles = NULL;
        scale_surf = NULL;
    }
    BackGround(Sprite* tiles, bool randomize, unsigned int width, unsigned int height){
        this->tiles = tiles;
        tile_rect.h = tiles->get_frame_height();
        tile_rect.w = tiles->get_frame_width();
        tile_rect.x = 0;
        tile_rect.y = 0;
        full_width = width;
        full_height = height;
        tiles_x = width / tile_rect.w;
        tiles_y = height / tile_rect.h;
        this->randomize = randomize;
        
        scale_surf=SDL_CreateRGBSurface(0,tile_rect.w, tile_rect.w,32,0,0,0,0);
        scale_surf->refcount++;
        SDL_SetColorKey(scale_surf, SDL_TRUE, SDL_MapRGB(scale_surf->format,0,0xff,0xa1)); 
        SDL_SetSurfaceBlendMode(scale_surf, SDL_BLENDMODE_NONE);

        generate_background();
    }
    void draw(SDL_Renderer* r){
        SDL_Texture* texture;
        tile_rect.x = 0;
        for(unsigned int i=0; i<tiles_x; tile_rect.x+=tile_rect.w, i++){
            tile_rect.y = 0;
            for(unsigned int j=0; j<tiles_y; tile_rect.y+=tile_rect.h, j++){
                SDL_BlitScaled((*tiles)[tile_info[i][j].index], NULL, scale_surf, NULL);
                texture = SDL_CreateTextureFromSurface(r, scale_surf);
                SDL_RenderCopyEx(r, texture, NULL, &tile_rect, 0, NULL, tile_info[i][j].flip);
                SDL_DestroyTexture(texture);
            }
        }
    } 
private:
    bool randomize;
    Sprite* tiles;
    SDL_Surface *scale_surf;
    SDL_Rect tile_rect;
    unsigned int full_width, full_height, tiles_x, tiles_y;

    void generate_background(){
        tile_info = new TileInfo*[tiles_x];
        unsigned int index=-1, flip;
        for(unsigned int i=0; i<tiles_x; i++){
            tile_info[i] = new TileInfo[tiles_y];
            for(unsigned int j=0; j<tiles_y; j++){
                if(randomize){
                    index = rand();
                    flip = rand()%4;
                }
                else{
                    index++;
                    flip = 0;
                }
                index %= tiles->max_frame + 1;
                tile_info[i][j].index = index;
                tile_info[i][j].flip = get_flip(flip);
            }
        }
    }
    SDL_RendererFlip get_flip(int in){
        switch(in){
            case 0:
                return SDL_FLIP_NONE;
            case 1:
                return SDL_FLIP_HORIZONTAL;
            case 2:
                return SDL_FLIP_VERTICAL;
            case 3:
                return (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
            default:
                return SDL_FLIP_NONE;
        }
    }
    struct TileInfo{
        int index;
        SDL_RendererFlip flip;
    };
    TileInfo** tile_info; 
};
#endif
