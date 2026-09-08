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
- `AshPCUltra` / `AshPCHigh` / `AshPCBalanced` / `AshPCPerf` / `AshDeck` — named presets
- `AshFSR` / `AshTSR` — upscaler toggle (FSR3 falls back to TSR if the plugin is missing)
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
- **Left Shift** — sprint (cancelled by aim)
- **V** — FPS / TPS (saved)
- **Q** or **1 / 2** — swap weapon
- **Esc** — pause (Enter resume, Esc again abort to campaign)

Debug execs (`AshUnlockAll`, `AshGrantCredits`, …) are **no-ops in Shipping**.

## DualSense / Xbox / Steam Deck (`IMC_Ashline_Gamepad`)

- Left stick move (radial deadzone 0.20), right stick look (0.18 / 0.92)
- **RT / R2** fire, **LT / L2** aim
- **A / Cross** jump + campaign confirm, **B / Circle** crouch
- **X / Square** reload, **Y / Triangle** swap, **LB** swap
- **LS click** sprint, **RS click** / Share / D-pad up — camera toggle
- **Start / Options** pause / back
- Trackpads: leave as mouse (Steam Input). See `Docs/STEAM_DECK.md`.
- Adaptive triggers and haptics: `UAshlineDualSense` → `AshlineGameController.mm` (Apple path)

## iOS touch (`IMC_Ashline_Touch`)

`UAshlineTouchHUD` shows virtual stick, fire, aim, and camera toggle on iOS only.
Subclass in UMG and assign `TouchHUDClass` on `AAshlinePlayerController`.
