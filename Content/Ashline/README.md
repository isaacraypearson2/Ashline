# Content / Ashline

Graybox art only. Binary `.uasset` / `.umap` files are **optional**. The first
playthrough is **code-driven**:

- `UAshlineRuntimeInput` — keyboard / mouse / DualSense binds without IA assets
- `AAshlineGrayboxBuilder` — unique ASH-01…ASH-12 layouts + campaign hub
- `AAshlineHUD` — campaign select, objectives, mission complete

## Optional authored maps

| Asset path | Purpose |
| --- | --- |
| `/Game/Ashline/Maps/ASH_Playable` | Dedicated host world (see `Scripts/create_ashline_play_assets.py`) |
| `/Game/Ashline/Maps/Frontend/ASH_Frontend` | Custom operator / armory hub |
| `/Game/Ashline/Maps/Campaign/ASH01_WireCut` … `ASH12_Ashline` | Hand-built replacements |

If those packages are missing, Play still works: GameMode hosts the campaign
on `/Engine/Maps/Entry` and rebuilds each mission in-place.
