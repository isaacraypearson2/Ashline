# Ashline performance budgets

Budgets are **targets for a 1–2 week public build**, not year-one live-ops.  
Measure on `X3D_AMD` (7500X3D / 32 GB / RX 9070 GRE) and a Steam Deck OLED/LCD.

Named DeviceProfile INI rows (`Ashline_PC_*`, `Ashline_SteamDeck`) are in **PR #8**. Runtime CVar presets (`AshPCUltra` / `AshPCBalanced` / `AshDeck`) are on this branch.

## Frame / resolution

| SKU | Resolution | Preset | Floor | Stretch |
| --- | --- | --- | --- | --- |
| Desktop high-refresh | 2560×1440 | `Ashline_PC_Ultra` | 80 fps | Uncapped |
| Desktop headroom | 2560×1440 | `Ashline_PC_Balanced` | 120 fps capable | Uncapped |
| Steam Deck 30W | 1280×800 | `Ashline_SteamDeck` | 40 fps | 60 fps + VSync |
| Steam Deck 15W | 1152×720 | Deck | 30 fps | 40 fps |

VSync **off** on desktop. VSync **on** + 60 cap on Deck.

## GPU / memory

| Preset | Streaming pool | Aniso | Lumen probe res | Foliage | RT shadows |
| --- | --- | --- | --- | --- | --- |
| Ultra | 5600 | 16 | 32 | 1.0 | Yes if HW RT |
| Balanced | 3800 | 12 | 16 | 0.7 | No |
| Deck | 1800 | 8 | 8 | 0.45 | RT off |

After Megascans land, **re-measure pool**. If VRAM > 90% on Deck, drop foliage and `r.ScreenPercentage` before turning Lumen off.

## CPU

- Campaign host + 8 AI archetypes + graybox spawn must stay under **8 ms game thread** on 7500X3D at Ultra (empty kits).
- After authored maps: budget **12 ms** game thread / **8 ms** render thread at 1440p Ultra.
- Deck: keep AI counts at difficulty tables; Extreme extra bots are a desktop luxury.

## Content cost (when packs exist)

| System | Budget |
| --- | --- |
| Nanite clusters on screen | Prefer Nanite meshes; no 4K unique tex on every trim |
| Shadow casters | VSM; no extra stationary cascades |
| Niagara muzzle | One system per shot, auto-destroy, no persistent ribbons |
| Audio voices | ≤ 32 concurrent; gunshots 2D/3D attenuate |
| Save | `AshlineCampaign` < 1 MB |

## How to profile (Development / Test, not Shipping)

```
stat fps
stat unit
stat rhi
stat streaming
r.ScreenPercentage
r.FidelityFX.FSR3.Enabled
r.Lumen.HardwareRayTracing
AshPCUltra
AshDeck
AshFSR
AshTSR
```

Do not ship a build that requires `stat unit` to be readable. Shipping has logging off.

## Upscaler order

1. FSR3 if CVars exist (`AshFSR`)
2. TSR always (`AshTSR`)
3. DLSS only if the user / NVIDIA plugin asks — never required, never default on Deck
