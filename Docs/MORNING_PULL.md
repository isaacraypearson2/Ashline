# Morning pull — overnight integrate (X3D_AMD)

Playable branch: **`cursor/overnight-integrate-morning`**

Stacks **#8** (maps / Movable lights / named DeviceProfiles) + **#14** (MI_Ashline materials on #8) + **#9** (HUD / feel) + **#13** (Shipping / cheat gating / quiet loads).

**Superseded — do not checkout:** PR **#10** (`cursor/release-readiness-fafe`). Release work is **#13** folded onto #9.

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

Do **not** stay on `cursor/movable-runtime-lights-5a13` (#8 only), `cursor/aaa-feel-layer-bb50` (#9 only), `cursor/release-fold-fafe` (#13 without #8 worlds), or `main`.

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

Aliases that must also work: `AshDeck` (same as `AshSteamDeck`), `AshPCPerf` / `AshPCHigh` / `AshPCBalanced`, `AshFSR` / `AshTSR`.

7. `AshSteamDeck` should snap handheld HUD / 800p-class / FSR. `AshPCUltra` should restore 1440p desktop Ultra.
8. Editor / PIE must **not hang** on DeviceProfiles (no self-parent loops). Named presets from **#8**: `Ashline_PC_Ultra` / `Ashline_PC_High` / `Ashline_PC_Balanced` / `Ashline_PC_Performance` / `Ashline_SteamDeck` / `Ashline_Laptop`.

Pass = playable morning branch. Full list: `Docs/TEST_PLAN.md`.
