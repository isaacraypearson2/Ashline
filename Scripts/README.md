# Scripts

Editor-only Python for UE 5.8.2. Play works without running any of these.

| Script | Purpose |
| --- | --- |
| `create_ashline_play_assets.py` | Optional `/Game/Ashline/Maps/ASH_Playable` + IA/IMC assets |
| `import_fab_kits.py` | Folder tree + kit / weapon / cosmetic / AI DataAsset stubs. **Does not download Fab.** |
| `assign_interim_meshes.py` | Engine / GASP / TP mannequin → hero+AI. Hides capsules. Stamps Starter materials on kits. |
| `validate_meta_catalog.py` | No-UE check of `Meta.json` |
| `validate_content_layout.py` | No-UE check of folders + `ContentBindings.json` |
| `validate_release_config.py` | DeviceProfiles (no self-parent), Shipping gates, named presets |

Tools → Execute Python Script, or Output Log:

```
py "Scripts/import_fab_kits.py"
```
