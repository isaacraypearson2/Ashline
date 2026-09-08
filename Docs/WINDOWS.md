# Ashline on Windows (primary)

Play and bug-test on a **Win64** PC with **UE 5.8.2**. This document is written for Isaac's machine (Ryzen 5 7500X3D, 32 GB DDR5-6000, Radeon RX 9070 GRE class).

## Install

1. Epic Launcher → Unreal Engine **5.8.2**.
2. Visual Studio 2022 with **Desktop development with C++**, **Windows 10/11 SDK**, **.NET desktop**.
3. Clone the repo. Double-click `Ashline.uproject` (engine association **5.8**).
4. Allow the editor to compile `AshlineEditor` (Win64 Development).

```bat
"%PROGRAMFILES%\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AshlineEditor Win64 Development -Project="%CD%\Ashline.uproject" -WaitMutex
```

If a dialog says `ASH_Playable` is missing, dismiss it. Play still hosts on `/Engine/Maps/Entry`.

## Project settings that matter

| File | Role |
| --- | --- |
| `Config/Windows/WindowsEngine.ini` | DX12, SM6, Nanite, Lumen, VSM, HW RT, FSR3 CVars, 1440p streaming pool |
| `Config/Windows/WindowsGame.ini` | Shipping-safe Game.ini (no debug HUD, SP only) |
| `Config/DefaultGameUserSettings.ini` | 2560×1440 fullscreen, VSync off, `Ashline_PC_Ultra` (Deck overrides at runtime) |
| `Config/DefaultEngine.ini` | GameDefaultMap, `UAshlineGameUserSettings`, Nanite project flag |
| `Config/DefaultDeviceProfiles.ini` | Named Ultra/High/Balanced/Perf/Laptop/SteamDeck profiles (parent to Windows or Linux — not to themselves) |
| `Config/Linux/LinuxEngine.ini` | Proton / Deck cook defaults (800p, 40 fps, no RT) |
| `Ashline.uproject` | `TargetPlatforms` starts with **Win64** |
| `Docs/PACKAGING.md` | Win64 Shipping cook / archive |
| `Docs/RELEASE_CHECKLIST.md` | Store page + content gaps + QA |

RHI: **DirectX 12**. Do not switch the project to Vulkan unless you are debugging.

## Plugins

Required:

- **Enhanced Input** (enabled in the `.uproject`)

Recommended (not required, not vendored):

- **AMD FidelityFX Super Resolution 3** from Fab — enables `r.FidelityFX.FSR3.*`. Without it, Ultra/Balanced use **TSR**.
- **Starter Content** — Add Feature or Content Pack. Presentation library prefers these PBR materials when present.
- **MetaHuman** plugin — only when you start Phase 2 character import.

Do **not** enable an ads plugin.

## First PIE

1. Play. Campaign select should list ASH-01…12.
2. Confirm the HUD graphics line shows `Ashline_PC_Ultra` and a D3D12 RHI name.
3. Console: `stat fps`, `stat unit`, `r.RayTracing`, `r.Lumen.HardwareRayTracing`.
4. Deploy ASH-01. You should see night lighting, fog, practicals, and a compound weapon in FPS — not a flat gray void.

Full checklist: `Docs/TEST_PLAN.md`.
