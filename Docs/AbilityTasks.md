# Custom GAS Ability Tasks

The project extends Unreal Engine's Gameplay Ability System with custom `UAbilityTask` implementations for gameplay that would otherwise become large, tightly coupled ability functions.

## Examples

| Task | Responsibility |
|---|---|
| `AbilityTask_TransformCharacter` | Coordinates character replacement, montages, VFX, camera handling and control restoration. |
| `AbilityTask_PluckOfMany` | Handles clone spawning, placement, timing and cleanup. |
| `AT_HomingProjectiles` | Creates target-driven homing projectiles and configures damage/VFX. |
| `AT_ExpandRingProjectiles` | Generates radial projectile patterns with configurable timing and spacing. |
| `AT_WallProjectiles` | Creates projectile formations using deferred actor spawning. |
| `UAT_CircleRhythm` | Coordinates orbiting actors, traces and timed gameplay events. |
| `AbilityTask_WaitSpawnEnemies` | Provides asynchronous enemy-spawn orchestration. |

## Common Lifecycle

```text
Gameplay Ability
      ↓
Create Task
      ↓
Activate Task
      ↓
Start asynchronous operation
      ↓
Callbacks / Delegates
      ↓
Broadcast result
      ↓
End Task / Ability continues
```

## Why Custom Tasks?

They keep abilities focused on gameplay intent while reusable asynchronous operations own their timers, delegates, spawning and completion behavior. This makes complex combat sequences easier to reuse and reason about.
