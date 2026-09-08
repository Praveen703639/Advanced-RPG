# Architecture

## High-Level Gameplay Flow

```text
Player Input / Gameplay Event
            ↓
     Gameplay Ability
            ↓
     Custom Ability Task
            ↓
 Animation / Targeting / Gameplay Operation
            ↓
      Gameplay Effect
            ↓
  Execution Calculation / Attributes
            ↓
     Gameplay Tags / UI / Cues
```

## Main C++ Areas

- `AbilitySystem/` — Gameplay Abilities, custom Ability Tasks, attributes and GAS components.
- `Characters/` — Hero, enemy and boss character implementations.
- `Components/Combat/` — reusable combat behavior and weapon interaction.
- `AI/` — AI controllers, Behavior Tree services/tasks and EQS integration.
- `Items/` — weapons, projectiles and pickups.
- `DataAssets/` — data-driven gameplay configuration.
- `Widgets/` — gameplay-facing UI.

## Design Goals

The project separates reusable gameplay operations from individual character classes. Complex asynchronous gameplay sequences are implemented as custom GAS Ability Tasks, while combat values and damage are routed through GAS attributes and Gameplay Effects.
