// Copyright Sparrow Inc.


#include "AbilitySystem/AbilityTasks/TeleportToLocation.h"
#include "AbilitySystemComponent.h"


UTeleportToLocation* UTeleportToLocation::CreateTeleportToLocation(UGameplayAbility* OwningAbility, AActor* Actor, const FVector& TargetLocation)
{
	UTeleportToLocation* MyObj = NewAbilityTask<UTeleportToLocation>(OwningAbility);
	MyObj->Actor = Actor;
	MyObj->SpawnLocation = TargetLocation;
	return MyObj;
}

void UTeleportToLocation::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

	//Local player: send mouse click location
	TeleportActor();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast();
	}
}
void UTeleportToLocation::TeleportActor()
{
	Actor->SetActorLocation(SpawnLocation, /* bSweep */ false);
}

