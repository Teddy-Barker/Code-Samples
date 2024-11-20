# Judgment Protocol Code Samples

This repository contains two key scripts from the game *Judgment Protocol*. These scripts demonstrate gameplay programming concepts, including audio management, dialogue systems, and decision-making logic.

## Scripts Overview

### 1. `MusicController.cs`
- **Purpose**: Manages the game's background music transitions between different states (e.g., menu and gameplay).
- **Key Features**:
  - Implements a singleton pattern to ensure a single instance across scenes.
  - Handles smooth crossfade transitions between audio tracks.
  - Supports persistent music management across scene transitions.

---

### 2. `GameController.cs`
- **Purpose**: Manages the core gameplay logic, including interactions, decision-making, and the game's outcome.
- **Key Features**:
  - Handles the flow of interviewing characters (humans or AI) and player decisions.
  - Manages dialogue, including typing animations for questions and responses.
  - Tracks game state, including admitted robots, denied humans, and overall progression.
  - Controls character movement, including sine wave animations and fall behavior for denied characters.

---

## About
These scripts showcase key elements of *Judgment Protocol*, highlighting:
- Audio management for immersive transitions.
- A complex decision-making system tied to player interaction.
- Dialogue systems with typing animations and dynamic responses.

## Play the Game
Check out the full game on Itch.io: [Judgment Protocol](https://henry-barker.itch.io/judgment-protocol)
