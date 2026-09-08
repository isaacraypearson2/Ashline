# FX

Runtime always flashes a muzzle **point light**. `UAshlineCombatFX` then tries, in order: weapon DataAsset Niagara → `/Game/Ashline/FX/...` → StarterContent Cascade (`P_Explosion`, `P_Sparks`, `P_Fire`) → Engine cube **tracer streak**.

```
Muzzle/NS_Muzzle_Default
Muzzle/NS_Muzzle_<WeaponId>
Tracers/NS_Tracer_Default
Impacts/NS_Impact_Default
Impacts/NS_Sparks
Impacts/NS_Blood
Explosions/NS_Explosion_Default
Explosions/P_Explosion_Stub
```

Missing assets are not an error. Heavy AI kills spawn the explosion stub.
