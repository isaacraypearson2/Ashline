# Ashline on Steam Deck / Proton

Public SKU is **Win64 Shipping** running under **Proton**. Native SteamOS Linux cook is configured (`Config/Linux`, `SteamDeck` DeviceProfile) but is **not** the first depot.

## Detection

Runtime (`AshlineShipping::IsSteamDeckHardware`):

- Environment `SteamDeck=1` (SteamOS / Proton sets this)
- Command line `-steamdeck`
- `SteamEnv` containing `steamdeck`

On detect, `UAshlineGraphicsSettings` applies **`Ashline_SteamDeck`**:

- 1280×800 fullscreen, VSync **on**, `t.MaxFPS=60`
- FSR3 Balanced (`QualityMode=2`) if the AMD plugin is present, else TSR @ 67%
- Hardware RT **off** (Deck iGPU + Proton RT is a battery / hitch risk)
- Streaming pool **1800**, anisotropy 8, cheap Lumen gather, foliage 0.45
- Frame generation **off**

Manual: console `AshDeck` (Development) or the same exec in a Test build.

Proton extra: `STEAM_COMPAT_DATA_PATH` / `PROTON_VERSION` mark Proton even on a desktop Steam install.

## Steam Input

Do **not** ship a custom In-Game Action file that steals the trackpads.

| Deck control | Ashline action |
| --- | --- |
| Left stick | Move (radial deadzone 0.20) |
| Right stick | Look (radial deadzone 0.18, 0.92 scale) |
| R2 / RT | Fire |
| L2 / LT | Aim (cancels sprint) |
| A / Cross | Jump; campaign **confirm** |
| B / Circle | Crouch |
| X / Square | Reload |
| Y / Triangle | Swap |
| L1 / LB | Swap (shoulder alt) |
| L3 / LS click | Sprint |
| R3 / RS click | Camera FPS/TPS |
| View / Share | Camera |
| D-pad up | Camera |
| D-pad | Campaign navigate |
| Menu / Options / Start | Pause / back |
| Trackpads | Mouse via Steam Input (default) |

Recommended Steam Input template: **Gamepad with Mouse Trackpad**.  
Gyro: optional community layout; not required.

## Proton notes

- Cook **DX12 / SM6** Win64. Do not ship Vulkan as the Windows RHI.
- Proton Experimental or Hotfix at upload time; pin a version in the store page if a specific Proton breaks FSR CVars.
- `r.FidelityFX.FSR3.*` no-ops if the plugin was not cooked. TSR still runs.
- Disable Steam Overlay FPS counter when profiling (`stat unit` in Test, not Shipping).
- Cloud saves: `%USERPROFILE%\Saved\SaveGames\AshlineCampaign.sav` under Proton prefix `steamapps/compatdata/<appid>/pfx/`.

## Handheld-safe recommendations

| Setting | Deck 30W | Deck 15W | Note |
| --- | --- | --- | --- |
| Preset | `Ashline_SteamDeck` | `Ashline_PC_Perf` or Deck + 40 fps | 15W is a stretch after Megascans land |
| Resolution | 1280×800 | 1152×720 | TDP, not native panel 800p pride |
| Upscaler | FSR Balanced / TSR 67 | FSR Perf / TSR 50 | Never native 100% after kits |
| RT | Off | Off | |
| Frame gen | Off | Off | Adds latency on a handheld |
| VSync | On | On | |

DeviceProfiles: `[Ashline_Deck DeviceProfile]` inherits **Windows** (Proton).  
`[SteamDeck DeviceProfile]` inherits **Linux** (native).  
Neither profile parents itself.

## Steam Deck Verified questionnaire (draft answers)

- Input: gamepad full, mouse/keyboard full, no text-entry required
- Glyphs: generic gamepad (A/B/X/Y). Deck-specific glyphs are a later UMG pass
- Default configuration plays without keyboard
- 1280×800 UI: Canvas HUD is large-type; UMG locker is not shipped
- Proton: expected
- Offline: yes
- Anti-cheat: none
