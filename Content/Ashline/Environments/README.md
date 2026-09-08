# Environments

Per-mission kits (empty until Fab/Megascans import):

```
ASH01_WireCut/ … ASH12_Ashline/
Shared/
```

Canonical materials: `M_Ground_<FolderName>`, `M_Wall_<FolderName>`, `M_Trim_<FolderName>`, `M_Foliage_<FolderName>`.

Runtime graybox (no binaries required): AAA-scale footprints, Movable lighting, volumetric fog, unbound + interior post-process, street lamps, crate stacks, trim, glass, foliage stubs, asphalt roads. Drop Megascans onto the kit DataAssets and they replace the Engine-tinted primitives.

See `Docs/PHASE2_FAB.md` for the exact search list and install order.
