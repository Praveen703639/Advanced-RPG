// pravin's  patashala all Rights Reserved 


#include "Items/Weapons/WarriorWeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "WarriorFunctionLibrary.h"

#include "WarriorDebugHelper.h"

// Sets default values
AWarriorWeaponBase::AWarriorWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetRootComponent());

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	


	WeaponCollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("WeaponCollisionCapsule"));
	WeaponCollisionCapsule->SetupAttachment(WeaponMesh);
	WeaponCollisionCapsule->SetCapsuleRadius(10.f);
	WeaponCollisionCapsule->SetCapsuleHalfHeight(30.f);
	WeaponCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//: Better - custom weapon channel (recommended)
	WeaponCollisionCapsule->SetCollisionObjectType(ECC_GameTraceChannel1); // your custom Weapon channel
	WeaponCollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollisionCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WeaponCollisionCapsule->SetCollisionProfileName(FName("OverlapAllDynamic")); // ✅ ADD THIS

	WeaponCollisionCapsule->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWarriorWeaponBase::OnCollisionBoxBeginOverLap);
	WeaponCollisionCapsule->OnComponentEndOverlap.AddUniqueDynamic(this, &AWarriorWeaponBase::OnCollisionBoxEndOverLap);
	WeaponCollisionCapsule->SetGenerateOverlapEvents(true); // usually default true, but be explicit
}

void AWarriorWeaponBase::OnCollisionBoxBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	//checkf(WeaponOwningPawn,TEXT(" Forgot to assign the instigator as owning pawn of the weapon : %s"), *GetName());
	
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn,HitPawn))
		{
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}


	}
	
	
	
	
	
	
	
	

}

void AWarriorWeaponBase::OnCollisionBoxEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	//checkf(WeaponOwningPawn, TEXT(" Forgot to assign the instigator as owning pawn of the weapon : %s"), *GetName());
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(WeaponOwningPawn, HitPawn))
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}


	}

}





