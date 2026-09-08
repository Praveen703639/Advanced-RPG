<!-- ═══════════════════════════════════════════════════════════ -->
<!-- ADVANCED RPG — UE5 C++ GAMEPLAY PROGRAMMING SHOWCASE -->
<!-- ═══════════════════════════════════════════════════════════ -->

<div align="center">

<img src="https://capsule-render.vercel.app/api?type=venom&height=250&text=ADVANCED%20RPG&fontSize=58&fontColor=00f3ff&stroke=00f3ff&strokeWidth=2&desc=UNREAL%20ENGINE%205.6%20%7C%20C%2B%2B%20%7C%20GAMEPLAY%20ABILITY%20SYSTEM&descAlignY=68&descSize=16&theme=matrix" width="100%"/>

<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=700&size=20&pause=1000&color=00F3FF&center=true&vCenter=true&width=850&lines=C%2B%2B+Gameplay+Programming;Gameplay+Ability+System+%28GAS%29;Custom+Ability+Tasks;Combat+%7C+AI+%7C+Boss+Systems;Animation+%7C+Niagara+%7C+Gameplay+Architecture" alt="Typing Animation"/>

[![Unreal Engine 5.6](https://img.shields.io/badge/UNREAL%20ENGINE-5.6-0E1128?style=for-the-badge&logo=unrealengine&logoColor=00F3FF)](https://www.unrealengine.com/)
[![C++](https://img.shields.io/badge/C%2B%2B-GAMEPLAY-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![GAS](https://img.shields.io/badge/GAS-GAMEPLAY%20ABILITY%20SYSTEM-FF00FF?style=for-the-badge)](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)
[![GitHub](https://img.shields.io/badge/GITHUB-REPOSITORY-0A0A0A?style=for-the-badge&logo=github)](https://github.com/Praveen703639/Advanced-RPG)

</div>

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🎮 `PROJECT.BOOT()`

```cpp
class AAdvancedRPG : public AGameplayProject
{
public:
    FString Engine = TEXT("Unreal Engine 5.6");
    FString Language = TEXT("C++");
    FString Framework = TEXT("Gameplay Ability System");

    TArray<FString> Systems = {
        TEXT("Combat"),
        TEXT("Custom Ability Tasks"),
        TEXT("Enemy AI"),
        TEXT("Boss Encounters"),
        TEXT("Animation-Driven Gameplay"),
        TEXT("Niagara VFX")
    };
};
```

> **A combat-focused third-person Action RPG built to explore real-time gameplay programming in Unreal Engine — with GAS at the core of abilities, combat, attributes and gameplay state.**

---

## 🎥 `GAMEPLAY.SHOWCASE`

<div align="center">

<a href="https://www.youtube.com/watch?v=sfk2eV6xraI">
<img src="https://img.youtube.com/vi/sfk2eV6xraI/maxresdefault.jpg" width="90%" alt="Advanced RPG Gameplay Showcase"/>
</a>

<br><br>

**▶ WATCH THE GAMEPLAY SHOWCASE**

</div>

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## ⚡ `SPECIAL.ABILITIES`

Custom GAS Ability Tasks power complex asynchronous gameplay sequences.

<table>
<tr>
<td width="33%" align="center">
<strong>PLUCK OF MANY</strong><br><br>
<img src="https://github.com/user-attachments/assets/ee7db30c-e9c8-47a2-b073-cfac9fa91455" width="100%" alt="Pluck of Many ability"/>
<br><br><sub>Clone spawning, placement, targeting and cleanup.</sub>
</td>
<td width="33%" align="center">
<strong>ALL-HIT WOLVES</strong><br><br>
<img src="https://github.com/user-attachments/assets/1b447033-16f4-4689-936e-09de068f49a8" width="100%" alt="All-Hit Wolves ability"/>
<br><br><sub>Multi-entity attack execution through a custom ability task.</sub>
</td>
<td width="33%" align="center">
<strong>CIRCLING WOLVES</strong><br><br>
<img src="https://github.com/user-attachments/assets/51e3e994-105e-42c1-af06-9422a1dba44c" width="100%" alt="Circling Wolves ability"/>
<br><br><sub>Timed movement, orbiting behavior and coordinated attacks.</sub>
</td>
</tr>
</table>

---

## ⚔️ `COMBAT.SYSTEM`

```text
INPUT
  │
  ▼
COMBAT ACTION
  │
  ▼
GAMEPLAY ABILITY
  │
  ├──────────────► Animation / Montage
  │
  ├──────────────► Targeting / Hit Detection
  │
  ├──────────────► Custom Ability Task
  │
  ▼
GAMEPLAY EFFECT
  │
  ▼
DAMAGE EXECUTION
  │
  ▼
ATTRIBUTES / GAMEPLAY TAGS
```

### Combat Features

| SYSTEM | IMPLEMENTATION |
|---|---|
| ⚔️ Weapons | Three weapon types with distinct attack sets |
| 🗡️ Attacks | Light / Heavy attacks and chained combos |
| 🛡️ Defense | Blocking and timing-based parry/counter |
| 💨 Mobility | Dodge / roll gameplay |
| 🎯 Targeting | Target lock and target-driven attacks |
| 💥 Damage | Gameplay Effects + custom execution calculation |
| ✨ Abilities | Weapon-specific and projectile-based abilities |

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🤖 `BOSS.ENCOUNTER`

A combat encounter combining player weapons, ranged enemy pressure, boss melee attacks and enemy summoning.

<table>
<tr>
<td width="50%" align="center">
<strong>PLAYER & WEAPONS</strong><br><br>
<img src="https://github.com/user-attachments/assets/a589f349-1ffd-4c2d-90ff-98933f3dfde4" width="100%" alt="Player and weapons"/>
<br><br><sub>Third-person character with three weapons available in the world.</sub>
</td>
<td width="50%" align="center">
<strong>RANGED ENEMY — HOMING PROJECTILES</strong><br><br>
<img src="https://github.com/user-attachments/assets/68afb58d-128d-4a4c-bdb2-5568181b985c" width="100%" alt="Ranged enemy homing projectiles"/>
<br><br><sub>Target-driven homing projectile attacks.</sub>
</td>
</tr>
<tr>
<td width="50%" align="center">
<strong>BOSS — MELEE ATTACK</strong><br><br>
<img src="https://github.com/user-attachments/assets/6b61dfa7-ead7-4cb4-ad9c-6096fe945631" width="100%" alt="Boss melee attack"/>
<br><br><sub>Close-range boss attack integrated into the encounter.</sub>
</td>
<td width="50%" align="center">
<strong>BOSS — ENEMY SUMMON</strong><br><br>
<img src="https://github.com/user-attachments/assets/a831e53f-3a83-446e-8194-26b6b0c35519" width="100%" alt="Boss enemy summon"/>
<br><br><sub>Boss summons ranged enemies to increase encounter pressure.</sub>
</td>
</tr>
</table>

### 🧠 `BOSS.AI`

<img width="100%" alt="Boss Behavior Tree" src="https://github.com/user-attachments/assets/8782707b-dd89-4ed3-bc94-9dcd44676e34" />

```text
PLAYER COMBAT
      ↓
RANGED ENEMY PRESSURE
      ↓
HOMING PROJECTILES
      ↓
BOSS ENCOUNTER
      ↓
MELEE ATTACK + SUMMON
      ↓
CLEAR SUMMONED ENEMIES
```

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🧠 `GAS.ARCHITECTURE`

The Gameplay Ability System acts as the backbone for ability execution, effects, attributes and gameplay state.

```text
┌───────────────────────┐
│ INPUT / GAMEPLAY EVENT│
└───────────┬───────────┘
            ↓
┌───────────────────────┐
│   GAMEPLAY ABILITY    │
└───────────┬───────────┘
            ↓
┌───────────────────────┐
│ CUSTOM ABILITY TASK   │
└───────────┬───────────┘
            ↓
┌───────────────────────┐
│ ANIMATION / TARGETING │
│ GAMEPLAY OPERATION    │
└───────────┬───────────┘
            ↓
┌───────────────────────┐
│   GAMEPLAY EFFECT     │
└───────────┬───────────┘
            ↓
┌───────────────────────┐
│ EXECUTION CALCULATION │
└───────────┬───────────┘
            ↓
┌───────────────────────┐
│     ATTRIBUTE SET     │
└───────────┬───────────┘
            ↓
   TAGS / CUES / UI
```

### 💥 Damage Pipeline

```text
Attack Ability
     ↓
Gameplay Effect Spec
     ↓
SetByCaller Damage / Attack Type
     ↓
Damage Execution Calculation
     ↓
Attack Power × Defence Power
     ↓
DamageTaken Attribute
     ↓
Health / Death State
```

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🧩 `CUSTOM.ABILITY.TASKS`

One of the main programming areas of Advanced RPG is extending GAS with reusable `UAbilityTask` implementations.

| Ability Task | Purpose |
|---|---|
| `AbilityTask_TransformCharacter` | Character replacement, montages, VFX, camera handling and control restoration |
| `AbilityTask_PluckOfMany` | Clone spawning, navigation-aware placement, staggered spawning and cleanup |
| `AT_HomingProjectiles` | Target-driven homing projectiles, damage specs and VFX |
| `AT_ExpandRingProjectiles` | Radial projectile patterns with configurable count, spacing and timing |
| `AT_WallProjectiles` | Configurable projectile formations using deferred spawning |
| `UAT_CircleRhythm` | Orbiting actors, periodic traces and timed gameplay events |
| `AbilityTask_WaitSpawnEnemies` | Asynchronous enemy-spawn orchestration |
| `AbilityTask_WaitSpawnBossEnemies` | Boss-specific asynchronous enemy spawning |

### Common Task Lifecycle

```text
Gameplay Ability
      ↓
Create Ability Task
      ↓
Start Async Gameplay Operation
      ↓
Delegates / Callbacks
      ↓
Ability Continues
      ↓
Task Ends / Cleans Up
```

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🤖 `ENEMY.AI`

Built using Unreal Engine's AI framework:

- **Behavior Trees** — high-level combat decision flow
- **EQS** — spatial queries and positioning
- **C++ AI Services / Tasks** — custom gameplay logic
- **Melee AI** — close-range combat behavior
- **Ranged AI** — projectile-based attacks
- **Boss AI** — attack and summon patterns

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## ✨ `VFX & GAMEPLAY`

The project combines gameplay programming with Niagara-driven presentation:

- Ability VFX
- Impact effects
- Projectile effects
- Transformation dissolve / form effects
- Combat feedback
- Gameplay-driven visual sequences

### Character Transformation

```text
Disable Movement
      ↓
Play Out Montage
      ↓
Dissolve / VFX
      ↓
Spawn Replacement Character
      ↓
Play In Montage
      ↓
Form VFX
      ↓
Restore Player Control
```

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🛠️ `TECH.ARSENAL`

<div align="center">

<img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white" />
<img src="https://img.shields.io/badge/UNREAL%20ENGINE%205.6-0E1128?style=for-the-badge&logo=unrealengine&logoColor=white" />
<img src="https://img.shields.io/badge/GAS-FF00FF?style=for-the-badge" />
<img src="https://img.shields.io/badge/ENHANCED%20INPUT-0E1128?style=for-the-badge" />
<img src="https://img.shields.io/badge/GAMEPLAY%20TAGS-0E1128?style=for-the-badge" />
<img src="https://img.shields.io/badge/BEHAVIOR%20TREES-0E1128?style=for-the-badge" />
<img src="https://img.shields.io/badge/EQS-0E1128?style=for-the-badge" />
<img src="https://img.shields.io/badge/NIAGARA-00F3FF?style=for-the-badge&logo=unrealengine&logoColor=black" />
<img src="https://img.shields.io/badge/GIT-F05032?style=for-the-badge&logo=git&logoColor=white" />

</div>

<div align="center">

| `C++ Gameplay` | `GAS` | `Combat` | `Ability Tasks` |
|:---:|:---:|:---:|:---:|
| `Behavior Trees` | `EQS` | `Niagara` | `Enhanced Input` |
| `Gameplay Tags` | `Gameplay Effects` | `Attribute Sets` | `Animation Montages` |

</div>

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 📚 `TECHNICAL.DOCUMENTATION`

| Document | Focus |
|---|---|
| [Architecture](Docs/Architecture.md) | High-level gameplay architecture and system flow |
| [Ability Tasks](Docs/AbilityTasks.md) | Custom GAS Ability Task design and responsibilities |
| [Combat](Docs/Combat.md) | Combat, damage and gameplay flow |
| [AI](Docs/AI.md) | Behavior Trees, EQS and enemy AI architecture |

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 📁 `CODEBASE.TOUR`

```text
Advanced-RPG/
│
├── Source/
│   ├── Character/        → Hero, enemy & character systems
│   ├── AbilitySystem/    → GAS abilities, tasks & attributes
│   ├── AI/               → Behavior Trees, services & tasks
│   ├── Components/       → Reusable gameplay components
│   ├── Weapons/          → Weapon and combat systems
│   └── UI/               → Gameplay UI systems
│
├── Docs/                 → Technical architecture documentation
└── README.md             → Project showcase
```

<img src="https://raw.githubusercontent.com/andreasbm/readme/master/assets/lines/colored.png" width="100%">

## 🎯 `WHAT.THIS.PROJECT.DEMONSTRATES`

```text
✓ Unreal Engine C++ gameplay programming
✓ Gameplay Ability System architecture
✓ Custom asynchronous GAS Ability Tasks
✓ Combat and damage system design
✓ Gameplay Effect / AttributeSet integration
✓ Behavior Tree + EQS enemy AI
✓ Boss encounter design
✓ Animation-driven gameplay
✓ Niagara gameplay VFX integration
✓ Reusable and modular gameplay systems
```

## 🚧 `PROJECT.STATUS`

**Active gameplay programming project.**

The project continues to evolve as a practical exploration of Unreal Engine C++, GAS, combat architecture, AI and gameplay systems.

---

<div align="center">

### ⚔️ Built with C++ • Unreal Engine 5.6 • Gameplay Ability System

**[⭐ Star the repository](https://github.com/Praveen703639/Advanced-RPG)** · **[🎮 View the code](https://github.com/Praveen703639/Advanced-RPG/tree/master/Source)**

</div>
