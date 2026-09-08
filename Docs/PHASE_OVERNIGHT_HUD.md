# Phase overnight — HUD / feel layer (morning summary)

Shipped on `cursor/aaa-feel-layer-bb50` from `cursor/aaa-content-art-pass-f576` (newer than `main`). **Did not touch** `AshlineGrayboxBuilder` mission layouts (map-scale agent owns that). Campaign loop + SP meta are intact. No multiplayer.

This VM cannot run UE 5.8 PIE. Compile/play on **Windows `X3D_AMD`**. Console `AshDeploy` still skips the briefing and drops straight into a mission.

## What a player sees now

### Campaign / menus
- Frontend is a dimmed **campaign board**: selected row highlight, right-side brief of the highlighted mission, rank / credits / skin name, graphics line.
- **Enter / A** opens a full **MISSION BRIEF** (location, briefing wrap, primary/optional objectives). **Enter / A** again deploys. **Esc** aborts to select.
- **Esc on frontend** opens **Settings** (same sheet as pause).
- Pause is a real menu: **Resume / Settings / Abort**. Esc from the root still aborts (old behavior). Esc from Settings returns to the pause list.
- Debrief shows **stars**, XP / credits / crates, **kill count**, then Enter returns to select. Finale still **ASHLINE CUT**.

### COD-style SP HUD (Engine Canvas + default fonts — no UMG assets required)
- **Steam Deck / 800p:** safe zone ≥ 8%, HUD scale 1.2×, larger ammo + **[LB / F]** interact plate. Forced when preset is `Ashline_SteamDeck`, resolution ≤ 1280×800, or Settings → Handheld HUD.
- **Desktop Ultra:** tighter inset, thinner bars, same layout language.
- Bottom-left **HP + armor** bars. Bottom-right **weapon plate**: display name, class, **AUTO/SEMI**, **skin display name**, mag / reserve, reload bar.
- Center **crosshair states**: hip, ADS (tight), empty (danger), reload (gold), hit/kill ticks.
- **Hit markers** + **KILLED &lt;archetype&gt;** + kill feed. Colorblind palettes (protan/deutan/tritan/high contrast) live in Settings.
- **Objective list** top-left. World **EXFIL / objective diamond** projected to screen.
- Near an unconsumed trigger: large **interact prompt**. Overlap still auto-completes (campaign cannot soft-lock). **F / E / LB / D-pad down** also completes.
- Damage **vignette**, opening **subtitle** from the mission briefing (8s), KIA overlay + **death cam** (~2.4s) then respawn at the last player start.

### Settings (persisted on `UAshlineGameUserSettings`)
Graphics preset (incl. **SteamDeck**), HUD scale, safe zone, colorblind, camera shake, hit markers, vignette, hip FOV, master/music volume, handheld HUD.

| Preset | Role |
| --- | --- |
| `Ashline_PC_Ultra` | 1440p / 9070 GRE, VSync off, RT if capable |
| `Ashline_PC_Balanced` | Same PC, cheaper Lumen |
| `Ashline_SteamDeck` | 1280×800, 60 fps cap, VSync on, **no RT**, TSR/FSR Performance, large HUD |

Console: `AshPCUltra` / `AshPCBalanced` / `AshDeck`. Hardware with `SteamDeck=1`, device “Jupiter”, or 1280×800 auto-selects Deck.

### Camera / combat feel
- ADS **FOV lerp** (sniper tighter than AR). Hip FOV from Settings.
- Fire/hit **camera kick** that damps (disable in Settings).
- Armor absorbs ~65% until empty; health bar + armor bar both update.
- Weapon fire still recoils the mouse; DualSense trigger path unchanged.

### VFX (soft ref → StarterContent → Engine primitive)
`UAshlineCombatFX` + `AAshlineTracerStreak`:
- Muzzle: Niagara on `UAshlineWeaponVisual`, else Starter `P_Explosion` tiny, plus the existing muzzle **point light**.
- **Tracers:** Niagara if assigned, else an emissive cube streak (`/Engine/BasicShapes/Cube` + Engine emissive).
- Impacts: decal + sparks (world) or blood-tinted burst (AI). Heavy kills spawn a small **explosion** stub.
- Drop authored Niagara on `/Game/Ashline/FX/{Muzzle,Tracers,Impacts,Explosions}/` — no code change.

### Audio beds
`UAshlineAudioDirector` now has music + **ambience** 2D beds, **combat stinger** on first hit/damage (volume-up of the music layer), surface-pitched **footsteps**, UI confirm/back, explosion. Soft refs first; StarterContent `Explosion01` / `Fire01` if present; **silent and crash-free** if missing.

Canonical paths: `Docs` in `Content/Ashline/Audio/README.md`. Volumes follow Settings buses.

## What was deliberately not done
- No edits to graybox **map scale / layout**.
- No netcode, no ads.
- No committed Fab/Niagara binaries (git policy). Systems run on Engine/Starter fallbacks until packs land.
- UMG widgets are still Phase 2 locker/armory; this pass ships a **complete Engine-font Canvas HUD** so public play does not wait on `.uasset` widgets.

## Morning playtest (Windows)
1. PIE → campaign board. Esc settings → cycle **SteamDeck** and confirm HUD jumps in size; restore **PC_Ultra**.
2. Enter on ASH-01 → brief → Enter deploy. Opening subtitle. Fire at a bot: muzzle light, tracer streak, hit ticks, kill confirm. ADS FOV eases in.
3. Walk to glow → large **F** prompt; overlap still completes. Extract finishes mission; debrief shows stars + kills.
4. `AshUnlockAll` + `AshDeploy 12` still skips brief. Meta execs (`AshBuySkin`, prestige) unchanged.
5. Die: red vignette, KIA, orbit death cam, respawn with full HP/armor.
6. Optional: Add Starter Content and confirm fire/hit/explosion cues + `P_Sparks`.

Campaign save slot remains `AshlineCampaign`.
