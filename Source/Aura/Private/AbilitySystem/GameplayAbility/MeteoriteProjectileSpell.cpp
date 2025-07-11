// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/MeteoriteProjectileSpell.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"


void UMeteoriteProjectileSpell::InputReleased(const FGameplayAbilitySpecHandle Handle, 
											const FGameplayAbilityActorInfo* ActorInfo,
											const FGameplayAbilityActivationInfo ActivationInfo)
{
	FString ChargeString = FString::Printf(TEXT("Charge time %.2f"), ChargeTime);
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimerHandle);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UMeteoriteProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	float lb = SpawnOffsetBounds[0], rb = SpawnOffsetBounds[1];
	const FVector2d SpawnOffset = { FMath::RandRange(-lb, lb), FMath::RandRange(-rb, rb) };
	const FVector ActorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector SpawnLocation = { ActorLocation[0] - SpawnOffset[0], ActorLocation[1] - SpawnOffset[1], SpawnHeight };
	FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AActor* OwningActor = GetOwningActorFromActorInfo();
	AMeteoriteProjectile* Projectile = GetWorld()->SpawnActorDeferred<AMeteoriteProjectile>(
		ProjectileClass, SpawnTransform,
		OwningActor, Cast<APawn>(OwningActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwningActor);
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();

	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(Projectile);

	//Owning Actor is AuraPlayerState, so IsNotFriend will not work properly
	//ContextHandle.AddInstigator(OwningActor, OwningActor);

	TArray<TWeakObjectPtr<AActor>> Actors = { Projectile };
	ContextHandle.AddActors(Actors);

	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	ContextHandle.AddHitResult(HitResult);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
	FGameplayEffectSpecHandle ResidualSpecHandle = SourceASC->MakeOutgoingSpec(ResidualDamageEffectClass, GetAbilityLevel(), ContextHandle.Duplicate());
	
	FString ChargeString = FString::Printf(TEXT("Charge time %.2f"), ChargeTime);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, ChargeString);
	for (auto Pair : DamageTypes)
	{
		if (Pair.Key.MatchesTagExact(FAuraGameplayTags::Get().Damage_Residual_Fire))
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(ResidualSpecHandle, Pair.Key, 
				Pair.Value.GetValueAtLevel(GetAbilityLevel()) * ChargedMultiplier.GetValueAtLevel(ChargeTime));
		}
		else
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, 
				Pair.Value.GetValueAtLevel(GetAbilityLevel()) * ChargedMultiplier.GetValueAtLevel(ChargeTime));
		}
	}

	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->PeriodicDamageEffectSpecHandle = ResidualSpecHandle;
	Projectile->SetSphereRadius(ChargeTime / MaxChargeTime);

	Projectile->FinishSpawning(SpawnTransform);
}

void UMeteoriteProjectileSpell::StartChargeTimeline()
{
	GetWorld()->GetTimerManager().SetTimer(ChargeTimerHandle, this, &UMeteoriteProjectileSpell::ChargeTick, FChargeTick, true);
}

void UMeteoriteProjectileSpell::ChargeTick()
{
	ChargeTime = FMath::Clamp(ChargeTime + FChargeTick, 0.f, MaxChargeTime);
}
