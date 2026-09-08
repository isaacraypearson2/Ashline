# Ashline master materials — parameter contract

Runtime factory: `UAshlineMaterialFactory`. Presentation helpers: `UAshlinePresentationLibrary::MakeTexturedMaterial` / `MakeWeaponMaterial` / `MakeCharacterMaterial` / `MakeSkinMaterial` / `MakeGlassMaterial`.

**Missing Quixel/Fab never breaks PIE or Shipping.** Empty texture slots stamp Engine fallbacks (`DefaultTexture`, `DefaultNormal`, `WhiteSquareTexture`, `Black`).

## Master graphs (create in-editor)

Run **Tools → Execute Python Script → `Scripts/create_master_materials.py`** after the first editor open (or `import_fab_kits.py`, which calls it).

| Master | Path | Used for |
| --- | --- | --- |
| Environment | `/Game/Ashline/Materials/PBR/Masters/M_Env_Master` | Ground, concrete, metal, wood, sand, snow, water, foliage, emissive |
| Weapon | `/Game/Ashline/Materials/PBR/Masters/M_Weapon_Master` | Guns, charms, attachments |
| Character | `/Game/Ashline/Materials/PBR/Masters/M_Character_Master` | Clothing, camos, AI uniforms |
| Skin | `/Game/Ashline/Materials/Skin/M_Skin_Master` | Faces, MetaHuman body SSS |
| Glass | `/Game/Ashline/Materials/Glass/M_Glass_Master` | Windows, Night Glass, harbor |
| Decal | `/Game/Ashline/Materials/Decals/M_Decal_Master` | Bullet / dirt / signage |

Surface instances live under `/Game/Ashline/Materials/PBR/Instances/MI_Env_*` (plus `MI_Glass_Clear`, `MI_Skin_Operator`). Canonical names: `UAshlineContentManifest::SurfaceInstancePath`.

## Texture parameter names (stamp every alias)

`StampTextureSet` writes **all** of these so Megascans, Fab military packs, and Engine materials all bind.

| Slot | Parameter aliases | Engine fallback |
| --- | --- | --- |
| Base color | `BaseColor`, `Base Color`, `Albedo`, `Diffuse`, `Color`, `BaseColorMap` | `/Engine/EngineResources/DefaultTexture` |
| Normal | `Normal`, `NormalMap`, `Normalmap` | `/Engine/EngineResources/DefaultNormal` |
| Packed ORM | `ORM`, `ARM`, `Mask`, `OcclusionRoughnessMetallic` | DefaultTexture (neutral) |
| Roughness | `Roughness`, `RoughnessMap`, `RoughnessValue` | scalar default per surface |
| Metallic | `Metallic`, `MetallicMap`, `MetallicValue` | scalar default per surface |
| AO | `AmbientOcclusion`, `AO`, `Occlusion` | white |
| Emissive | `Emissive`, `EmissiveMap`, `EmissiveColor`, `EmissiveStrength` | Black |
| Height | `Height`, `Displacement`, `HeightMap` | Black |
| Opacity | `Opacity`, `OpacityMask`, `OpacityValue` | white / 1.0 |
| UV | `Tiling`, `UVScale`, `TextureScale` | 1–2.5 by surface |
| Glass | `IOR` (1.52), `Opacity` (0.28) | — |
| Skin | `Subsurface`, `SubsurfaceColor` | — |

Megascans packed maps are **ORM** (R=AO, G=Roughness, B=Metallic). Some Fab weapon packs use **ARM**. Both names are stamped.

## Runtime resolve order (surfaces)

1. Mission kit soft ptr (`DA_Kit_ASH##` Ground/Wall/Trim/Foliage/Glass/Decal)
2. Canonical kit file `M_Ground_<Slug>` / `M_Wall_*` / `M_Glass_*`
3. Authored surface MI `MI_Env_*`
4. Master graph + texture set (Fab maps if assigned, else Engine)
5. Starter Content (`M_Ground_Grass`, `M_Concrete_Tiles`, `M_Metal_Steel`, …)
6. Engine `DefaultMaterial` / `WorldGridMaterial` / `BasicShapeMaterial`

Weapons: locker `MaterialOverride` → `DA_WPN_*.SkinMaterial` / `TextureSet` → `M_Weapon_Master` → metal Engine tint.

Characters: camo `MaterialOverride` → `DA_Hero_Operator.BodyMaterialOverride` / `SkinMaterial` → `M_Character_Master` / `M_Skin_Master` → Engine tint.

## After Fab import (no C++ change)

1. Duplicate or rename the Megascans material onto the kit canonical path **or** assign the soft ptr on `DA_Kit_ASH##`.
2. If the pack uses different parameter names, create an MI of `M_Env_Master` and plug the textures into the aliases above.
3. PIE. Graybox layout numbers are unchanged; only the material chain upgrades.

Do **not** commit Quixel/Fab/MetaHuman binaries unless the license allows **and** you intend Git LFS.
