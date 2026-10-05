# Lanternfall / Unreal Engine 5.4

A compact first-person RPG about allocating a town's last reserve power circuit. Two energy cells can restore a field clinic or call for outside help through the relay. The destination changes the dialogue, objective and reward.

The C++ gameplay systems, Blueprint graph generator and original courtyard scene are implemented. Native engine compilation and gameplay recording are in progress; downloadable gameplay and builds will be linked when available.

## Playable story

1. Speak with Nera and choose the clinic or the relay.
2. Explore the courtyard and collect two unique energy cells.
3. Connect the cells to the chosen terminal.
4. Return to Nera to resolve the choice and receive its outcome.

You can change the destination before spending the cells. The inventory tracks collected items and rewards. Autosave preserves progress after each successful change; manual save/load also restores the player position and collected world objects.

## C++ and Blueprint

`Core/Journey` is an engine-independent state machine that validates quest transitions, inventory counts and save snapshots before changing state. `JourneySubsystem` exposes those systems to Blueprint and persists them through USaveGame.

The Blueprint courier calls C++ from BeginPlay. The Nera, energy-cell and terminal Blueprints override Interact and call their native action. The editor-only PortfolioForge plugin builds and compiles the event graphs and saves the courtyard map, material and Blueprint classes as native assets.

## Build and launch

```bash
python3 Tools/portfolio.py --engine /path/to/UE5.4 --run
```

Use Unreal Engine 5.4. Add `--package` for a Development build or `--capture` for an MP4 recorded from the engine viewport. Capture requires FFmpeg and a working graphics renderer. The scripted demonstration follows the clinic route; `-RelayRoute` selects the other branch when launching the game manually.

Controls: WASD move, mouse look, E interact, 1–3 dialogue choice, Tab journal, F6 save, F7 load and Space jump.

## Gameplay checks

```bash
cmake -S Checks -B build
cmake --build build
./build/journey_checks
```

Forty-eight assertions cover both story branches, cell requirements, unique pickups, destination validation, reward counts, surplus inventory, atomic failed actions and invalid save snapshots. These checks validate the portable C++ state machine; they do not establish native runtime behavior.

The environment uses original procedural geometry and engine primitive meshes. The project has no paid asset dependencies.
