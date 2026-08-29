
# Advanced RPG


<video src="/public/PluckOfMany.mp4" width="100%" controls></video>

A combat-focused Action RPG built in **Unreal Engine 5** using **C++** and the **Gameplay Ability System (GAS)**.

## Features

### Hero Combat System
- **Three Weapon Types** — each with unique light attacks, heavy attacks, and aerial finishers
- **Combo System** — chain light attacks into a heavy jump-to-finisher
- **Block & Parry** — active block with timing-based parry into counter-attack
- **Target Lock** — soft-lock camera on enemies during combat
- **Dodge/Roll** — i-frame dodge with GAS gameplay cue integration
- **Special Abilities** (per weapon):
  - Rage mode
  - Pluck of Many (Wukong-style clones)
  - All-Kill Wolves / Paragon Shinbi-style wolf summons
  - Circling Wolves 

### Enemy AI
- **Melee Grunt** — approaches and attacks when in range
- **Ranged Enemy** — fires projectiles; switches to melee counter-attack if player closes distance
- **Boss Enemy** with multi-phase abilities:
  - Hand & leg melee attacks
  - Summon aerial & ground minions
  - Homing curved projectiles
  - Circling projectile patterns
  - Mid-air enemy spawns

### Technical Stack
| Tech | Usage |
|------|-------|
| Unreal Engine 5 | Core engine |
| C++ | Gameplay framework, GAS, AI |
| Gameplay Ability System | All abilities, attributes, effects |
| Behavior Trees / EQS | Enemy AI decision making |
| Niagara | VFX for abilities and impacts |
| Animation Blueprints | Locomotion, combat states |

## Project Structure

Source/AdvancedRPG/
├── AbilitySystem/          # GAS abilities, tasks, attribute sets
├── AI/                     # Behavior trees, AI controllers
├── AnimInstances/          # Animation Blueprint logic in C++
├── Characters/             # Hero, enemies, boss, ice clone
├── Components/Combat/      # Combat logic (light/heavy attacks)
├── Components/UI/          # UI components for hero and enemies
├── Controllers/            # Player & AI controllers
├── DataAssets/             # Startup data for ability grants
├── Items/                  # Weapons, projectiles, pickups
├── Widgets/                # UI widgets and options menus
└── SaveGame/               # Save game subsystem

<img width="300" height="150" alt="Image" src="https://github.com/user-attachments/assets/d5945d40-a7c8-4b67-a2c6-0a6f24ad9731" />
