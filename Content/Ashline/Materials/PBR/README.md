# PBR masters

Author these in-editor. Runtime tints Engine / Starter Content until they exist. Loads are `LOAD_NoWarn | LOAD_Quiet`.

| Asset | Path | Used by |
| --- | --- | --- |
| Weapon master | `/Game/Ashline/Materials/PBR/M_WeaponMaster` | `MakeWeaponMaterial` |
| Skin master | `/Game/Ashline/Materials/PBR/M_SkinMaster` | `MakeSkinMaterialInstance` |
| Character master | `/Game/Ashline/Materials/PBR/M_CharacterMaster` | `MakeCharacterMaterial` |
| Environment master | `/Game/Ashline/Materials/PBR/M_EnvironmentMaster` | `MakeEnvironmentMaterial` |

Create **Material Instances** from the matching master (`MI_WPN_*`, `MI_SKIN_*`, `MI_CHAR_*`, `MI_ENV_*`) rather than unique graphs.

## Parameter names (set on MIDs)

| Param | Type | Notes |
| --- | --- | --- |
| `Color` / `BaseColor` / `Tint` / `Albedo` | Vector | Locker tint |
| `Roughness` | Scalar | 0–1 |
| `Metallic` | Scalar | 0–1 |
| `Specular` | Scalar | 0–1 |
| `Emissive` / `EmissiveStrength` | Scalar | ASH-12 trim |
| `EmissiveColor` | Vector | |
| `Normal` / `BaseColorTexture` / `RoughnessTexture` / `MetallicTexture` / `AOTexture` | Texture | Optional; ignored if missing |

Helpers: `UAshlinePresentationLibrary::ApplyMaterialParams`, `DefaultParamsForSurface`, `DefaultParamsForWeaponClass`.

Per-mission kit materials still win when present (`M_Ground_ASH01_WireCut`, …).
