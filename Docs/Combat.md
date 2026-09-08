# Combat System

## Damage Flow

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
Health / Death State
```

## Player Combat

- Three weapon types with distinct attack sets.
- Light and heavy attack chains.
- Combo progression and finishers.
- Blocking and timing-based parry/counter behavior.
- Dodge/roll with invulnerability-frame behavior.
- Target lock for focused combat.
- Weapon-specific special abilities.

## Projectile Systems

Custom Ability Tasks support multiple projectile patterns, including homing, wall formations and expanding radial patterns. Projectile creation is kept separate from the character class so abilities can configure the operation without owning all of its implementation details.

## Data-Driven Configuration

Ability and startup configuration is exposed through Data Assets, allowing gameplay setup to be changed without hard-coding every configuration value into individual abilities.
