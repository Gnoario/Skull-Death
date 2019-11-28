

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>

#define MAX_BULLETS 1000

typedef struct
{
  float x, y, dy;
  short life;
  char *name;
  int currentSprite, walking, facingLeft, shooting, visible;
  int alive;
  
  SDL_Texture *sheetTexture;
} Man;

typedef struct
{
  float x, y, dx;
} Bullet;

SDL_Texture *bulletTexture;
SDL_Texture *backgroundTexture;
Bullet *bullets[MAX_BULLETS] = { NULL };
Man enemy;

int globalTime = 0;

void addBullet(float x, float y, float dx)
{
  int found = -1;
  for(int i = 0; i < MAX_BULLETS; i++)
  {
    if(bullets[i] == NULL)
    {
      found = i;
      break;
    }
  }
    
  if(found >= 0)
  {
    int i = found;
    bullets[i] = malloc(sizeof(Bullet));
    bullets[i]->x = x;
    bullets[i]->y = y;
    bullets[i]->dx = dx;    
  }
}

void removeBullet(int i)
{
  if(bullets[i])
  {
    free(bullets[i]);
    bullets[i] = NULL;
  }
}

int processEvents(SDL_Window *window, Man *man)
{
  SDL_Event event;
  int done = 0;

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_WINDOWEVENT_CLOSE:
      {
        if(window)
        {
          SDL_DestroyWindow(window);
          window = NULL;
          done = 1;
        }
      }
      break;
      case SDL_KEYDOWN:
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            done = 1;
          break;
        }
      }
      break;
      case SDL_QUIT:
        done = 1;
      break;
    }
  }
  
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if(!man->shooting)
  {
    if(state[SDL_SCANCODE_LEFT])
    {
      man->x -= 3;
      man->walking = 1;
      man->facingLeft = 1;
    
      if(globalTime % 6 == 0)
      {
        man->currentSprite++;
        man->currentSprite %= 4;  
      }  
    }
    else if(state[SDL_SCANCODE_RIGHT])
    {
      man->x += 3;
      man->walking = 1;
      man->facingLeft = 0;
     
      if(globalTime % 6 == 0)
      {
        man->currentSprite++;
        man->currentSprite %= 4;  
      }  
    }
    else
    {
      man->walking = 0;
      man->currentSprite = 4;
    }
  }

  if(!man->walking)
  {
    if(state[SDL_SCANCODE_SPACE])
    {
      if(globalTime % 6 == 0)
      {
        if(man->currentSprite == 4)         
          man->currentSprite = 5;
        else
          man->currentSprite = 4;
          
        if(!man->facingLeft)
        {
          addBullet(man->x+35, man->y+20, 3); 
        }
        else
        {
          addBullet(man->x+5, man->y+20, -3);         
        }
      }  
  
      man->shooting = 1;
    }
    else 
    {
      man->currentSprite = 4;  
      man->shooting = 0;
    }
  }
  
  if(state[SDL_SCANCODE_UP] && !man->dy)
  {
    man->dy = -8;
  }
  if(state[SDL_SCANCODE_KP_6])
  {
      enemy.x += 3;
      enemy.walking = 1;
      enemy.facingLeft = 0;
      if(globalTime % 6 == 0)
      {
        enemy.currentSprite++;
        enemy.currentSprite %= 4;  
      } 
  }
  if(state[SDL_SCANCODE_KP_4]){
      enemy.x -= 3;
      enemy.walking = 1;
      enemy.facingLeft = 1;
    
      if(globalTime % 6 == 0)
      {
        enemy.currentSprite++;
        enemy.currentSprite %= 4;  
      }  
  }

  if(state[SDL_SCANCODE_KP_8] && !enemy.dy)
  {
    enemy.dy = -8;
  }

  if(!enemy.walking)
  {
    if(state[SDL_SCANCODE_KP_5])
    {
      if(globalTime % 6 == 0)
      {
        if(enemy.currentSprite == 4)         
          enemy.currentSprite = 5;
        else
          enemy.currentSprite = 4;
          
        if(enemy.facingLeft)
        {
          addBullet(enemy.x+35, enemy.y+20, 3); 
        }
        else
        {
          addBullet(enemy.x+5, enemy.y+20, -3);         
        }
      }  
  
      enemy.shooting = 1;
    }
    else 
    {
      enemy.currentSprite = 4;  
      enemy.shooting = 0;
    }
  }
  
  return done;
}

void doRender(SDL_Renderer *renderer, Man *man)
{
  
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  
  
  SDL_RenderClear(renderer);
  
  
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  
  
  SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

  
  if(man->visible)
  {
    SDL_Rect srcRect = { 40*man->currentSprite, 0, 40, 50 };  
    SDL_Rect rect = { man->x, man->y, 60, 70 };  
    SDL_RenderCopyEx(renderer, man->sheetTexture, &srcRect, &rect, 0, NULL, man->facingLeft);
  }

  
  if(enemy.visible)
  {
    SDL_Rect eSrcRect = { 40*enemy.currentSprite, 0, 40, 50 };  
    SDL_Rect eRect = { enemy.x, enemy.y, 60, 70 };  
    SDL_RenderCopyEx(renderer, enemy.sheetTexture, &eSrcRect, &eRect, 0, NULL, enemy.facingLeft);
  }

  for(int i = 0; i < MAX_BULLETS; i++) if(bullets[i])
  {
    SDL_Rect rect = { bullets[i]->x, bullets[i]->y, 8, 8 };  
    SDL_RenderCopy(renderer, bulletTexture, NULL, &rect);
  }
  
  
  SDL_RenderPresent(renderer);
}

void updateLogic(Man *man)
{
  man->y += man->dy;
  man->dy += 0.5;
  if(man->y > 592)
  {
    man->y = 592;
    man->dy = 0;
  }
  
  for(int i = 0; i < MAX_BULLETS; i++) if(bullets[i])
  {
    bullets[i]->x += bullets[i]->dx;
    
    
    if(bullets[i]->x > enemy.x && bullets[i]->x < enemy.x+40 && 
       bullets[i]->y > enemy.y && bullets[i]->y < enemy.y+50)
    {
      enemy.alive = 0;
    } 
    
    if(bullets[i]->x < -1000 || bullets[i]->x > 1000)
      removeBullet(i);
  }
  
  if(enemy.alive == 0 && globalTime % 6 == 0)
  {
    if(enemy.currentSprite < 6)
      enemy.currentSprite = 6;
    else if(enemy.currentSprite >= 6)
    {
      enemy.currentSprite++;
      if(enemy.currentSprite > 7)
      {
        enemy.visible = 0;
        enemy.currentSprite = 7;      
      }
    }
  }
  
  globalTime++;
}

int main(int argc, char *argv[])
{
  SDL_Window *window;                   
  SDL_Renderer *renderer;                
  
  SDL_Init(SDL_INIT_VIDEO);              

  Man man;
  man.x = 50;
  man.y = 0;
  man.currentSprite = 4;  
  man.alive = 1;
  man.visible = 1;
  man.facingLeft = 0;
  
  enemy.x = 900;
  enemy.y = 592;
  enemy.currentSprite = 4;
  enemy.facingLeft = 1;  
  enemy.alive = 1;
  enemy.visible = 1;
  
  window = SDL_CreateWindow("\t\t\t\t\t\t\t\t\t\tSkull-Death",                    
                            SDL_WINDOWPOS_UNDEFINED,           
                            SDL_WINDOWPOS_UNDEFINED,
                            900,                               
                            600,                               
                            0                                 
                            );
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_RenderSetLogicalSize(renderer, 1080, 720);

  
  SDL_Surface *sheet = IMG_Load("ninja.png");
  if(!sheet)
  {
    printf("Não foi possível encontrar a imagem\n");
    return 1;
  }
  
  man.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);  
  SDL_FreeSurface(sheet);
  
 
  sheet = IMG_Load("zomb.png");
  if(!sheet)
  {
    printf("Não foi possível encontrar a imagem\n");
    return 1;
  }
  
  enemy.sheetTexture = SDL_CreateTextureFromSurface(renderer, sheet);  
  SDL_FreeSurface(sheet);  

  
  SDL_Surface *bg = IMG_Load("background5.png");
  
  if(!sheet)
  {
    printf("Não foi possível encontrar a imagem\n");
    return 1;
  }

  backgroundTexture = SDL_CreateTextureFromSurface(renderer, bg);
  SDL_FreeSurface(bg);

  //load the bullet  
  SDL_Surface *bullet = IMG_Load("balaBoa.png");
  
  if(!bullet)
  {
    printf("Não foi possível encontrar a imagem\n");
    return 1;
  }

  bulletTexture = SDL_CreateTextureFromSurface(renderer, bullet);
  SDL_FreeSurface(bullet);
  
  
  int done = 0;
  
 
  while(!done)
  {
    
    done = processEvents(window, &man);
    
    
    updateLogic(&man);

    updateLogic(&enemy);
    
    
    doRender(renderer, &man);
    
    
    SDL_Delay(10);
  }
  
  
  
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(man.sheetTexture);
  SDL_DestroyTexture(backgroundTexture);
  SDL_DestroyTexture(bulletTexture);
  SDL_DestroyTexture(enemy.sheetTexture);
  
  for(int i = 0; i < MAX_BULLETS; i++)
    removeBullet(i);
  
  
  SDL_Quit();
  return 0;
}


