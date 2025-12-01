// Copyright Sparrow Inc.


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	OnSpellPointsAddedDelegate.Broadcast(AuraPlayerState->GetSpellPoints());

	
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState);
	AuraPlayerState->SpellPointsDelegate.AddLambda(
		[this](int32 SpellPoints) {
			OnSpellPointsAddedDelegate.Broadcast(SpellPoints);
		}
	);
}

void USpellMenuWidgetController::OnSpellPointsAdded(int32 IncomingSpellPoints)
{
	OnSpellPointsAddedDelegate.Broadcast(IncomingSpellPoints);
}
