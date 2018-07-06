#pragma once
#pragma once
using Component_type = unsigned int;

enum class Component {Position, SpriteSheet, State};
enum class System {Renderer, Movement, Collision};
enum class Entity_event{Spawned, Despawned, };