# Ashline campaign (ASH-01 … ASH-12)

Runtime source of truth: `UAshlineMissionCatalog::BuildCampaign()`.
Designer mirror: `Content/Ashline/Data/Campaign.json`.
Environment mood / kit: `UAshlinePresentationLibrary::MoodForMission` + `UAshlineEnvironmentKit`.

Saves between missions in slot `AshlineCampaign`. Completing a mission unlocks the next, grants XP, and awards crate tokens.

| Code | Title | Location | Lighting mood |
| --- | --- | --- | --- |
| ASH-01 | Wire Cut | Border relay | Night moonlight, warm hut practicals |
| ASH-02 | Dust Market | Coastal bazaar | Harsh desert noon |
| ASH-03 | Holdfast | River firebase | Bleached defense day |
| ASH-04 | Night Glass | Glassworks ridge | Counter-sniper night |
| ASH-05 | Convoy Ghost | Highway 9 flats | Amber dusk + vehicle lights |
| ASH-06 | Ash Harbor | Municipal docks | Overcast port, crane spot |
| ASH-07 | Whiteout | Winter plateau | Dense white fog, beacon |
| ASH-08 | Catacomb | Metro / ossuary | Warm subterranean dark |
| ASH-09 | Ridge Wire | Signal ridge | High-altitude clear |
| ASH-10 | False Flag | Broadcast campus | Urban night |
| ASH-11 | Last Train | Freight corridor | Industrial overcast |
| ASH-12 | Ashline | Buried terminus | Red finale |

`AAshlineGrayboxBuilder` spawns a unique **themed blockout** for every mission: PBR-tinted surfaces, Sky Atmosphere, volumetric fog, unbound post-process, practical lights, foliage, vehicles/sandbags/doors, glowing objective markers.

Walk into the markers. The extract / final required volume completes the mission.

### What is playable vs Phase 2

| Playable now | Phase 2 on the Windows box (`Docs/FAB_PACKS.md`) |
| --- | --- |
| ASH-01…12 unique layouts + distinct moods | Megascans / Fab kits on canonical paths |
| Move, look, jump, crouch, fire, aim, reload, swap, FPS/TPS | MetaHuman + GASP locomotion |
| Compound weapon + muzzle light + impact decals | Fab / Lyra weapon meshes + Niagara |
| Humanoid blockout or mannequin/MetaHuman if present | Unique AI wardrobe |
| Audio **slots** (silent until cues exist) | Fire / reload / music beds |
| `Ashline_PC_Ultra` / Balanced | Profile on the 9070 GRE |
| Ads off | Stay off |
