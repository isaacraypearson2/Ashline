# Frontend

Create `/Game/Ashline/Maps/Frontend/ASH_Frontend` as a graybox hub.

Suggested widgets (UMG, editor-authored):

- Operator creator (`UAshlineOperatorCreator` + `UAshlineProgressionSubsystem::ApplyOperator`)
- Campaign select (catalog from `UAshlineMissionCatalog`)
- Armory / attachments / upgrades
- Prestige confirm (`PrestigeReset` at rank 50)
- Play-earned crate open
- Graphics settings (`UAshlineGraphicsSettings`)
- Difficulty (`SetDifficulty`)

Game instance helpers: `OpenFrontend`, `TravelToMission`.
