# A simple shooting platform game

This is a platform game written in C++ using [SDL3](https://github.com/libsdl-org/SDL).
The purpose of the project was building a working [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) from scratch, then build a game on top of it.

## Table of Contents

* **[Getting started](#getting-started)**
* **[How to play](#how-to-play)**
* **[ECS](#ecs)**
* **[Notes for reviewer](#notes-for-reviewer)**

## Getting started

To run the game, you should have [cmake > 3.18](https://cmake.org/) installed, and a working compiler which supports c++17.
After you had those set up, you can build the game and run it by issuing these commands

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make
./shooting_game
```

## How to play

Move your character using right-click without touching the enemies or fall down to the holes.

You can shoot enemies using left-click as well.

## ECS

All of the ecs implementation lied inside `ecs` folder.
The implementation based on this [awesome ECS guide by the author of flecs](https://ajmmertens.medium.com/building-an-ecs-1-where-are-my-entities-and-components-63d07c7da742). But I also stole some ideas from [entt](https://github.com/skypjack/entt) and [bevy](https://bevyengine.org/).

The ECS is well tested using [googletest](https://github.com/google/googletest), it works fairly well but has some limitations:

* It wouldn't work with vector, string, data which is dynamic allocated, etc
* It's not thread safe. Systems must have to be carefully design to not stuck in dead-loop

You can run all the ECS tests to make sure it works correctly:

```bash
tests/ecs/test_ecs
```

## Notes for reviewer

The **Compiling, Testing** point are defined in [getting started](#getting-started) and [ecs](#ecs).
Here are some notes to address others rubric points

### Loops, functions, I/O

* **The project reads data from a file**: game configurations are stored inside [config.json](config.json). When the game is first loaded, all the configurations are read and stored in `GameConfig` class. These are later retrieved by static `GameConfig::data` function. The implementation details can be found in [include/config.h](include/config.h#L253).
* **The project accepts user input and processes the input**: game events are polled every game loop and processed by `event_systems`. Implementation details can be found in [src/game.cpp](src/game.cpp#L36).
* **The project uses data structures and immutable variables**:
  * data structures: Lots of vector, map, set were used. One of them is the threads vector in [include/ecs/system.h](include/ecs/system.h#L43).
  * constants variables: [config.json](config.json) file is stored as a constant variable in [include/config.h](include/config.h#L262).

### Object Oriented Programming

I followed data-oriented design in game, so the game implementation itself is not OOP heavy.
But the OOP designs are easily found in the `ecs` implementation.

* **One or more classes are added**: for the game itself, there are `Game` class which defined in [include/game.h](include/game.h#L23).
* **Class constructors utilize member initialization lists**: You can find them in [src/game.cpp](src/game.cpp#L31).
* **Classes abstract implementation details from their interfaces**: I implemented class `World` followed this point, it can be found in [include/ecs/world.h](include/ecs/world.h#L10).
* **Overloaded functions allow the same function to operate on different parameters**: there are are two `get_column` methods in [include/ecs/table/table.h](include/ecs/table/table.h#L70). One of them is template method, other is not.
* **Classes follow an appropriate inheritance hierarchy with virtual and override functions**: There is `AddArchetypeEvent` class follow this rubric, it's in [include/ecs/observer/event.h](include/ecs/observer/event.h#L25)
* **Templates generalize functions or classes in the project**: There are lots of template classes and functions. One of them is the `get_column` in [include/ecs/table/table.h](include/ecs/table/table.h#L70)

### Memory Management

* **The project makes use of references in function declarations**: there are some functions can be found in [include/player/player.h](include/player/player.h#L28) followed this.
* **The project uses destructors appropriately**: `GameBase` class has a destructor where it calls `SDL_Quit` properly, it can be found in [src/game.cpp](src/game.cpp#L10)
* **The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate**: I think the `GameBase` class mentioned above follow this point, however I'm not too sure since the `GameBase` will never be destroyed until we quit the game.
* **The project follows the Rule of 5**: One of the example is the `GameBase` class above, all copy constructor and move constructor are marked as delete. Or more of them can be found in the `Query` class defined in [include/ecs/query/query.h](include/ecs/query/query.h#L9) which implement move constructor properly.
* **The project uses move semantics to move data instead of copying it, where possible**: One function use move semantics is the `Game` constructor defined in [include/game.h](include/game.h#L25)

### Concurrency

* **The project uses multithreading**: multiple threading implementation can be found in `SystemManager` class [include/ecs/system.h](include/ecs/system.h#L10). They are used to define the systems and added through `add_parallel` functions. Those functions are used a lot in [src/main.cpp](src/main.cpp#L27).
* **A mutex or lock is used in the project**: lock is used when initialize `ColumnIterator` class. It can be found in [include/ecs/column/column.h](include/ecs/column/column.h#L125). Every time, a column data is access through iterator, a lock is defined, pass into `ColumnIterator` constructor, since it is a `unique_lock`, after `ColumnIterator` out of scope, it will automatically unlock the mutex.
