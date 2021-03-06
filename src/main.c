#include "main.h"

typedef SDL_Window * SDL_Window_Access;
typedef SDL_Renderer * SDL_Renderer_Access;
typedef SDL_Event * SDL_Event_Access;

int GRID_LENGTH = 24;
int MAX_COUNT_X = 25;
int MAX_COUNT_Y = 20;
SDL_Rect position = {
  .x = 0,
  .y = 0,
  .w = 24,
  .h = 24 
};


void set_player_center(void) {
  int CENTER_X = (MAX_COUNT_X + 1) / 2;
  int CENTER_Y = MAX_COUNT_Y / 2;

  Player->base->Graph_Position.x =  CENTER_X;
  Player->base->Real_Position.x = CENTER_X;

  Player->base->Graph_Position.y = CENTER_Y;
  Player->base->Real_Position.y = CENTER_Y;
}

Yes_No occupy_position_by_others(Point_Type point, Status_Access *result) {
  if (character_pool.find_position(character_use_pool, result, &point) == FOUND) {
    return YES;
  }
  else {
    return NO;
  }
}

Yes_No can_move_position(Point_Type point) {
  if ((point.x >= 0) && (point.x <= (MAX_COUNT_X - 1))
      && (point.y >= 0) && (point.y <= (MAX_COUNT_Y - 1))) {
    return YES;
  }
  else {
    return NO;
  }
}

bool key_process(SDL_Event_Access event) {
  Status_Access npc = NULL;
  Point_Type point = {
    Player->base->Real_Position.x,
    Player->base->Real_Position.y
  };

  Point_Type move_point = {0, 0};

  switch ((event->key).keysym.sym) {
    case SDLK_UP:
      point.y -= 1;

      if (can_move_position(point) == YES) {
        move_point.y = -1;
      }
      break;
    case SDLK_DOWN:
      point.y += 1;

      if (can_move_position(point) == YES) {
        move_point.y = 1;
      }
      break;
    case SDLK_LEFT:
      point.x -= 1;

      if (can_move_position(point) == YES) {
        move_point.x = -1;
      }
      break;
    case SDLK_RIGHT:
      point.x += 1;

      if (can_move_position(point) == YES) {
        move_point.x = 1;
      }
      break;
    case SDLK_q:
      return false;
      break;
    default:
      printf("other\n");
      break;
  }

  if (occupy_position_by_others(point, &npc) == NO) {
    Player->base->Graph_Position.x = Player->base->Graph_Position.x + move_point.x;
    Player->base->Real_Position.x = Player->base->Real_Position.x + move_point.x;
    Player->base->Graph_Position.y = Player->base->Graph_Position.y + move_point.y;
    Player->base->Real_Position.y = Player->base->Real_Position.y + move_point.y;
  }
  else {
    if (character.attack(Player, npc) == DEAD) {
      character.set_style(npc, dead);
    }
  }
  return true;
}

void draw_view( SDL_Renderer_Access render) {
  SDL_Rect rect = {.x = 0, .y = 0, .w = GRID_LENGTH, .h = GRID_LENGTH};
  SDL_SetRenderDrawColor(render, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(render);

  for (int count = 0; count <= MAX_COUNT_X - 1; count++) {
    rect.x = count * GRID_LENGTH;
    rect.y = 0;

    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(render, &(rect));
  }
  for (int count = 0; count <= MAX_COUNT_Y - 1; count++) {
    rect.x = 0;
    rect.y = count * GRID_LENGTH;

    SDL_SetRenderDrawColor(render, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(render, &(rect));
  }

  position.x = GRID_LENGTH * Player->base->Graph_Position.x;
  position.y = GRID_LENGTH * Player->base->Graph_Position.y;
  SDL_RenderCopy(render, Player->base->Mark->access, NULL, &(position));

  uint8_t used = character_use_pool->status->max_size - character_use_pool->status->current_size;
  for (int next = 1; next < used; next++) {
    Status_Access npc = &(character_use_pool->status->pool[next]);

    if (npc->base->status == IN_USE) {
      if (!Point.eq(&(npc->base->Real_Position), &(Player->base->Real_Position))) {
        rect.x = GRID_LENGTH * npc->base->Graph_Position.x;
        rect.y = GRID_LENGTH * npc->base->Graph_Position.y;
        SDL_RenderCopy(render, npc->base->Mark->access, NULL, &(rect));
      }
    }
  }

  SDL_RenderPresent(render);
}

Execute_Result init_view(SDL_Renderer_Access render) {
  SDL_Color white = {255, 255, 255};

  TTF_Font* font = TTF_OpenFont(FONT_FAMILY, 512);
  if (!font) {
    printf("TTF_OpenFont: %s\n", TTF_GetError());
    return EXECUTE_FAILED;
  }

  SDL_Surface* surfaceMessage = NULL;
  uint8_t counter = 0;
  Style_Access result = Style_Pool_Interface.next(style_pool, &counter);

  while (result != NULL) {
    surfaceMessage = TTF_RenderUTF8_Solid(font,
        result->mark,
        white);
    result->access = SDL_CreateTextureFromSurface(render, surfaceMessage);

    SDL_FreeSurface(surfaceMessage);

    result = Style_Pool_Interface.next(style_pool, &counter);
  }
  TTF_CloseFont(font);

  result = Style_Pool_Interface.find(style_pool, "player");
  character.set_style(Player, result);

  dead = Style_Pool_Interface.find(style_pool, "dead");
  return EXECUTE_SUCCESS;
}

int main(int argc, char *argv[]) {
  SDL_Window_Access win;
  SDL_Renderer_Access render;
  bool running = true;

  String_Pool_start_stack(string_pool, 1000);
  style_pool = Style_Pool_Interface.start(256);
  character_prepare_pool = character_pool.start(20);
  character_use_pool = character_pool.start(100);

  Player = character_pool.malloc(character_use_pool);
  set_player_center();

  char *CONFIG_FILE = NULL;
  if (argc >= 2) {
    size_t path_lang = strlen(argv[1]);
    CONF_PATH = MAIN_STRING_malloc(path_lang);
    strcpy(CONF_PATH, argv[1]);

    char *CONFIG_NAME = "/config/init.css";
    CONFIG_FILE = use_path(CONFIG_NAME);
    setup_character_config_path(argv[1]);
  }
  else {
    goto DONE;
  }

  setup_style(CONFIG_FILE);
  setup_npc_by_dir();
  use_npc("goblin", "random-name");

  SDL_Init(SDL_INIT_EVERYTHING);
  if (TTF_Init() != 0) {
    goto INIT_FAILED;
  }

  win = SDL_CreateWindow(GAME_TITLE, 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
  render = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

  if (init_view(render) != EXECUTE_SUCCESS) {
    goto INIT_FAILED;
  }

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_KEYDOWN:
          running = key_process(&event);
          break;
        default:
          break;
      }

      draw_view(render);
    }
  }

  Style_Pool_Interface.gc(style_pool);

  SDL_DestroyRenderer(render);
  SDL_DestroyWindow(win);

INIT_FAILED:
  TTF_Quit();
  SDL_Quit();

DONE:
  character_pool.stop(character_use_pool);
  character_pool.stop(character_prepare_pool);
  Style_Pool_Interface.stop(style_pool);
  string_pool->stop(string_pool);
}
