#ifndef ENTITY_H
#define ENTITY_H
#include "tools.h"

//TODO completely remove sparse entity system....
#define MAX_ENTITY 1000
#define INVALID_ENTITY 0
static u32 entity_id = 1;
typedef u32 Entity;

static Entity
create_entity(void)
{
    return entity_id++;
}

static b32 entity_equals(Entity l, Entity r)
{
    return (l == r);
}

typedef struct PositionComponent
{
    vec3 pos;

}PositionComponent;

typedef IntHashMap EntityHashMap;
typedef struct PositionManager
{
    //maybe make them pointers into an arena allocator???
    PositionComponent positions[MAX_ENTITY];
    Entity entities[MAX_ENTITY];
    u32 next_index = 0;
    EntityHashMap table;
    
}PositionManager;

static void 
init_position_manager(PositionManager* m)
{
    m->table = create_hashmap(20);
}
//creates an empty component of type position TODO generalize?
static PositionComponent*
add_component(PositionManager* manager, Entity entity)
{
  // INVALID_ENTITY is not allowed!
  assert(entity != INVALID_ENTITY);

  // Only one of this component type per entity is allowed!
  //assert(lookup_hashmap(&manager->table, entity) == -1);

  // Update the entity lookup table:
  insert_hashmap(&manager->table, entity, manager->next_index);

  // New components are always pushed to the end:
  manager->positions[manager->next_index] = {0}; 

  // Also push corresponding entity:
  manager->entities[manager->next_index] = entity;

  return &manager->positions[manager->next_index++];
}

//removes an entity and its component from the manager's data TODO generalize?
static void remove_entity(PositionManager* manager, Entity entity)
{
  u32 index = lookup_hashmap(&manager->table, entity);
  if (index != -1)
  {
    // Directly index into components and entities array:
    Entity entity = manager->entities[index];

    if (index < manager->next_index)
    {
      // Swap out the dead element with the last one:
      manager->positions[index] = manager->positions[manager->next_index-1];// try to use move
      manager->entities[index] = manager->entities[manager->next_index-1];

      //NOTE: this is not ver performant..
      remove_hashmap(&manager->table,entity);
      remove_hashmap(&manager->table,manager->entities[index]);
      insert_hashmap(&manager->table,manager->entities[index], index); 
    }

    // Shrink the container
    manager->next_index--;
  }
}

static PositionComponent* get_component(PositionManager* manager, Entity entity)
{
    i32 index = lookup_hashmap(&manager->table, entity);
    if (index != -1)
    {
        return &manager->positions[index];
    }
    return NULL;
}

//NOTE: maybe I should make a component.h for all different components no?
typedef struct TransformComponent
{
    vec3 position;
    vec3 scale;
    Quaternion rotation;
}TransformComponent;
typedef struct TransformManager
{
     //maybe make them pointers into an arena allocator???
    TransformComponent transforms[MAX_ENTITY];
    Entity entities[MAX_ENTITY];
    u32 next_index = 0;
    EntityHashMap table;
}TransformManager;

/* Entity test
    {
        for (u32 i = 0; i < 100; ++i){
            Entity entity = create_entity();
            PositionComponent* position_component = add_component(&position_manager,entity);
            position_component->pos = v3((f32)i,(f32)i,(f32)i);
        }
        remove_entity(&position_manager,1);
        remove_entity(&position_manager,2);
        remove_entity(&position_manager,3);
    }
*/




#endif
