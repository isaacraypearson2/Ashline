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

Named presets: **`Ashline_PC_Ultra`** (default on Windows), **`Ashline_PC_Balanced`**, **`Ashline_PC_Perf`**, **`Ashline_SteamDeck`**. Console: `AshPCUltra` / `AshPCBalanced` / `AshPCPerf` / `AshSteamDeck`. CVars: `Docs/GRAPHICS.md`. Overnight armory: `Docs/PHASE_OVERNIGHT.md`.

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
2. **Up/Down** highlight ASH-01 Wire Cut (READY). **Left/Right** set difficulty. **Enter** deploys.
3. In mission:
   - **WASD** move, **mouse** look, **Space** jump, **C** crouch
   - **LMB** fire, **RMB** aim, **R** reload, **B** fire-mode, **Q** swap, **V** FPS/TPS
   - Walk into **glowing objective markers** to complete. The extract / last required volume finishes the mission.
   - Hostiles use a **humanoid mesh when one can be resolved** (mannequin / MetaHuman / assigned DataAsset); otherwise a tinted blockout body.
4. On **MISSION COMPLETE**, note XP / **credits** / crate tokens, press **Enter** to return to campaign select. ASH-02 is now READY. Frontend shows Rank / Prestige / Credits and the equipped camo + primary skin.
5. Repeat through **ASH-12**. After the finale, **ASHLINE CUT** means the save unlocked the whole spine.
6. Progress lives in save slot `AshlineCampaign` (`Saved/SaveGames`). Delete that file to start a new operator.
7. Apply **Ashline_PC_Ultra** (`AshPCUltra`) after PIE if you changed scalability.

**Smoke-test checklist** — see `Docs/TEST_PLAN.md`.

Ads stay off (`UAshlineMonetizationHooks`).

## What is final vs still import work

| In this repo now | Phase 2 (you import — not in git) |
| --- | --- |
| Playable ASH-01…12 campaign loop | Quixel / Fab Megascans environment kits |
| Themed lighting, fog, PP, decals, foliage blockout | Authored `.umap` art passes per mission |
| PBR-tinted Engine/StarterContent materials | Unique scanned surfaces |
| Weapons — 36-gun armory + attachments + gunfeel + skin tints | Fab military weapon packs (soft-ref swap) |
| Hero/AI skeletal hooks (MetaHuman / mannequin) | MetaHuman Creator characters + AnimBPs |
| Audio **slots** (fire / reload / hit / footsteps / music) | Authored MetaSounds / Sound Cues |
| SP meta: credits, rank 1–50, cosmetics, skins, equipment, prestige 1–2 | MetaHuman wardrobe + Fab skin materials |
| `Ashline_PC_Ultra` / `Balanced` / `Perf` / `SteamDeck` + DX12 config | Profile on the 9070 GRE and tune |

**This repo does not contain Quixel, Fab, or MetaHuman binary packs.** If those folders are empty, that is expected.

**Install next (exact names / URLs / 1440p notes):** `Docs/PHASE2_FAB.md`  
Also: `Docs/FAB_PACKS.md`, `Docs/CONTENT_PIPELINE.md`, `Docs/CHARACTERS.md`, `Docs/GRAPHICS.md`, `Docs/PHASE_OVERNIGHT.md`, `Docs/TEST_PLAN.md`.

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
Locker / economy / prestige: `UAshlineMetaCatalog` + `Content/Ashline/Data/Meta.json` (`Docs/META.md`). Armory: `Docs/PHASE_OVERNIGHT.md`. Save: `UAshlineProgressionSubsystem`.

## Systems

- **FPS/TPS toggle** — first-person camera vs spring-arm third-person. Preference is saved.
- **Weapons** — AR, SMG, sniper, shotgun, sidearm, DMR, LMG, launcher, melee. Compound mesh + muzzle + impacts + fire modes + recoil patterns + equipped skin tint on `UAshlineWeaponComponent`.
- **Operator locker** — clothing slots, camos, faces, voice packs, charms, lethals/tacticals/field. Equipped ids tint the hero / blockout and hang a charm on the gun.
- **Economy / prestige** — credits from missions and rank-ups; spend on cosmetics, skins, paid guns, attachments, equipment, weapon upgrades; prestige at rank 50 keeps the locker and grants gilt (prestige 2 diamond).
- **AI** — eleven archetypes, perception + push/hold, humanoid mesh when assigned.
- **Difficulty** — Recruit, Regular, Veteran, Extreme.
- **Graphics** — `UAshlineGraphicsSettings` (Windows FSR3/TSR/RT + Steam Deck / Perf; Apple MetalFX still gated).
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
