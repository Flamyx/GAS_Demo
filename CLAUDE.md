# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

"Aura" is an Unreal Engine 5.7 top-down action RPG built around the Gameplay Ability System (GAS). The C++ module (`Aura`) provides the gameplay framework — attribute sets, gameplay abilities, execution/modifier calculations, widget controllers — while `Content/Blueprints` supplies data assets, Blueprint subclasses, animations, and UI built on top of the C++ base classes. Most gameplay tuning (abilities, damage effects, character class defaults) lives in Blueprint/data assets that derive from the C++ classes in `Source/Aura`, so a change is often incomplete without checking the corresponding Blueprint in the editor.

## Build, run, and iterate

There is no CLI test suite or linter in this repo — it's a game project built through Unreal's toolchain. There are no Automation Spec/unit tests defined in `Source/Aura`.

- **Compile C++ changes**: build the `Aura` target (Development Editor, Win64) via the generated `Aura.sln` in Visual Studio/Rider, or regenerate project files first with the Unreal Editor if `.h`/`.cpp` files were added/removed (right-click `Aura.uproject` → "Generate Visual Studio project files").
- **Run/test in-editor**: open `Aura.uproject` in Unreal Editor 5.7 and Play-In-Editor. There is no headless run path documented here — verifying gameplay changes means opening the editor and testing the relevant map/ability by hand.
- **Hot reload**: for small C++ changes, "Live Coding" (Ctrl+Alt+F11) from the editor is faster than a full recompile+restart.
- When adding new `UCLASS`/`USTRUCT`/`UENUM` types or new source files, project files must be regenerated before the IDE will pick them up.

## Architecture

### GAS layering

`AAuraCharacterBase` (implements `IAbilitySystemInterface` and `ICombatInterface`) is the common base for `AAuraCharacter` (player) and `AAuraEnemy`. Each owns a `UAuraAbilitySystemComponent` (ASC) and `UAuraAttributeSet`. Attribute defaults are applied via three layered `UGameplayEffect` classes set per-character: `DefaultVitalAttributes`, `DefaultPrimaryAttributes`, `DefaultSecondaryAttributes` — primary attributes come from per-class data (see below), vital/secondary are shared, applied in `AAuraCharacterBase::InitializeDefaultAttributes`.

- **`UAuraAttributeSet`** defines Primary (Strength/Intelligence/Resilience/Vigor), Secondary (derived: MaxHealth, MaxMana, Armor, crit stats, regen, resistances), Vital (Health/Mana), and Meta attributes (`IncomingDamage`, `IncomingXP` — write-only pipes, not persisted state). `PostGameplayEffectExecute` intercepts writes to the meta attributes and turns them into `HandleIncomingDamage`/`HandleIncomingXP`, which is where damage is actually subtracted from Health, debuffs/knockback/death are triggered, and floating combat text + XP events are fired. Never expect `IncomingDamage`/`IncomingXP` to hold a resting value — treat them as "apply and consume in the same frame."
- **`FAuraGameplayEffectContext`** (in `AuraAbilityTypes.h`) extends the stock GAS effect context with custom-execution-calculation output: crit/block/debuff flags, debuff damage/duration/frequency, damage type tag, death/knockback impulse vectors. `UAuraAbilitySystemLibrary` exposes Blueprint-callable getters/setters over this context so Blueprint execution calculations and abilities can read/write it without touching C++.
- **`UExecCalc_Damage`** is the single custom execution calculation for all damage: captures resistance/armor/crit attributes from source+target, applies armor mitigation, rolls crit and block, applies per-damage-type resistance, and writes debuff info into the effect context. Damage-type-to-resistance and damage-type-to-debuff mappings are native `FGameplayTag` maps in `FAuraGameplayTags`.
- **`FAuraDamageEffectParams`** (built via `UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults`) is the standard payload passed into `UAuraAbilitySystemLibrary::ApplyDamageEffect` to actually spec and apply a damage `UGameplayEffect` between source/target ASCs — this is the path essentially every offensive ability uses instead of applying effects directly.

### Gameplay tags: two sources of truth

Gameplay tags come from two places and both must stay in sync when adding new tags:
- **Native tags** (`Source/Aura/Public/AuraGameplayTags.h` / `.cpp`) — the `FAuraGameplayTags` singleton, registered via `FAuraGameplayTags::InitializeNativeGameplayTags()`, called from `UAuraAssetManager::StartInitialLoading()`. Use these for tags referenced from C++ (attributes, input, abilities, damage types, statuses, cooldowns).
- **Config tags** (`Config/DefaultGameplayTags.ini`) — tags only ever referenced from Blueprint/animation assets (event tags for anim notifies, `GameplayCue.*` tags, UI message tags). Add here when a tag never needs a C++ `FGameplayTag` reference.

### Ability class hierarchy

`UAuraGameplayAbility` (base: input tag, description helpers) → `UAuraDamageGameplayAbility` (adds damage type/magnitude, debuff params, death/knockback impulses, `CauseDamage`/`MakeDamageEffectParamsFromClassDefaults`) → spell-specific subclasses:
- `UAuraProjectileSpell` → `UFireBolt` (multi-projectile spread + homing), `UMeteoriteProjectileSpell`.
- `UAuraDamageGameplayAbility` → `UAuraMeleeAttack`, `UElectrocute` (spawns `AElectrocuteActor` beam, has `InputReleased` override), `UElectrocuteCourse`.
- `UAuraSummonAbility`, `UAuraBuffAbility` for non-damage abilities (minion summons, passive buffs).

Abilities are granted/tracked through `UAuraAbilitySystemComponent`, which also owns the input-tag-to-ability dispatch (`AbilityInputTagPressed/Held/Released`), spell slot equip/upgrade RPCs (`ServerEquipSpell`/`ClientEquipSpell`, `ServerUpgradeSpell`), and attribute point spend (`ServerUpgradeAttribute`). Ability/input/status/type tags for a given ability are looked up via `UAuraAbilitySystemLibrary::FindAbilityTagFromSpec` and friends, backed by the `UAbilityInfo` data asset (`Content` side) that maps ability tags to icons, descriptions, and level requirements for the spell menu UI.

### Data-driven character classes

`UCharacterClassInfo` (a `UDataAsset`) maps `ECharacterClass` (Elementalist/Warrior/Ranger) to per-class primary-attribute GE, startup abilities, behavior tree, and XP reward curve, plus shared vital/secondary GEs and a damage-calculation coefficient curve table used by `UExecCalc_Damage`. `UAuraAbilitySystemLibrary::InitializeEnemyAttributes`/`GiveStartupAbilities` read from this asset — enemies are initialized purely from data, not hardcoded per-enemy-class C++.

### Input

`UAuraInputConfig` (data asset) maps `UInputAction` → `FGameplayTag` pairs. `AAuraPlayerController` resolves Enhanced Input actions to gameplay tags and forwards to the ASC's `AbilityInputTagPressed/Held/Released`. The controller also owns click-to-move (spline-based auto-run) and cursor-trace enemy highlighting (`IEnemyInterface`).

### UI: WidgetController pattern

UI never reads attributes/ASC state directly. `AAuraHUD` owns three `UAuraWidgetController` subclasses — `UOverlayWidgetController` (HUD vitals/XP/level), `UAttributeMenuWidgetController`, `USpellMenuWidgetController` — each constructed lazily from an `FWidgetControllerParams` (PlayerController/PlayerState/ASC/AttributeSet) via `AAuraHUD::Get*WidgetController`, or from Blueprint via `UAuraAbilitySystemLibrary::Get*WidgetController`. Widget controllers bind to ASC/attribute delegates in `BindCallbacksToDependencies` and push initial state in `BroadcastInitialValues`; widgets bind to the controller's delegates, not the ASC.

### Interfaces

- `ICombatInterface` — implemented by all combat-capable actors (level, combat sockets, hit react montage, death/knockback, per-attack montage selection, minion count tracking).
- `IEnemyInterface` — enemy-only concerns (highlight on hover, combat target).
- `IPlayerInterface` — player-only concerns (XP/level/attribute-point queries).

### Networking

Attributes replicate via `OnRep_*` on `UAuraAttributeSet` (standard GAS pattern). Ability equip/upgrade and effect-applied notifications go through explicit `Server`/`Client` RPCs on `UAuraAbilitySystemComponent` and `AAuraPlayerController` (e.g. `ServerEquipSpell`/`ClientEquipSpell`, `ClientEffectApplied`, `ShowDamageNumber`) rather than relying solely on GAS's built-in replication, so multiplayer-facing changes need matching authority checks (`HasAuthority()`) and RPC pairs.
