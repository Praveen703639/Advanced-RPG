// pravin's patashala all Rights Reserved

#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/WarriorHeroCharacter.h"
#include "WarriorDebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/InGameWidgets/WarriorWidgetBase.h"
#include "Controllers/WarriorHeroController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "WarriorFunctionLibrary.h"
#include "warriorGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// ✅ FIX: restore first, then cleanup
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUp();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedTarget ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(CurrentLockedTarget, WarriorGameplayTags::Shared_Status_Death) ||
		UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetWarriorHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Death))
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetPosition();

	const bool bShouldOverrideRotation =
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetWarriorHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Roll) &&
		!UWarriorFunctionLibrary::NativeDoesActorHaveTag(GetWarriorHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Blocking);

	if (bShouldOverrideRotation)
	{
		 FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(
			GetWarriorHeroCharacterFromActorInfo()->GetActorLocation(),
			CurrentLockedTarget->GetActorLocation()
		);

		 LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);


		const FRotator CurrentControlRot = GetWarriorHeroCharacterFromActorInfo()->GetControlRotation();

		const FRotator TargetRot = FMath::RInterpTo(
			CurrentControlRot,
			LookAtRot,
			DeltaTime,
			TargetLockRotationInterpSpeed
		);

		GetWarriorHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		GetWarriorHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{

	GetAvailableTargetsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetToLock = nullptr;

	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);

	if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)	if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock = GetNearestTargetFromAvailableTargetActors(ActorsOnLeft);
	}
	else
	{
		NewTargetToLock = GetNearestTargetFromAvailableTargetActors(ActorsOnRight);
	}

	if (NewTargetToLock)
	{
		CurrentLockedTarget = NewTargetToLock;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableTargetsToLock();

	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockedTarget = GetNearestTargetFromAvailableTargetActors(AvailableActorsToLock);

	if (CurrentLockedTarget)
	{
		DrawTargetLockWidget();
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableTargetsToLock()
{
	AvailableActorsToLock.Empty();

	TArray<FHitResult> BoxTraceHits;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWarriorHeroCharacterFromActorInfo(),
		GetWarriorHeroCharacterFromActorInfo()->GetActorLocation(),
		GetWarriorHeroCharacterFromActorInfo()->GetActorLocation() +
		GetWarriorHeroCharacterFromActorInfo()->GetActorForwardVector() * TargetLockRange,
		TargetLockBoxExtent / 2.f,
		GetWarriorHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannel,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);

	for (const FHitResult& Hit : BoxTraceHits)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor != GetWarriorHeroCharacterFromActorInfo())
			{
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableTargetActors(const TArray<AActor*>& InAvailableActors)
{
	float ClosestDistance = 0.f;

	return UGameplayStatics::FindNearestActor(
		GetWarriorHeroCharacterFromActorInfo()->GetActorLocation(),
		InAvailableActors,
		ClosestDistance
	);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight)
{

	if (!CurrentLockedTarget || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	const FVector PlayerLocation = GetWarriorHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedTarget->GetActorLocation() - PlayerLocation).GetSafeNormal();

	for (AActor* AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor == CurrentLockedTarget) continue;

		const FVector PlayerToAvailableNormalized = (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();

		const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if (CrossResult.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}



void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot To Assign a valid TargetLockWidgetClass in Blueprint"));

		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(
			GetWarriorHeroControllerFromActorInfo(),
			TargetLockWidgetClass
		);

		check(DrawnTargetLockWidget);

		DrawnTargetLockWidget->AddToViewport();
	}
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	if (GetWarriorHeroCharacterFromActorInfo())
	{
		CachedDefaultMaxWalkSpeed =
			GetWarriorHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;

		GetWarriorHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed =
			TargetLockMaxWalkSpeedModifier;
	}
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetWarriorHeroControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(TargetLockInputMappingContext, 3);

}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableActorsToLock.Empty();
	CurrentLockedTarget = nullptr;

	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;

	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget || !CurrentLockedTarget)
	{
		return;
	}

	FVector2D WidgetScreenPosition;

	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetWarriorHeroControllerFromActorInfo(),
		CurrentLockedTarget->GetActorLocation(),
		WidgetScreenPosition,
		true
	);

	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[&](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			});
	}

	WidgetScreenPosition -= (TargetLockWidgetSize / 2.f);

	DrawnTargetLockWidget->SetPositionInViewport(WidgetScreenPosition, false);
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if (GetWarriorHeroCharacterFromActorInfo())
	{
		GetWarriorHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed =
			CachedDefaultMaxWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetWarriorHeroControllerFromActorInfo())
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = GetWarriorHeroControllerFromActorInfo()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->RemoveMappingContext(TargetLockInputMappingContext);
}
