// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/ElectrocuteCourse.h"

#include "GameFramework/Character.h"

void UElectrocuteCourse::StoreOwnerVariables()
{
	OwnerPlayerController = GetCurrentActorInfo()->PlayerController.Get();
	OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
}

void UElectrocuteCourse::StoreHitResult(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitActor = HitResult.GetActor();
		MouseHitLocation = HitResult.ImpactPoint;
	}
	else
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, true);
	}
}
