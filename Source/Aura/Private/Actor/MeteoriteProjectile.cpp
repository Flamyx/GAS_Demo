// Copyright Sparrow Inc.


#include "Actor/MeteoriteProjectile.h"
#include "Aura/Aura.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"


AMeteoriteProjectile::AMeteoriteProjectile()
{
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	
	ProjectileMovement->InitialSpeed = 400.f;
	ProjectileMovement->MaxSpeed = 500.f;
	ProjectileMovement->bRotationFollowsVelocity = 1;
	ProjectileMovement->ProjectileGravityScale = 0.1f;
}

void AMeteoriteProjectile::SetSphereRadius(float ChargeRatio)
{
	BlastRadius = FMath::Lerp(BlastRadius, MaxBlastRadius, ChargeRatio);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::SanitizeFloat(BlastRadius, 2));
}

void AMeteoriteProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(MaxLifeSpan);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMeteoriteProjectile::OnOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AMeteoriteProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		OnHit();
	}
	Super::Destroyed();
}

void AMeteoriteProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Avatar = DamageEffectParams.SourceASC->GetAvatarActor();
	
	if (Avatar == OtherActor || !UAuraAbilitySystemLibrary::IsNotFriend(OtherActor, Avatar)) 
		return;
	
	if (IsPendingKillPending())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("PendingKill"));
		return;
	}

	if (!bHit)
		OnHit();

	if (HasAuthority() && !bHit)
	{
		const TArray<AActor*> IgnoredActors = { Avatar };
		TArray<AActor*> AffectedActors;
		
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(Avatar, AffectedActors, IgnoredActors, BlastRadius, GetActorLocation());

		for (AActor* AffectedActor : AffectedActors)
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AffectedActor))
			{
				DamageEffectParams.TargetASC = TargetASC;
				DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
				UAuraAbilitySystemLibrary::ApplyDamageEffectParams(DamageEffectParams);
			}
		}
		
		Destroy();
	}

	else bHit = true;
}