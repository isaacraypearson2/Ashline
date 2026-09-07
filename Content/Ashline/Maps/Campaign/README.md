# Campaign maps (graybox)

Create one Unreal level per mission. Geometry is blockout only — no authored art ships in this repo.

Place:

- Player start
- `AAshlineAICharacter` with archetypes from `UAshlineAICatalog`
- `AAshlineObjectiveTrigger` volumes matching catalog objective ids (`INFIL`, `CUT`, `EXFIL`, …)

World settings GameMode override: `AAshlineGameMode`. Set `ActiveMission` on the map's GameMode default to the matching `EAshlineMissionId`.
