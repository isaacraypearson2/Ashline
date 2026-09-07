# Ashline controls

Enhanced Input. Create mapping contexts in `/Game/Ashline/Input/` and assign them on `AAshlinePlayerController`.

## Keyboard / mouse (`IMC_Ashline_KBM`)

- **WASD** — move
- **Mouse** — look
- **LMB** — fire
- **RMB** — aim
- **R** — reload
- **Space** — jump
- **C** — crouch
- **V** — FPS / TPS
- **Q** or **1 / 2** — swap weapon
- **Esc** — pause (widget, editor)

## DualSense (`IMC_Ashline_Gamepad`)

- Left stick move, right stick look
- **R2** fire, **L2** aim
- **Square** reload, **Cross** jump, **Circle** crouch, **Triangle** swap
- Touch pad — camera toggle
- Adaptive triggers and haptics: `UAshlineDualSense` → `AshlineGameController.mm` (`GameController.framework`, `GCDualSenseGamepad`, `GCDeviceHaptics`)

## iOS touch (`IMC_Ashline_Touch`)

`UAshlineTouchHUD` shows virtual stick, fire, aim, and camera toggle on iOS only. Subclass in UMG and implement `ConfigureTouchLayout`.
