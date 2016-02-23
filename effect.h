#ifndef EFFECT_H
#define EFFECT_H

#include "sprite.h"

using namespace std;

class Effect{
public:
    bool done,loop;
    int current_frame;
    Vector2d pos;
    Effect(Sprite* sprite, int ms_per_frame, bool loop, int scale, Vector2d pos){
        done = false;
        this->pos = pos;
        this->loop = loop;
        current_frame = 0;
        delta_t = 0;
        this->ms_per_frame = ms_per_frame;
        this->sprite = sprite;
        
        draw_rect.w = sprite->get_frame_width()*scale;
        draw_rect.h = sprite->get_frame_height()*scale;
        
        draw_rect.x=this->pos.x - draw_rect.w/2;
        draw_rect.y=this->pos.y - draw_rect.h/2;

        scale_surf=SDL_CreateRGBSurface(0,draw_rect.w,draw_rect.h,32,0,0,0,0);
        SDL_SetColorKey(scale_surf, SDL_TRUE, SDL_MapRGB(scale_surf->format,0,0xff,0xa1)); 
        SDL_SetSurfaceBlendMode(scale_surf, SDL_BLENDMODE_NONE);
    }
    Effect(const Effect& other){
        done = other.done;
        pos = other.pos;
        loop = other.loop;
        delta_t = other.delta_t;
        ms_per_frame = other.ms_per_frame;
        sprite = other.sprite;
        draw_rect = other.draw_rect;
        scale_surf = scale_surf;
        if(scale_surf != NULL) scale_surf->refcount++;
    }
    ~Effect(){
        free_mem();
    }
        
    void free_mem(){
        if(scale_surf!=NULL){
            if(scale_surf->refcount == 1){
                SDL_FreeSurface(scale_surf);
            }
            else scale_surf->refcount--;
        }
        scale_surf = NULL;
    }
    void update(int delta_ms){
        delta_t += delta_ms;
        if(delta_t >= ms_per_frame){
            delta_t = 0;
            sprite->set_frame(current_frame++);
        }
        if(current_frame > sprite->max_frame){
            if(loop){
                current_frame = 0;
            }
            else{
                done = true;
                current_frame = sprite->max_frame;
            }
        }
    }
    void draw(SDL_Renderer* r, SDL_Window* window){
        SDL_BlitScaled((*sprite)[current_frame],NULL,scale_surf,NULL); 

        SDL_Texture* tex=SDL_CreateTextureFromSurface(r, scale_surf);
        SDL_RenderCopyEx(r, tex, NULL, &draw_rect, 0, NULL, SDL_FLIP_NONE);

        SDL_DestroyTexture(tex);
        SDL_FillRect(scale_surf,NULL,SDL_MapRGB(scale_surf->format,0,0xff,0xa1)); 
    }
private:
    int ms_per_frame, delta_t;
    Sprite* sprite;
    SDL_Rect draw_rect;
    SDL_Surface *scale_surf;
};
#endif
