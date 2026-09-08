# Ashline packaging — Win64 Shipping

Primary public binary: **Windows 64-bit Shipping**, DX12, UE 5.8.2.  
Steam Deck customers run this same depot through **Proton**.

This Cloud / Linux agent **cannot** cook. Run everything on `X3D_AMD`.

## One-shot (recommended)

```bat
set UE58=C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles
set PROJ=%CD%\Ashline.uproject
set OUT=%CD%\Dist\Win64

"%UE58%\RunUAT.bat" BuildCookRun ^
  -project="%PROJ%" ^
  -platform=Win64 ^
  -clientconfig=Shipping ^
  -serverconfig=Shipping ^
  -cook -stage -pak -package -archive ^
  -archivedirectory="%OUT%" ^
  -build -prereqs -distribution ^
  -utf8output
```

Development iterate (do **not** upload):

```bat
"%UE58%\Build.bat" AshlineEditor Win64 Development -Project="%PROJ%" -WaitMutex
```

## What the project already sets

| Setting | Where |
| --- | --- |
| Shipping default cook config | `Config/DefaultGame.ini` → `ProjectPackagingSettings` |
| `ForDistribution=True`, no debug files | same |
| Skip editor content | `bSkipEditorContent=True` |
| Pak + shared shader code | same |
| Required cook map | `/Engine/Maps/Entry` only |
| Always cook | `/Game/Ashline`, `/Game/Ashline/Data` |
| Never cook | `/Game/Ashline/__EditorOnly` |
| No logging in Shipping | `Source/Ashline.Target.cs` |
| Cheats inert | `AshlineShipping::CheatsAllowed()` |
| Debug HUD off | `DefaultGame.ini` — empty `DebugDisplay` |

**Do not** add `/Game/Ashline/Maps/ASH_Playable` to `MapsToCook` until that package exists. A missing map fails the cook. Runtime remaps GameDefaultMap when the asset is absent.

## Hygiene checklist before upload

1. `python Scripts/validate_release_config.py` (any machine)
2. Optional on the cook box: Starter Content + FSR3 plugin enabled so they **are** cooked
3. Fab binaries you intend to ship must be in the project **before** cook (not in git)
4. Confirm Shipping exe: `AshUnlockAll` does nothing; campaign still plays
5. Steamworks: single Win64 depot + redistributable prereqs (VC++ / DirectX)
6. Do not enable an ads plugin. Do not cook dedicated server

## Mac / iOS

Modules still compile. They are **not** the public SKU. Do not archive Mac/iOS for the first Steam build.

## Failure modes

| Symptom | Likely cause |
| --- | --- |
| Cook fails on `ASH_Playable` | Map listed but missing — remove from MapsToCook |
| Ensure on missing `/Game/Ashline/Weapons/...` | Old `LoadObject`; this branch uses `AshlineLoad` quiet loader |
| Black boot | GameDefaultMap missing and Entry remap failed — check `FAshlineModule` |
| Huge download | You cooked Starter Content + Megascans; expected. Pak + chunk later |
| FSR CVars missing in package | Plugin not enabled at cook; TSR still works |
