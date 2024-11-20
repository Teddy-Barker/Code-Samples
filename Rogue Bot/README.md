# Rogue-Bot Code Samples

This repository contains three key scripts from the game *Rogue-Bot*. Each script highlights specific gameplay mechanics and features developed for the game.

## Scripts Overview

### 1. `PlayerShoot.cs`
- **Purpose**: Handles the player's shooting mechanics.
- **Key Features**:
  - Manages ammo consumption and fire rate.
  - Spawns projectiles at a designated shooting point.
  - Prevents shooting when UI elements like inventory or skill trees are active.

---

### 2. `EnemyMovement.cs`
- **Purpose**: Controls basic enemy behavior and movement.
- **Key Features**:
  - Tracks the player's position and enables movement when in range.
  - Includes health management and visual feedback when damaged.
  - Spawns a money item upon death if applicable.

---

### 3. `ChainBotController.cs`
- **Purpose**: Governs the advanced behavior of a specific enemy type, the Chain Bot.
- **Key Features**:
  - Animates movement and death sequences.
  - Synchronizes weapon effects with the enemy's state.
  - Plays movement sounds and footsteps based on speed.
  - Implements death logic, including disabling AI and collisions.

---

## About
These scripts demonstrate gameplay programming in Unity, showcasing:
- Player mechanics.
- AI behavior and state management.
- Integration of animations, sound, and visual effects.
