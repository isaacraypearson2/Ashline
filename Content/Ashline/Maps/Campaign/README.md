# Campaign maps (graybox)

**Playable now without .umap files.** `AAshlineGrayboxBuilder` spawns a unique
layout for ASH-01…ASH-12 at runtime (floors, walls, cover, lights, AI, objective
volumes, PlayerStart). Completing a mission saves via `UAshlineProgressionSubsystem`
and unlocks the next in the in-game campaign select.

Authored levels are optional. If you later save a `.umap` at the catalog path
(` /Game/Ashline/Maps/Campaign/ASH01_WireCut`, …), `TravelToMission` will open it;
otherwise GameMode rebuilds the runtime graybox in the current world.

Each runtime layout still uses `AAshlineObjectiveTrigger` ids from
`UAshlineMissionCatalog` (`INFIL`, `CUT`, `EXFIL`, …). Walk into the glowing
marker cubes to tick objectives. The last required / extract volume completes
the mission.

World settings GameMode: `AAshlineGameMode`.
