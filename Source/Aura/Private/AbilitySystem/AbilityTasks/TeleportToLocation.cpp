// Copyright Sparrow Inc.


#include "AbilitySystem/AbilityTasks/TeleportToLocation.h"
#include "AbilitySystemComponent.h"


UTeleportToLocation* UTeleportToLocation::CreateTeleportToLocation(UGameplayAbility* OwningAbility, AActor* Actor, const FVector& TargetLocation)
{
	UTeleportToLocation* MyObj = NewAbilityTask<UTeleportToLocation>(OwningAbility);
	MyObj->Actor = Actor;
	MyObj->SpawnLocation = TargetLocation;
	MyObj->SpawnLocation[2] += 3;
	return MyObj;
}

void UTeleportToLocation::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

	//Local player: send mouse click location
	if (IsLocallyControlled()) {
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("TP"));
		TeleportActor();
	}

	if (IsForRemoteClient())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Waiting"));
		SetWaitingOnRemotePlayerData();
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Broadcast in Act"));
		OnCompleted.Broadcast();
	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("END"));
	EndTask();
}

void UTeleportToLocation::TeleportActor()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	Actor->SetActorLocation(SpawnLocation, /* bSweep */ false);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Broadcast in TP"));
		OnCompleted.Broadcast();
	}
}

