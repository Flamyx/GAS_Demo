// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/Electrocute.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/ElectrocuteActor.h"
#include "Interaction/CombatInterface.h"

void UElectrocute::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo)
{
	bInputReleased = true;
	
	FGameplayEventData Payload;
	Payload.EventMagnitude = 1.f;
	FGameplayTag MontageTag = FAuraGameplayTags::Get().Montage_Attack_2;
	Payload.EventTag = MontageTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActorFromActorInfo(), MontageTag, Payload);
	
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (ElectricBeam == nullptr)
	{
		return;
	}
	ElectricBeam->Destroy();
}

void UElectrocute::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), 
																					FAuraGameplayTags::Get().CombatSocket_Weapon);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	ElectricBeam = GetWorld()->SpawnActorDeferred<AElectrocuteActor>(
		ElectrocuteActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	//EffectContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), ElectricBeam);
	EffectContextHandle.AddSourceObject(ElectricBeam);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);
	
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));
	
	if (ElectricBeam == nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "ElectricBeam is nullptr");
		return;
	}
	ElectricBeam->InfiniteDamageEffectSpecHandle = SpecHandle;

	ElectricBeam->FinishSpawning(SpawnTransform);
}
