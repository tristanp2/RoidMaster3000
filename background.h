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
        offset_x = 0;
        offset_y = 0;
        last_x = 0;
        last_y = 0;
        full_width = width + 2;     //Want to generate tiles outside of screen to allow scrolling
        full_height = height + 2;
        tiles_x = width / tile_rect.w;
        tiles_y = height / tile_rect.h;
        this->randomize = randomize;
        
        scale_surf=SDL_CreateRGBSurface(0,tile_rect.w, tile_rect.h,32,0,0,0,0);
        scale_surf->refcount++;
        SDL_SetColorKey(scale_surf, SDL_TRUE, SDL_MapRGB(scale_surf->format,0,0xff,0xa1)); 
        SDL_SetSurfaceBlendMode(scale_surf, SDL_BLENDMODE_NONE);

        generate_background();
    }
    void update(int x, int y){
        offset_x = x;
        offset_y = y;
        delta.set_x(offset_x - last_x);
        delta.set_y(offset_y - last_y);
    }
    void draw(SDL_Renderer* r){
        SDL_Texture* texture;
        if(abs(delta.get_x()) > tile_rect.w)  load_column();
        if((delta.get_y()) > tile_rect.h)  load_row();
        tile_rect.x = offset_x % tile_rect.w;
        for(unsigned int i=0; i<tiles_x; tile_rect.x+=tile_rect.w, i++){
            tile_rect.y = offset_y % tile_rect.h;
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
    Point delta;
    SDL_Surface *scale_surf;
    SDL_Rect tile_rect;
    unsigned int full_width, full_height, tiles_x, tiles_y, offset_x, offset_y,last_x,last_y;

    struct TileInfo{
        int index;
        SDL_RendererFlip flip;
    };
    TileInfo** tile_info; 

    void load_column(){
        last_x = offset_x;
        if(delta.get_x()>0){
            //Need to shift tiles to the left and get new column on right
            for(unsigned int i=1; i<tiles_x; i++){
                for(unsigned int j=0; j<tiles_y; j++){
                    tile_info[i-1][j] = tile_info[i][j];
                    if(i == tiles_x - 1){
                        tile_info[i][j].index = (abs(offset_x + offset_y)%13) % (tiles->max_frame + 1);
                        tile_info[i][j].flip = get_flip((abs(offset_x + offset_y)%13)%4);
                    }
                }
            }
        }
        else{
            for(unsigned int i=tiles_x-2; i>=0; i--){
                for(unsigned int j=0; j<tiles_y; j++){
                    tile_info[i+1][j] = tile_info[i][j];
                    if(i == 0){
                        tile_info[i][j].index = ((abs(offset_x + offset_y))%13) % (tiles->max_frame + 1);
                        tile_info[i][j].flip = get_flip((abs(offset_x + offset_y)%13)%4);
                    }
                }
            }
        }
    }       
    void load_row(){
        last_y = offset_y;
        if(delta.get_y()>0){
            //Need to shift tiles down and get new row on top
            for(unsigned int i=0; i<tiles_x; i++){
                for(unsigned int j=tiles_y-2; j>=0; j++){
                    tile_info[i][j+1] = tile_info[i][j];
                    if(j == 0){
                        tile_info[i][j].index = (abs(offset_x + offset_y)%13) % (tiles->max_frame + 1);
                        tile_info[i][j].flip = get_flip((abs(offset_x + offset_y)%13)%4);
                    }
                }
            }
        }
        else{
            for(unsigned int i=0; i<tiles_x; i++){
                for(unsigned int j=1; j<tiles_y; j++){
                    tile_info[i][j-1] = tile_info[i][j];
                    if(i == tiles_y-1){
                        tile_info[i][j].index = ((abs(offset_x) + offset_y)%13) % (tiles->max_frame + 1);
                        tile_info[i][j].flip = get_flip((abs(offset_x + offset_y)%13)%4);
                    }
                }
            }
        }

    }
            
        
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
};
#endif
