# Scripts

Editor-only Python for UE 5.8.2. Play works without running any of these.

| Script | Purpose |
| --- | --- |
| `create_ashline_play_assets.py` | Optional `/Game/Ashline/Maps/ASH_Playable` + IA/IMC assets |
| `import_fab_kits.py` | Folder tree + kit / weapon / cosmetic / AI / equipment DataAsset stubs. **Does not download Fab.** |
| `assign_interim_meshes.py` | Engine / GASP / TP mannequin → hero+AI. Hides capsules. Stamps Starter materials on kits. |
| `generate_overnight_catalogs.py` | Regenerates `Weapons.json` / `Meta.json` / `AI.json` / `Equipment.json` / bindings from C++ |
| `validate_meta_catalog.py` | No-UE check of `Meta.json` |
| `validate_content_layout.py` | No-UE check of folders + `ContentBindings.json` |

Tools → Execute Python Script, or Output Log:

```
py "Scripts/import_fab_kits.py"
```
