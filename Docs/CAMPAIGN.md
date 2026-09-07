# Ashline campaign (ASH-01 … ASH-12)

Runtime source of truth: `UAshlineMissionCatalog::BuildCampaign()`.
Designer mirror: `Content/Ashline/Data/Campaign.json`.

Saves between missions in slot `AshlineCampaign`. Completing a mission unlocks the next, grants XP, and awards crate tokens.

| Code | Title | Location | Tone |
| --- | --- | --- | --- |
| ASH-01 | Wire Cut | Border relay | Quiet infil, cut fiber, ghost optional |
| ASH-02 | Dust Market | Coastal bazaar | HVT + case, civilians |
| ASH-03 | Holdfast | River firebase | Defense waves, mortar pit |
| ASH-04 | Night Glass | Glassworks ridge | Counter-sniper, suppressed |
| ASH-05 | Convoy Ghost | Highway 9 flats | Ambush, no escape for command wagon |
| ASH-06 | Ash Harbor | Municipal docks | Crane, freighter, canisters |
| ASH-07 | Whiteout | Winter plateau | Nav beacon, crew extract |
| ASH-08 | Catacomb | Metro / ossuary | CQB, server dump |
| ASH-09 | Ridge Wire | Signal ridge | EW plant and hold |
| ASH-10 | False Flag | Broadcast campus | Deception, walk out clean |
| ASH-11 | Last Train | Freight corridor | Running fight on the consist |
| ASH-12 | Ashline | Buried terminus | Cut the spine, collapse, end |

`AAshlineGrayboxBuilder` spawns a unique graybox for every mission on Play.
Walk into the glowing objective cubes. The extract / final required volume
completes the mission, grants XP + crate tokens, unlocks the next mission, and
saves slot `AshlineCampaign`.

Authored `AAshlineObjectiveTrigger` volumes still use the catalog ids (`INFIL`, `CUT`, `EXFIL`, …).

### What is playable vs stubbed

| Playable now | Still stubbed |
| --- | --- |
| ASH-01…ASH-12 unique graybox layouts | Characters / weapon meshes / audio / lighting polish |
| Move, look, jump, crouch, fire, aim, reload, swap, FPS/TPS | Authored `.umap` art passes |
| Campaign select, difficulty, save / unlock | Operator creator UI, armory UMG, crate UI |
| AI capsules that chase and shoot | Behavior trees, navmesh, voice |
| DualSense + MetalFX hooks | iOS touch widget Blueprint |
| Ads off (`UAshlineMonetizationHooks` stub) | Any IAP / ads SDK |
