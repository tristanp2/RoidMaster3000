#include <SDL2/SDL.h>

class Point{
public:
    Point(){
        point.x = 0;
        point.y = 0;
    }
    Point(int x, int y){
        point.x = x;
        point.y = y;
    }
    SDL_Point get_sdl_point(){
        return point;
    }
    int get_x(){
        return point.x;
    }
    int get_y(){
        return point.y;
    }
    int operator[] (int i){
        if(i==0)    return point.x;
        if(i==1)    return point.y;
        return 0;
    }
private:
    SDL_Point point;
};
