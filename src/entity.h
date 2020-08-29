#ifndef ENTITY_H
#define ENTITY_H
#include "tools.h"
#include <unordered_map> //TODO this has to GOO

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

typedef struct PosisitonManager
{
    //maybe make them pointers into an arena allocator???
    PositionComponent positions[MAX_ENTITY];
    Entity entities[MAX_ENTITY];
    u32 next_index = 0;
    std::unordered_map<Entity, u32> lookup;
    
}PositionManager;

//creates an empty component of type position TODO generalize?
static PositionComponent*
add_component(PositionManager* manager, Entity entity)
{
  // INVALID_ENTITY is not allowed!
  assert(entity != INVALID_ENTITY);

  // Only one of this component type per entity is allowed!
  assert(manager->lookup.find(entity) == manager->lookup.end());

  // Update the entity lookup table:
  manager->lookup[entity] = manager->next_index;

  // New components are always pushed to the end:
  manager->positions[manager->next_index] = {0}; 

  // Also push corresponding entity:
  manager->entities[manager->next_index] = entity;

  return &manager->positions[manager->next_index++];
}

//removes an entity and its component from the manager's data TODO generalize?
void remove_entity(PositionManager* manager, Entity entity)
{
  auto it = manager->lookup.find(entity);
  if (it != manager->lookup.end())
  {
    // Directly index into components and entities array:
    u32 index = it->second;
    Entity entity = manager->entities[index];

    if (index < manager->next_index)
    {
      // Swap out the dead element with the last one:
      manager->positions[index] = manager->positions[manager->next_index-1];// try to use move
      manager->entities[index] = manager->entities[manager->next_index-1];

      // Update the lookup table:
      manager->lookup[manager->entities[index]] = index;
    }

    // Shrink the container:
    manager->next_index--;
    manager->lookup.erase(entity);
  }
}

PositionComponent* get_component(PositionManager* manager, Entity entity)
{
    auto it = manager->lookup.find(entity);
    if (it != manager->lookup.end())
    {
        return &manager->positions[it->second];
    }
    return NULL;
}

//NOTE: maybe I should make a component.h for all different components no?
typedef struct TransformComponent
{
    vec3 position;
    vec3 scale;
    Quaternion rotation;
};
typedef struct TransformManager
{
     //maybe make them pointers into an arena allocator???
    TransformComponent transforms[MAX_ENTITY];
    Entity entities[MAX_ENTITY];
    u32 next_index = 0;
    std::unordered_map<Entity, u32> lookup;

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
