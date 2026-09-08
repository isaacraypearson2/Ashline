# Ashline graphics (UE 5.8.2)

`UAshlineGraphicsSettings` is the single runtime path. Windows DX12 is primary. Steam Deck / Proton is a first-class **scalability** target now (native 800p, FSR, 30/40/60 caps); a Linux cook comes later.

On **Windows** it probes DX12 / `GRHISupportsRayTracing` / FSR3 CVars / GPU brand / RAM / `STEAMDECK=1`, then applies a named preset.

On **Apple** it still asks `IAshlineMetalFX` for MetalFX + RT and refuses to force RT when the RHI reports no device.

## Named presets

| Preset | Who it's for | Native res | Screen % (FSR3 / TSR) | RT | Cap | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `Ashline_PC_Ultra` | 9070 GRE class @ 1440p | 2560×1440 | 77 / 85 | On if supported | Uncapped | Default Windows desktop. Pool 5600. VSync off. |
| `Ashline_PC_High` | Mid discrete (8 GB class) | 1440p/1080p | 67 / 77 | On if supported | Uncapped | Pool 4200, cheaper VSM/Lumen. |
| `Ashline_PC_Balanced` | Same PC, extra headroom | 1440p | 59 / 70 | On if supported | Uncapped | Cheaper Lumen gather, pool 3800. |
| `Ashline_PC_Performance` | 6 GB / last-gen | 1080p-class | 50 / 59 | Off | 60 | Software Lumen, pool 2200. |
| `Ashline_SteamDeck` | Deck LCD/OLED, Proton later | **1280×800** | 59 / 67 | Off | **40** | Aggressive Nanite/VSM/Lumen, HUD safe zone 7%. `AshFPS 30/40/60`. |
| `Ashline_Laptop` | iGPU / Intel / AMD APU | panel | 59 / 67 | Off | 60 | Pool 1400. |
| Epic / Cinematic | Generic | 100 | TSR | Off unless asked | — | Scalability 3 |
| High / Medium / Low | Mac / leftover | 100–67 | MetalFX or TSR | Off | — | Mac default is High |

Auto-detect (`AshGfxAuto`, default on first run):

1. `STEAMDECK=1`, Van Gogh / Sephiroth / Jupiter / Galileo → **SteamDeck**
2. Intel UHD/Iris or AMD “Radeon Graphics” without RX → **Laptop**
3. 9070 / 7900 / 4080 / 4090-class + ≥16 GB RAM → **Ultra**
4. 7800 / 4070 / 3080-class → **High**
5. <12 GB RAM → **Performance**
6. Else Windows → **Ultra** (Isaac's playtest box)

Saved override: any `AshPC*` command sets `bAutoDetectPreset=false`. `AshGfxAuto` turns it back on.

Console / pause:

```
AshPCUltra
AshPCHigh
AshPCBalanced
AshPCPerf
AshSteamDeck
AshLaptop
AshGfxAuto
AshGfxCycle          ; also F8 / gamepad Select
AshFPS 30|40|60|0
```

DeviceProfiles live in `Config/DefaultDeviceProfiles.ini` (named profiles parent to **Windows** or **Linux**, never to themselves). Runtime CVars from `UAshlineGraphicsSettings` win after boot. Do **not** edit `BaseProfileName` on the Windows/Mac/IOS platform profiles.

## 9070 GRE CVar sheet (`Ashline_PC_Ultra`)

Applied from code (`ApplyNamedMachinePreset`) and mirrored in `Config/Windows/WindowsEngine.ini`:

```
r.VSync=0
t.MaxFPS=0
r.FinishCurrentFrame=0
r.AntiAliasingMethod=4
r.TemporalAA.Upsampling=1
r.ScreenPercentage=77
r.Nanite=1
r.Nanite.ProjectEnabled=1
r.Shadow.Virtual.Enable=1
r.Shadow.Virtual.MaxQuality=3
r.DynamicGlobalIlluminationMethod=1
r.ReflectionMethod=1
r.Lumen.DiffuseIndirect.Allow=1
r.Lumen.Reflections.Allow=1
r.Lumen.HardwareRayTracing=1          ; only if GRHISupportsRayTracing
r.Lumen.Reflections.HardwareRayTracing=1
r.RayTracing=1
r.RayTracing.Shadows=1                ; Ultra only
r.MaxAnisotropy=16
r.Streaming.PoolSize=5600
r.Streaming.LimitPoolSizeToVRAM=1
r.ViewDistanceScale=1.15
r.BloomQuality=5
r.AmbientOcclusionLevels=2
r.MotionBlurQuality=0
r.Tonemapper.Sharpen=0.45
foliage.DensityScale=1.0
r.SkinCache.CompileShaders=1
r.SkinCache.Mode=1

; FSR 3 (plugin present)
r.FidelityFX.FSR3.Enabled=1
r.FidelityFX.FSR3.QualityMode=1
r.FidelityFX.FI.Enabled=0             ; frame gen off by default

; DLSS (optional NVIDIA plugin — never required)
r.NGX.DLSS.Enable=0
```

## Steam Deck CVar sheet (`Ashline_SteamDeck`)

Mirrored in `Config/Linux/LinuxEngine.ini` for Proton later. Runtime applies this on `STEAMDECK=1` even on a Windows test (`AshSteamDeck`).

```
1280x800 fullscreen, VSync on, t.MaxFPS=40 (AshFPS 30 or 60 for battery/performance)
r.ScreenPercentage=59                 ; FSR Balanced
r.Nanite.MaxPixelsPerEdge=4
r.Shadow.Virtual.MaxQuality=1
r.Shadow.Virtual.SMRT.RayCountDirectional=2
r.Lumen.ScreenProbeGather.DownsampleFactor=32
r.Lumen.TraceMeshSDFs=0
r.RayTracing=0
r.Streaming.PoolSize=1800
r.ViewDistanceScale=0.65
foliage.DensityScale=0.35
HUD SafeZoneScale=0.07
```

## Texture streaming / VT (Ultra vs Steam Deck)

Named-preset **pools stay** (Ultra 5600, High 4200, Balanced 3800, Deck **1800**, Laptop 1400). `ApplyTextureStreamingCVars` adds VT and amortize on top — it does not rewrite pool size.

| Preset | Pool (MB) | `r.VT.PoolSizeScale` | Mip bias | Uploads/frame | Aniso |
| --- | --- | --- | --- | --- | --- |
| Ultra | 5600 | 1.15 | 0 | 24 | 16 / VT 8 |
| High | 4200 | 1.0 | 0 | 18 | 8 / VT 4 |
| Balanced | 3800 | 0.8 | 0 | 12 | 8 / VT 4 |
| Steam Deck | **1800** | 0.45 | 0.5 | 8 | 4 / VT 4 |
| Laptop | 1400 | 0.55 | 0.25 | 10 | 4 |

Also: `r.TextureStreaming=1`, `r.Streaming.LimitPoolSizeToVRAM=1`, `r.Streaming.AmortizeCPUToGPUCopy=1`, `r.VT.Enable=1`. After Megascans land, `stat streaming` then drop foliage density before touching pools.

`UAshlineGraphicsSettings::SetFrameGeneration(true)` sets `r.FidelityFX.FI.Enabled=1` when that CVar exists.

Phase 2 import + 1440p Ultra notes after Megascans/MetaHuman land: **`Docs/PHASE2_FAB.md`**.

## Upscaler order (Windows)

1. **FSR 3** if `r.FidelityFX.FSR3.Enabled` exists.
2. **TSR** otherwise (`r.AntiAliasingMethod=4`).
3. **DLSS** only if the user calls `SetUpscaler(DLSS)` and NGX CVars exist.

MetalFX modes are ignored on Windows.

## Hardware RT

Gated on `GRHISupportsRayTracing`. RDNA4 should report RT on DX12. Ultra/High may enable it; Performance / Deck / Laptop never force it. If a driver/OS combo does not report RT, Ultra still runs Lumen software traces and logs a warning. We do **not** force `r.RayTracing=1` in `DefaultEngine.ini` so Mac stays safe; `Config/Windows/WindowsEngine.ini` opts in for Win64.
