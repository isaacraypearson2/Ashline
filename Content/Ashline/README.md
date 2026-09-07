# Content / Ashline

**Pipeline folders, not a Quixel dump.** Binary `.uasset` / `.umap` files are optional. Play is code-driven.

Runtime:

- `UAshlineRuntimeInput` — keyboard / mouse / gamepad without IA assets
- `AAshlineGrayboxBuilder` — unique ASH-01…ASH-12 layouts + themed lighting / materials / dressing
- `UAshlinePresentationLibrary` — DataAsset → StarterContent → Engine fallbacks
- `AAshlineHUD` — campaign select, objectives, mission complete

## Honest status

| Path | In git? | What belongs here |
| --- | --- | --- |
| `Data/*.json` | Yes | Designer mirrors of C++ catalogs |
| `Data/Kits/` | Stubs only | `UAshlineEnvironmentKit` / weapon / hero DataAssets |
| `Characters/`, `Weapons/`, `Environments/` | Folders + README | Fab / MetaHuman / Megascans **after you import** |
| `Maps/ASH_Playable` | No (optional script) | Dedicated host world |

See `Docs/CONTENT_PIPELINE.md`. **Do not assume Megascans are cooked into this clone.**

## Optional authored maps

| Asset path | Purpose |
| --- | --- |
| `/Game/Ashline/Maps/ASH_Playable` | Dedicated host world (`Scripts/create_ashline_play_assets.py`) |
| `/Game/Ashline/Maps/Frontend/ASH_Frontend` | Custom operator / armory hub |
| `/Game/Ashline/Maps/Campaign/ASH01_WireCut` … `ASH12_Ashline` | Hand-built replacements |

If those packages are missing, Play still works on `/Engine/Maps/Entry`.
