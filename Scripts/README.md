# Scripts

Editor-only Python for UE 5.8.2. Play works without running any of these.

| Script | Purpose |
| --- | --- |
| `create_ashline_play_assets.py` | Optional `/Game/Ashline/Maps/ASH_Playable` + IA/IMC assets |
| `import_fab_kits.py` | Folder tree + kit / weapon / cosmetic / AI DataAsset stubs. Calls `create_master_materials.py`. **Does not download Fab.** |
| `create_master_materials.py` | `MI_Ashline*` / `M_Ashline*` / `M_*` in `/Game/Ashline/Materials/PBR/` |
| `fab_install_plan.py` | Print the evening clock (no UE) |
| `assign_interim_meshes.py` | Engine / GASP / TP mannequin → hero+AI. Hides capsules. Stamps Starter materials on kits. |
| `validate_meta_catalog.py` | No-UE check of `Meta.json` |
| `validate_content_layout.py` | No-UE check of folders + `ContentBindings.json` |
| `validate_release_config.py` | No-UE Shipping / DeviceProfile / cheat-gate hygiene |

Tools → Execute Python Script, or Output Log:

```
py "Scripts/import_fab_kits.py"
```
