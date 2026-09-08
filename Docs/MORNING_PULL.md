# Morning pull — overnight integrate (X3D_AMD)

Playable branch: **`cursor/overnight-integrate-morning`**

Stack (merge order already applied):

| Order | PR | Branch | What you get |
| --- | --- | --- | --- |
| Base | **#8** | `cursor/movable-runtime-lights-5a13` | Maps, Movable lights, named DeviceProfiles |
| 1 | **#14** | `cursor/fab-material-pipeline-97e3` | `MI_Ashline*` MaterialFactory, `create_master_materials.py`, PHASE2_FAB, streaming pools |
| 2 | **#9** | `cursor/aaa-feel-layer-bb50` | HUD / VFX / audio / feel |
| 3 | **#13** | `cursor/release-fold-fafe` | Shipping, cheat gating, quiet loads, sprint, `AshFSR` / `AshTSR` |
| 4 | **#12** | `cursor/overnight-armory-production-fdb2` | Production SP armory: **45 weapons**, **64 attachments**, **153 cosmetics**, **140 skins**, 16 equipment, deep gunfeel |

**#12 is folded.** Do not checkout `cursor/overnight-armory-production-fdb2` for morning PIE — this integrate already contains it.

**Superseded — do not checkout:**
- PR **#10** (`cursor/release-readiness-fafe`) — replaced by **#13** stacked on #9
- PR **#11** (`cursor/content-texture-pipeline-97e3`) — old parallel texture branch; **#14** is the texture pipeline on #8

Machine: Windows 11 `X3D_AMD`, repo at `C:\Users\isaac\Documents\Ashline`, UE **5.8.2**.

## 1. Fetch and checkout

Open **cmd**. Exact commands:

```bat
cd /d C:\Users\isaac\Documents\Ashline
git fetch origin
git checkout cursor/overnight-integrate-morning
git pull --ff-only origin cursor/overnight-integrate-morning
```

If checkout refuses because of local edits:

```bat
git stash push -u -m "morning-before-overnight"
git checkout cursor/overnight-integrate-morning
git pull --ff-only origin cursor/overnight-integrate-morning
```

Do **not** stay on `cursor/movable-runtime-lights-5a13` (#8 only), `cursor/fab-material-pipeline-97e3` (#14 only), `cursor/aaa-feel-layer-bb50` (#9 only), `cursor/release-fold-fafe` (#13 without #8 worlds), `cursor/overnight-armory-production-fdb2` (#12 without feel/shipping), or `main`.

## 2. Rebuild AshlineEditor (Win64 Development)

```bat
cd /d C:\Users\isaac\Documents\Ashline
"%PROGRAMFILES%\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AshlineEditor Win64 Development -Project="%CD%\Ashline.uproject" -WaitMutex
```

If VS is stale after the C++ stack:

```bat
"%PROGRAMFILES%\Epic Games\UE_5.8\Engine\Build\BatchFiles\GenerateProjectFiles.bat" -project="%CD%\Ashline.uproject" -game -engine
"%PROGRAMFILES%\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" AshlineEditor Win64 Development -Project="%CD%\Ashline.uproject" -WaitMutex
```

This VM has no Unreal Editor. The Win64 compile is the morning rebuild on `X3D_AMD`.

## 3. PIE smoke (do this before anything else)

1. Double-click `C:\Users\isaac\Documents\Ashline\Ashline.uproject` (engine **5.8**).
2. If a dialog says **`ASH_Playable` is missing**, dismiss it. Play still hosts on `/Engine/Maps/Entry`.
3. Click **Play**. Campaign select should list ASH-01…12.
4. Deploy **ASH-01 Wire Cut** (briefing Enter → deploy).
5. Confirm **no editor banner** `Lighting needs to be rebuilt` (Movable sun/fill, `r.AllowStaticLighting=False`, `bForceNoPrecomputedLighting=True`).
6. Console (`~`), **in this order**:

```
AshSteamDeck
AshPCUltra
```

Aliases that must also work: `AshDeck` (same as `AshSteamDeck`), `AshPCPerf` / `AshPCHigh` / `AshPCBalanced`, `AshPCLow` / `AshPCMed`, `AshFSR` / `AshTSR`.

7. `AshSteamDeck` should snap handheld HUD / 800p-class / FSR. `AshPCUltra` should restore 1440p desktop Ultra.
8. Editor / PIE must **not hang** on DeviceProfiles (no self-parent loops). Named presets from **#8**: `Ashline_PC_Ultra` / `Ashline_PC_High` / `Ashline_PC_Balanced` / `Ashline_PC_Performance` / `Ashline_SteamDeck` / `Ashline_Laptop`.
9. **B** (gamepad D-pad right) cycles fire mode. HUD plate shows `AUTO` / `BURST` / `SEMI` plus ammo type.
10. Optional armory cheats (no-ops in Shipping except graphics): `AshListArmory`, `AshBuyWeapon WPN_AR_M4K`, `AshBuyEquipment EQ_SEMTEX`, `AshEquipEquipment Lethal EQ_SEMTEX`.
11. Frontend locker line should mention **Lethal / Tactical**. `AshListArmory` should print a 45-gun roster.

Pass = playable morning branch. Full list: `Docs/TEST_PLAN.md`.
