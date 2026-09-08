# Ashline

Windows-first single-player military **FPS/TPS** campaign for **Unreal Engine 5.8.2 (DX12 / SM6)**.

Twelve missions (**ASH-01…ASH-12**): Wire Cut, Dust Market, Holdfast, Night Glass, Convoy Ghost, Ash Harbor, Whiteout, Catacomb, Ridge Wire, False Flag, Last Train, Ashline.

This repository is a full C++ Unreal project: campaign loop, weapons, AI, progression, a cross-platform graphics path, and an AAA **content pipeline** (DataAssets + soft refs + editor scripts). **Play/test on Windows.** Mac and iOS modules still compile; they are not the primary target.

## Target hardware (Isaac's PC)

| | |
| --- | --- |
| CPU | AMD Ryzen 5 7500X3D |
| RAM | 32 GB DDR5-6000 |
| GPU | Radeon RX 9070 GRE class (RDNA4) |
| Resolution | **2560×1440** Ultra / high-refresh |
| RHI | DirectX 12, Shader Model 6 |
| GI / shadows | Lumen + Virtual Shadow Maps + Nanite |
| RT | Hardware ray tracing **ON when the RHI reports it** |
| Upscaling | **FSR 3** (Temporal Upscale). TSR is the built-in fallback. DLSS is optional and never required. |

Named presets: **`Ashline_PC_Ultra`** (default on Isaac's desktop), **`PC_High`**, **`PC_Balanced`**, **`PC_Performance`**, **`SteamDeck`** (800p / FSR / 40 fps), **`Laptop`**. Auto-detect on boot (`AshGfxAuto`); cycle with **F8** / `AshGfxCycle`. CVars: `Docs/GRAPHICS.md`.

## Open on Windows (UE 5.8.2)

1. Install **Unreal Engine 5.8.2** with **Win64** (Visual Studio 2022 + Windows 10/11 SDK + .NET).
2. Clone this repo.
3. Right-click `Ashline.uproject` → **Generate Visual Studio project files**, or double-click to compile.
4. First Play lands in the **runtime campaign select**. `AAshlineGrayboxBuilder` hosts ASH-01…ASH-12 in the current world with **themed lighting, PBR-tinted surfaces, practical lights, fog, post-process, foliage, and weapon/character presentation hooks**. If `/Game/Ashline/Maps/ASH_Playable` does not exist yet, the module remaps startup to `/Engine/Maps/Entry` and still builds the campaign there.

Optional editor content (makes PIE look closer to final art — **not committed as binaries**):

- **Add Content Pack → Starter Content** (grass, concrete, metal, water materials).
- Enable **AMD FidelityFX Super Resolution 3** from Fab if you want the FSR3 plugin (CVars are already wired; TSR runs without it).
- Run `Scripts/import_fab_kits.py` then `Scripts/assign_interim_meshes.py` (mannequin hero+AI, no capsules). See `Docs/PHASE2_FAB.md`.

### Rebuild from source (Windows)

```bat
REM From the Ashline repo root. Adjust the engine path if needed.
set UE58=C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat
set PROJ=%CD%\Ashline.uproject

"%UE58%" AshlineEditor Win64 Development -Project="%PROJ%" -WaitMutex
"%UE58%" Ashline Win64 Development -Project="%PROJ%" -WaitMutex
```

## Play the full campaign

1. Click **Play** (PIE). You should see the **ASHLINE** campaign list.
2. **Up/Down** highlight ASH-01 Wire Cut (READY). **Left/Right** set difficulty. **Enter** opens the **mission brief**; **Enter** again deploys. **Esc** opens Settings (graphics incl. Steam Deck, HUD scale, colorblind).
3. In mission:
   - **WASD** move, **mouse** look, **Space** jump, **C** crouch
   - **LMB** fire, **RMB** aim, **R** reload, **Q** swap, **V** FPS/TPS, **F / E** interact
   - COD-style HUD: ammo / firemode / skin, HP+armor, objective diamond, hit markers
   - Walk into **glowing objective markers** (or press F when prompted). The extract / last required volume finishes the mission.
   - Hostiles use a **humanoid mesh when one can be resolved** (mannequin / MetaHuman / assigned DataAsset); otherwise a tinted blockout body.
4. On **MISSION COMPLETE**, note XP / **credits** / crate tokens, press **Enter** to return to campaign select. ASH-02 is now READY. Frontend shows Rank / Prestige / Credits and the equipped camo + primary skin.
5. Repeat through **ASH-12**. After the finale, **ASHLINE CUT** means the save unlocked the whole spine.
6. Progress lives in save slot `AshlineCampaign` (`Saved/SaveGames`). Delete that file to start a new operator.
7. Apply **Ashline_PC_Ultra** (`AshPCUltra`) after PIE if you changed scalability. **F8** cycles Ultra → High → Balanced → Perf → Deck → Laptop. `AshSteamDeck` is the handheld profile (safe-zone HUD, 800p, 40 fps) — not the default on the 9070 GRE.

**Smoke-test checklist** — see `Docs/TEST_PLAN.md`.

Ads stay off (`UAshlineMonetizationHooks`).

## What is final vs still import work

| In this repo now | Phase 2 (you import — not in git) |
| --- | --- |
| Playable ASH-01…12 campaign loop | Quixel / Fab Megascans environment kits |
| Themed lighting, fog, PP, decals, foliage blockout | Authored `.umap` art passes per mission |
| PBR-tinted Engine/StarterContent materials | Unique scanned surfaces |
| Compound weapon meshes + muzzle light + impact decals | Fab military weapon packs (soft-ref swap) |
| Hero/AI skeletal hooks (MetaHuman / mannequin) | MetaHuman Creator characters + AnimBPs |
| Audio **slots** (fire / reload / hit / footsteps / music) | Authored MetaSounds / Sound Cues |
| SP meta: credits, rank 1–50, cosmetics, weapon skins, prestige | MetaHuman wardrobe + Fab skin materials |
| AAA-scale graybox ASH-01…12 (infil / compound / exfil, trim, lamps, crates) | Quixel kits on the same footprints |
| Master-material **paths** (`MI_AshlineConcrete` etc.) + Engine fallbacks | Authored Nanite MIs / Megascans in `Content/Ashline/Materials/PBR/` |
| `Ashline_PC_Ultra` / High / Balanced / Perf / **SteamDeck** / Laptop | Profile on 9070 GRE **and** a Deck; FSR3 plugin from Fab |
| COD-style SP HUD, brief/debrief, pause settings, tracers/muzzle/impacts | Authored Niagara + UMG locker |

**This repo does not contain Quixel, Fab, or MetaHuman binary packs.** If those folders are empty, that is expected.

Install next: `Docs/PHASE2_FAB.md` (evening clock) · `Docs/MATERIALS.md` (MI_Ashline parameter contract)  
Also: `Docs/FAB_PACKS.md`, `Docs/CONTENT_PIPELINE.md`, `Docs/CHARACTERS.md`, `Docs/GRAPHICS.md`, `Docs/TEST_PLAN.md`.

## Campaign

| Code | Title | Theme / mood |
| --- | --- | --- |
| ASH-01 | Wire Cut | Night raid / cold moonlight |
| ASH-02 | Dust Market | Harsh desert noon |
| ASH-03 | Holdfast | Bleached firebase day |
| ASH-04 | Night Glass | Counter-sniper overwatch |
| ASH-05 | Convoy Ghost | Amber highway dusk |
| ASH-06 | Ash Harbor | Overcast port |
| ASH-07 | Whiteout | Arctic white-out |
| ASH-08 | Catacomb | Subterranean warm dark |
| ASH-09 | Ridge Wire | High-altitude clear |
| ASH-10 | False Flag | Urban night campus |
| ASH-11 | Last Train | Industrial overcast |
| ASH-12 | Ashline | Finale — red buried terminus |

Briefings, XP, and crate tokens: `UAshlineMissionCatalog` + `Content/Ashline/Data/Campaign.json`.  
Locker / economy / prestige: `UAshlineMetaCatalog` + `Content/Ashline/Data/Meta.json` (`Docs/META.md`). Save: `UAshlineProgressionSubsystem`.

## Systems

- **FPS/TPS toggle** — first-person camera vs spring-arm third-person. Preference is saved.
- **Weapons** — 13 guns (AR/SMG/PDW/BR/DMR/LMG/shotgun/pistol), attachments including ammo types, skins, upgrade tiers, muzzle/reload FX slots, pellet traces on shotguns.
- **Operator locker** — clothing slots (incl. headset/backpack), camos, faces, voice packs, charms. Equipped ids tint the hero / blockout and hang a charm on the gun.
- **Economy / prestige** — credits from missions and rank-ups; spend on cosmetics, skins, weapon upgrades; prestige at rank 50 keeps the locker and grants gilt.
- **AI** — eleven archetypes (incl. grenadier / radio / CQB), perception + push/hold, humanoid mesh when assigned.
- **Difficulty** — Recruit, Regular, Veteran, Extreme.
- **Graphics** — `UAshlineGraphicsSettings` (Windows FSR3/TSR/RT; Steam Deck 800p profile; Apple MetalFX still gated). Auto-detect + F8 cycle.
- **MonetizationHooks** — stub only. Ads stay off. No multiplayer.

## Source layout

```
Ashline.uproject
Source/Ashline/                 Game module (campaign, graphics, presentation)
Source/AshlineApple/            MetalFX + DualSense (compiles as stubs on Windows)
Config/                         Default + Windows/ + Mac/ + IOS/
Content/Ashline/                Pipeline folders + JSON (no Fab binaries)
Docs/                           Windows, graphics, content, characters, test plan
Scripts/                        Editor Python (play map + Fab kit stubs)
```

## Mac / iOS (secondary)

Kept compiling. See `Docs/APPLE.md`. Graphics default to High + MetalFX when available; HW RT stays capability-gated. Do not treat Mac as the playtest machine.

UE 5.8.2 Mac compile notes (already in tree): `EditorStartupMap` via `FSoftObjectPath` assign (no `SetEditorStartupMap`); `CameraActor` include + `static_cast` for `SetViewTarget`; AshlineApple `PCHUsageMode.NoPCHs`.

## License

Project files are provided as-is for the Ashline campaign. Unreal Engine is subject to Epic’s EULA. Fab / Quixel / MetaHuman assets remain under their own licenses after you import them.
