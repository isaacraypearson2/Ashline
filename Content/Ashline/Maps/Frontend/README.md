# Frontend

**Runtime campaign select is live on Play** (Canvas HUD on `AAshlineHUD`).
Up/Down pick a mission, Left/Right change difficulty, Enter deploys.
Locked missions stay locked until the previous mission is completed (or
`AshUnlockAll` in the console).

Optional authored hub: `/Game/Ashline/Maps/Frontend/ASH_Frontend`.
`UAshlineGameInstance::OpenFrontend` opens that map when it exists, otherwise
it returns to the runtime select.

Suggested later UMG (not required for the playthrough):

- Operator creator (`UAshlineOperatorCreator`)
- Armory / attachments / upgrades
- Prestige confirm
- Play-earned crate open
- Graphics settings
