# Ashline AAA content pipeline (Phase 1 scaffolding)

The campaign is **playable without any marketplace binaries**. Install order and exact Fab names: **`Docs/PHASE2_FAB.md`**. This page is the folder / DataAsset contract.

## Folder layout (`Content/Ashline/`)

```
Content/Ashline/
  Characters/Hero/          MetaHuman or mannequin body/face
    Characters/Hero/Cosmetics/  SK_{CosmeticId} MeshOverride
    Characters/Hero/Parts/      SM_{Id} clothing (Helmet/Vest/…)
    Characters/AI/            Per-archetype meshes
    Characters/MetaHuman/     Bridge drop folder
    Weapons/Meshes/           FAB rifle/SMG/sniper/etc.
    Weapons/Materials/        M_{SkinId} MaterialOverride
  Weapons/Attachments/      Optics, muzzles
  Environments/
    ASH01_WireCut/ … ASH12_Ashline/
    Shared/                 Reused kit pieces
  Materials/PBR/            Master materials + instances
  Materials/Decals/         Bullet / dirt / signage
  Audio/Weapons/            Fire / reload / hit cues
  Audio/Footsteps/
  Audio/Music/              Per-mission beds + frontend
  Audio/Foley/
  FX/Muzzle/                Niagara (preferred) or Cascade
  FX/Impacts/
  UI/
  Data/Kits/                UAshlineEnvironmentKit / WeaponVisual / CharacterPresentation
  Maps/Campaign/            Authored .umap replacements (optional)
  Input/
```

Empty directories are intentional. **Nothing in those folders is a Quixel pack unless you put one there.**

## Runtime fallback chain

`UAshlinePresentationLibrary` resolves assets in this order:

1. Authored DataAsset at `/Game/Ashline/Data/Kits/…` (soft refs you assign after import)
2. **Starter Content** (`/Game/StarterContent/Materials/…`) if you added the pack
3. Engine materials / meshes (`DefaultMaterial`, `WorldGridMaterial`, `BasicShapes`, editor mannequin)

Maps never hard-crash when a Fab mesh is missing.

## DataAssets

| Class | Path convention | Purpose |
| --- | --- | --- |
| `UAshlineEnvironmentKit` | `/Game/Ashline/Data/Kits/DA_Kit_ASH01` … `ASH12` | Mood + ground/wall/foliage/decal + prop slots + music bed |
| `UAshlineWeaponVisual` | `/Game/Ashline/Data/Kits/DA_WPN_WPN_AR_ASH16` | World mesh, muzzle FX, impact decal, fire/reload/hit cues |
| `UAshlineCharacterPresentation` | `/Game/Ashline/Data/Kits/DA_Hero_Operator` | MetaHuman/mannequin + anim class |
| `UAshlineMetaCatalog` | `Content/Ashline/Data/Meta.json` + `ContentBindings.json` | Cosmetics, skins, rank curve, prestige. Soft refs auto-bound. |
| `UAshlineCosmeticVisual` | `/Game/Ashline/Data/Kits/DA_COS_*` / `DA_SKIN_*` | Optional overlay for Mesh/Material/PartMesh |

Create them with `Scripts/import_fab_kits.py` inside the editor (does **not** download Fab).

Project Settings → **Ashline Presentation** (`UAshlinePresentationSettings`) also holds default hero/AI mesh soft refs.

## Fab / Quixel Bridge (Phase 2 — you do this on the Windows PC)

These packs are **not in the repo**. Typical shopping list (swap for whatever you license):

| Need | Suggested Fab / Quixel search |
| --- | --- |
| Hero body | MetaHuman Creator → Bridge → `/Game/Ashline/Characters/MetaHuman/` |
| AI bodies | Same MetaHuman wardrobe, cheaper LODs, or military mannequin pack |
| Weapons | "military weapon pack UE5" / "modular assault rifle Nanite" |
| ASH-01 / 04 / 10 night | Megascans dirt, concrete walls, chainlink, flood lights |
| ASH-02 / 03 / 05 desert | Megascans sand, adobe, highway asphalt |
| ASH-06 harbor | Metal plates, crates, water, crane parts |
| ASH-07 snow | Snow ground, pine, ice |
| ASH-08 catacomb | Brick, pipes, server racks |
| ASH-09 ridge | Rock cliffs, grass, antenna |
| ASH-11 train | Rail, gravel, freight cars |
| ASH-12 finale | Brutalist concrete, emissive red trim |

Steps:

1. Epic Launcher / Fab → add to UE 5.8 vault.
2. Quixel Bridge → destination `/Game/Ashline/Environments/<mission>/`.
3. Open the matching `DA_Kit_ASH##` and assign Ground / Wall / PropMeshes / MusicBed.
4. PIE the mission. Gameplay volumes stay where the C++ builder placed them unless you author a `.umap` and set `FAshlineMissionDefinition.MapAsset`.

## Starter Content (recommended today)

Editor → **Add Feature or Content Pack** → Starter Content.

Immediately used if present:

- `M_Ground_Grass`, `M_Concrete_Tiles`, `M_Metal_Steel`, `M_Wood_*`, `M_Water_Ocean`, `M_Rock_Marble`

No git LFS required.

## Editor scripts

| Script | What it does |
| --- | --- |
| `Scripts/create_ashline_play_assets.py` | Optional `ASH_Playable` map + IA/IMC |
| `Scripts/import_fab_kits.py` | Creates DataAsset stubs + folder tree; prints the Fab list |

Run: **Tools → Execute Python Script**.
