#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

#define WIDTH 600
#define HEIGHT 600
#define PADDLE_WIDTH  10
#define PADDLE_HEIGHT 40
#define PADDLE_SPEED 20
#define BALL_SIZE 10
#define BALL_SPEED 5

void init_rect(SDL_Rect *rect, int x, int y, int w, int h) {
  rect->x = x;
  rect->y = y;
  rect->w = w;
  rect->h = h;
}

void ball_move(SDL_Rect *ball,  int *ballVx, int *ballVy) {
  ball->x += *ballVx;
  ball->y += *ballVy;
  if(ball->y >= HEIGHT - BALL_SIZE) {
    *ballVy *= -1;
  }
  if(ball->y <= 0) {
    *ballVy *= -1;
  }
}

void check_goal(SDL_Rect *ball, int *player1_score, int *player2_score) {
  if(ball->x > HEIGHT - BALL_SIZE) {
    *player1_score += 1;
    ball->x = WIDTH/2 - BALL_SIZE/2;
    ball->y = HEIGHT/2 - BALL_SIZE;
  }
  if(ball->x < 0) {
    *player2_score += 1;
    ball->x = WIDTH/2 - BALL_SIZE/2;
    ball->y = HEIGHT/2 - BALL_SIZE;
  }
}

void player_move(SDL_Rect *player, int player_move) {
  if(player_move == 1) {
    if(player->y <= 0) {}
    else {
      player->y -= PADDLE_SPEED;
    }
  }
  if(player_move == -1) {
    if(player->y >= HEIGHT - PADDLE_HEIGHT) {}
    else {
      player->y += PADDLE_SPEED;
    }
  }
}

int main() {
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  int running = 1;

  SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();

  window = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_UNDEFINED, 
                            SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, 0);

  srand(time(0));
  TTF_Font* Noto = TTF_OpenFont("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf", 24);

  SDL_Color White = {255, 255, 255};

  const double MAXBOUNCEANGLE = 5*M_PI/12;

  SDL_Rect player1;
  SDL_Rect score1_rect;
  int player1_move = 0;
  int player1_score = 0;
  score1_rect.x = 0;
  score1_rect.y = 0;
  score1_rect.w = 80;
  score1_rect.h = 80;

  init_rect(&player1, 0, HEIGHT/2-PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT);

  SDL_Rect player2;
  SDL_Rect score2_rect;
  int player2_move = 0;
  int player2_score = 0;
  score2_rect.x = 520;
  score2_rect.y = 520;
  score2_rect.w = 80;
  score2_rect.h = 80;

  init_rect(&player2, WIDTH-PADDLE_WIDTH, HEIGHT/2-PADDLE_HEIGHT/2, PADDLE_WIDTH, PADDLE_HEIGHT);

  SDL_Rect ball;
  init_rect(&ball, WIDTH/2 - BALL_SIZE/2, HEIGHT/2 - BALL_SIZE/2, BALL_SIZE, BALL_SIZE);


  int ballVx = BALL_SPEED;
  int ballVy = 0;

  while(running) {
    SDL_Event event;
    const unsigned char *keyboard_state_array = SDL_GetKeyboardState(NULL);

    while(SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
        break;
      }

      if(event.type == SDL_KEYDOWN) {
        if(keyboard_state_array[SDL_SCANCODE_UP]) {
          player2_move = 1;
        }
        if(keyboard_state_array[SDL_SCANCODE_DOWN]) {
          player2_move = -1;
        }
        if(keyboard_state_array[SDL_SCANCODE_W]) {
          player1_move = 1;
        }
        if(keyboard_state_array[SDL_SCANCODE_S]) {
          player1_move = -1;
        }
      }

      if(event.type == SDL_KEYUP) {
        if(!keyboard_state_array[SDL_SCANCODE_UP]) {
          player2_move = 0;
        }
        if(!keyboard_state_array[SDL_SCANCODE_DOWN]) {
          player2_move = 0;
        }
        if(!keyboard_state_array[SDL_SCANCODE_W]) {
          player1_move = 0;
        }
        if(!keyboard_state_array[SDL_SCANCODE_S]) {
          player1_move = 0;
        }
      }
    }
    
    //check collision
    if(SDL_HasIntersection(&player1, &ball)) {
      int intersectY =  ball.y - player1.y;
      int relativeIntersectY = (player1.y + (PADDLE_HEIGHT/2)) - intersectY;
      int normalizedRelativeIntersectionY = (relativeIntersectY/(PADDLE_HEIGHT/2));
      int bounceAngle = normalizedRelativeIntersectionY * MAXBOUNCEANGLE;
      ballVx = BALL_SPEED*cos(bounceAngle);
      ballVy = BALL_SPEED*-sin(bounceAngle);
    }

    if(SDL_HasIntersection(&player2, &ball)) {
      int intersectY =  ball.y - player2.y;
      int relativeIntersectY = (player2.y + (PADDLE_HEIGHT/2)) - intersectY;
      int normalizedRelativeIntersectionY = (relativeIntersectY/(PADDLE_HEIGHT/2));
      int bounceAngle = normalizedRelativeIntersectionY * MAXBOUNCEANGLE;
      ballVx = BALL_SPEED*-cos(bounceAngle);
      ballVy = BALL_SPEED*sin(bounceAngle);
    }

    // move players and ball
    player_move(&player1, player1_move);
    player_move(&player2, player2_move);
    ball_move(&ball, &ballVx, &ballVy);

    //check both goals
    check_goal(&ball, &player1_score, &player2_score);
    
    // clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // draw players
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player1);
    SDL_RenderFillRect(renderer, &player2);

    // draw ball
    SDL_RenderFillRect(renderer, &ball);

    //draw halfline
    SDL_RenderDrawLine(renderer, WIDTH/2, 0, WIDTH/2, HEIGHT);

    // draw score
    char score1_str[100];
    sprintf(score1_str, "%d", player1_score);
    SDL_Surface* surfaceScore1 = TTF_RenderText_Solid(Noto, score1_str, White);
    SDL_Texture* textureScore1 = SDL_CreateTextureFromSurface(renderer, surfaceScore1);
    SDL_RenderCopy(renderer, textureScore1, NULL, &score1_rect);

    char score2_str[100];
    sprintf(score2_str, "%d", player2_score);
    SDL_Surface* surfaceScore2 = TTF_RenderText_Solid(Noto, score2_str, White);
    SDL_Texture* textureScore2 = SDL_CreateTextureFromSurface(renderer, surfaceScore2);
    SDL_RenderCopy(renderer, textureScore2, NULL, &score2_rect);

    //outputs all the stuff
    SDL_RenderPresent(renderer);

    // limit to 60 fps
    SDL_Delay(33);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return 0;
}
