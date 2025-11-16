// Copyright Sparrow Inc.


#include "Actor/MeteoriteProjectile.h"
#include "Aura/Aura.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystemComponent.h"


AMeteoriteProjectile::AMeteoriteProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);

	Sphere->SetGenerateOverlapEvents(true);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	//Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
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
	SetLifeSpan(LifeSpan);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMeteoriteProjectile::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AMeteoriteProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		PlayImpact();
	}
	Super::Destroyed();
}

void AMeteoriteProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	/*FString DebugInfoString = FString::Printf(
		TEXT("Actor Spawned: %s (ID: %d) Overlap with: %s (Component: %s)"),
		*GetName(),
		GetUniqueID(),
		*GetNameSafe(OtherActor),
		*GetNameSafe(OtherComp)
	);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DebugInfoString);*/

	if (!DamageEffectSpecHandle.Data.IsValid()) return;

	FGameplayEffectSpec* EffectSpec = DamageEffectSpecHandle.Data.Get();
	auto Avatar = EffectSpec->GetEffectContext().GetEffectCauser();

	FGameplayEffectSpec* PeriodicEffectSpec = PeriodicDamageEffectSpecHandle.Data.Get();

	if (Avatar == OtherActor || !UAuraAbilitySystemLibrary::IsNotFriend(OtherActor, Avatar)) 
	{

		/*FString DebugInfoString = FString::Printf(
			TEXT("Ignored Actor Name: %s Label: %s Location: %s"),
			*OtherActor->GetName(),
			*OtherActor->GetActorLabel(),
			*OtherActor->GetActorLocation().ToString()
		);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, DebugInfoString);

		if (!DamageEffectSpecHandle.Data.IsValid()) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not valid"));
		if (EffectSpec->GetContext().GetEffectCauser() == OtherActor)  GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Red, TEXT("Same actor"));*/

		return;
	}

	if (IsPendingKillPending())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("PendingKill"));
		return;

	}

	if (!bHit)
	{
		PlayImpact();
	}

	if (HasAuthority() && !bHit)
	{
		const TArray<AActor*> IgnoredActors = { Avatar };

		/*FString SphereRadiusDebugString = FString::Printf(
			TEXT("Scaled R %.2f Unscaled %.2f"),
			Sphere->GetScaledSphereRadius(),
			Sphere->GetUnscaledSphereRadius()
		);
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, SphereRadiusDebugString);*/

		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(Avatar, AffectedActors, IgnoredActors, BlastRadius, GetActorLocation());

		for (AActor* AffectedActor : AffectedActors)
		{
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AffectedActor))
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec);
				TargetASC->ApplyGameplayEffectSpecToSelf(*PeriodicEffectSpec);
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("AppliedEffect"));
			}
		}
		//UKismetSystemLibrary::DrawDebugCircle(Avatar, GetActorLocation(), BlastRadius, 
		//					12.f, FLinearColor::Black, /* Duration */ 2.f);

		Destroy();
	}

	else
	{
		bHit = true;
	}
}

void AMeteoriteProjectile::PlayImpact()
{
	UGameplayStatics::PlaySoundAtLocation(this,
		ImpactSound,
		GetActorLocation(),
		FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent) LoopingSoundComponent->Stop();
}
