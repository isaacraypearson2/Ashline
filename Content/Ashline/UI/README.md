# UI

Campaign select, **mission brief**, combat HUD, pause/settings, and debrief are a Canvas HUD (`AAshlineHUD`) using Engine default fonts. Layout scales for **Steam Deck 800p** (safe zone, large interact, large ammo) and desktop Ultra.

Combat plate shows weapon **name / class / AUTO|SEMI / skin / mag / reserve**. Hit markers, kill confirm, objective diamond, HP+armor, damage vignette, death-cam overlay.

UMG operator / armory / crate screens remain Phase 2. Touch widget for iOS remains optional (`UAshlineTouchHUD`). Settings persist on `UAshlineGameUserSettings::Feel`.
