// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/FireBolt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, AActor* HomingTarget, bool bOverridePitch, float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), FAuraGameplayTags::Get().CombatSocket_Weapon);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	const FVector LeftOfSpread = Rotation.Vector().RotateAngleAxis(-Spread / 2.f, FVector::UpVector);
	
	const int NumProjectiles = FMath::Min(ProjectileLimit, GetAbilityLevel());
	const float DeltaSpread = Spread / (NumProjectiles - 1);
	
	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::GetEvenlyScacedRotators(Rotation.Vector(), FVector::UpVector, Spread, EffectiveNumProjectiles);
	for (const FRotator& Rot : Rotations)
	{
		FTransform Transform = FTransform();
		Transform.SetRotation(Rot.Quaternion());
		Transform.SetLocation(SocketLocation);
		
		auto Bolt = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, Transform, 
			GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Bolt->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		
		if (HomingTarget->Implements<UCombatInterface>())
		{
			Bolt->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		//static meshes case
		else
		{
			Bolt->HomingTargetComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Bolt->HomingTargetComponent->SetWorldLocation(ProjectileTargetLocation);
			Bolt->ProjectileMovement->HomingTargetComponent = Bolt->HomingTargetComponent;
		}
		Bolt->ProjectileMovement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
		Bolt->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectiles;
		Bolt->FinishSpawning(Transform);
		ProjectileActors.Add(Bolt);
	}
}

void UFireBolt::HomeToTarget(AAuraProjectile* Projectile, AActor* HomingTarget, bool bOverridePitch, float PitchOverride)
{
	Projectile->EnableHoming(HomingTarget);
	
}
