#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL2/SDL.h>

#include "sprite.h"
#include "hitbox.h"
#include "enumeration.h"

using namespace std;

class GameObject{
public:
    ObjectType type;
    Vector2d direction;
    Vector2d velocity; //Pixels/Second
    int accel, raccel;    //Pixels/Second/Second
    Vector2d pos, last_pos;   //represents centre of sprite, last_pos holds previous update's pos
    double rotation;
    string file_name;
    double rspeed;     //Degrees/Second
    bool animated;
    int scale;

    GameObject(){
        type=enum_misc;
        scale=1;
        scale_surf=NULL;
    }
    GameObject(const GameObject& other){
        type = other.type;
        direction = other.direction;
        velocity = other.velocity;
        accel = other.accel;
        raccel = other.raccel;
        pos = other.pos;
        last_pos = other.last_pos;
        rotation = other.rotation;
        file_name = other.file_name;
        rspeed = other.rspeed;
        animated = other.animated;
        scale = other.scale;
        sprite = other.sprite;
        hit_box = other.hit_box;
        if(other.center==NULL) center = other.center;
        else{
            center = new SDL_Point;
            center->x = other.center->x;
            center->y = other.center->y;
        }
        frame_dt = other.frame_dt;
        current_frame = other.current_frame;
        ms_per_frame = other.ms_per_frame;
        max_speed = other.max_speed;
        draw_rect = other.draw_rect;
        offset_pos = other.offset_pos;
        skip_first = other.skip_first;
        if(other.scale_surf==NULL) scale_surf=NULL;
        else{  
            scale_surf=other.scale_surf;
            scale_surf->refcount++;
        }

    }
    ~GameObject(){
        free_mem();
        //if(center!=NULL)    delete center;
        cout<<file_name<<" has been destroyed"<<endl;
    }
    void free_mem(){
        if(scale_surf!=NULL){
            if(scale_surf->refcount==1){
               cout<<"refcount is 1. deleting...\n";
               SDL_FreeSurface(scale_surf);
            }
            else scale_surf->refcount--;
        }
        scale_surf = NULL;
    }
    GameObject(ObjectType type, Sprite* sprite, int scale, Vector2d pos, bool animated, Vector2d v, int rotation, double rotv, int pframe, bool skip_first){
        accel=0;
        raccel=0;
        max_speed=360;
        frame_dt=0;
        current_frame=0;
        this->sprite = sprite;
        file_name=sprite->file_name;
        this->type=type;
        this->scale=scale;
        this->skip_first=skip_first;
        draw_rect.w=sprite->get_frame_width() * scale;
        draw_rect.h=sprite->get_frame_height() * scale;
        this->pos=pos;
        last_pos=pos;
        this->animated=animated;
        draw_rect.x=this->pos.x - draw_rect.w/2;
        draw_rect.y=this->pos.y - draw_rect.h/2;
        velocity=v;
        direction= v / v.length();
        this->rotation=rotation;
        this->rspeed=rotv;
        this->ms_per_frame=pframe;
        this->skip_first=skip_first;
        center=NULL;
        SDL_Point p = {(int)pos.x, (int)pos.y};
        hit_box = HitBox(rotation, 4, p, draw_rect.h/2);     //basically just a placeholder hitbox 

        scale_surf=SDL_CreateRGBSurface(0,draw_rect.w,draw_rect.h,32,0,0,0,0);
        scale_surf->refcount++;
        SDL_SetColorKey(scale_surf, SDL_TRUE, SDL_MapRGB(scale_surf->format,0,0xff,0xa1)); 
        SDL_SetSurfaceBlendMode(scale_surf, SDL_BLENDMODE_NONE);
    }
    void make_hitbox(int num_points){
        SDL_Point p = {(int)pos.x, (int)pos.y};

        hit_box = HitBox(rotation, num_points, p, draw_rect.h/2);
    }
    void draw(SDL_Renderer* r, SDL_Window* window){
        SDL_BlitScaled((*sprite)[current_frame],NULL,scale_surf,NULL); 

        SDL_Texture* tex=SDL_CreateTextureFromSurface(r, scale_surf);
        SDL_RenderCopyEx(r, tex, NULL, &draw_rect, rotation, center, SDL_FLIP_NONE);

        SDL_DestroyTexture(tex);
        SDL_FillRect(scale_surf,NULL,SDL_MapRGB(scale_surf->format,0,0xff,0xa1)); 
    }
    void update(int delta_ms){
        double delta_s = delta_ms / 1000.0;
        frame_dt+=delta_ms;
        double dec;
        Vector2d dv;
        last_pos=pos;
        if(abs(rspeed) > max_speed){
            if(rspeed<0)    rspeed = -max_speed;
            else            rspeed = max_speed;
        }
        rspeed += raccel * delta_s;
        rotation += rspeed * delta_s;
        dec = rotation - (int)rotation;
        rotation = (int)rotation%360 + dec;
        direction.x = cos(rotation*M_PI/180);
        direction.y = sin(rotation*M_PI/180);
        hit_box.update(rotation, (int)pos.x, (int)pos.y);
        velocity += accel*delta_s*direction;
        pos += velocity*delta_s;
        draw_rect.x = pos.x - draw_rect.w/2;
        draw_rect.y = pos.y - draw_rect.h/2;

        if(animated) animate();
    }
    void set_frame(int frame){
        current_frame=frame;
    }       
    int get_frame(){
        return current_frame;
    }
    HitBox get_hitbox(){
        return hit_box;
    }
    bool is_collided(GameObject& other){
        HitBox other_box = other.get_hitbox();
        SDL_Point* other_point = other_box.get_points();
        for(int i=0; i<other_box.num_points; i++){
            if(hit_box.is_in_box(other_point[i])){
                delete[] other_point;
                return true;
            }
        }
        delete[] other_point;
        return false;
    }
    //This point is used only for rotations
    //pos is the real center of the sprite drawn on screen
    //NULL tells sdl to use default center
    void set_center(int x, int y){
        if(center==NULL) center = new SDL_Point;
        center->x = x * scale;
        center->y = y * scale;
    }
/*    GameObject operator= (GameObject other){
        GameObject obj(other);
        return obj;
    }
*/
private:
    Sprite* sprite;
    HitBox hit_box;
    SDL_Surface *scale_surf;
    SDL_Point* center;
    int current_frame;    
    int frame_dt;
    int ms_per_frame;
    int max_speed;
    SDL_Rect draw_rect;
    bool offset_pos, skip_first;

    void animate(){
        if(frame_dt<ms_per_frame) return;

        frame_dt=0;
        current_frame++;
        if(current_frame > sprite->max_frame) current_frame = 0;
        if(skip_first and current_frame==0) current_frame=1;
    }
};

#endif
