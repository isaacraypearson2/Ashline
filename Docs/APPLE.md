# Ashline on Apple platforms (secondary)

**Playtest target is Windows.** Mac/iOS modules still compile.

## Modules

Validated against **Unreal Engine 5.8.2** on Apple Silicon. Install the **Metal Toolchain** from Xcode → Settings → Components before relying on MetalFX.

**AshlineApple** links:

- `GameController.framework` (hard)
- `CoreHaptics.framework` (hard)
- `Metal.framework` / `QuartzCore.framework` (engine)
- `MetalFX.framework` (weak, **only if the SDK actually contains it**)

`ASHLINE_APPLE_NATIVE=1` is defined only for Mac and iOS targets. Windows compiles stub implementations.

## MetalFX

`AshlineApple_ProbeMetalFXFramework()` looks up `MTLFXSpatialScalerDescriptor` / `MTLFXTemporalScalerDescriptor` at runtime. `UAshlineGraphicsSettings::SetUpscaler` refuses MetalFX when the probe fails.

CVars: `r.MetalFX.Enabled`, `r.MetalFX.Mode`, `r.MetalFX.UpscaleMode`.

## Ray tracing

`GRHISupportsRayTracing` is the gate. Mac configs keep `r.RayTracing=0`. Do not ship Mac ini that forces RT on.

## DualSense

`AshlineGameController.mm` finds `GCProductCategoryDualSense`, pulses haptics, and sets adaptive triggers from weapon class.

## UE 5.8.2 Mac compile

Already in tree: `EditorStartupMap` via `FSoftObjectPath` (no `SetEditorStartupMap`); `CameraActor` include + `static_cast` for `SetViewTarget`; AshlineApple `PCHUsageMode.NoPCHs`.
