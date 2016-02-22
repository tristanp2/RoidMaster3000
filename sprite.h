#ifndef SPRITE_H
#define SPRITE_H

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <SDL2/SDL.h>

using namespace std;

class Sprite{
public:
    int max_frame;
    string file_name;

    Sprite(){
        height=0;
        width=0;
        max_frame=0;
        num_rows=0;
        num_cols=0;
        current_frame=0;
    }
    ~Sprite(){
        SDL_FreeSurface(full_surf);
        SDL_FreeSurface(frame_surf); 
    }
    Sprite(string file_name, int max, int cols, int rows){
        load_file(file_name, max, cols, rows);
    }
    bool load_file(string file_name, int max, int cols, int rows){
        full_surf=SDL_LoadBMP(file_name.c_str());
        if(full_surf==NULL){
            cout<<"could not load file: "<<file_name<<"\t"<<SDL_GetError()<<endl;
            return false;
        }
        this->file_name=file_name;
        current_frame=-1;   //To allow the first blit onto the frame_surf
        max_frame=max;
        width=full_surf->w;
        height=full_surf->h;
        frame_rect.x=0;
        frame_rect.y=0;
        frame_rect.w=width/cols;
        frame_rect.h=height/rows;
        frame_width=frame_rect.w;
        frame_height=frame_rect.h;
        frame_surf=SDL_CreateRGBSurface(0,frame_rect.w,frame_rect.h,32,0,0,0,0);
        SDL_SetColorKey(full_surf, SDL_TRUE, SDL_MapRGB(full_surf->format,0,0xff,0xa1)); 
        SDL_SetColorKey(frame_surf, SDL_TRUE, SDL_MapRGB(frame_surf->format,0,0xff,0xa1)); 
        SDL_SetSurfaceBlendMode(full_surf, SDL_BLENDMODE_NONE);
        SDL_SetSurfaceBlendMode(frame_surf, SDL_BLENDMODE_NONE);
        num_cols=cols;
        num_rows=rows;
        return true;
    }
    void free(){
        if(full_surf==NULL){
            cout<<"no valid surface"<<endl;
            return;
        }
        SDL_FreeSurface(full_surf);
    }
    int get_width(){
        return width;
    }
    int get_height(){
        return height;
    }
    int get_frame_width(){
        return frame_width;
    }
    int get_frame_height(){
        return frame_height;
    }
    SDL_Surface* set_frame(int frame){
        if(frame == current_frame and frame_surf!=NULL) return frame_surf;

        SDL_FillRect(frame_surf,NULL,SDL_MapRGB(frame_surf->format,0,0xff,0xa1)); 
        if(frame > max_frame){
            cout<<"invalid frame chosen for: "<<file_name<<endl;
            return NULL;
        }
        frame_rect.x = frame%num_cols * frame_rect.w;
        frame_rect.y = frame/num_cols * frame_rect.h;
        //cout<<"frame rect: "<<frame_rect.x<<"\t"<<frame_rect.y<<endl;
        SDL_BlitSurface(full_surf, &frame_rect, frame_surf, NULL);
        current_frame = frame;
        return frame_surf;
    }      
    SDL_Surface* operator [] (int i){
        return set_frame(i);
    }
    int get_frame(){
        return this->current_frame;
    }
private:
    SDL_Rect frame_rect;
    SDL_Surface* full_surf, *frame_surf;
    int current_frame;
    int height, width;
    int frame_height, frame_width;
    int num_rows, num_cols;
};

#endif
