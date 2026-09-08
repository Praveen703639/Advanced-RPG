# Enemy AI

The project uses Unreal Engine's AI framework to drive combat decision making for enemies and boss encounters.

## Stack

- Behavior Trees for high-level decision flow.
- Custom C++ Behavior Tree services and tasks.
- EQS for spatial queries and positioning.
- Melee and ranged enemy behaviors.
- Boss-specific combat abilities and summon patterns.

## Boss Encounter Flow

```text
Player Combat
     ↓
Ranged Enemy Pressure
     ↓
Homing Projectile Attacks
     ↓
Boss Encounter
     ↓
Boss Melee + Enemy Summoning
     ↓
Player Clears Summoned Enemies
```

The repository README contains a visual capture of the actual boss Behavior Tree used by the encounter.
