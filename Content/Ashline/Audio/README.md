# Audio slots

Resolver: assigned soft ref → `/Game/Ashline/Audio/...` → StarterContent. Silent if nothing loads (no crash).

```
Weapons/SC_Fire_<WeaponId>
Weapons/SC_Fire_Default
Weapons/SC_Reload_Default
Weapons/SC_Hit_Default
Weapons/SC_Hit_Flesh
Weapons/SC_Explosion
Footsteps/SC_Footstep_Default
Footsteps/SC_Footstep_{Concrete,Metal,Wood,Sand,Snow,Water,Dirt}
Music/SC_Bed_ASH01 … SC_Bed_ASH12
Music/SC_Bed_Frontend
Ambience/SC_Amb_ASH##  SC_Amb_Default
Stingers/SC_Combat_Stinger
```

Starter Content `Explosion01` / `Fire01` are used if present. Volumes: Settings master / SFX / music. Combat first-hit raises the music bed and plays the stinger slot.
