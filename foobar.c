#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <unistd.h>
#include <math.h>
#include "foobar.h"


//全局变量
SDL_Surface *sprite = NULL;
SDL_Surface *background = NULL;
SDL_Surface *message = NULL;
SDL_Surface *info = NULL;
SDL_Surface *mouse = NULL;
SDL_Surface *screen = NULL;
TTF_Font *font = NULL;
Mix_Music *bgmusic = NULL;   //背景音乐
Mix_Chunk *howling1 = NULL;   //音效
Mix_Chunk *howling2 = NULL;
Mix_Chunk *howling3 = NULL;
Mix_Chunk *howling4 = NULL;
typedef struct {
        int type;
        int x;  //坐标
        int y;  //坐标
        int f;  //frame计数
        int sc; //sprite clip计数
        int active;   //静止标志，0：静止  大于0：动态
    } Sprite;
SDL_Rect clip[4];

void blit_surface(int x, int y, SDL_Surface *source, SDL_Surface *dest,SDL_Rect *clip);
int init(char *a_caption);
SDL_Surface *load_image(char *filename, int is_sprite);
void cleanup();
int load_ttf(char *filename, short font_size);
int load_sound();
int show_sprite(Sprite *sprite, int frames, int active);


int main(int argc,char *argv[])
{
    int bg_x_count = SCREEN_WIDTH/BG_WIDTH + 1;
    int bg_y_count = SCREEN_HEIGHT/BG_HEIGHT + 1;
    int mouse_x = 690;
    int mouse_y = 250;
    Sprite zombie1 = {ZOMBIE_WALK_LEFT,390,250,0,0,1};
    Sprite zombie2 = {ZOMBIE_WALK_LEFT,490,250,0,0,1};
    Sprite zombie4 = {ZOMBIE_WALK_LEFT,690,250,0,0,1};
    clip[ZOMBIE_WALK_LEFT_0].x = 0;
    clip[ZOMBIE_WALK_LEFT_0].y = 95;
    clip[ZOMBIE_WALK_LEFT_0].w = 32;
    clip[ZOMBIE_WALK_LEFT_0].h = 48;
    clip[ZOMBIE_WALK_LEFT_1].x = 32;
    clip[ZOMBIE_WALK_LEFT_1].y = 95;
    clip[ZOMBIE_WALK_LEFT_1].w = 32;
    clip[ZOMBIE_WALK_LEFT_1].h = 48;
    clip[ZOMBIE_WALK_LEFT_2].x = 64;
    clip[ZOMBIE_WALK_LEFT_2].y = 95;
    clip[ZOMBIE_WALK_LEFT_2].w = 32;
    clip[ZOMBIE_WALK_LEFT_2].h = 48;
    clip[ZOMBIE_WALK_LEFT_3].x = 96;
    clip[ZOMBIE_WALK_LEFT_3].y = 95;
    clip[ZOMBIE_WALK_LEFT_3].w = 32;
    clip[ZOMBIE_WALK_LEFT_3].h = 48;
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
    SDL_Color font_color = {255,0,0};
    
    //初始化
    if(init("FOOBAR") == -1) {
        printf("init() failed...\n");
        return 0;
    }  
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
    //加载文字
    if(load_ttf("/home/kqs/Project/SDL1.2/resource/YaHeiConsolas.ttf",40) == -1) {
        printf("load_ttf() failed...\n");
        return -1;
    }
    //加载声音文件
    if(load_sound() == -1) {
        printf("load_sound() failed...\n");
        return -1;
    }
    //播放背景音乐
    Mix_PlayMusic(bgmusic,-1);

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
                        clock_start = 1;
                        clock = SDL_GetTicks();
                        break;
                    case SDLK_DOWN:
                        message =  TTF_RenderUTF8_Solid(font,"DOWN KEY",font_color);
                        presskey = SDLK_DOWN;
                        Mix_PlayChannel(-1,howling2,0);
                        break;
                    case SDLK_LEFT:
                        message =  TTF_RenderUTF8_Solid(font,"LEFT KEY",font_color);
                        presskey = SDLK_LEFT;
                        Mix_PlayChannel(-1,howling3,0);
                        break;
                    case SDLK_RIGHT:
                        message =  TTF_RenderUTF8_Solid(font,"RIGHT KEY",font_color);
                        presskey = SDLK_RIGHT;
                        Mix_PlayChannel(-1,howling4,0);
                        break;
                }
            }
            if(event.type == SDL_KEYUP) {
                presskey = 0;
                message = NULL;
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
                    if(font_color.r == 255) {
                        font_color.r = 0;
                        font_color.g = 255;
                        font_color.b = 0;
                    }else {
                        font_color.r = 255;
                        font_color.g = 0;
                        font_color.b = 0;
                    }            
                }
            }     
        }
        //延迟按UP的音效
        if(clock_start == 1 && SDL_GetTicks() - clock > 2000) {
            Mix_PlayChannel(-1,howling1,0);
            clock_start = 0;
        }
        //图像应用到窗口
        for(int y = 0; y < bg_y_count; y++) {
            for(int x = 0; x < bg_x_count; x++) {
                blit_surface(x*BG_WIDTH,y*BG_HEIGHT,background,screen,NULL);
            }
        }
        //zombie1由键盘控制
        switch(presskey) {
            case SDLK_UP:
                zombie1.y = (zombie1.y <= 0 ? 0:zombie1.y - 1);
                break;
            case SDLK_DOWN:
                zombie1.y = (zombie1.y >= SCREEN_HEIGHT - 48 ? SCREEN_HEIGHT - 48:zombie1.y + 1);
                break;
            case SDLK_LEFT:
                zombie1.type = ZOMBIE_WALK_LEFT;
                zombie1.x = (zombie1.x <= 0 ? 0:zombie1.x - 1);
                break;
            case SDLK_RIGHT:
                zombie1.x = (zombie1.x >= SCREEN_WIDTH - 32 ? SCREEN_WIDTH - 32:zombie1.x + 1);
                break;
        }
        show_sprite(&zombie1,30,presskey);      
        //zombie4 跟随鼠标移动
        if(zombie4.x < mouse_x) {
            zombie4.x++;
        }
        if(zombie4.x > mouse_x) {
            zombie4.x--;
        }
        if(zombie4.y < mouse_y) {
            zombie4.y++;
        }
        if(zombie4.y > mouse_y) {
            zombie4.y--;
        }
        show_sprite(&zombie4,15,zombie4.active);
        if(mouse != NULL) {
            blit_surface(20,20,mouse,screen,NULL);
        }
        if(message != NULL) {
            blit_surface((SCREEN_WIDTH - message->w)/2,(SCREEN_HEIGHT - message->h)/2,message,screen,NULL);
        }
        //显示FPS
        char str_fps[20] = "";
        sprintf(str_fps,"FPS:%d",FPS);
        info = TTF_RenderUTF8_Solid(font,str_fps,font_color);
        blit_surface(SCREEN_WIDTH - 160,SCREEN_HEIGHT - 80,info,screen,NULL);
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
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);
    if(screen == NULL) {
        return -1;
    }
    //屏幕填充黑色
    SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0,0,0));
    //设置窗口标题
    SDL_WM_SetCaption(a_caption,NULL);
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
    SDL_FreeSurface(background);
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
            blit_surface(sp->x,sp->y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_0]);
        }
        if(sp->sc == 1) {
            blit_surface(sp->x,sp->y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_1]);
        }
        if(sp->sc == 2) {
            blit_surface(sp->x,sp->y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_2]);
        }
        if(sp->sc == 3) {
            blit_surface(sp->x,sp->y,sprite,screen,&clip[ZOMBIE_WALK_LEFT_3]);
        }
        if(active != 0) {
            sp->f++;
        }
        if(sp->f >= frames) {
            sp->f = 0;
            sp->sc = (sp->sc + 1 >= 4 ? 0 : sp->sc + 1);
        }
    }
   
}
