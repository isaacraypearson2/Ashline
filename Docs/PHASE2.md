# Phase 2 — next steps (Windows box `X3D_AMD`)

Exact pack names, install order, and 9070 GRE notes: **`Docs/PHASE2_FAB.md`**. Soft refs are wired in the catalog (`MeshOverride` / `MaterialOverride` point at `ContentBindings.json` paths).

Do these on the 9070 GRE machine:

1. **Add Starter Content** and re-PIE every mission (instant PBR upgrade).
2. **Enable AMD FSR3** from Fab; confirm `r.FidelityFX.FSR3.Enabled` and compare vs TSR.
3. **MetaHuman** hero + 2–3 AI variants (`Docs/CHARACTERS.md`).
4. **Fab weapon pack** — assign meshes on `UAshlineWeaponVisual` for ASH-16, C9, G28-L, M870-K, M17-A, SASS-7, M250.
5. **Megascans kits** per mission folder; fill `DA_Kit_ASH01`…`12`.
6. Author **Niagara** muzzle / impact; drop Sound Cues into `Content/Ashline/Audio/`.
7. Replace runtime blockouts with **authored `.umap`s** once kits land (`MapAsset` already points at `/Game/Ashline/Maps/Campaign/ASH##_*`).
8. Locomotion AnimBPs, navmesh, behavior trees.
9. UMG frontend / **armory / operator locker** (Canvas HUD + C++ meta remain playable). Bind `MeshOverride` / `MaterialOverride` on `FAshlineCosmeticDefinition` and `FAshlineWeaponSkinDefinition`.
10. Package a **Win64 Shipping** build (`Docs/PACKAGING.md`) and profile Lumen/RT at 1440p plus Steam Deck (`Docs/STEAM_DECK.md`, `Docs/PERFORMANCE.md`). Public-release gate: `Docs/RELEASE_CHECKLIST.md`.

Still do **not** add ads. Still do **not** add multiplayer netcode.
