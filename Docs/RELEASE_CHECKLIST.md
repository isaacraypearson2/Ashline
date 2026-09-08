# Ashline — public-release checklist

Honest status for a **1–2 week initial public release** (Steam Win64 + Steam Deck / Proton).  
This document is the release gate. Engineering readiness is in this branch (feel layer + Shipping hygiene). **Marketplace / Fab / authored art is not.**

**Named DeviceProfiles** (`Ashline_PC_Ultra` / `High` / `Balanced` / `Performance` / `Laptop` / `SteamDeck`) live in **PR #8** (`cursor/movable-runtime-lights-5a13`). This fold does **not** duplicate them. Platform roots here only strip self-parent loops.

Playable campaign loop, weapons, AI, progression, and Shipping hygiene can ship.  
A store-page-quality look **cannot** ship until Isaac imports Fab / Quixel / MetaHuman binaries on the Windows box.

---

## Verdict (today)

| Gate | Status | Blocks store? |
| --- | --- | --- |
| Win64 Shipping package config | Ready in repo (cook on `X3D_AMD`) | No — needs a local cook |
| Steam Deck / Proton notes | Ready (`AshDeck`, FSR/TSR, Proton doc). Named INI profiles in **PR #8** | Verify on hardware |
| Input (KBM + gamepad + Deck) | Ready (Enhanced Input runtime IMC, sprint, radial deadzones) | Verify on Deck |
| Cheat gating in Shipping | Ready (`AshlineShipping::CheatsAllowed`) | No |
| Missing-asset crash hardening | Ready (`AshlineLoad` quiet loader) | No |
| 12-mission campaign loop | Playable (runtime graybox host + feel-layer HUD) | No for “playable demo”; **yes** for AAA store page |
| Authored maps / Megascans | **Not in git** | **Yes** for store screenshots |
| Fab weapon / character meshes | Soft-ref stubs only | **Yes** for trailer / caps |
| Authored audio (MetaSounds) | Slots only; silent fallback | **Yes** for “sounds finished” |
| Steam page assets | Not produced | **Yes** |
| Steamworks App ID / depots | Not configured | **Yes** to upload |
| Ratings / legal | Not filed | **Yes** to publish |

---

## 1. Steam page blockers (must exist before “Coming Soon” goes live)

- [ ] Steamworks **App ID** created; store page in Draft
- [ ] Capsule: header 460×215, small 231×87, main 616×353, hero 1920×620
- [ ] **5+ screenshots** that are not graybox cubes (requires Phase 2 import or honest “vertical slice” labeling)
- [ ] Library capsule + library header
- [ ] Trailer (30–90s) — gameplay, not editor PIE with console
- [ ] Short + long description, tags (`Action`, `FPS`, `Singleplayer`, `Military`, `Story Rich`)
- [ ] Supported OS: **Windows 10/11 64-bit**. Steam Deck: Proton (see `Docs/STEAM_DECK.md`)
- [ ] Age ratings: ESRB / PEGI / IARC — military shooter, gun violence, no gore pack yet but assume **Mature / 16+**
- [ ] Privacy policy URL (even SP offline — Steamworks asks)
- [ ] Controller support: Full (Xbox / DualSense / Deck). Keyboard + mouse first-class
- [ ] Cloud saves: optional. Slot name `AshlineCampaign` (`Saved/SaveGames`)
- [ ] Achievements: **none implemented** — either add a minimal set or leave the Steam achievement section empty
- [ ] Build uploaded to a depot (Win64 Shipping + prerequisites)
- [ ] **Do not** list Mac/iOS as playable. They compile; they are not the public SKU
- [ ] Ads stay **off**. No multiplayer. No always-online

Honest store copy if art is still blockout: call it **prologue / technical preview**, not “AAA finished campaign.”

---

## 2. Known content gaps (Marketplace / Fab — not a code failure)

These folders being empty is **expected**. Soft refs point at `ContentBindings.json` paths.

| Gap | Where | What to import | Soft-ref already? |
| --- | --- | --- | --- |
| Megascans environment kits | `Content/Ashline/Environments/ASH##_*` | Quixel / Fab kits per mission | `DA_Kit_ASH01`…`12` |
| Authored `.umap`s | `Content/Ashline/Maps/Campaign/` | Art-pass maps | `MapAsset` on catalog |
| Military weapon packs | `Content/Ashline/Weapons/Meshes` | Fab weapon pack | `UAshlineWeaponVisual` |
| Skin / camo materials | `Content/Ashline/Materials/Cosmetics` | Fab materials | `M_SKIN_*` / `M_CAMO_*` |
| MetaHuman hero + AI | `Content/Ashline/Characters/` | MetaHuman Creator + wardrobe | `DA_Hero_Operator` / `DA_AI_*` |
| Niagara muzzle / impact | `Content/Ashline/FX/` | Authored Niagara | `MuzzleFX` / impact decal |
| MetaSounds / cues | `Content/Ashline/Audio/` | Fire / reload / hit / footsteps / music | `UAshlineAudioDirector` |
| UMG armory / locker | `Content/Ashline/UI/` | Designer UMG | C++ Canvas HUD is playable |
| Locomotion AnimBPs | Characters | GASP / custom | Not required to Play |
| Starter Content | Editor pack | Add Content Pack | Presentation fallbacks |
| AMD FSR3 plugin | Fab plugin | Enable in `.uproject` | CVars already wired; TSR fallback |

Exact pack names: `Docs/PHASE2_FAB.md`, `Docs/FAB_PACKS.md`.  
**Do not commit Fab binaries.** License stays with Fab / Quixel / Epic.

---

## 3. Shipping Win64 build

Cook **on the Windows box** (this Cloud VM has no UE 5.8 editor).

```bat
set UE58=C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles
set PROJ=%CD%\Ashline.uproject

"%UE58%\Build.bat" Ashline Win64 Shipping -Project="%PROJ%" -WaitMutex
"%UE58%\RunUAT.bat" BuildCookRun -project="%PROJ%" -platform=Win64 -clientconfig=Shipping -serverconfig=Shipping -cook -stage -pak -archive -archivedirectory="%CD%\Dist\Win64" -build -prereqs -distribution
```

Config that must stay true:

| File | Shipping rule |
| --- | --- |
| `Source/Ashline.Target.cs` | `bUseLoggingInShipping=false` |
| `Config/DefaultGame.ini` | `BuildConfiguration=PPBC_Shipping`, `ForDistribution=True`, **no** `DebugDisplay` |
| `Config/Windows/WindowsGame.ini` | SP `MaxPlayers=1`, no debug HUD |
| Cook maps | `/Engine/Maps/Entry` only as required; **do not** list missing `ASH_Playable` |
| Cheats | `AshUnlockAll` / `AshGrantCredits` / etc. are **no-ops** in Shipping |
| Graphics execs | `AshPCUltra` / `AshDeck` / `AshFSR` / `AshTSR` stay available |

Full cook notes: `Docs/PACKAGING.md`.

---

## 4. Hardware / Deck

Runtime presets on this branch: `AshPCUltra` / `AshPCBalanced` / `AshDeck` plus `AshFSR` / `AshTSR`.  
Named DeviceProfile INI rows are **PR #8** — merge that before a hardware pass if you want INI inheritance as well as CVar presets.

| Preset | Who | Screen % | Upscaler | RT | Cap |
| --- | --- | --- | --- | --- | --- |
| `Ashline_PC_Ultra` | 9070 GRE @ 1440p | 77 FSR / 85 TSR | FSR3 → TSR | On if supported | Uncapped |
| `Ashline_PC_Balanced` | Same PC, headroom | 59 / 70 | FSR Balanced | On if supported | Uncapped |
| `Ashline_SteamDeck` | Deck / Proton | 67 | FSR Balanced / TSR | **Off** | 60 + VSync |

DeviceProfiles **never self-parent**. Validate with `python Scripts/validate_release_config.py`.  
Handheld notes: `Docs/STEAM_DECK.md`. Budgets: `Docs/PERFORMANCE.md`.

---

## 5. Known bugs / residual risks

| Risk | Severity | Notes |
| --- | --- | --- |
| Missing `ASH_Playable` map dialog in editor | Low | Runtime remaps to `/Engine/Maps/Entry` |
| Empty Fab paths log (Verbose / Warning) | Low | Quiet load returns null; blockout / Engine mesh is used |
| FSR3 plugin absent | Low | TSR path; `AshFSR` falls back |
| HW RT driver miss on RDNA4 | Med | Ultra still runs Lumen software traces |
| No navmesh / BT | Med | AI is perception + push/hold, not cover-smart |
| No authored audio | Med | Silent is OK; do not ship “complete soundtrack” claims |
| Save slot `2.0.0` | Low | Older slots migrate; wipe `AshlineCampaign` to retest |
| Graybox lighting ≠ final art | High for store | Do not use PIE graybox as Steam caps without labeling |
| Console key (`~`) in Development | Info | Cheats inert in Shipping even if console is opened |
| Mac/iOS | Info | Compile-only; not a release SKU |

---

## 6. Art / audio / QA (Week-of-ship)

### Art
- [ ] Starter Content added on the cook machine (PBR fallback)
- [ ] At least **one** mission kit imported (ASH-01) for store shots
- [ ] Hero mannequin or MetaHuman (no visible collision capsule)
- [ ] One Fab weapon mesh on ASH-16
- [ ] Lighting moods still distinct after kits (`bOverrideMood` false unless authored)

### Audio
- [ ] Fire / reload / hit / footstep cues on the weapons **or** accept silence
- [ ] Music bed for frontend + ASH-01 + ASH-12
- [ ] Mix: gunshots must not clip at 0 dB on Deck speakers

### QA (Windows)
- [ ] `Docs/TEST_PLAN.md` campaign loop
- [ ] Shipping package boots to campaign select without `ensure`
- [ ] `AshUnlockAll` does **nothing** in Shipping
- [ ] `AshPCUltra` / `AshDeck` / `AshFSR` / `AshTSR` work
- [ ] Xbox pad + DualSense + KBM; Left Shift / LS click sprint
- [ ] Save / prestige / skin tint persist after restart
- [ ] Alt-Tab, 1440p exclusive, borderless

### QA (Deck / Proton)
- [ ] 40–60 fps @ 1280×800 on `AshDeck`
- [ ] Gyro / trackpad: Steam Input mouse (do not fight it)
- [ ] Sleep/resume does not corrupt `AshlineCampaign`
- [ ] 15W vs 30W TDP once — 30W is the advertised target

---

## 7. What this repo will never contain

- Quixel / Fab / MetaHuman **binaries**
- Paid Marketplace packs
- Ads SDK
- Multiplayer / listen-server
- Self-parented DeviceProfiles

If a reviewer asks “where is the AAA art?” — point at `Docs/PHASE2_FAB.md`. The code is waiting. The binaries are a local import on `X3D_AMD`.
