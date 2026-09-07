# Phase 2 shopping list — install on `X3D_AMD` (Windows 11, UE 5.8)

**Prefer `Docs/PHASE2_FAB.md`** (install order, clothing/skin paths, 1440p Ultra notes). This page is the short list.

Nothing below is in git. After install, drop or migrate assets onto the **canonical paths** in `UAshlineContentManifest` / `ContentBindings.json`. Soft refs / DataAssets pick them up with **no architecture rewrite**.

Run `Scripts/import_fab_kits.py` once in the editor to create the folder tree + empty DataAssets.

## 0. Do this first (all free, in-engine)

| # | What | How | Lands / bind to |
| --- | --- | --- | --- |
| 0.1 | **Starter Content** | Editor → Add → Add Feature or Content Pack → Starter Content | Auto-used by `UAshlinePresentationLibrary` (grass, concrete, steel, water, explosion particle, audio) |
| 0.2 | **Enhanced Input** | Already enabled in `Ashline.uproject` | — |
| 0.3 | **AMD FSR for UE 5.8** | Download from [GPUOpen FSR UE 5.8](https://gpuopen.com/learn/amd-fsr-plugin-updated-for-unreal-engine-58/) (RDNA4 / RX 9000 path). Older Fab listing [FSR 3](https://www.fab.com/listings/0a68f49c-695c-4c85-86f7-18a7244296c2) is 5.1–5.4 only. | Enables `r.FidelityFX.FSR3.*`. Without it, Ultra uses **TSR**. |
| 0.4 | Confirm DX12 | Project Settings already set. PIE HUD should say D3D12. | `AshPCUltra` |

## 1. Characters (free-first)

| Priority | Pack | Price | URL / source | Copy onto |
| --- | --- | --- | --- | --- |
| **P0** | **MetaHuman Creator** + MetaHuman plugin | Free (Epic account) | [MetaHuman](https://www.unrealengine.com/metahuman) + Bridge | `/Game/Ashline/Characters/MetaHuman/` then assign body to `SK_AshlineHero` **or** `DA_Hero_Operator.BodyMesh` |
| **P0** | **Game Animation Sample (GASP) 5.8** | Free | [Fab listing](https://www.fab.com/listings/880e319a-a59e-4ed2-b268-b32dac7fa016) | Migrate `SKM_Manny` / UEFN mannequin + Motion Matching AnimBP → hero `AnimClass` |
| **P1** | Third Person template mannequin | Free (create a TP project, migrate) | Epic Launcher templates | `/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple` (already in resolve list) |
| **P2 optional paid** | **US Marine GRENADIER - Modular** (MetaHuman + Manny compatible) | Paid (~$36–$81) | [Fab listing](https://www.fab.com/listings/0059b2fd-9dfd-40db-ad9a-c0e720056b34) | `/Game/Ashline/Characters/AI/SK_AI_Rifleman` (+ Officer/Heavy variants) |

Steps for MetaHuman: `Docs/CHARACTERS.md`.

## 2. Weapons (free-first, then one paid pack)

| Priority | Pack | Price | URL / source | Bind |
| --- | --- | --- | --- | --- |
| **P0** | Compound placeholder already in C++ | — | In-repo | Looks like a rifle in FPS today |
| **P1 free** | **Lyra Starter Game** weapon meshes + fire audio (migrate, do not replace the project) | Free | [Lyra](https://www.unrealengine.com/marketplace/en-US/learn/lyra) | `SM_WPN_AR_ASH16` etc. + `SC_Fire_*` |
| **P2 optional paid** | **Modern Firearms & Explosives Pack** (40 weapons, muzzle VFX, Lyra audio) | Paid | [Fab listing](https://www.fab.com/listings/b6e3d970-e841-4bc7-ad23-c4d07126eb1f) | Map 7 Ashline IDs → `UAshlineWeaponVisual` |

Canonical mesh names (`UAshlineContentManifest::WeaponMeshPath`):

```
/Game/Ashline/Weapons/Meshes/SM_WPN_AR_ASH16
/Game/Ashline/Weapons/Meshes/SM_WPN_SMG_C9
/Game/Ashline/Weapons/Meshes/SM_WPN_SNP_G28L
/Game/Ashline/Weapons/Meshes/SM_WPN_SHG_M870K
/Game/Ashline/Weapons/Meshes/SM_WPN_PIS_M17A
/Game/Ashline/Weapons/Meshes/SM_WPN_DMR_SASS
/Game/Ashline/Weapons/Meshes/SM_WPN_LMG_M250
```

Muzzle Niagara: `/Game/Ashline/FX/Muzzle/NS_Muzzle_<WeaponId>`  
Impact decal: `/Game/Ashline/Materials/Decals/M_Impact_Bullet`  
Fire cue: `/Game/Ashline/Audio/Weapons/SC_Fire_<WeaponId>`

## 3. Environments — Megascans / Fab (free-first)

Quixel Megascans live on **Fab**. Claim what your Epic/Fab license still allows (library promotions change). Search these **exact collection names** and drop surfaces into the mission folder.

| Mission | Search on Fab (free-first) | Drop folder | DataAsset |
| --- | --- | --- | --- |
| ASH-01 Wire Cut | "Forest Floor", "Dirt Ground", "Concrete Wall", "Chainlink", "Flood Light" | `/Game/Ashline/Environments/ASH01_WireCut/` | `DA_Kit_ASH01` |
| ASH-02 Dust Market | "Desert Sand", "Adobe", "Market Stall", "Fabric Canopy" | `.../ASH02_DustMarket/` | `DA_Kit_ASH02` |
| ASH-03 Holdfast | "Sandbag", "Desert Ground", "Dry Grass" | `.../ASH03_Holdfast/` | `DA_Kit_ASH03` |
| ASH-04 Night Glass | "Brick Industrial", "Painted Steel", "Water Tower" | `.../ASH04_NightGlass/` | `DA_Kit_ASH04` |
| ASH-05 Convoy Ghost | "Asphalt", "Tanker", "Roadside Rock" | `.../ASH05_ConvoyGhost/` | `DA_Kit_ASH05` |
| ASH-06 Ash Harbor | "Metal Plate", "Shipping Crate", "Ocean Water", "Crane" | `.../ASH06_AshHarbor/` | `DA_Kit_ASH06` |
| ASH-07 Whiteout | "Snow Ground", "Pine Tree", "Ice" | `.../ASH07_Whiteout/` | `DA_Kit_ASH07` |
| ASH-08 Catacomb | "Brick Tunnel", "Industrial Pipe", "Server Rack" | `.../ASH08_Catacomb/` | `DA_Kit_ASH08` |
| ASH-09 Ridge Wire | "Cliff Rock", "Mountain Grass", "Antenna Tower" | `.../ASH09_RidgeWire/` | `DA_Kit_ASH09` |
| ASH-10 False Flag | "Urban Concrete", "Office Glass", "Signage Decal" | `.../ASH10_FalseFlag/` | `DA_Kit_ASH10` |
| ASH-11 Last Train | "Rail Track", "Gravel Ballast", "Freight Car" | `.../ASH11_LastTrain/` | `DA_Kit_ASH11` |
| ASH-12 Ashline | "Brutalist Concrete", "Emissive Panel", "Bunker Door" | `.../ASH12_Ashline/` | `DA_Kit_ASH12` |

Ground material name the resolver also tries: `M_Ground_<MissionSlug>` inside that folder.

**Do not claim Megascans are in this repo.** If Fab still requires a paid seat for a scan, skip it — Starter Content + lighting already play.

## 4. Audio / FX (free-first)

| Pack | Price | Bind |
| --- | --- | --- |
| Starter Content `Explosion01` / `Fire01` / `P_Explosion` | Free | Already in fallback lists |
| Lyra weapon fire / hit / reload waves | Free | `Content/Ashline/Audio/Weapons/` |
| MetaSounds gunshot (engine examples) | Free | Same slots |
| Paid: any "Military Foley" pack | Optional | Footsteps + music beds `SC_Bed_ASH01`…`12` |

## 5. After each import (no code change)

1. Rename or duplicate the imported asset to the canonical path **or**
2. Open `DA_Kit_ASH##` / `DA_WPN_*` / `DA_Hero_Operator` and set the soft pointer.
3. Optional: Project Settings → **Ashline Presentation**.
4. PIE. Campaign loop is unchanged.

## Honesty

Fab listing titles and prices move. URLs above were current when this PR was written. If a listing 404s, search the **bold name**. Never commit Quixel/Fab binaries unless the license allows and you intend LFS.
