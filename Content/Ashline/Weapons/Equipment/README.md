# Equipment meshes

Drop lethals / tacticals / field props as `SM_{EquipmentId}` (e.g. `SM_EQ_FRAG`).

Catalog: `UAshlineEquipmentCatalog`. Soft refs: `ContentBindings.json` → `equipment`.

Runtime locker owns/equips these; authored throwables land with Niagara later. Missing meshes never crash — `LOAD_Quiet`.
