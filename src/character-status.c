#include "character-status.h"


static void print_status(Status_Access access) {
  Character_Base_Access base = access->base;

  printf("--------- status -------\n");
  printf("%s : %s\n", base->name, access->race);
  printf("Mark: %s\n", base->Mark->mark);
  printf("Damage: %d\n", access->damage);
}


static void character_init(Status_Access access) {
  character_base_init(access->base);
}


static void character_copy(Status_Access access, Status_Access from) {
  from->base->copy(access->base, from->base);
  access->race = from->race;

  access->faction = from->faction;
  access->damage = from->damage;
}


static Is_Alive attack_character(Status_Access from, Status_Access to) {
  to->damage += 1;

  if (to->damage >= 3) {
    to->base->is_alive = false;
    to->base->crossable = true;
    to->base->attackable = false;
    return DEAD;
  }
  else {
    return ALIVE;
  }
}


// -----------------------------------------
// SET BASE VALUE
// -----------------------------------------
static void set_base_name(Status_Access access, String name) {
  access->base->name = name;
}

static void set_race(Status_Access access, String race) {
  access->race = race;
}

static void set_base_style(Status_Access access, Style_Access style) {
  access->base->Mark = style;
}

static void set_base_mark(Status_Access access, String mark) {
  access->base->Mark->mark = mark;
}


// -----------------------------------------
// RELATION
// -----------------------------------------
static Relation_Type get_relation(Status_Access access) {
  return access->faction;
}

static void set_ally(Status_Access access) {
  access->faction = RELATION_ALLY;
}

static void set_enemy(Status_Access access) {
  access->faction = RELATION_ENEMY;
}

static void set_neutral(Status_Access access) {
  access->faction = RELATION_NEUTRAL;
}


// -----------------------------------------
// EXPOSE API
// -----------------------------------------
Character_API character = {
  .init = character_init,
  .copy = character_copy,
  .print_status = print_status,
  .attack = attack_character,
  .set_name = set_base_name,
  .set_race = set_race,
  .set_style = set_base_style,
  .set_mark = set_base_mark,

  .get_relation = get_relation,
  .set_relation_ally = set_ally,
  .set_relation_enemy = set_enemy,
  .set_relation_neutral = set_neutral,

  .attack = attack_character
};
