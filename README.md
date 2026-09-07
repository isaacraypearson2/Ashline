# Ashline

Apple-first single-player military **FPS/TPS** campaign for **Unreal Engine 5.8.2 (Metal)**.

Twelve missions (**ASH-01…ASH-12**): Wire Cut, Dust Market, Holdfast, Night Glass, Convoy Ghost, Ash Harbor, Whiteout, Catacomb, Ridge Wire, False Flag, Last Train, Ashline.

This repository is a full C++ Unreal project (modules, config, campaign/weapon/AI data, Apple platform hooks). Art is **graybox**. Hardware ray tracing is **capability-gated** and off unless the RHI reports it.

## Open on Apple Silicon (UE 5.8.2)

1. Install **Unreal Engine 5.8.2** with **Mac** (and optionally **iOS**) support. Apple Silicon required for the intended Metal path.
2. Install **Xcode** and the command-line tools. In **Xcode → Settings → Components**, install the **Metal Toolchain**. Without it, MetalFX headers/framework may be missing. AshlineApple will still compile (MetalFX is optional / weakly linked only when the SDK has it).
3. Clone this repo.
4. Double-click `Ashline.uproject` (associated with **5.8**) or *File → Open* from the Epic Launcher / Unreal Editor.
5. If the editor says the project could not be compiled, rebuild from source with `Build.sh` (below), then reopen.

First launch uses the engine Entry map. Create graybox campaign maps under `/Game/Ashline/Maps/` as listed in `Content/Ashline/README.md`, then set `GameDefaultMap` in Project Settings when you are ready.

Do not expect a Windows DX12 workflow; this project is authored for Metal.

### Rebuild from source (Mac)

Replace the engine path if your 5.8.2 install lives elsewhere (Epic launcher default is shown):

```bash
# From the Ashline repo root
UE58="/Users/Shared/Epic Games/UE_5.8/Engine/Build/BatchFiles/Mac/Build.sh"
PROJ="$(pwd)/Ashline.uproject"

# Editor target — this is what the .uproject open path compiles
"$UE58" AshlineEditor Mac Development -Project="$PROJ" -WaitMutex

# Game target (optional)
"$UE58" Ashline Mac Development -Project="$PROJ" -WaitMutex
```

If `UE_5.8` is under your home library instead:

```bash
UE58="$HOME/Epic Games/UE_5.8/Engine/Build/BatchFiles/Mac/Build.sh"
```

After a successful `AshlineEditor` build, open `Ashline.uproject` again. Check `Saved/Logs/` if it still fails.

**Mac UE 5.8.2 compile is verified** on Apple Silicon after: `PCHUsageMode.NoPCHs` on AshlineApple (avoids FVector vs CarbonCore/Foundation), `PublicFrameworks` for GameController/CoreHaptics, a plain-C `AshlineAppleNative.h`, and an ObjC++ Game Controller file that does not include Unreal headers.

## Mac

- Target: **arm64**, macOS **14+**, Metal SM5/SM6 (`Config/Mac/MacEngine.ini`).
- Pair a **DualSense** over Bluetooth/USB. Adaptive triggers and haptics go through `GameController.framework` in **AshlineApple**.
- Keyboard and mouse are first-class (WASD, mouse look, standard shooter binds).
- Graphics: apply presets from `UAshlineGraphicsSettings`. **MetalFX** is weakly linked (`MetalFX.framework`). If the scaler classes are missing, upscaling stays off. **Do not** force `r.RayTracing=1` on machines that do not report HW RT.

## iOS

- Minimum **iOS 17**, landscape, Metal (`Config/IOS/IOSEngine.ini`).
- Remote build from a Mac with Xcode. Enable iOS in Unreal project settings and provision your team.
- Touch: `UAshlineTouchHUD` + `IMC_Ashline_Touch` (virtual stick, fire, aim, camera toggle). Create the widget Blueprint subclass in editor and assign `TouchHUDClass` on `AAshlinePlayerController`.
- Game Controller / DualSense work when iOS reports an extended gamepad.

## Controls

| Action | Keyboard / mouse | DualSense | iOS touch |
| --- | --- | --- | --- |
| Move | WASD | Left stick | Virtual stick |
| Look | Mouse | Right stick | Look region |
| Fire | LMB | R2 | Fire button |
| Aim | RMB | L2 | Aim button |
| Reload | R | Square / X | Reload |
| Jump | Space | Cross / A | Jump |
| Crouch | C | Circle / B | Crouch |
| FPS / TPS | V | Touch pad | Camera toggle |
| Swap weapon | Q or 1/2 | Triangle / Y | Swap |

Enhanced Input assets live under `/Game/Ashline/Input/` once created in the editor. C++ binds are on `AAshlineCharacter`. See `Config/DefaultInput.ini`.

## Campaign

| Code | Title | Theme |
| --- | --- | --- |
| ASH-01 | Wire Cut | Night raid / comms sabotage |
| ASH-02 | Dust Market | Urban recon / HVT intercept |
| ASH-03 | Holdfast | Firebase defense |
| ASH-04 | Night Glass | Counter-sniper overwatch |
| ASH-05 | Convoy Ghost | Ambush / mobile assault |
| ASH-06 | Ash Harbor | Port / shipboard |
| ASH-07 | Whiteout | Arctic storm navigation |
| ASH-08 | Catacomb | Subterranean CQB |
| ASH-09 | Ridge Wire | Mountain EW |
| ASH-10 | False Flag | Deception / urban night |
| ASH-11 | Last Train | Rail extract |
| ASH-12 | Ashline | Finale — cut the spine |

Briefings, objectives, XP, and crate tokens are in `UAshlineMissionCatalog` and `Content/Ashline/Data/Campaign.json`. Progress saves between missions in slot `AshlineCampaign` via `UAshlineProgressionSubsystem`.

## Systems

- **FPS/TPS toggle** — `AAshlineCharacter` first-person camera vs spring-arm third-person. Preference is saved.
- **Weapons** — AR, SMG, sniper, shotgun, sidearm, DMR, LMG (`UAshlineWeaponCatalog`). Attachments: optic, muzzle, underbarrel, magazine, stock, laser. Five upgrade tiers.
- **AI** — Rifleman, Breacher, Marksman, Machine Gunner, Officer, Scout, Heavy, Irregular. Perception + simple push/hold.
- **Difficulty** — Recruit, Regular, Veteran, Extreme (damage, accuracy, health, extra AI).
- **Operator creator** — Callsign, name, voice, camo, face (`UAshlineOperatorCreator`).
- **Armory / prestige / crates** — Unlock by rank, prestige at 50 (keeps cosmetics), play-earned crate tokens (no IAP).
- **MonetizationHooks** — `UAshlineMonetizationHooks` stub only. Ads stay off. Campaign loot is play-earned.

## Source layout

```
Ashline.uproject
Source/Ashline/                 Game module
Source/AshlineApple/            MetalFX.framework + GameController
Config/                         Engine, input, Mac, iOS
Content/Ashline/                Graybox scaffolding + JSON data
```

## Honest limits

- **Graybox art.** No shipped characters, weapons meshes, audio, or finished lighting. Maps are created in editor from the folder plan.
- **Hardware RT is gated.** `UAshlineMetalFXSubsystem` reads `GRHISupportsRayTracing`. If the device does not report RT, the setting refuses to enable. Most Apple GPUs will stay on software Lumen / no RT.
- **MetalFX is optional.** Weak-linked. Spatial/Temporal only when `MTLFX*ScalerDescriptor` exists (typically macOS 13+ / iOS 16+ on supported GPUs).
- **Input actions** must be created once in the editor and assigned on the character/controller defaults.
- This repo is not a packaged App Store build. You still need Apple Developer signing for iOS devices.

## License

Project files are provided as-is for the Ashline campaign. Unreal Engine is subject to Epic’s EULA.
