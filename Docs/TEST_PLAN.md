# Test plan — Windows UE 5.8.2 (9070 GRE class)

Primary playtest machine: **Windows 11 `X3D_AMD`**, Ryzen 5 7500X3D, ~32 GB, **AMD Radeon RX 9070 GRE**, **1440p**, UE 5.8.2 already installed.

This VM / CI environment does **not** run Unreal Editor. Execute this list on the Windows PC.

## Install

- [ ] UE 5.8.2 + VS 2022 C++ workload
- [ ] `Ashline.uproject` opens and compiles `AshlineEditor` (Win64 Development)
- [ ] Missing `ASH_Playable` dialog (if any) can be dismissed; Play still works
- [ ] Optional: Starter Content pack
- [ ] Optional: AMD FSR3 plugin from Fab (TSR must still work without it)

## Graphics

- [ ] PIE HUD shows **Ashline_PC_Ultra** (or the auto-detected rec) and an RHI name containing **D3D12**
- [ ] `stat fps` at 1440p — high-refresh capable (VSync off) on the 9070 GRE
- [ ] `r.RayTracing` / `r.Lumen.HardwareRayTracing` are 1 if the driver reports HW RT
- [ ] `AshPCHigh` / `AshPCBalanced` / `AshPCPerf` drop cost; `AshPCUltra` restores
- [ ] `AshSteamDeck` snaps 1280×800, 40 fps, safe-zone HUD; `AshPCUltra` returns to 1440p
- [ ] `AshGfxAuto` re-detects; **F8** cycles named presets
- [ ] `AshDeck` / `AshFSR` / `AshTSR` apply without a crash
- [ ] Nanite/Lumen/VSM do not spam fatal logs
- [ ] After `create_master_materials.py`, Content Browser shows `MI_AshlineConcrete` (or PIE still quiet-falls back to Engine)
- [ ] `stat streaming` on Ultra: pool ~5600; `AshSteamDeck`: pool 1800, `r.VT.PoolSizeScale` ~0.45

## Campaign loop (do not break)

- [ ] PIE opens campaign select (not an empty void)
- [ ] Enter on a READY mission opens **MISSION BRIEF**; Enter again deploys. Esc from brief returns to select
- [ ] WASD / mouse / jump / crouch / **Left Shift sprint** / fire / aim / reload / swap / V toggle / F interact
- [ ] Combat HUD: weapon name + **AUTO/SEMI** + **skin display name** + mag/reserve; HP + armor bars; objective marker
- [ ] Hit markers / kill confirm on AI; muzzle light + tracer streak; ADS FOV eases in
- [ ] ASH-01 INFIL → CUT → EXFIL awards XP **and credits** and unlocks ASH-02
- [ ] Esc pause → Settings (graphics preset incl. **Ashline_SteamDeck**) → Back → Resume; Esc on pause root aborts to select
- [ ] `AshDeck` enlarges HUD / safe zone; `AshPCUltra` restores desktop
- [ ] Frontend shows Rank / Prestige / Credits / equipped camo + primary skin
- [ ] Difficulty Left/Right changes AI pressure (Veteran/Extreme extra bots)
- [ ] Esc pause → Enter resume; Esc twice aborts to select
- [ ] `AshUnlockAll` then `AshDeploy 12` reaches the finale (skips briefing)
- [ ] Shipping package: `AshUnlockAll` and `AshGrantCredits` do nothing; `AshDeck` still works
- [ ] After ASH-12, **ASHLINE CUT** and save slot `AshlineCampaign` persists
- [ ] Ads stay off

## SP meta (locker / economy / prestige)

- [ ] New save starts at 750 credits, `CAMO_FIELD`, `SKIN_FACTORY`
- [ ] `AshGrantCredits 5000` then `AshBuySkin SKIN_FDE` then `AshEquipSkin WPN_AR_ASH16 SKIN_FDE` — redeploy and the ASH-16 tints tan
- [ ] `AshBuyCosmetic CAMO_NIGHT` + `AshEquipCosmetic Camo CAMO_NIGHT` — TPS / blockout body reads darker blue
- [ ] `AshUnlockMeta` owns the catalog; prestige-gated gilt stays locked until prestige
- [ ] `AshSetRank 50` then `AshPrestige` grants/equips `CAMO_PRESTIGE`, `SKIN_GOLD`, `CHARM_SPINE`; rank returns to 1
- [ ] Weapon upgrade spends credits (`UpgradeWeapon` / armory path)
- [ ] 1.x save slot migrates to 2.0.0 without wiping missions
- [ ] `AshListMeta` dumps cosmetic + skin ids
- [ ] No multiplayer / listen-server objects were added

## Presentation (Phase 1 bar)

- [ ] Each of ASH-01…12 has a **distinct** lighting/fog mood (night vs desert vs snow vs red finale)
- [ ] ASH-01 is a **night raid**, not a courtyard: LZ woods → trench → outer fence → motor pool → relay compound → creek / QRF (≈520 m class)
- [ ] Remaining missions have long infil → objective → exfil lanes (3–6× the old graybox footprint)
- [ ] Buildings show metal trim + glass; roads read asphalt; crates/lamps/debris exist without Fab kits
- [ ] No editor banner **Lighting needs to be rebuilt** in PIE (Movable lights + force no precomputed lighting)
- [ ] No spam about competing directional lights / missing StarterContent when the pack is not installed
- [ ] Surfaces are not a single flat gray — materials tint / Engine PBR (StarterContent only if the pack exists)
- [ ] Practical lights on objectives; fog + post-process visible; interior volumes in CQB maps
- [ ] FPS weapon is a **compound rifle-like mesh**, not a lone cube
- [ ] Firing shows a muzzle flash light + tracer; impacts spawn a decal / sparks (blood tint on AI) when Engine/Starter assets exist
- [ ] Death: vignette → KIA death cam → respawn with full HP/armor
- [ ] AI is a humanoid mesh when a mannequin/MetaHuman path resolves; otherwise a tinted body (not an invisible capsule)
- [ ] Audio is silent unless cues were imported — no crash when slots are empty. StarterContent `Explosion01` / `Fire01` play if the pack is added
- [ ] 1280×800 or `AshDeck`: interact prompt readable, ammo large, HUD inside safe zone

## Plugins / input

- [ ] Enhanced Input (runtime IMC still works with no `.uasset` IA files)
- [ ] Keyboard/mouse first-class; DualSense/XInput gamepad mapping is active on Windows
- [ ] Left Shift / LS click sprint; aim cancels sprint; LB stays interact
- [ ] Steam Deck / Proton: `SteamDeck=1` or `-steamdeck` selects `Ashline_SteamDeck` (`AshDeck`)

## Honest failures

If Quixel/Fab/MetaHuman folders are empty, that is **not** a test failure. Phase 2 import is documented in `Docs/PHASE2_FAB.md`.

## Phase 2 content wiring (after scripts on the Windows box)

- [ ] `import_fab_kits.py` then `assign_interim_meshes.py` — hero+AI are mannequins (or blockout), **not capsules**
- [ ] Dropping `M_SKIN_FDE` at the bindings path + `AshEquipSkin WPN_AR_ASH16 SKIN_FDE` paints the gun
- [ ] Dropping `M_CAMO_NIGHT` + equip Camo applies the material
- [ ] Empty `DA_Kit_ASH##` does **not** reset ASH-01 to daylight (`bOverrideMood` false)
- [ ] Each mission still has a distinct mood (fill/moon, fog, PP) without Megascans
