# Advanced RPG — UE5 Gameplay Programming Project

> A combat-focused third-person Action RPG built in **Unreal Engine 5.6**, with gameplay systems implemented primarily in **C++** and **Gameplay Ability System (GAS)**.

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.6-black?logo=unrealengine)](https://www.unrealengine.com/)
[![C++](https://img.shields.io/badge/C%2B%2B-Gameplay-blue)](https://isocpp.org/)
[![GAS](https://img.shields.io/badge/Gameplay%20Ability%20System-GAS-orange)](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)

## 🎥 Gameplay

[![Watch Gameplay](https://img.youtube.com/vi/sfk2eV6xraI/maxresdefault.jpg)](https://www.youtube.com/watch?v=sfk2eV6xraI)

A combat prototype focused on **ability-driven gameplay, responsive combat, enemy AI, boss encounters, and reusable gameplay tasks**.

---

## ⚡ Special Ability Showcase

These abilities are powered by **custom GAS Ability Tasks** written in C++. The screenshots below show the systems running in-game.

<table>
<tr>
<td width="33%" align="center">
<strong>Pluck of Many</strong><br><br>
<img src="https://github.com/user-attachments/assets/ee7db30c-e9c8-47a2-b073-cfac9fa91455" width="100%" alt="Pluck of Many ability showcase">
<br><br>
<sub>Clone spawning, targeting and gameplay orchestration.</sub>
</td>
<td width="33%" align="center">
<strong>All-Hit Wolves</strong><br><br>
<img src="https://github.com/user-attachments/assets/1b447033-16f4-4689-936e-09de068f49a8" width="100%" alt="All-Hit Wolves ability showcase">
<br><br>
<sub>Multi-entity attack execution driven by a custom ability task.</sub>
</td>
<td width="33%" align="center">
<strong>Circling Wolves</strong><br><br>
<img src="https://github.com/user-attachments/assets/51e3e994-105e-42c1-af06-9422a1dba44c" width="100%" alt="Circling Wolves ability showcase">
<br><br>
<sub>Timed movement and coordinated combat behavior.</sub>
</td>
</tr>
</table>

Basic Character with Three Weapons In World :
<img width="1482" height="826" alt="Image" src="https://github.com/user-attachments/assets/a589f349-1ffd-4c2d-90ff-98933f3dfde4" />

### What is happening behind the scenes?

```text
Input / Gameplay Event
        ↓
Gameplay Ability
        ↓
Custom GAS Ability Task
        ↓
Spawn / Move / Target / Execute
        ↓
Gameplay Effects + Damage
        ↓
Cleanup / Ability Completion
```

The important part is that these are not one-off character functions. The complex asynchronous gameplay operations are encapsulated inside reusable **`UAbilityTask`** implementations.

---

## 🧠 Engineering Highlights

| System | Implementation |
|---|---|
| Gameplay Abilities | Custom C++ abilities built around GAS |
| Ability Tasks | Reusable custom `UAbilityTask` implementations for complex gameplay sequences |
| Combat | Light/heavy attacks, combos, blocking, parrying, dodging and weapon-specific abilities |
| Damage | Gameplay Effects + custom execution calculation + attribute-driven damage flow |
| Targeting | Target lock and target-driven projectile abilities |
| Projectiles | Homing, wall, expanding-ring and other ability-driven projectile patterns |
| Character Transformation | GAS-driven hero/bow transformation sequence with montage, VFX and control restoration |
| Enemy AI | Behavior Trees, AI services/tasks and EQS-based positioning/decision making |
| VFX | Niagara-driven ability and impact effects |
| Data Driven Gameplay | Ability/startup configuration through Data Assets |
| UI | C++ gameplay/UI components and widgets |

---

## ⚔️ Combat Systems

### Weapon-Based Combat

- Three weapon types with distinct attack sets
- Light/heavy attack chains
- Combo progression and finishers
- Block and timing-based parry/counter
- Dodge/roll with invulnerability-frame gameplay behavior
- Target lock for focused combat
- Weapon-specific special abilities

### GAS Ability Architecture

The project uses GAS as the gameplay backbone rather than implementing every ability as a monolithic character function.

```text
Input / Gameplay Event
        ↓
Gameplay Ability
        ↓
Custom Ability Task
        ↓
Animation / Targeting / Gameplay Operation
        ↓
Gameplay Effect
        ↓
Execution Calculation
        ↓
Attribute Set
        ↓
Gameplay Tags / UI / Gameplay Cue
```

This approach keeps complex ability sequences isolated and reusable.

---

## 🧩 Custom GAS Ability Tasks

One of the main engineering areas of the project is extending GAS with custom `UAbilityTask` implementations.

Examples include:

- `AbilityTask_TransformCharacter` — coordinates character replacement, montages, VFX, camera handling and control restoration.
- `AbilityTask_PluckOfMany` — orchestrates multiple clone spawns, navigation-aware placement, staggered spawning and cleanup.
- `AT_HomingProjectiles` — spawns projectiles toward an explicit target and configures homing behavior, damage specs and VFX.
- `AT_ExpandRingProjectiles` — generates radial projectile patterns with configurable count, spacing, stagger and lifetime.
- `AT_WallProjectiles` — creates configurable projectile formations using deferred actor spawning.
- `UAT_CircleRhythm` — coordinates orbiting actors, periodic traces and timed gameplay events.
- `AbilityTask_WaitSpawnEnemies` / `AbilityTask_WaitSpawnBossEnemies` — asynchronous enemy-spawn orchestration for abilities.

The common design is:

```text
Gameplay Ability
      ↓
Custom Ability Task
      ↓
Start asynchronous gameplay operation
      ↓
Callbacks / delegates
      ↓
Ability continues or task ends
```

---

## 💥 Damage Pipeline

Damage is routed through GAS rather than directly modifying health from individual attacks.

```text
Attack Ability
     ↓
Gameplay Effect Spec
     ↓
SetByCaller Damage / Attack Type
     ↓
Damage Execution Calculation
     ↓
Attack Power vs Defence Power
     ↓
DamageTaken Attribute
     ↓
Health / Death Gameplay Tags
```

This provides a central place for combat scaling and attribute-based damage calculation.

---

## 🤖 Enemy AI

The project uses Unreal's AI framework for combat decision making:

- Behavior Trees for high-level decision flow
- Custom C++ Behavior Tree services/tasks
- EQS for spatial queries and positioning
- Melee and ranged enemy behaviors
- Boss-specific combat abilities and summon patterns

Boss abilities include projectile patterns, aerial/ground summons, melee attacks and multi-entity encounters.

---

## ✨ Ability & VFX Examples

### Pluck of Many

The clone ability combines gameplay orchestration with navigation-aware placement and timed cleanup.

### Projectile Patterns

Custom ability tasks are used for radial, wall and homing projectile patterns instead of embedding the spawning logic directly inside individual abilities.

### Character Transformation

The transformation system coordinates:

```text
Disable Movement
      ↓
Play Out Montage
      ↓
Dissolve / VFX
      ↓
Spawn Replacement Character
      ↓
Possess Replacement
      ↓
Play In Montage
      ↓
Restore Player Control
```

---

## 🗂️ Codebase Tour

```text
Source/AdvancedRPG/
├── AbilitySystem/
│   ├── Abilities/          # Gameplay Abilities
│   ├── AbilityTasks/       # Custom GAS Ability Tasks
│   ├── AttributeSet/       # Gameplay attributes
│   └── Components/         # Ability System components
├── AI/                     # AI controllers, BT services/tasks
├── AnimInstances/          # C++ animation logic
├── Characters/             # Hero, enemies, boss and variants
├── Components/
│   ├── Combat/             # Combat components
│   └── UI/                 # Gameplay/UI components
├── Controllers/            # Player and AI controllers
├── DataAssets/             # Data-driven ability/startup configuration
├── Items/                  # Weapons, projectiles and pickups
├── Widgets/                # UI widgets
└── SaveGame/               # Save/load systems
```

---

## 🛠️ Technology

- **Unreal Engine 5.6**
- **C++**
- **Gameplay Ability System (GAS)**
- **Gameplay Tags**
- **Enhanced Input**
- **Behavior Trees**
- **EQS**
- **Niagara**
- **Animation Blueprints**
- **Data Assets**

---

## 🎯 What This Project Demonstrates

This project was built to explore the engineering problems behind combat-heavy gameplay rather than only assembling gameplay in Blueprints.

Key areas include:

- Designing gameplay around GAS
- Extending GAS with custom Ability Tasks
- Building reusable gameplay operations
- Passing Gameplay Effect Specs through asynchronous gameplay systems
- Managing timers, delegates and task lifecycles
- Building attribute-driven combat and damage calculation
- Integrating animation, VFX and gameplay state
- Building AI behavior with Behavior Trees and EQS
- Structuring gameplay code into focused C++ classes and components

---

## 📌 Project Status

This is an actively developed personal gameplay programming project. Systems are continuously being refined as part of my study of **Unreal Engine C++, gameplay architecture and technical game development**.

## 👨‍💻 Developer

**Praveen** — BTech CSE student at IIIT Ranchi, aspiring **Unreal Engine Gameplay Programmer / Indie Game Developer**.

If you're reviewing this repository for a gameplay programming role, the most relevant starting points are the **AbilitySystem**, **AbilityTasks**, **Characters**, **Components/Combat**, and **AI** directories.
