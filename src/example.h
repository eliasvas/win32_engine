


static current_id = 0;

typedef struct Entity
{
    std::vector<Component> components;
    u32 id;

}Entity;

typedef struct Component
{
    virtual void Update() = 0;

    virtual void Init() = 0;

    virtual void Render() = 0;

    Component();
};


typedef struct HealthComponent : public Component
{
    u32 hp;

    void Update()
    {
       this->hp+= 2;
    }

    void Init()
    {
        this->hp = 100;
    }

    HealthComponent(){Init()}
}HealthComponent;


void main()
{
    Entity Player;
    Player.components.push_back(new HealthComponent());

    for (Component& c : Player.components)
        c.Update();

    for (Component& c : Player.components)
        c.Render();

}
