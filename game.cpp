#include <iostream>
#include <cmath>
#include <cstdlib>
#include <list>
#include <SDL2/SDL.h>

#include "vector2d.h"
#include "enumeration.h"
#include "colourRGB.h"
#include "gameobject.h"
#include "sprite.h"

using namespace std;

static const int WINDOW_WIDTH=800;
static const int WINDOW_HEIGHT=600;

class GameCanvas{
public:
    static const int CANVAS_WIDTH=800;
    static const int CANVAS_HEIGHT=600;
    GameObject *player,ast;
    list<GameObject> object_list;
    Sprite *alien, *asteroid, *player_surf, *bullet;
    GameCanvas(){
        srand(time(NULL));
        load_resources();
        object_list.push_back(GameObject(enum_player, player_surf, 2, Vector2d(400,300), false, Vector2d(0,0), 180,0, 60, true));
        object_list.front().set_center(14,8);
        object_list.front().make_hitbox(3);
        player=&(object_list.front());
//        ast = GameObject(enum_asteroid,asteroid,3,Vector2d(100,300),true,Vector2d(0,0),0,360,0,false);
//        object_list.push_back(ast);
    }
    ~GameCanvas(){
        object_list.clear(); 
        delete alien;
        delete asteroid;
        delete player_surf;
        delete bullet;
    }
            
    void load_resources(){
        alien = new Sprite("./resources/alien.bmp", 2, 3, 1);
        asteroid = new Sprite("./resources/asteroid.bmp", 0,1,1);
        player_surf = new Sprite("./resources/spaceship.bmp", 2, 3, 1);
        bullet = new Sprite("./resources/bullet.bmp", 1,6,1);
    }
    void show_menu(SDL_Renderer* r, SDL_Window* window){

    }
    void frame_loop(SDL_Renderer* r, SDL_Window* window){
        refire=0;
        respawn=1000;
        firing=false;
        unsigned int last_frame = SDL_GetTicks();
        unsigned int delta_spawn=0;
        unsigned int frame_t=17;
        while(1){
            unsigned int current_frame=SDL_GetTicks();
            unsigned int delta_t=current_frame - last_frame;
      //      cout<<"\t"<<object_list.size()<<endl;
            refire+=delta_t;
            delta_spawn+=delta_t;
            frame_t+=delta_t;
            if(firing and refire>fire_rate) fire_bullet();

            update_objects(delta_t);
            if(frame_t > 16){
                draw_objects(r,window);
                frame_t=0;
            }
            delete_objects();
            check_collisions();
            delta_spawn=spawn_objects(delta_spawn);
            SDL_Event e;
            while(SDL_PollEvent(&e)){
                switch(e.type){
                    case(SDL_QUIT):
                        return;
                    case(SDL_KEYDOWN):
                        handle_key_down(e.key.keysym.sym);
                        break;
                    case(SDL_KEYUP):
                        handle_key_up(e.key.keysym.sym);
                        break;
                    default:
                        break;
                }
            }
            last_frame=current_frame;
        }
    }
    void draw_objects(SDL_Renderer *r, SDL_Window *w){
        SDL_RenderClear(r);
        GameObject obj;
        list<GameObject>::iterator it=object_list.begin();
        ++it; //skip past player. need to draw on top of bullets
        for(; it!=object_list.end(); ++it){
            (*it).draw(r, w);
        }
        it=object_list.begin();
        (*it).draw(r, w);
        SDL_RenderPresent(r);
    }
    void update_objects(int delta_ms){
        for(list<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); ++it){
            (*it).update(delta_ms);
        }
    }
    //Delete the objects that have gone out of the screen
    void delete_objects(){
        for(list<GameObject>::iterator it=object_list.begin(); it!=object_list.end(); ++it){
            if((*it).type!=enum_player and ((*it).pos.x<-150 or (*it).pos.y<-150 or (*it).pos.x>CANVAS_WIDTH + 150 or (*it).pos.y>CANVAS_HEIGHT + 150)){
               (*it).free_mem();
               it=object_list.erase(it);
            }
        }
    }
    int spawn_objects(int dt){
        if(dt>respawn){
            GameObject obj;
            int rv=rand()%40;
            int sidex=rand()%2;
            int x=-100*sidex + 900*abs(sidex-1); //x coord for asteroid spawn. either spawns at -100 or +900 (100 pixels outside of the screen)
            int y=rand()%600;   //y coord for asteroid spawn
            Vector2d pos(x,y);
            Vector2d end_point(300,rand()%400+100); //Get some random point from center x of screen so that the asteroids
                                                    //to set up vector for asteroid direction
            Vector2d dir = end_point - pos;
            dir=dir.unit_vector();
            cout<<"spawning object at: "<<pos<<" going toward "<<end_point<<endl<<"vel: "<<rv*dir<<endl;
            obj=GameObject(enum_asteroid,asteroid,rand()%3 + 1,pos,true,rv*dir,0,rand()%180 - 360,0,false);
            object_list.push_back(obj);
            respawn = 1000 + rand()%4000; //Spawn every 1-5 seconds
            return 0;
        }    
        else return dt;
        return 0;   //Only to stop compiler from complaining
    }
    void check_collisions(){
        list<GameObject>::iterator it1,it2;
        for(it1=object_list.begin(); it1!=object_list.end(); ++it1){
            it2=it1;
            ++it2;
            for(;it2!=object_list.end(); ++it2){
                if((*it1).is_collided(*it2) and (*it1).type!=enum_player and (*it2).type!=enum_player){
                    cout<<"collision between "<<(*it1).type<<" and "<<(*it2).type<<endl;
                    handle_collision(*it1,*it2);
                    (*it1).free_mem();
                    (*it2).free_mem();
                    object_list.erase(it2);
                    it1=object_list.erase(it1);
                    break;
                }
            }
        }
    }
    void handle_collision(GameObject obj1, GameObject obj2){
        int new_scale, num_objects;
        Vector2d old_pos;
        if(obj1.type==enum_asteroid and obj2.type==enum_asteroid){
            if(obj1.scale < obj2.scale){
                new_scale = obj2.scale - 1;   
                old_pos = obj2.pos;
            }
            else{
                new_scale = obj1.scale - 1;
                old_pos = obj1.pos;
            }
            num_objects = obj2.scale + obj1.scale - 1;
        }        
        else if(obj1.type==enum_bullet or obj2.type==enum_bullet){ //At the moment it should be impossible for two bullets to collide
            GameObject other;
            if(obj1.type==enum_bullet){
                other=obj2;
            }
            else{
                other=obj1;
            }
            new_scale = other.scale - 1;
            num_objects = new_scale + 1;
            old_pos = other.pos;
        }

        GameObject new_obj;
        Vector2d new_dir, new_pos;
        double angle_rad;
        int angle_deg;
        for(int i=0; i<num_objects; i++){
            angle_deg = (i*360/num_objects + rand()%45)%360;
            angle_rad = angle_deg*M_PI/180;
            new_dir = Vector2d(cos(angle_rad),sin(angle_rad));
            new_pos = new_scale*30*new_dir + old_pos;
            object_list.push_back(GameObject(enum_asteroid,asteroid,new_scale,new_pos,false,(rand()%121 - 20)*new_dir,angle_deg,rand()%81 - 40,0,false));
        }
    }
        
private:
    int respawn;
    bool firing;
    int refire;
    static const int fire_rate=200;    //ms/bullet
    void handle_key_down(SDL_Keycode key){
        if(key == SDLK_RIGHT){
            player->raccel=360;
        }
        else if(key == SDLK_LEFT){
            player->raccel=-360;
        }
        else if(key == SDLK_SPACE){
            firing=true; 
        }
        else if(key == SDLK_LSHIFT){
            player->animated=true;
            player->accel=60;
        }
    }
    void handle_key_up(SDL_Keycode key){
        if(key == SDLK_LEFT){
            player->raccel=0;
        }
        else if(key == SDLK_RIGHT){
            player->raccel=0;
        }
        else if(key == SDLK_SPACE){
            firing=false;
        }
        else if(key == SDLK_LSHIFT){
            player->accel=0;
            player->animated=false;
            player->set_frame(0);
        }
    }
    //Move to a ship class at some point
    void fire_bullet(){
        GameObject obj(enum_bullet,bullet,2,player->pos + player->direction*50,
                true,350*player->direction,player->rotation,0,100,false);
        object_list.push_back(obj); 
        refire=0;
    }
};

int main(){
    SDL_Window* window=SDL_CreateWindow("Space Game",
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,0);

    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    GameCanvas canvas;
    GameState state=enum_menu;
    while(1){
        switch(state){
            case enum_menu:
                canvas.show_menu(renderer,window);
                break;
            case enum_play:
                canvas.frame_loop(renderer, window);
                break;
            default:
                break;
        }
    }

    return 0;
}
