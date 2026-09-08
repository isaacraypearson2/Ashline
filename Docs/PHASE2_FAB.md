# Phase 2 — Fab / Quixel / MetaHuman install bible (`X3D_AMD`)

Windows 11 · UE **5.8.2** · Ryzen 5 7500X3D · 32 GB · **RX 9070 GRE** · 2560×1440.

This is the **one-evening install order** plus the full pack map. Soft refs are wired in `UAshlineMetaCatalog`, `UAshlineContentManifest`, `UAshlineWeaponComponent`, `UAshlineMaterialFactory`, and `AAshlineGrayboxBuilder`. **Nothing below is in git.** Campaign play, locker, economy, and prestige work without these binaries.

Exact path map: `Content/Ashline/Data/ContentBindings.json`.  
Master parameter contract: **`Docs/MATERIALS.md`**.  
Printable schedule (no UE): `python3 Scripts/fab_install_plan.py`.

## Honesty — what cannot ship in this repo

| Asset class | Why it is not in git | What the repo does instead |
| --- | --- | --- |
| MetaHuman bodies / wardrobes | Epic license + multi-GB | Soft refs + mannequin interim |
| Quixel Megascans / Fab environment kits | License + size | C++ moods + Starter Content fallback |
| Fab military weapon packs | License + size | Compound FPS placeholders + `DA_WPN_*` |
| Authored Niagara / MetaSounds / `.umap` art | Binary / not portable | Slots + Engine / Starter fallbacks |
| `.uasset` DataAssets | Require the editor to create | Python stubs on the Windows box |

If a listing 404s, search the **bold name**. Never commit Quixel/Fab/MetaHuman binaries unless the license allows **and** you intend Git LFS.

## One evening — do this in order (do not skip 0)

Target: first public-looking PIE of all 12 missions before midnight. Paid packs are optional; free path is enough to look like a game.

| Clock | Block | What you finish |
| --- | --- | --- |
| **0:00–0:25** | Engine + scripts | Starter Content, folder tree, **master materials**, mannequin interim |
| **0:25–1:20** | Characters | MetaHuman body/face + GASP 5.8 locomotion AnimBP |
| **1:20–2:10** | Weapons + libraries | Lyra (free) or firearms pack + `M_Weapon_Master` skins |
| **2:10–4:10** | Megascans | ASH-01 / 02 / 07 / 12 first, then glass (04/10), then the rest |
| **4:10–5:00** | Audio / FSR / PIE | Fire cues, FSR 5.8, `stat fps` / `stat streaming` on Ultra |

### 0 — Engine content (free, in-editor, do first)

1. Open `Ashline.uproject` in UE 5.8.2. Compile `AshlineEditor` Win64 Development.
2. **Add Feature or Content Pack → Starter Content.** Immediate PBR upgrade (grass, concrete, steel, water).
3. Optional but recommended: create a throwaway **Third Person** template project and **Migrate** `SKM_Manny` / `SKM_Quinn` into this project under `/Game/Characters/Mannequins/`.
4. Tools → Execute Python Script:
   1. `Scripts/import_fab_kits.py` — folder tree + `DA_Kit_*` / `DA_WPN_*` / `DA_COS_*` / `DA_AI_*` stubs with canonical soft paths. **Also runs `create_master_materials.py`.**
   2. `Scripts/create_master_materials.py` — `M_Env_Master`, `M_Weapon_Master`, `M_Character_Master`, `M_Glass_Master`, `M_Skin_Master`, `M_Decal_Master` + surface MIs. Safe to re-run (skips existing).
   3. `Scripts/assign_interim_meshes.py` — first available mannequin → hero + all AI DataAssets. **Capsules stay hidden.** Starter materials stamp onto kits; **mood stays C++** (`bOverrideMood` remains false).
5. PIE. You should see a mannequin (or tinted humanoid blockout), not a capsule. ASH-01 is still night. Surfaces use masters + Engine fallback textures even with zero Megascans.

### 0b — Material libraries (free-first, after masters)

| # | Pack | Price | Drop / bind |
| --- | --- | --- | --- |
| 0b.1 | Masters created above | — | `/Game/Ashline/Materials/PBR/Masters` |
| 0b.2 | Starter Content PBR | Free | Already in `GetSurfaceMaterial` fallbacks |
| 0b.3 | **Megascans surface collection** (dirt/concrete/metal/snow) | Epic/Fab license | Instances of `M_Env_Master` — plug ORM + Normal + Albedo |
| 0b.4 | Military camo / fabric library (optional) | Paid | `/Game/Ashline/Materials/Libraries/` → clothing MIs |
| 0b.5 | Glass / window pack (optional) | Free/paid | Duplicate `M_Glass_Master` → `MI_Glass_Clear` |

Parameter aliases (Quixel ORM **and** Fab ARM): `Docs/MATERIALS.md`. C++ stamps every alias onto MIDs.

### 1 — Characters (free-first)

| # | Pack | Price | Source | Drop / bind |
| --- | --- | --- | --- | --- |
| 1.1 | **MetaHuman** plugin + Creator | Free (Epic) | [MetaHuman](https://www.unrealengine.com/metahuman) · Bridge | `/Game/Ashline/Characters/MetaHuman/` then body → `DA_Hero_Operator.BodyMesh` **or** `SK_AshlineHero` |
| 1.2 | **Game Animation Sample (GASP) 5.8** | Free | [Tech blog](https://www.unrealengine.com/tech-blog/download-the-latest-game-animation-sample-project-now-updated-for-ue-5-8) · Fab search **Game Animation Sample** | Migrate `SKM_UEFN_Mannequin` + Motion Matching AnimBP → `DA_Hero_Operator.AnimClass` |
| 1.3 | Third Person mannequin | Free | Epic template migrate | Already in `HumanoidMeshCandidates` |
| 1.4 | **MetaHuman Crowds Sample** (optional AI variants) | Free | Fab / 5.8 plugin content | Duplicate cheaper LODs → `SK_AI_Rifleman` … `SK_AI_Irregular` |
| 1.5 | US Marine GRENADIER — Modular (optional) | Paid | Fab search **US Marine GRENADIER** | Clothing parts onto `Characters/Hero/Parts/{Helmet,Vest,Pants,Gloves,Boots}` |

MetaHuman steps: `Docs/CHARACTERS.md`. Clothing part file names must match `ContentBindings.json` (`SM_HELM_FAST`, `SM_VEST_PLATE`, …).

Skin / subsurface: assign `DA_Hero_Operator.SkinMaterial` → `M_Skin_Master` (or the MetaHuman body material). Face textures go on `M_Skin_Master` (`BaseColor`, `Normal`, `Subsurface`).

GASP AnimBP: set `DA_Hero_Operator.AnimClass` **and** the same class on each `DA_AI_*` you want animated. Runtime applies `AnimClass` when the DataAsset is present; T-pose is the honest fallback.

### 2 — Weapons + skins (free-first)

| # | Pack | Price | Bind |
| --- | --- | --- | --- |
| 2.1 | In-repo compound receiver/barrel/stock/mag | — | Already FPS-readable |
| 2.2 | **Lyra Starter Game** weapon meshes + fire waves | Free | Rename/migrate to `SM_WPN_AR_ASH16` … `SM_WPN_LMG_M250` + `SC_Fire_*` |
| 2.3 | Modern Firearms & Explosives Pack (optional) | Paid | Same seven IDs on `DA_WPN_*` |

Skin materials (drop or duplicate MI):

```
/Game/Ashline/Weapons/Materials/M_SKIN_FACTORY
/Game/Ashline/Weapons/Materials/M_SKIN_FDE
/Game/Ashline/Weapons/Materials/M_SKIN_OD
/Game/Ashline/Weapons/Materials/M_SKIN_SNOW
/Game/Ashline/Weapons/Materials/M_SKIN_GOLD
/Game/Ashline/Weapons/Materials/M_SKIN_ASH16_NIGHT
… (every id in ContentBindings.json → skins)
```

`UAshlineMetaCatalog` already points `MaterialOverride` at those paths. Equipping `SKIN_FDE` after the MI exists tints/paints the gun with **no C++ change**.

Create each skin as an MI of **`M_Weapon_Master`**. Plug packed ORM + normal from the weapon pack; C++ still stamps `Tint` from the catalog if the MI has that parameter. `DA_WPN_*.TextureSet` can point at the raw textures if you prefer not to author an MI.

### 3 — Environments (Megascans on Fab)

Quixel lives on **Fab**. Claim what your Epic license still allows. Search the **bold** names. Destination is the mission folder; then either rename to the canonical material or assign the soft ptr on `DA_Kit_ASH##`.

Leave `bOverrideMood` **unchecked** unless you authored a custom mood. Empty kit DAs no longer flatten lighting to default daylight.

| Mission | Fab / Quixel search (free-first) | Drop folder | Canonical materials |
| --- | --- | --- | --- |
| ASH-01 Wire Cut | **Forest Floor**, **Dirt Ground**, **Concrete Wall**, **Chainlink**, **Flood Light** | `/Game/Ashline/Environments/ASH01_WireCut/` | `M_Ground_ASH01_WireCut`, `M_Wall_*`, `M_Foliage_*` |
| ASH-02 Dust Market | **Desert Sand**, **Adobe**, **Market Stall**, **Fabric Canopy** | `ASH02_DustMarket/` | same pattern |
| ASH-03 Holdfast | **Sandbag**, **Desert Ground**, **Dry Grass** | `ASH03_Holdfast/` | |
| ASH-04 Night Glass | **Brick Industrial**, **Painted Steel**, **Water Tower** | `ASH04_NightGlass/` | |
| ASH-05 Convoy Ghost | **Asphalt**, **Tanker**, **Roadside Rock** | `ASH05_ConvoyGhost/` | |
| ASH-06 Ash Harbor | **Metal Plate**, **Shipping Crate**, **Ocean Water**, **Crane** | `ASH06_AshHarbor/` | |
| ASH-07 Whiteout | **Snow Ground**, **Pine Tree**, **Ice** | `ASH07_Whiteout/` | |
| ASH-08 Catacomb | **Brick Tunnel**, **Industrial Pipe**, **Server Rack** | `ASH08_Catacomb/` | |
| ASH-09 Ridge Wire | **Cliff Rock**, **Mountain Grass**, **Antenna Tower** | `ASH09_RidgeWire/` | |
| ASH-10 False Flag | **Urban Concrete**, **Office Glass**, **Signage Decal** | `ASH10_FalseFlag/` | |
| ASH-11 Last Train | **Rail Track**, **Gravel Ballast**, **Freight Car** | `ASH11_LastTrain/` | |
| ASH-12 Ashline | **Brutalist Concrete**, **Emissive Panel**, **Bunker Door** | `ASH12_Ashline/` | |

**Glass (ASH-04 Night Glass, ASH-06 harbor, ASH-10 campus):** duplicate `M_Glass_Master` → `M_Glass_<Slug>` in the mission folder, **or** set `DA_Kit_ASH##.GlassMaterial`. Opacity ~0.28, IOR 1.52.

Kit DataAsset slots: Ground / Wall / Trim / Foliage / Glass / Decal / `PropMeshes` (`Tree`, `Bush`). Music bed: `/Game/Ashline/Audio/Music/SC_Bed_ASH##`.

Do **ASH-01, ASH-02, ASH-07, ASH-12** first (night dirt, desert, snow, finale). Those four sell the campaign. Then glass missions. Then the rest.

### 4 — Audio / FX (free-first)

| Pack | Bind |
| --- | --- |
| Starter `P_Explosion` / `P_Fire` / `Explosion01` | Already in fallback lists |
| Lyra fire / hit / reload | `Content/Ashline/Audio/Weapons/SC_Fire_{WeaponId}` |
| MetaSounds gunshot examples | Same slots |
| Optional military foley | Footsteps + `SC_Bed_ASH01`…`12` |

### 5 — Upscaler plugin

| Pack | Note |
| --- | --- |
| **AMD FSR for UE 5.8** | [GPUOpen FSR UE 5.8](https://gpuopen.com/learn/amd-fsr-plugin-updated-for-unreal-engine-58/). Older Fab **FSR 3** listing is 5.1–5.4 only. |
| Without the plugin | `Ashline_PC_Ultra` uses **TSR**. That is supported. |

### 6 — After each import (no architecture rewrite)

1. Rename or duplicate onto the **ContentBindings** path, **or**
2. Open `DA_COS_*` / `DA_SKIN_*` / `DA_WPN_*` / `DA_Kit_ASH##` / `DA_Hero_Operator` and set the soft pointer.
3. Optional: Project Settings → **Ashline Presentation**.
4. PIE. Campaign loop, credits, prestige unchanged.

Re-run `assign_interim_meshes.py` any time you add GASP/Manny and want every AI DA updated.

## Folder layout (git keeps folders + README, not binaries)

```
Content/Ashline/
  Characters/Hero/                 SK_AshlineHero
  Characters/Hero/Cosmetics/       SK_{CosmeticId}          ← MeshOverride
  Characters/Hero/Parts/{Slot}/    SM_{CosmeticId}          ← clothing parts
  Characters/AI/                   SK_AI_{Archetype}
  Characters/MetaHuman/            Bridge drop
  Weapons/Meshes/                  SM_{WeaponId}
  Weapons/Materials/               M_{SkinId}               ← MaterialOverride
  Weapons/Charms/                  SM_CHARM_*
  Materials/PBR/Masters/          M_Env_Master, M_Weapon_Master, M_Character_Master
  Materials/PBR/Instances/        MI_Env_Ground … MI_Char_Clothing
  Materials/Glass/                M_Glass_Master, MI_Glass_Clear
  Materials/Skin/                 M_Skin_Master, MI_Skin_Operator
  Materials/Decals/               M_Decal_Master, M_Impact_Bullet
  Materials/Libraries/            Fab material packs (not in git)
  Materials/Cosmetics/            M_{CosmeticId}
  Environments/ASH01_WireCut/…ASH12_Ashline/
  Data/Kits/                      DA_* (created in-editor)
  Data/ContentBindings.json       path contract
  Data/MaterialBindings.json      master / MI / Engine fallback contract
  Data/FabInstallPlan.json        evening clock
```

## Performance — 1440p Ultra on RX 9070 GRE

Named preset: **`Ashline_PC_Ultra`** (`AshPCUltra`). Details: `Docs/GRAPHICS.md`.

Target: 2560×1440, VSync off, high refresh. RDNA4 + DX12.

| System | Ultra setting | If you drop below ~90–120 fps |
| --- | --- | --- |
| Nanite | On (`r.Nanite=1`) | Keep on. Megascans should be Nanite-enabled. |
| Lumen GI / reflections | Software + HW RT when `GRHISupportsRayTracing` | `AshPCBalanced` cheapens gather; or `r.Lumen.TraceMeshSDFs=0` |
| Hardware RT | On if the RHI reports it | Driver/OS miss → Ultra still runs Lumen software traces |
| VSM | Quality 3 | Balanced drops quality |
| FSR 3 | Quality mode 1, screen 77% | Without plugin: TSR @ 85%. Do **not** enable frame gen (`r.FidelityFX.FI.Enabled=0`) until you verify latency. |
| Streaming pool | **5600 MB**, limit to VRAM | 16 GB class GRE: stay ≤ 8 GB pool if you import all 12 kits |
| Virtual texturing | `r.VT.Enable=1`, `r.VT.PoolSizeScale=1.15` | Do **not** flip project `r.VirtualTextures=True` until masters are VT-ready (material recompile). Runtime VT CVars are safe no-ops on non-VT mats. |
| Foliage | Density 1.0 | After Megascans pines/grass, cap `foliage.DensityScale=0.7` |
| Motion blur | Off | Leave off for FPS |
| View distance | 1.15 | 1.0 if Nanite overdraw spikes on ASH-02 market |

**Steam Deck / handheld** — console `AshSteamDeck` or auto-detect `SteamDeck=1`. Pool **1600 MB**, VT scale **0.45**, aniso 4, TSR 70%, **no HW RT**, VSync on, 60 fps cap. Device profile: `Config/DefaultDeviceProfiles.ini` `[SteamDeck DeviceProfile]`.

Recommended PIE check after packs land:

```
stat fps
stat unit
stat lumen
stat streaming
r.Streaming.PoolSize
r.VT.PoolSizeScale
r.FidelityFX.FSR3.Enabled
r.RayTracing
AshPCUltra
AshPCBalanced
AshSteamDeck
```

HUD should still say **Ashline_PC_Ultra** and **D3D12**.

Do **not** enable Lumen scene lighting quality 3 + HW RT reflections + RT shadows + full Megascans + MetaHuman + Niagara at 100% screen on the first import. Land packs, then profile.

## Campaign / meta — do not break

After art lands, re-run `Docs/TEST_PLAN.md`:

- ASH-01 INFIL → CUT → EXFIL still awards XP **and credits**
- Locker equip (`AshEquipCosmetic`, `AshEquipSkin`) still applies (now with real materials if present)
- Prestige at 50 still grants gilt + prestige camo + spine charm
- Ads stay off. No multiplayer.

## Script cheat sheet

| Script | When |
| --- | --- |
| `Scripts/import_fab_kits.py` | Once, after first editor open (also tries masters) |
| `Scripts/create_master_materials.py` | Masters + surface MIs (safe re-run) |
| `Scripts/assign_interim_meshes.py` | After Starter / GASP / Manny exist |
| `Scripts/fab_install_plan.py` | Print the evening clock (no UE) |
| `Scripts/create_ashline_play_assets.py` | Optional `ASH_Playable` map |
| `Scripts/validate_meta_catalog.py` | CI / pre-commit (no UE) |
| `Scripts/validate_content_layout.py` | CI / pre-commit (no UE) |
