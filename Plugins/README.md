# Plugins

Enabled from `Ashline.uproject`:

- **EnhancedInput** — all gameplay binds
- **ModelingToolsEditorMode** — editor blockout only

Recommended on the Windows machine (not vendored — missing plugin must not be required to open the project):

- **AMD FSR for UE 5.8** from [GPUOpen](https://gpuopen.com/learn/amd-fsr-plugin-updated-for-unreal-engine-58/) — `Docs/FAB_PACKS.md`
- **MetaHuman** plugin — only when importing Creator characters
- **Niagara** — engine module, already a game-module dependency

TSR is the built-in fallback if FSR is absent. Ads/IAP stay out (`UAshlineMonetizationHooks`).
