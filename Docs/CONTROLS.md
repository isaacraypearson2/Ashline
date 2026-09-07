# Ashline controls

Enhanced Input is created at runtime by `UAshlineRuntimeInput`. Editor IA/IMC
assets are optional.

## Campaign select (frontend)

- **Up / Down** — highlight a mission
- **Enter** — deploy if unlocked (READY / ACTIVE / DONE). LOCKED is ignored
- **Left / Right** — difficulty (Recruit / Regular / Veteran / Extreme)
- **Esc** — unused on frontend

Console cheats (Output Log or `~` if enabled):

- `AshUnlockAll` — every mission becomes playable
- `AshDeploy 3` — jump to ASH-03 (also unlocks all)
- `AshPCUltra` / `AshPCBalanced` — named 1440p presets for the 9070 GRE
- `AshComplete` — finish the active mission and grant XP + credits
- `AshFrontend` — abort back to campaign select
- `AshGrantCredits 5000` / `AshSetRank 50` / `AshPrestige` / `AshOpenCrate`
- `AshBuySkin SKIN_FDE` / `AshEquipSkin WPN_AR_ASH16 SKIN_FDE`
- `AshBuyCosmetic CAMO_NIGHT` / `AshEquipCosmetic Camo CAMO_NIGHT`
- `AshUnlockMeta` / `AshListMeta` — locker catalog (`Docs/META.md`)

## Keyboard / mouse (in mission)

- **WASD** — move
- **Mouse** — look
- **LMB** — fire
- **RMB** — aim
- **R** — reload
- **Space** — jump
- **C** or **Left Ctrl** — crouch
- **V** — FPS / TPS (saved)
- **Q** or **1 / 2** — swap weapon
- **Esc** — pause (Enter resume, Esc again abort to campaign)

## DualSense (`IMC_Ashline_Gamepad`, still applied on Mac/iOS)

- Left stick move, right stick look
- **R2** fire, **L2** aim
- **Square** reload, **Cross** jump, **Circle** crouch, **Triangle** swap
- Share / D-pad up — camera toggle
- Adaptive triggers and haptics: `UAshlineDualSense` → `AshlineGameController.mm`

## iOS touch (`IMC_Ashline_Touch`)

`UAshlineTouchHUD` shows virtual stick, fire, aim, and camera toggle on iOS only.
Subclass in UMG and assign `TouchHUDClass` on `AAshlinePlayerController`.
