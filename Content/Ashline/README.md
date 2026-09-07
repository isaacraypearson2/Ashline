# Content / Ashline

Graybox art only. These folders are the intended editor layout. Binary `.uasset` / `.umap` files are created when you open the project in Unreal Editor 5.8.2 and save maps, input assets, and widgets.

## Create these maps first

| Asset path | Purpose |
| --- | --- |
| `/Game/Ashline/Maps/Frontend/ASH_Frontend` | Operator, armory, campaign select, crates, settings |
| `/Game/Ashline/Maps/Campaign/ASH01_WireCut` | ASH-01 |
| `/Game/Ashline/Maps/Campaign/ASH02_DustMarket` | ASH-02 |
| `/Game/Ashline/Maps/Campaign/ASH03_Holdfast` | ASH-03 |
| `/Game/Ashline/Maps/Campaign/ASH04_NightGlass` | ASH-04 |
| `/Game/Ashline/Maps/Campaign/ASH05_ConvoyGhost` | ASH-05 |
| `/Game/Ashline/Maps/Campaign/ASH06_AshHarbor` | ASH-06 |
| `/Game/Ashline/Maps/Campaign/ASH07_Whiteout` | ASH-07 |
| `/Game/Ashline/Maps/Campaign/ASH08_Catacomb` | ASH-08 |
| `/Game/Ashline/Maps/Campaign/ASH09_RidgeWire` | ASH-09 |
| `/Game/Ashline/Maps/Campaign/ASH10_FalseFlag` | ASH-10 |
| `/Game/Ashline/Maps/Campaign/ASH11_LastTrain` | ASH-11 |
| `/Game/Ashline/Maps/Campaign/ASH12_Ashline` | ASH-12 finale |

Drop `AAshlineObjectiveTrigger` volumes into each campaign map and set `ObjectiveId` to the ids in `UAshlineMissionCatalog`.

## Input assets to create

Under `/Game/Ashline/Input/`:

- `IMC_Ashline_KBM`, `IMC_Ashline_Gamepad`, `IMC_Ashline_Touch`
- Actions: `IA_Move`, `IA_Look`, `IA_Jump`, `IA_Fire`, `IA_Aim`, `IA_Reload`, `IA_CameraToggle`, `IA_SwapWeapon`, `IA_Crouch`

Assign them on `AAshlineCharacter` and `AAshlinePlayerController` defaults (or a Blueprint subclass).
