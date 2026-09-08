# Ashline controls

Enhanced Input is created at runtime by `UAshlineRuntimeInput`. Editor IA/IMC
assets are optional.

## Campaign select (frontend)

- **Up / Down** — highlight a mission
- **Enter / A** — mission brief, then deploy if unlocked (READY / ACTIVE / DONE). LOCKED is ignored
- **Left / Right** — difficulty (Recruit / Regular / Veteran / Extreme)
- **Esc** — Settings (graphics, HUD, colorblind, volumes)

Console cheats (Output Log or `~` if enabled). **No-ops in Shipping** except graphics:

- `AshUnlockAll` — every mission becomes playable
- `AshDeploy 3` — jump to ASH-03 (also unlocks all, skips briefing)
- `AshPCUltra` / `AshPCHigh` / `AshPCBalanced` / `AshPCPerf` / `AshSteamDeck` / `AshDeck` / `AshPCLow` / `AshPCMed` / `AshLaptop` / `AshGfxAuto` / `AshGfxCycle` / `AshFPS 40` — named presets (`Docs/GRAPHICS.md`). **F8** cycles Ultra → High → Balanced → Perf → Deck → Laptop.
- `AshFSR` / `AshTSR` — FSR3 (TSR fallback) / Unreal TSR. Stay live in Shipping.
- `AshComplete` — finish the active mission and grant XP + credits
- `AshFrontend` — abort back to campaign select
- `AshGrantCredits 5000` / `AshSetRank 50` / `AshPrestige` / `AshOpenCrate`
- `AshBuySkin SKIN_FDE` / `AshEquipSkin WPN_AR_ASH16 SKIN_FDE`
- `AshBuyCosmetic CAMO_NIGHT` / `AshEquipCosmetic Camo CAMO_NIGHT` / `AshEquipCosmetic Headset HEAD_COMTAC` / `AshEquipCosmetic Backpack PACK_ASSAULT`
- `AshBuyWeapon WPN_AR_M4K` / `AshBuyAttachment WPN_AR_ASH16 MAG_60`
- `AshBuyEquipment EQ_SEMTEX` / `AshEquipEquipment Lethal EQ_SEMTEX`
- `AshUnlockMeta` / `AshListMeta` / `AshListArmory` — locker + armory (`Docs/META.md`)

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
- **Left Shift** — sprint (cancels on aim)
- **F** or **E** — interact (prompt when near an objective)
- **B** — cycle fire mode (Semi / Burst / Auto when the gun supports it)
- **Esc** — pause (Resume / Settings / Abort). Esc on pause root aborts to campaign

## DualSense (`IMC_Ashline_Gamepad`, still applied on Mac/iOS)

- Left stick move, right stick look
- **R2** fire, **L2** aim
- **Square** reload, **Cross** jump, **Circle** crouch, **Triangle** swap
- **LB / D-pad down** — interact
- **LS click** — sprint
- Share / D-pad up — camera toggle
- Start — pause / back
- D-pad right — cycle fire mode
- Adaptive triggers and haptics: `UAshlineDualSense` → `AshlineGameController.mm`

## iOS touch (`IMC_Ashline_Touch`)

`UAshlineTouchHUD` shows virtual stick, fire, aim, and camera toggle on iOS only.
Subclass in UMG and assign `TouchHUDClass` on `AAshlinePlayerController`.
