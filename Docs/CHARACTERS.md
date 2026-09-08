# Characters — mannequin → MetaHuman

`AAshlineCharacter` and `AAshlineAICharacter` prefer a **skeletal mesh** over the old cube body.

## Runtime resolve order (hero)

1. `AAshlineCharacter::HeroMeshOverride`
2. Project Settings → Ashline Presentation → `DefaultHeroMesh` / `HeroPresentation`
3. `/Game/Ashline/Data/Kits/DA_Hero_Operator`
4. Engine / template mannequin paths (`SKM_Manny`, editor DefaultSkeletalMesh, …)
5. Tinted humanoid blockout if nothing loads (capsule is hidden)

AI is the same with `BodyMeshOverride` + per-archetype DataAssets + a tint so rifleman / officer / heavy read differently even on a shared mesh.

When a skeletal mesh is assigned, the cube `GrayboxBody` is hidden.

Equipped operator cosmetics (`Docs/META.md`) tint the hero mesh (or the head/torso/limb blockout) from catalog `PreviewTint`. Catalog `MeshOverride` / `MaterialOverride` are **auto-bound** to `Content/Ashline/Data/ContentBindings.json` paths — when you drop `SK_CAMO_NIGHT` or `M_SKIN_FDE` they resolve with no extra assign. Clothing parts (helmet/vest/pants/gloves/boots) attach if `SM_{Id}` exists under `Characters/Hero/Parts/`. Voice packs are ids only until MetaSounds land. AI archetypes include Grenadier / Elite / Spotter (`Docs/PHASE_OVERNIGHT.md`).

Run `Scripts/assign_interim_meshes.py` so hero+AI use Engine / GASP / TP mannequins instead of capsules.

## MetaHuman Creator import (Phase 2)

These assets are **not shipped in git**.

1. Install the **MetaHuman** plugin for UE 5.8.2.
2. Create or download a character in **MetaHuman Creator** (web).
3. Quixel Bridge → Unreal → destination:
   - `/Game/Ashline/Characters/MetaHuman/`
4. Open the generated Blueprint (e.g. `BP_AshlineHero`).
5. Either:
   - Reparent / copy the body mesh onto `AAshlineCharacter` (C++ class), **or**
   - Create `DA_Hero_Operator` (`UAshlineCharacterPresentation`) and set:
     - `BodyMesh` → MetaHuman body
     - `FaceMesh` → face (optional)
     - `AnimClass` → the MetaHuman anim BP
     - `MeshRelativeLocation` = `(0, 0, -96)`
     - `MeshRelativeRotation` = `(0, -90, 0)`
6. Assign the same DataAsset in Project Settings → Ashline Presentation.
7. PIE. FPS hides the body (`OwnerNoSee`); TPS should show the MetaHuman. Weapons stay on the first-person camera.

## Animation

Phase 1 does **not** ship locomotion AnimBPs. The mannequin/MetaHuman will T-pose or use whatever default the mesh has until you assign `AnimClass`. Gameplay (move, crouch, fire) does not depend on it.

Suggested Phase 2: Game Animation Sample or MetaHuman locomotion, then an AimOffset for FPS arms.

## Operator cosmetics

`UAshlineOperatorCreator` still stores callsign / camo / face index. Wire those to MetaHuman texture/wardrobe slots in Phase 2 — do not pretend they already swap Wardrobe items.
