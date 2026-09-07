# Ashline graphics (UE 5.8.2)

`UAshlineGraphicsSettings` is the single runtime path. It is **not** Apple-only.

On **Windows** it probes DX12 / `GRHISupportsRayTracing` / FSR3 CVars / optional DLSS CVars, then applies a named preset.

On **Apple** it still asks `IAshlineMetalFX` for MetalFX + RT and refuses to force RT when the RHI reports no device.

## Named presets

| Preset | Who it's for | Screen % | Upscaler | RT | Notes |
| --- | --- | --- | --- | --- | --- |
| `Ashline_PC_Ultra` | 9070 GRE @ 1440p high-refresh | 77 (FSR3) / 85 (TSR) | FSR3 → TSR | On if supported | Default on Windows. Streaming pool 5600. VSync off. `t.MaxFPS=0`. |
| `Ashline_PC_Balanced` | Same PC, extra headroom | 59 / 70 | FSR3 Balanced → TSR | On if supported | Cheaper Lumen gather, pool 3800. |
| Epic / Cinematic | Generic | 100 | TSR | Off unless asked | Scalability 3 |
| High / Medium / Low | Laptops / Mac | 100–67 | MetalFX or TSR | Off | Mac default is High |

Console:

```
AshPCUltra
AshPCBalanced
```

(or execs on the player controller with the same names)

## 9070 GRE CVar sheet (`Ashline_PC_Ultra`)

Applied from code (`ApplyNamedPCPreset`) and mirrored in `Config/Windows/WindowsEngine.ini`:

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

; FSR 3 (plugin present)
r.FidelityFX.FSR3.Enabled=1
r.FidelityFX.FSR3.QualityMode=1
r.FidelityFX.FI.Enabled=0             ; frame gen off by default

; DLSS (optional NVIDIA plugin — never required)
r.NGX.DLSS.Enable=0
```

`UAshlineGraphicsSettings::SetFrameGeneration(true)` sets `r.FidelityFX.FI.Enabled=1` when that CVar exists.

## Upscaler order (Windows)

1. **FSR 3** if `r.FidelityFX.FSR3.Enabled` exists.
2. **TSR** otherwise (`r.AntiAliasingMethod=4`).
3. **DLSS** only if the user calls `SetUpscaler(DLSS)` and NGX CVars exist.

MetalFX modes are ignored on Windows.

## Hardware RT

Gated on `GRHISupportsRayTracing`. RDNA4 should report RT on DX12. If a driver/OS combo does not, Ultra still runs Lumen software traces and logs a warning. We do **not** force `r.RayTracing=1` in `DefaultEngine.ini` so Mac stays safe; `Config/Windows/WindowsEngine.ini` opts in for Win64.
