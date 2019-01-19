#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <unistd.h>
#include <math.h>
#include "foobar.h"


//全局变量
SDL_Surface *sprite = NULL;
SDL_Surface *sprite2 = NULL;
SDL_Surface *sprite3 = NULL;
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *fps_info = NULL;
SDL_Surface *sprite_info1 = NULL;
SDL_Surface *sprite_info4 = NULL;
SDL_Surface *sprite_info2 = NULL;
SDL_Surface *sprite_info3 = NULL;
SDL_Surface *mouse = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *block0 = NULL;
TTF_Font *font = NULL;
Mix_Music *bgmusic = NULL;   //背景音乐
Mix_Chunk *howling1 = NULL;   //音效
Mix_Chunk *howling2 = NULL;
Mix_Chunk *howling3 = NULL;
Mix_Chunk *howling4 = NULL;
typedef struct 
{
    unsigned int left : 1;
    unsigned int right :1;
    unsigned int up: 1;
    unsigned int down: 1;
} Collision;      //用于存储碰撞检测状态
typedef struct 
{
    int type;
    SDL_Rect box;  //存储精灵边框信息
    int f;  //frame计数
    int sc; //sprite clip计数
    int active;   //静止标志，0：静止  大于0：动态
    Collision coll;    //碰撞检测，四个方向
} Sprite;
SDL_Rect clip[48];
SDL_Rect camera;

void blit_surface(int x, int y, SDL_Surface *source, SDL_Surface *dest,SDL_Rect *clip);
int init(char *a_caption);
SDL_Surface *load_image(char *filename, int is_sprite);
void cleanup();
int load_ttf(char *filename, short font_size);
int load_sound();
int show_sprite(Sprite *sprite, int frames, int active);
int show_block(Sprite *block,int frames, int active);
int collision_detect(Sprite *sp,Sprite *block);

int main(int argc,char *argv[])
{
    int bg_x_count = SCREEN_WIDTH/BG_WIDTH + 1;
    int bg_y_count = SCREEN_HEIGHT/BG_HEIGHT + 1;
    int mouse_x = 690;
    int mouse_y = 250;
    Sprite zombie1 = {ZOMBIE_WALK_LEFT,{390,250,SPRITE_WIDTH,SPRITE_HEIGHT},0,0,0,{0,0,0,0}};
    Sprite zombie4 = {FZOMBIE_WALK_LEFT,{690,250,SPRITE_WIDTH,SPRITE_HEIGHT},0,0,0,{0,0,0,0}};
    Sprite human   = {HUMAN_WALK_LEFT,{CAMERA_WIDTH/2 - SPRITE_WIDTH/2 - 1,CAMERA_HEIGHT/2 - SPRITE_HEIGHT/2 - 1,SPRITE_WIDTH,SPRITE_HEIGHT},\
                      0,0,0,{0,0,0,0}};
    Sprite cabinet0 = {CABINET_0,{500,400,256,64},0,0,0,{0,0,0,0}};
    clip[ZOMBIE_WALK_LEFT_0].x = 0; clip[ZOMBIE_WALK_LEFT_0].y = 48; clip[ZOMBIE_WALK_LEFT_0].w = 32; clip[ZOMBIE_WALK_LEFT_0].h = 48;
    clip[ZOMBIE_WALK_LEFT_1].x = 32; clip[ZOMBIE_WALK_LEFT_1].y = 48; clip[ZOMBIE_WALK_LEFT_1].w = 32; clip[ZOMBIE_WALK_LEFT_1].h = 48;
    clip[ZOMBIE_WALK_LEFT_2].x = 64; clip[ZOMBIE_WALK_LEFT_2].y = 48; clip[ZOMBIE_WALK_LEFT_2].w = 32; clip[ZOMBIE_WALK_LEFT_2].h = 48;
    clip[ZOMBIE_WALK_LEFT_3].x = 96; clip[ZOMBIE_WALK_LEFT_3].y = 48; clip[ZOMBIE_WALK_LEFT_3].w = 32; clip[ZOMBIE_WALK_LEFT_3].h = 48;
    clip[ZOMBIE_WALK_RIGHT_0].x = 0; clip[ZOMBIE_WALK_RIGHT_0].y = 96; clip[ZOMBIE_WALK_RIGHT_0].w = 32; clip[ZOMBIE_WALK_RIGHT_0].h = 48;
    clip[ZOMBIE_WALK_RIGHT_1].x = 32; clip[ZOMBIE_WALK_RIGHT_1].y = 96; clip[ZOMBIE_WALK_RIGHT_1].w = 32; clip[ZOMBIE_WALK_RIGHT_1].h = 48;
    clip[ZOMBIE_WALK_RIGHT_2].x = 64; clip[ZOMBIE_WALK_RIGHT_2].y = 96; clip[ZOMBIE_WALK_RIGHT_2].w = 32; clip[ZOMBIE_WALK_RIGHT_2].h = 48;
    clip[ZOMBIE_WALK_RIGHT_3].x = 96; clip[ZOMBIE_WALK_RIGHT_3].y = 96; clip[ZOMBIE_WALK_RIGHT_3].w = 32; clip[ZOMBIE_WALK_RIGHT_3].h = 48;
    clip[ZOMBIE_WALK_UP_0].x = 0; clip[ZOMBIE_WALK_UP_0].y = 144; clip[ZOMBIE_WALK_UP_0].w = 32; clip[ZOMBIE_WALK_UP_0].h = 48;
    clip[ZOMBIE_WALK_UP_1].x = 32; clip[ZOMBIE_WALK_UP_1].y = 144; clip[ZOMBIE_WALK_UP_1].w = 32; clip[ZOMBIE_WALK_UP_1].h = 48;
    clip[ZOMBIE_WALK_UP_2].x = 64; clip[ZOMBIE_WALK_UP_2].y = 144; clip[ZOMBIE_WALK_UP_2].w = 32; clip[ZOMBIE_WALK_UP_2].h = 48;
    clip[ZOMBIE_WALK_UP_3].x = 96; clip[ZOMBIE_WALK_UP_3].y = 144; clip[ZOMBIE_WALK_UP_3].w = 32; clip[ZOMBIE_WALK_UP_3].h = 48;
    clip[ZOMBIE_WALK_DOWN_0].x = 0; clip[ZOMBIE_WALK_DOWN_0].y = 0; clip[ZOMBIE_WALK_DOWN_0].w = 32; clip[ZOMBIE_WALK_DOWN_0].h = 48;
    clip[ZOMBIE_WALK_DOWN_1].x = 32; clip[ZOMBIE_WALK_DOWN_1].y = 0; clip[ZOMBIE_WALK_DOWN_1].w = 32; clip[ZOMBIE_WALK_DOWN_1].h = 48;
    clip[ZOMBIE_WALK_DOWN_2].x = 64; clip[ZOMBIE_WALK_DOWN_2].y = 0; clip[ZOMBIE_WALK_DOWN_2].w = 32; clip[ZOMBIE_WALK_DOWN_2].h = 48;
    clip[ZOMBIE_WALK_DOWN_3].x = 96; clip[ZOMBIE_WALK_DOWN_3].y = 0; clip[ZOMBIE_WALK_DOWN_3].w = 32; clip[ZOMBIE_WALK_DOWN_3].h = 48;
    clip[BLOCK_CABINET_0].x = 0; clip[BLOCK_CABINET_0].y = 192; clip[BLOCK_CABINET_0].w = 256; clip[BLOCK_CABINET_0].h = 64;
    clip[FZOMBIE_WALK_LEFT_0].x = 0; clip[FZOMBIE_WALK_LEFT_0].y = 48; clip[FZOMBIE_WALK_LEFT_0].w = 32; clip[FZOMBIE_WALK_LEFT_0].h = 48;
    clip[FZOMBIE_WALK_LEFT_1].x = 32; clip[FZOMBIE_WALK_LEFT_1].y = 48; clip[FZOMBIE_WALK_LEFT_1].w = 32; clip[FZOMBIE_WALK_LEFT_1].h = 48;
    clip[FZOMBIE_WALK_LEFT_2].x = 64; clip[FZOMBIE_WALK_LEFT_2].y = 48; clip[FZOMBIE_WALK_LEFT_2].w = 32; clip[FZOMBIE_WALK_LEFT_2].h = 48;
    clip[FZOMBIE_WALK_LEFT_3].x = 96; clip[FZOMBIE_WALK_LEFT_3].y = 48; clip[FZOMBIE_WALK_LEFT_3].w = 32; clip[FZOMBIE_WALK_LEFT_3].h = 48;
    clip[FZOMBIE_WALK_RIGHT_0].x = 0; clip[FZOMBIE_WALK_RIGHT_0].y = 96; clip[FZOMBIE_WALK_RIGHT_0].w = 32; clip[FZOMBIE_WALK_RIGHT_0].h = 48;
    clip[FZOMBIE_WALK_RIGHT_1].x = 32; clip[FZOMBIE_WALK_RIGHT_1].y = 96; clip[FZOMBIE_WALK_RIGHT_1].w = 32; clip[FZOMBIE_WALK_RIGHT_1].h = 48;
    clip[FZOMBIE_WALK_RIGHT_2].x = 64; clip[FZOMBIE_WALK_RIGHT_2].y = 96; clip[FZOMBIE_WALK_RIGHT_2].w = 32; clip[FZOMBIE_WALK_RIGHT_2].h = 48;
    clip[FZOMBIE_WALK_RIGHT_3].x = 96; clip[FZOMBIE_WALK_RIGHT_3].y = 96; clip[FZOMBIE_WALK_RIGHT_3].w = 32; clip[FZOMBIE_WALK_RIGHT_3].h = 48;
    clip[FZOMBIE_WALK_UP_0].x = 0; clip[FZOMBIE_WALK_UP_0].y = 144; clip[FZOMBIE_WALK_UP_0].w = 32; clip[FZOMBIE_WALK_UP_0].h = 48;
    clip[FZOMBIE_WALK_UP_1].x = 32; clip[FZOMBIE_WALK_UP_1].y = 144; clip[FZOMBIE_WALK_UP_1].w = 32; clip[FZOMBIE_WALK_UP_1].h = 48;
    clip[FZOMBIE_WALK_UP_2].x = 64; clip[FZOMBIE_WALK_UP_2].y = 144; clip[FZOMBIE_WALK_UP_2].w = 32; clip[FZOMBIE_WALK_UP_2].h = 48;
    clip[FZOMBIE_WALK_UP_3].x = 96; clip[FZOMBIE_WALK_UP_3].y = 144; clip[FZOMBIE_WALK_UP_3].w = 32; clip[FZOMBIE_WALK_UP_3].h = 48;
    clip[FZOMBIE_WALK_DOWN_0].x = 0; clip[FZOMBIE_WALK_DOWN_0].y = 0; clip[FZOMBIE_WALK_DOWN_0].w = 32; clip[FZOMBIE_WALK_DOWN_0].h = 48;
    clip[FZOMBIE_WALK_DOWN_1].x = 32; clip[FZOMBIE_WALK_DOWN_1].y = 0; clip[FZOMBIE_WALK_DOWN_1].w = 32; clip[FZOMBIE_WALK_DOWN_1].h = 48;
    clip[FZOMBIE_WALK_DOWN_2].x = 64; clip[FZOMBIE_WALK_DOWN_2].y = 0; clip[FZOMBIE_WALK_DOWN_2].w = 32; clip[FZOMBIE_WALK_DOWN_2].h = 48;
    clip[FZOMBIE_WALK_DOWN_3].x = 96; clip[FZOMBIE_WALK_DOWN_3].y = 0; clip[FZOMBIE_WALK_DOWN_3].w = 32; clip[FZOMBIE_WALK_DOWN_3].h = 48;
    clip[HUMAN_WALK_LEFT_0].x = 0; clip[HUMAN_WALK_LEFT_0].y = 48; clip[HUMAN_WALK_LEFT_0].w = 32; clip[HUMAN_WALK_LEFT_0].h = 48;
    clip[HUMAN_WALK_LEFT_1].x = 32; clip[HUMAN_WALK_LEFT_1].y = 48; clip[HUMAN_WALK_LEFT_1].w = 32; clip[HUMAN_WALK_LEFT_1].h = 48;
    clip[HUMAN_WALK_LEFT_2].x = 64; clip[HUMAN_WALK_LEFT_2].y = 48; clip[HUMAN_WALK_LEFT_2].w = 32; clip[HUMAN_WALK_LEFT_2].h = 48;
    clip[HUMAN_WALK_LEFT_3].x = 96; clip[HUMAN_WALK_LEFT_3].y = 48; clip[HUMAN_WALK_LEFT_3].w = 32; clip[HUMAN_WALK_LEFT_3].h = 48;
    clip[HUMAN_WALK_RIGHT_0].x = 0; clip[HUMAN_WALK_RIGHT_0].y = 96; clip[HUMAN_WALK_RIGHT_0].w = 32; clip[HUMAN_WALK_RIGHT_0].h = 48;
    clip[HUMAN_WALK_RIGHT_1].x = 32; clip[HUMAN_WALK_RIGHT_1].y = 96; clip[HUMAN_WALK_RIGHT_1].w = 32; clip[HUMAN_WALK_RIGHT_1].h = 48;
    clip[HUMAN_WALK_RIGHT_2].x = 64; clip[HUMAN_WALK_RIGHT_2].y = 96; clip[HUMAN_WALK_RIGHT_2].w = 32; clip[HUMAN_WALK_RIGHT_2].h = 48;
    clip[HUMAN_WALK_RIGHT_3].x = 96; clip[HUMAN_WALK_RIGHT_3].y = 96; clip[HUMAN_WALK_RIGHT_3].w = 32; clip[HUMAN_WALK_RIGHT_3].h = 48;
    clip[HUMAN_WALK_UP_0].x = 0; clip[HUMAN_WALK_UP_0].y = 144; clip[HUMAN_WALK_UP_0].w = 32; clip[HUMAN_WALK_UP_0].h = 48; 
    clip[HUMAN_WALK_UP_1].x = 32; clip[HUMAN_WALK_UP_1].y = 144; clip[HUMAN_WALK_UP_1].w = 32; clip[HUMAN_WALK_UP_1].h = 48; 
    clip[HUMAN_WALK_UP_2].x = 64; clip[HUMAN_WALK_UP_2].y = 144; clip[HUMAN_WALK_UP_2].w = 32; clip[HUMAN_WALK_UP_2].h = 48; 
    clip[HUMAN_WALK_UP_3].x = 96; clip[HUMAN_WALK_UP_3].y = 144; clip[HUMAN_WALK_UP_3].w = 32; clip[HUMAN_WALK_UP_3].h = 48; 
    clip[HUMAN_WALK_DOWN_0].x = 0; clip[HUMAN_WALK_DOWN_0].y = 0; clip[HUMAN_WALK_DOWN_0].w = 32; clip[HUMAN_WALK_DOWN_0].h = 48; 
    clip[HUMAN_WALK_DOWN_1].x = 32; clip[HUMAN_WALK_DOWN_1].y = 0; clip[HUMAN_WALK_DOWN_1].w = 32; clip[HUMAN_WALK_DOWN_1].h = 48;
    clip[HUMAN_WALK_DOWN_2].x = 64; clip[HUMAN_WALK_DOWN_2].y = 0; clip[HUMAN_WALK_DOWN_2].w = 32; clip[HUMAN_WALK_DOWN_2].h = 48;
    clip[HUMAN_WALK_DOWN_3].x = 96; clip[HUMAN_WALK_DOWN_3].y = 0; clip[HUMAN_WALK_DOWN_3].w = 32; clip[HUMAN_WALK_DOWN_3].h = 48;
    camera.x = 0;
    camera.y = 0;
    camera.w = CAMERA_WIDTH;
    camera.h = CAMERA_HEIGHT;
    long start_time = 0;
    long delay_time = 0;
    long frame_time1 = 0;
    long frame_time2 = 0;
    int FPS = 0;
    int frame_count = 0;
    Uint8 *keystate = NULL;
    Uint32 clock = 0;
    int clock_start = 0;
    short quit = 0;
    short presskey = 0;   // 0：未按键  
    SDL_Event event;   
    SDL_Color font_color = {0,0,200};
    
    //初始化
    if(init("FOOBAR") == -1) {
        printf("init() failed...\n");
        return 0;
    }

    //主循环
    frame_time1 = SDL_GetTicks();
    while(quit != 1) {
        start_time = SDL_GetTicks();
        //窗口填充黑色
        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0,0,0));
        //循环_事件捕获和处理
        while(SDL_PollEvent(&event)) {
            //退出
            if(event.type == SDL_QUIT) {
                quit = 1;
            }
            //键盘事件
            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        message =  TTF_RenderUTF8_Solid(font,"UP KEY",font_color);
                        presskey = SDLK_UP;
                        zombie1.type = ZOMBIE_WALK_UP;
                        zombie1.active = 1;
                        clock_start = 1;
                        clock = SDL_GetTicks();
                        break;
                    case SDLK_DOWN:
                        message =  TTF_RenderUTF8_Solid(font,"DOWN KEY",font_color);
                        presskey = SDLK_DOWN;
                        zombie1.active = 1;
                        zombie1.type = ZOMBIE_WALK_DOWN;
                        Mix_PlayChannel(-1,howling2,0);
                        break;
                    case SDLK_LEFT:
                        message =  TTF_RenderUTF8_Solid(font,"LEFT KEY",font_color);
                        presskey = SDLK_LEFT;
                        zombie1.active = 1;
                        zombie1.type = ZOMBIE_WALK_LEFT;
                        Mix_PlayChannel(-1,howling3,0);
                        break;
                    case SDLK_RIGHT:
                        message =  TTF_RenderUTF8_Solid(font,"RIGHT KEY",font_color);
                        presskey = SDLK_RIGHT;
                        zombie1.active = 1;
                        zombie1.type = ZOMBIE_WALK_RIGHT;
                        Mix_PlayChannel(-1,howling4,0);
                        break;
                    case SDLK_a:
                        presskey = SDLK_a;
                        human.active = 1;
                        human.type = HUMAN_WALK_LEFT;
                        break;
                    case SDLK_d:
                        presskey = SDLK_d;
                        human.active = 1;
                        human.type = HUMAN_WALK_RIGHT;
                        break;
                    case SDLK_w:
                        presskey = SDLK_w;
                        human.active = 1;
                        human.type = HUMAN_WALK_UP;
                        break;
                    case SDLK_s:
                        presskey = SDLK_s;
                        human.active = 1;
                        human.type = HUMAN_WALK_DOWN;
                        break;
                }
            }
            if(event.type == SDL_KEYUP) {
                presskey = 0;
                message = NULL;
                zombie1.active = 0;
                human.active = 0;
            }
            //鼠标事件
            if(event.type == SDL_MOUSEMOTION) {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                char mouse_coord[20] = "";
                sprintf(mouse_coord,"mouse:(%d,%d)",mouse_x,mouse_y);
                mouse = TTF_RenderUTF8_Solid(font,mouse_coord,font_color);
            }
            if(event.type == SDL_MOUSEBUTTONUP) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                    if(font_color.b == 200) {
                        font_color.r = 0;
                        font_color.g = 255;
                        font_color.b = 0;
                    }else {
                        font_color.r = 0;
                        font_color.g = 0;
                        font_color.b = 200;
                    }            
                }
            }     
        }
        //延迟按UP的音效
        if(clock_start == 1 && SDL_GetTicks() - clock > 2000) {
            Mix_PlayChannel(-1,howling1,0);
            clock_start = 0;
        }
        //zombie1由键盘控制
        switch(presskey) {
            case SDLK_UP:
                if(zombie1.coll.up != 1) {
                    zombie1.box.y = (zombie1.box.y <= 0 ? 0:zombie1.box.y - 1);
                }
                break;
            case SDLK_DOWN:
                if(zombie1.coll.down != 1) {
                    zombie1.box.y = (zombie1.box.y >= SCREEN_HEIGHT - 48 ? SCREEN_HEIGHT - 48:zombie1.box.y + 1);
                }          
                break;
            case SDLK_LEFT:
                if(zombie1.coll.left != 1) {
                    zombie1.box.x = (zombie1.box.x <= 0 ? 0:zombie1.box.x - 1);
                }
                break;
            case SDLK_RIGHT:
                if(zombie1.coll.right != 1) {
                    zombie1.box.x = (zombie1.box.x >= SCREEN_WIDTH - 32 ? SCREEN_WIDTH - 32:zombie1.box.x + 1);
                }         
                break;
            //camera控制
            case SDLK_a:
                if(human.coll.left != 1) {
                    if(human.box.x <= camera.w/2 - SPRITE_WIDTH/2 - 1) {
                        camera.x = (camera.x - CAMERA_SPEED < 0 ? 0 : camera.x - CAMERA_SPEED);
                    } 
                    human.box.x = (camera.x == 0 ? (human.box.x - CAMERA_SPEED >= 0 ? human.box.x - CAMERA_SPEED : 0) \
                                   : (camera.x >= SCREEN_WIDTH -camera.w - 2 ? (human.box.x - CAMERA_SPEED) : camera.w/2 - SPRITE_WIDTH/2 - 1));
                }            
                break;
            case SDLK_d:
                if(human.coll.right != 1) {
                    if(human.box.x >= camera.w/2 - SPRITE_WIDTH/2 - 1) {
                        camera.x = (camera.x + camera.w + CAMERA_SPEED >= SCREEN_WIDTH ? camera.x : camera.x + CAMERA_SPEED);
                    }                   
                    human.box.x = (camera.x >= SCREEN_WIDTH - camera.w - 2 ?  \
                          (human.box.x + CAMERA_SPEED <= camera.w - human.box.w - 1 ? human.box.x + CAMERA_SPEED : camera.w - human.box.w - 1) \
                                   : (camera.x == 0 ? human.box.x + CAMERA_SPEED : camera.w/2 - SPRITE_WIDTH/2 - 1));
                }        
                break;
            case SDLK_w:
                if(human.coll.up != 1) {
                    if(human.box.y <= camera.h/2 - SPRITE_HEIGHT/2 - 1) {
                        camera.y = (camera.y - CAMERA_SPEED < 0 ? 0 : camera.y - CAMERA_SPEED);
                    }
                    human.box.y = (camera.y == 0 ? (human.box.y - CAMERA_SPEED >= 0 ? human.box.y - CAMERA_SPEED : 0) \
                                  :(camera.y >= SCREEN_HEIGHT - camera.h - 2 ? (human.box.y - CAMERA_SPEED) : camera.h/2 - SPRITE_HEIGHT/2 - 1));
                }
                break;
            case SDLK_s:
                if(human.coll.down != 1) {
                    if(human.box.y >= camera.h/2 - SPRITE_HEIGHT/2 - 1) {
                        camera.y = (camera.y + camera.h + CAMERA_SPEED >= SCREEN_HEIGHT ? camera.y : camera.y + CAMERA_SPEED);
                    }                  
                    human.box.y = (camera.y >= SCREEN_HEIGHT - camera.h - 2 ? \
                         (human.box.y + CAMERA_SPEED <= camera.h - human.box.h - 1 ? human.box.y + CAMERA_SPEED : camera.h - human.box.h - 1) \
                            : (camera.y == 0 ? human.box.y + CAMERA_SPEED : camera.h/2 - SPRITE_HEIGHT/2 - 1 ));
                }
                break;
        }    
        //zombie4 跟随鼠标移动
        if(zombie4.box.x < mouse_x + camera.x && zombie4.coll.right != 1) {
            zombie4.box.x++; 
        }
        if(zombie4.box.x > mouse_x + camera.x && zombie4.coll.left != 1) {
            zombie4.box.x--;   
        }
        if(zombie4.box.y < mouse_y + camera.y && zombie4.coll.down != 1) {
            zombie4.box.y++;
        }
        if(zombie4.box.y > mouse_y + camera.y && zombie4.coll.up != 1) {
            zombie4.box.y--;
        }
        //zombie4动作判断
        if(zombie4.box.x < mouse_x + camera.x && mouse_x + camera.x - zombie4.box.x >= abs(zombie4.box.y - mouse_y - camera.y)) {
            zombie4.type = ZOMBIE_WALK_RIGHT;
        }
        if(zombie4.box.x > mouse_x  + camera.x && zombie4.box.x - mouse_x - camera.x >= abs(zombie4.box.y - mouse_y - camera.y)) {
            zombie4.type = ZOMBIE_WALK_LEFT;
        }
        if(zombie4.box.y > mouse_y + camera.y && zombie4.box.y - mouse_y - camera.y >= abs(zombie4.box.x - mouse_x - camera.x)) {
            zombie4.type = ZOMBIE_WALK_UP;
        }
        if(zombie4.box.y < mouse_y + camera.y && mouse_y + camera.y - zombie4.box.y >= abs(zombie4.box.x - mouse_x - camera.x)) {
            zombie4.type = ZOMBIE_WALK_DOWN;
        }
        if(zombie4.box.x == mouse_x + camera.x && zombie4.box.y == mouse_y + camera.y) {
            zombie4.active = 0;
        }else {
            zombie4.active = 1;
        }
        //绘制background
        for(int y = 0; y < bg_y_count; y++) {
            for(int x = 0; x < bg_x_count; x++) {
                blit_surface(x*BG_WIDTH-camera.x,y*BG_HEIGHT-camera.y,background,screen,NULL);
            }
        }
        //blit_surface((-1)*camera.x,(-1)*camera.y,background,screen,0);
        //绘制精灵
        show_sprite(&zombie1,30,zombie1.active);   
        show_sprite(&zombie4,15,zombie4.active);
        show_sprite(&human,20,human.active);
        if(mouse != NULL) {
            blit_surface(20,20,mouse,screen,NULL);
        }
        if(message != NULL) {
            //blit_surface((SCREEN_WIDTH - message->w)/2,(SCREEN_HEIGHT - message->h)/2,message,screen,NULL);
        }
        //绘制block
        show_block(&cabinet0,FRAMERATE,cabinet0.active);
        //碰撞检测
        collision_detect(&zombie1,&cabinet0);
        collision_detect(&zombie4,&cabinet0);
        collision_detect(&human,&cabinet0);
        //显示精灵状态
        char str_zombie1[100] = "";
        char str_zombie4[100] = "";
        char str_human[100] = "";
        char str_camera[100] = "";
        sprintf(str_zombie1,"zombie1-->(x:%d y:%d)  left:%d right:%d up:%d down:%d" \
                ,zombie1.box.x,zombie1.box.y,zombie1.coll.left,zombie1.coll.right,zombie1.coll.up,zombie1.coll.down);
        sprintf(str_zombie4,"zombie4-->(x:%d y:%d)  left:%d right:%d up:%d down:%d" \
                ,zombie4.box.x,zombie4.box.y,zombie4.coll.left,zombie4.coll.right,zombie4.coll.up,zombie4.coll.down);
        sprintf(str_human,"human-->(x:%d y:%d)  left:%d right:%d up:%d down:%d" \
                ,human.box.x,human.box.y,human.coll.left,human.coll.right,human.coll.up,human.coll.down);
        sprintf(str_camera,"camera-->(x:%d y:%d)",camera.x,camera.y);
        sprite_info1 = TTF_RenderText_Solid(font,str_zombie1,font_color);
        sprite_info4 = TTF_RenderText_Solid(font,str_zombie4,font_color);
        sprite_info2 = TTF_RenderText_Solid(font,str_human,font_color);
        sprite_info3 = TTF_RenderText_Solid(font,str_camera,font_color);
        blit_surface(20,50,sprite_info1,screen,NULL);
        blit_surface(20,80,sprite_info4,screen,NULL);
        blit_surface(20,110,sprite_info2,screen,NULL);
        blit_surface(20,140,sprite_info3,screen,NULL);
        //显示FPS
        char str_fps[20] = "";
        sprintf(str_fps,"FPS:%d",FPS);
        fps_info = TTF_RenderUTF8_Solid(font,str_fps,font_color);
        blit_surface(SCREEN_WIDTH - 100,SCREEN_HEIGHT - 50,fps_info,screen,NULL);
        //更新窗口
        SDL_Flip(screen);
        frame_count++;
        //控制帧率
        delay_time = (long)(1000/FRAMERATE - (SDL_GetTicks() - start_time));
        if(delay_time > 0) {
            SDL_Delay(delay_time);
        }
        //计算FPS
        frame_time2 = SDL_GetTicks();
        FPS = (int)frame_count*1000.00/(frame_time2 - frame_time1);
    }
    //暂停2s
    cleanup();
    return 0;
}

int init(char *a_caption) 
{
     //初始化SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        return -1;
    }
    //初始化TTF
    if(TTF_Init() == -1) {
        return -1;
    }
    //初始化MIXER
    if(Mix_OpenAudio(22050,MIX_DEFAULT_FORMAT,2,4096) == -1) {
        return -1;
    }
    //设置窗口
    screen = SDL_SetVideoMode(CAMERA_WIDTH,CAMERA_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);
    if(screen == NULL) {
        return -1;
    }
    //屏幕填充黑色
    SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0,0,0));
    //设置窗口标题
    SDL_WM_SetCaption(a_caption,NULL);
    //加载图片
    background = load_image("/home/kqs/Project/SDL1.2/resource/bg2.jpg",0);
    if(background == NULL) {
        printf("load_image() failed...\n");
        return 0;
    }
    sprite = load_image("/home/kqs/Project/SDL1.2/resource/zombie.png",1);
    if(sprite == NULL) {
        printf("load_image() failed...\n");
        return 0;
    }
    sprite2 = load_image("/home/kqs/Project/SDL1.2/resource/zombie2.png",1);
    if(sprite == NULL) {
        printf("load_image() failed...\n");
        return 0;
    }
    sprite3 = load_image("/home/kqs/Project/SDL1.2/resource/human.png",1);
    if(sprite == NULL) {
        printf("load_image() failed...\n");
        return 0;
    }
    block0 = load_image("/home/kqs/Project/SDL1.2/resource/material.png",0);
    if(block0 == NULL) {
        printf("load_image() failed...\n");
        return 0;
    }   
    //加载文字
    if(load_ttf("/home/kqs/Project/SDL1.2/resource/YaHeiConsolas.ttf",20) == -1) {
        printf("load_ttf() failed...\n");
        return -1;
    }
    //加载声音文件
    if(load_sound() == -1) {
        printf("load_sound() failed...\n");
        return -1;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME/3);   //调整music音量
    //播放背景音乐
    Mix_PlayMusic(bgmusic,-1);
    return 0;
}

SDL_Surface *load_image(char *filename,int is_sprite) 
{
    SDL_Surface *image = NULL;
    SDL_Surface *refine_image = NULL;
    image = IMG_Load(filename);
    if(image != NULL) {
        refine_image = SDL_DisplayFormat(image);
        SDL_FreeSurface(image);
    }
    if(is_sprite == 1) {
        Uint32 colorkey = SDL_MapRGB(refine_image->format,0xFF,0xFF,0xFF);
        SDL_SetColorKey(refine_image,SDL_SRCCOLORKEY,colorkey);
    }
    
    return refine_image;
}

int load_ttf(char *filename,short font_size)
{
    font = TTF_OpenFont(filename,font_size);
    if(font == NULL) {
        return -1;
    }
    return 0;
}

int load_sound()
{
    bgmusic = Mix_LoadMUS("/home/kqs/Project/SDL1.2/resource/bgmusic.mp3");
    howling1 = Mix_LoadWAV("/home/kqs/Project/SDL1.2/resource/howling1.wav");
    howling2 = Mix_LoadWAV("/home/kqs/Project/SDL1.2/resource/howling2.wav");
    howling3 = Mix_LoadWAV("/home/kqs/Project/SDL1.2/resource/howling3.wav");
    howling4 = Mix_LoadWAV("/home/kqs/Project/SDL1.2/resource/howling4.wav");
    if(bgmusic == NULL || howling1 == NULL || howling2 == NULL || howling3 == NULL || howling4 == NULL ) {
        return -1;
    }
    return 0;
}

void blit_surface(int x, int y, SDL_Surface *source, SDL_Surface *dest,SDL_Rect *clip)
{
    //创建矩形
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface(source,clip,dest,&offset);
}

void cleanup()
{
    //释放已加载图像
    SDL_FreeSurface(sprite);
    SDL_FreeSurface(sprite2);
    SDL_FreeSurface(sprite3);
    SDL_FreeSurface(background);
    SDL_FreeSurface(fps_info);
    SDL_FreeSurface(message);
    SDL_FreeSurface(sprite_info1);
    SDL_FreeSurface(sprite_info4);
    SDL_FreeSurface(mouse);
    TTF_CloseFont(font);
    TTF_Quit();
    Mix_FreeMusic(bgmusic);
    Mix_FreeChunk(howling1);
    Mix_FreeChunk(howling2);
    Mix_FreeChunk(howling3);
    Mix_FreeChunk(howling4);
    Mix_CloseAudio();
    //退出
    SDL_Quit();
}

int show_sprite(Sprite *sp,int frames, int active)
{
    if(sp->type == ZOMBIE_WALK_LEFT) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_3]);
        }
    }
    if(sp->type == ZOMBIE_WALK_RIGHT) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_RIGHT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_RIGHT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_RIGHT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_RIGHT_3]);
        }
    }
    if(sp->type == ZOMBIE_WALK_UP) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_UP_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_UP_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_UP_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_UP_3]);
        }
    }
    if(sp->type == ZOMBIE_WALK_DOWN) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_DOWN_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_DOWN_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_DOWN_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite,screen,&clip[ZOMBIE_WALK_DOWN_3]);
        }
    }
    if(sp->type == FZOMBIE_WALK_LEFT) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_LEFT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_LEFT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_LEFT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_LEFT_3]);
        }
    }
    if(sp->type == FZOMBIE_WALK_RIGHT) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_RIGHT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_RIGHT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_RIGHT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_RIGHT_3]);
        }
    }
    if(sp->type == FZOMBIE_WALK_UP) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_UP_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_UP_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_UP_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_UP_3]);
        }
    }
    if(sp->type == FZOMBIE_WALK_DOWN) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_DOWN_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_DOWN_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_DOWN_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x - camera.x,sp->box.y - camera.y,sprite2,screen,&clip[FZOMBIE_WALK_DOWN_3]);
        }
    }
    if(sp->type == HUMAN_WALK_LEFT) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_LEFT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_LEFT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_LEFT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_LEFT_3]);
        }
    }
    if(sp->type == HUMAN_WALK_RIGHT) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_RIGHT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_RIGHT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_RIGHT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_RIGHT_3]);
        }
    }
    if(sp->type == HUMAN_WALK_UP) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_UP_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_UP_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_UP_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_UP_3]);
        }
    }
    if(sp->type == HUMAN_WALK_DOWN) {
        if(sp->sc == 0) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_DOWN_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_DOWN_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_DOWN_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->box.x,sp->box.y,sprite3,screen,&clip[HUMAN_WALK_DOWN_3]);
        }
    }
        //精灵是否活动控制
        if(active != 0) {
            sp->f++;
        }
        if(sp->f >= frames) {
            sp->f = 0;
            sp->sc = (sp->sc + 1 >= 4 ? 0 : sp->sc + 1);
        }
    return 0;
}

int show_block(Sprite *block,int frames, int active)
{
    if(block->type == CABINET_0) {
        blit_surface(block->box.x - camera.x,block->box.y - camera.y,block0,screen,&clip[BLOCK_CABINET_0]);
    }
    //block是否活动控制
        if(active != 0) {
            block->f++;
        }
        if(block->f >= frames) {
            block->f = 0;
            block->sc = (block->sc + 1 >= 4 ? 0 : block->sc + 1);
        }
    return 0;
}

int collision_detect(Sprite *sp,Sprite *block) 
{
    int sp_left = sp->box.x;
    int sp_right = sp->box.x + sp->box.w;
    int sp_up = sp->box.y;
    int sp_down = sp->box.y + sp->box.h;
    int block_left = block->box.x;
    int block_right = block->box.x + block->box.w;
    int block_up = block->box.y;
    int block_down = block->box.y + block->box.h;
    //printf("sp_l=%d sp_r=%d sp_u=%d sp_d=%d bl_l=%d bl_r=%d bl_u=%d bl_d=%d\n",\
    //        sp_left,sp_right,sp_up,sp_down,block_left,block_right,block_up,block_down);
    if(sp_left > block_right || sp_right < block_left || sp_up > block_down || sp_down < block_up) {
        sp->coll.left = 0;
        sp->coll.right = 0;
        sp->coll.up = 0;
        sp->coll.down = 0;
    }else {
        if(sp_left == block_right) {
            sp->coll.left = 1;
        }
        if(sp_right == block_left) {
            sp->coll.right = 1;
        }
        if(sp_up == block_down) {
            sp->coll.up = 1;
        }
        if(sp_down == block_up) {
            sp->coll.down = 1;
        }
        /*if(sp_left >= block_left && sp_left <= block_right) {
            sp->coll.left = 1;
        }
        if(sp_right >= block_left && sp_right <= block_right) {
            sp->coll.right = 1;
        }
        if(sp_up <= block_down && sp_up >= block_up) {
            sp->coll.up = 1;
        }
        if(sp_down <= block_down && sp_down >= block_up) {
            sp->coll.down = 1;
        }*/
    }
    return 0;
}
