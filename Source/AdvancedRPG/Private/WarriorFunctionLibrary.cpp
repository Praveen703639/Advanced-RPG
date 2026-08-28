// pravin's  patashala all Rights Reserved 


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "warriorGameplayTags.h"
#include "WarriorTypes/WarrriorCountDownAction.h"
#include "WarriorGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/WarriorSaveGame.h"

#include "WarriorDebugHelper.h"



UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);
	return  CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));

	
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}

}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}

}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);

	
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfimType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EWarriorConfimType::Yes : EWarriorConfimType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);	
	 
	// Try to find a combat component on the actor
	if(IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}


	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType)
{

	UPawnCombatComponent* PawnCombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = PawnCombatComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;
	return PawnCombatComponent;
}


bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	//check(QueryPawn && TargetPawn);

	IGenericTeamAgentInterface* QueryTeamAgent =	Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetPawnTeamAgent =	Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());
	
	if (QueryTeamAgent&& TargetPawnTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId() != TargetPawnTeamAgent->GetGenericTeamId();
	}



	return false;
}

float UWarriorFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionGameplayTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector DirectionToAttacker = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float Dotproduct =(FVector::DotProduct(VictimForward, DirectionToAttacker));

	OutAngleDifference = UKismetMathLibrary::DegAcos(Dotproduct);

	FVector CrossProduct = FVector::CrossProduct(VictimForward, DirectionToAttacker);

	if (CrossProduct.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}
	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;
	}
	else if(OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}
	else if (OutAngleDifference > 135.f || OutAngleDifference <= -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}

	return WarriorGameplayTags::Shared_Status_HitReact_Front;
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	// Recommended threshold: -0.5f (about 120 degrees between forward vectors).
	// -1.0 => facing exactly opposite (180deg), 0.0 => 90deg, +1.0 => same direction (0deg)
	const float BlockDotThreshold = -0.5f;
	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());
	const bool bIsValid = DotResult < BlockDotThreshold;

	const FString DebugString = FString::Printf(TEXT("Dot Result: %f, Threshold: %f => %s"), DotResult, BlockDotThreshold, bIsValid ? TEXT("Valid Block") : TEXT("Invalid Block"));
	

	return bIsValid;
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	FActiveGameplayEffectHandle  ActiveGameplayEffectHandle  = SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);

	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}



void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput, UPARAM(DisplayName = "Output") EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;

	if (GEngine)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return;
	}

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	FWarriorCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	if (CountDownInput == EWarriorCountDownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(
				LatentInfo.CallbackTarget,
				LatentInfo.UUID,
				new FWarriorCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo)
			);
		}
	}

	if (CountDownInput == EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}
}

UWarriorGameInstance* UWarriorFunctionLibrary::GetWarriorGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetGameInstance<UWarriorGameInstance>();
		}
	}

	return nullptr;
}

void UWarriorFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode)
{
	APlayerController* PlayerController = nullptr;

	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			PlayerController = World->GetFirstPlayerController();
		}
	}

	if (!PlayerController)
	{
		return;
	}

	FInputModeGameOnly GameOnlyMode;
	FInputModeUIOnly UIOnlyMode;

	switch (InInputMode)
	{
	case EWarriorInputMode::GameOnly:

		PlayerController->SetInputMode(GameOnlyMode);
		PlayerController->bShowMouseCursor = false;

		break;

	case EWarriorInputMode::UIOnly:

		PlayerController->SetInputMode(UIOnlyMode);
		PlayerController->bShowMouseCursor = true;

		break;

	default:
		break;
	}
}

void UWarriorFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave)
{
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());

	if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
	{
		WarriorSaveGameObject->SavedCurrentGameDifficulty = InDifficultyToSave;

		const bool bWasSaved = UGameplayStatics::SaveGameToSlot(WarriorSaveGameObject, WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		Debug::Print(bWasSaved ? TEXT("Difficulty Saved") : TEXT("Difficulty NOT Saved"));
	}
}

bool UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSavedDifficulty)
{
	if (UGameplayStatics::DoesSaveGameExist(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0))
	{
		USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);

		if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
		{
			OutSavedDifficulty = WarriorSaveGameObject->SavedCurrentGameDifficulty;

			Debug::Print(TEXT("Loading Successful"), FColor::Green);

			return true;
		}
	}

	return false;
}





