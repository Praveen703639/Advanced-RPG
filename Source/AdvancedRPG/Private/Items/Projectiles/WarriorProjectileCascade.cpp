// pravin's  patashala all Rights Reserved 


#include "Items/Projectiles/WarriorProjectileCascade.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h" 
#include "GameFramework/ProjectileMovementComponent.h"
#include "WarriorFunctionLibrary.h"
#include "warriorGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

#include  "warriorDebugHelper.h"

// Sets default values
AWarriorProjectileCascade::AWarriorProjectileCascade()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>("ProjectileCollisionBox");
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);


	ProjectileCollisionBox->OnComponentHit.AddUniqueDynamic(this, &ThisClass::OnProjectileHit);
	ProjectileCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnProjectileBeginOverlap);


	ProjectileParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>("ProjectileParticleSystemComponent");
	ProjectileParticleSystemComponent->SetupAttachment(GetRootComponent());

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
	ProjectileMovementComp->InitialSpeed = 700.f;
	ProjectileMovementComp->MaxSpeed = 900.f;
	ProjectileMovementComp->Velocity = FVector(1.f, 0.f, 0.f);
	ProjectileMovementComp->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 4.f;

}

// Called when the game starts or when spawned
void AWarriorProjectileCascade::BeginPlay()
{
	Super::BeginPlay();

	Super::BeginPlay();

	if (ProjectileDamagePolicy == EWarriorProjectileDamagePolicy::OnBeginOverlap)
	{
		ProjectileCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	
}

void AWarriorProjectileCascade::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnSpawnProjectileHitFX(Hit.ImpactPoint);

	APawn* HitPawn = Cast<APawn>(OtherActor);

	if (!HitPawn || !UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
	{


		Destroy();
		return;
	}
	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHaveTag(HitPawn, WarriorGameplayTags::Player_Status_Blocking);


	if (bIsPlayerBlocking)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(this, HitPawn);
	}
	FGameplayEventData Data;
	Data.Instigator = this;
	Data.Target = HitPawn;

	if (bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitPawn,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}


	else
	{
		HandleApplyProjectileDamage(HitPawn, Data);
	}

	Destroy();

}

void AWarriorProjectileCascade::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverLappedActors.Contains(OtherActor))
	{
		return;
	}
	OverLappedActors.AddUnique(OtherActor);

	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		FGameplayEventData Data;
		Data.Instigator = this;
		Data.Target = HitPawn;

		if (UWarriorFunctionLibrary::IsTargetPawnHostile(GetInstigator(), HitPawn))
		{

			HandleApplyProjectileDamage(HitPawn, Data);
		}


	}
}






void AWarriorProjectileCascade::HandleApplyProjectileDamage(APawn* InHitPawn, const FGameplayEventData& InPayload)
{
	checkf(ProjectileDamageEffectSpecHandle.IsValid(), TEXT("Forhot to assign  a valid spec handle to thte projectile : %s"), *GetActorNameOrLabel());
	const bool bWasApplied = UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToActor(GetInstigator(), InHitPawn, ProjectileDamageEffectSpecHandle);

	if (bWasApplied)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			InHitPawn,
			WarriorGameplayTags::Shared_Event_HitReact,
			InPayload
		);
	}
}





