#pragma once
#include "tools.h"
#include "quad.h"
#define MAX_COLLIDERS 100
#include <vector>
using std::vector;

static u32 collider_id = 0;

struct HitBox
{
    vec2 min;
    f32 w, h;
};

typedef struct Box
{
    vec2 min;
    f32 w, h;
    HitBox hb;

    vec2 velocity;
    f32 restitution = 1.f;
    f32 mass = 1.f;
    b32 on_ground;
    b32 is_colliding;
    b32 active = 1;
    b32 is_destroyed;
} Box;

static vector<Box*> colliders;

static void init_Box(Box* b,vec2 min,f32 w, f32 h)
{
    b->min = min;
    b->w = w;
    b->h = h;
    b->velocity = {0.f,0.f};
    colliders.push_back(b);
    b->hb = {{0,0},1.f,1.f};
}
//NOTE(ilias): this is not performant and it's called a LOT of times, optimize!
i32 collide(Box *b1, Box *b2)
{
    //render_collider_in_pos (mat, {b->min.x + b->hb.min.x,b->min.y + b->hb.min.y,-1.f}, {b->w * b->hb.w, b->h * b->hb.h}, (float)b->is_colliding);
    if ((b1 == NULL || b2 == NULL || b1 == b2 || !b1->active || !b2->active))return 0;
    vec2 b1_min = {b1->min.x + b1->hb.min.x, b1->min.y + b1->hb.min.y};
    vec2 b2_min = {b2->min.x + b2->hb.min.x, b2->min.y + b2->hb.min.y};
    vec2 b1_max = {b1_min.x + b1->w * b1->hb.w, b1_min.y + b1->h*b1->hb.h};
    vec2 b2_max = {b2_min.x + b2->w * b2->hb.w, b2_min.y + b2->h*b2->hb.h};
    if (b1_max.x < b2_min.x || b1_min.x > b2_max.x)return 0;
    if (b1_max.y < b2_min.y || b1_min.y > b2_max.y)return 0;
    return 1;

}

Box* check_collision(Box b1)
{
    for (Box * b : colliders)
    {
        if (collide(&b1, b))return b;
    }
    return 0;
}


static void 
resolve_collision(Box* A, Box* B)
{
  // Calculate relative velocity
  vec2 rv = sub_vec2(B->velocity,A->velocity);
  vec2 normal = sub_vec2(add_vec2(B->min, {B->w/2.f,B->h/2.f}),add_vec2(A->min, {A->w/2.f,A->h/2.f}));//{0,-1}; 
  // Calculate relative velocity in terms of the normal direction
  f32 velAlongNormal = dot_vec2(rv, normal);
 
  // Do not resolve if velocities are separating
  if(velAlongNormal > 0)
    return;
 
  // Calculate restitution
  f32 e = min( A->restitution, B->restitution);
 
  // Calculate impulse scalar
  f32 j = -(1 + e) * velAlongNormal;
  j /= (1 / A->mass) + (1 / B->mass);
 
  // Apply impulse
  vec2 impulse = mul_vec2f(normal, j);
  A->velocity = sub_vec2(A->velocity, mul_vec2f(impulse,(1 / A->mass)));
  B->velocity = add_vec2(B->velocity,mul_vec2f(impulse,(1 / B->mass)));
}

static void 
handle_collision_basic(Box *b1, Box *b2)
{
    b1->is_colliding = 1;
    b1->velocity.x = 5.f * global_platform.dt * (b1->min.x - b2->min.x) /abs(b1->min.x - b2->min.x);
    b1->velocity.y = 5.f * global_platform.dt * (b1->min.y - b2->min.y) /abs(b1->min.y - b2->min.y);
}


/*These are just for testing intersection stuff!*/
static quad collider_render_Box;

static void 
init_collider_render_quad()
{
    init_quad(&collider_render_Box);
}

static void 
render_collider_in_pos (mat4 mat, vec3 pos, vec2 scale,f32 red = 0.f)
{
    mat4 pos_mat = translate_mat4(pos);
    mat4 scale_mat = m4d(1.f);//scale_mat4({scale.x, scale.y, 1.f});
    scale_mat.elements[0][0] = scale.x;
    scale_mat.elements[1][1] = scale.y;
    mat = mul_mat4(mat, mul_mat4(pos_mat,scale_mat));
    render_collider(&collider_render_Box, (float*)mat.elements, red);
}


static void
render_collider(mat4 mat, Box* b)
{
    vec3 pos = {b->min.x, b->min.y, -1.f};
    vec2 scale = {b->w,b->h};
    f32 red = !b->is_colliding;
    mat4 pos_mat = translate_mat4(pos);
    mat4 scale_mat = m4d(1.f);//scale_mat4({scale.x, scale.y, 1.f});
    scale_mat.elements[0][0] = scale.x;
    scale_mat.elements[1][1] = scale.y;
    mat = mul_mat4(mat, mul_mat4(pos_mat,scale_mat));
    render_collider(&collider_render_Box, (float*)mat.elements,red);
}


//NEW VERSION OF PHYSICS ENGINE


typedef struct AABB
{
    vec2 min;
    vec2 max;
}AABB;


b32 AABBvsAABB(AABB a, AABB b)
{
    //search for a Separating Axis 
    if (a.max.x < b.min.x || a.min.x > b.max.x)return 0;
    if (a.max.y < b.min.y || a.min.y > b.max.y)return 0;

    //if not found, there must be a collision
    return 1;
}

typedef struct Circle
{
    f32 radius;
    vec2 position;
}Circle;

f32 dist(vec2 a, vec2 b)
{
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

b32 CirclevsCircle(Circle c1, Circle c2)
{
    f32 r = c1.radius + c2.radius;
    return (r < dist(c1.position, c2.position));
}

typedef enum ShapeType 
{
   CIRCLE = 1,
   BOX,
   CAPSULE
}ShapeType;


struct PhysicsMaterial
{
    f32 density;
    f32 restitution;
};
struct MassData
{
    f32 mass;
    f32 inv_mass;

    f32 inertia;
    f32 inv_inertia;
};

typedef struct Shape 
{
    union
    {
        Circle circle;
        AABB box;
    };
    ShapeType type;
}Shape;

typedef struct Manifold
{
    Shape* c1;
    Shape* c2;
    f32 penetration;
    vec2 normal;
};

static b32 CirclevsCircle(Manifold* m)
{
    Circle* c1 = &m->c1->circle;
    Circle* c2 = &m->c2->circle;

    //collision normal
    vec2 n = sub_vec2(c2->position , c1->position);


    f32 r = c2->radius + c1->radius;
    r*=r;


    if (length_vec2(n)*length_vec2(n) > r)
        return 0;


    f32 d = length_vec2(n);


    //if the distance of the radii is not zero
    if (d!=0)
    {
        m->penetration = r - d;

        m->normal = v2(r/d, r/d);
        return 1;
    }
    else
    {
        m->penetration = c2->radius;
        m->normal = v2(1,0);
        return 1;
    }
}

static void
AABBvsAABB(Manifold* m)
{
    AABB b1 = m->c1->box;
    AABB b2 = m->c2->box;

    vec2 b1_mid = mul_vec2f(add_vec2(b1.min, b1.max), 0.5f);
    vec2 b2_mid = mul_vec2f(add_vec2(b2.min, b2.max), 0.5f);

    vec2 e_b1 = abs_vec2(mul_vec2f(sub_vec2(b1.max, b1.min), 0.5f)); 
    vec2 e_b2 = abs_vec2(mul_vec2f(sub_vec2(b2.max, b2.min), 0.5f)); 

    vec2 d = sub_vec2(b2_mid, b1_mid);


    //calculate the overlap on x and y axis
    f32 dx = e_b1.x + e_b2.x - abs(d.x);
    if (dx < 0)return;
    f32 dy = e_b1.y + e_b2.y - abs(d.y);
    if (dy < 0)return;

    vec2 n;
    f32 depth;
    vec2 p;


    //x-axis overlap is smaller
    if (dx < dy)
    {
       depth = dx; 

       if (d.x < 0)
       {
            n = v2(-1.f,0.f);
            p = sub_vec2(b1_mid, v2(e_b1.x,0.f));
       }
       else
       {
           n = v2(1.f,0.f);
           p = add_vec2(b1_mid, v2(e_b1.x, 0));
       }
    }
    else 
    {
        depth = dy;
        if (d.y < 0)
        {
            n = v2(0,-1.f);
            p = sub_vec2(b1_mid, v2(0, e_b1.y));
        }
        else
        {
            n = v2(0,1.f);
            p = add_vec2(b1_mid, v2(0,e_b1.y));
        }
    }
    //m->contact_points[0] = p;
    //m->depths[0] = depth;
    m->normal = n;
    m->penetration = depth;//???

}

static void aabb_update(AABB* aabb, vec2 pos, f32 w, f32 h)
{
    vec2 min = pos;
    vec2 max = v2(pos.x+w, pos.y + h);
    aabb->min = min;
    aabb->max = max;
}



typedef struct PhysicsBodyComponent  //managers??
{
  Shape *shape;
  mat4 transform;
  PhysicsMaterial material;
  MassData mass_data;
  vec2 velocity;
  vec2 force;
  f32 gravity_scale;
}PhysicsBody;


struct PhysicsBody2DComponent
{
    vec2 min;
    f32 w, h;
    AABB collider;

    vec2 velocity;
    f32 restitution = 1.f;
    f32 mass = 1.f;
    b32 on_ground;
    b32 is_colliding;
    b32 active = 1;
    b32 is_destroyed;
};

#define MAX_SHAPES 1000
typedef struct ColliderManager
{
    Shape* shapes;
    u32 current_index;
    u32 max_shapes;
}ColliderManager;

static ColliderManager 
colldier_manager_init()
{
    ColliderManager m = {0};
    m.shapes = (Shape*)arena_alloc(&global_platform.permanent_storage, MAX_SHAPES * sizeof(Shape));
    m.current_index = 0;
    m.max_shapes = MAX_SHAPES;

    return m;
}

static Shape* 
collider_manager_add(ColliderManager* m, Shape shape)
{
    if (m->current_index < m->max_shapes)
    {
        m->shapes[m->current_index++] = shape;
        return &m->shapes[m->current_index - 1];
    }else
        return NULL;
}



