# Data / Kits

Create these Primary DataAssets in-editor (`Scripts/import_fab_kits.py`):

- `DA_Kit_ASH01` … `DA_Kit_ASH12` (`UAshlineEnvironmentKit`) — leave `bOverrideMood` false unless you authored lighting
- `DA_Hero_Operator` + `DA_AI_*` (`UAshlineCharacterPresentation`)
- `DA_WPN_{WeaponId}` (`UAshlineWeaponVisual`)
- `DA_COS_{CosmeticId}` / `DA_SKIN_{SkinId}` (`UAshlineCosmeticVisual`)

Catalog C++ already binds MeshOverride / MaterialOverride to `ContentBindings.json` paths. DataAssets overlay those when present.

Then run `Scripts/assign_interim_meshes.py` so hero+AI are mannequins, not capsules.
