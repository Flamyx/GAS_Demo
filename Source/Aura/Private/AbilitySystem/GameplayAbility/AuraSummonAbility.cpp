// Copyright Sparrow Inc.


#include "AbilitySystem/GameplayAbility/AuraSummonAbility.h"

int UAuraSummonAbility::CurrentMinionNumber;
 
int32 UAuraSummonAbility::GetCurrentMinionNumber()
{
	return CurrentMinionNumber;
}



TArray<FVector> UAuraSummonAbility::GetSummonLocations()
{
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector ForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const float Delta = -Spread / MinionSpawnNumber;
	const FVector RightOfSpread = ForwardVector.RotateAngleAxis(Spread / 2.f, FVector::UpVector);
	TArray<FVector> Ret = {};
	for (int i = 0; i < MinionSpawnNumber ; ++i)
	{
		const FVector Direction = RightOfSpread.RotateAngleAxis(Delta * i, FVector::UpVector);
		const float Scale = FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);
		FVector SpawnLocation = Location + Direction * Scale;
		
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation + FVector(0.f, 0.f, 400.f),
			SpawnLocation + FVector(0.f, 0.f, -400.f), ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			SpawnLocation = HitResult.ImpactPoint;
		}

		Ret.Add(SpawnLocation);
	}	

	return Ret;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
