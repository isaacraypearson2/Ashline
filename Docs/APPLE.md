# Ashline on Apple platforms

## Modules

**AshlineApple** links:

- `GameController.framework` (hard)
- `CoreHaptics.framework` (hard)
- `Metal.framework` / `QuartzCore.framework` (hard)
- `MetalFX.framework` (weak)

`ASHLINE_APPLE_NATIVE=1` is defined only for Mac and iOS targets. Other platforms compile stub implementations.

## MetalFX

`AshlineApple_ProbeMetalFXFramework()` looks up `MTLFXSpatialScalerDescriptor` / `MTLFXTemporalScalerDescriptor` at runtime. `UAshlineGraphicsSettings::SetUpscaler` refuses MetalFX when the probe fails.

CVars touched when available: `r.MetalFX.Enabled`, `r.MetalFX.Mode`, `r.MetalFX.UpscaleMode`.

## Ray tracing

`GRHISupportsRayTracing` is the gate. `TryEnableRayTracing(true)` returns false and leaves `r.RayTracing` / `r.Lumen.HardwareRayTracing` at 0 when the RHI does not report a device. Do not ship configs that force RT on.

## DualSense

`AshlineGameController.mm` finds `GCProductCategoryDualSense` (or any extended gamepad), pulses `GCDeviceHaptics`, and sets `GCDualSenseGamepad` adaptive trigger modes from weapon class.
