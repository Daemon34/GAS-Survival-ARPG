// Clement Lecoeur All Rights Reserved


#include "SARPGFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "SARPGGameplayTags.h"
#include "SARPGTypes/SARPGCountDownAction.h"
#include "SARPGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGame/SARPGSaveGame.h"

#include "SARPGDebugHelper.h"

USARPGAbilitySystemComponent* USARPGFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<USARPGAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void USARPGFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	USARPGAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void USARPGFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	USARPGAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);

	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool USARPGFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	USARPGAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void USARPGFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, ESARPGConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? ESARPGConfirmType::Yes : ESARPGConfirmType::No; 
}

UPawnCombatComponent* USARPGFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor)) {
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

UPawnCombatComponent* USARPGFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, ESARPGValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);

	OutValidType = CombatComponent ? ESARPGValidType::Valid : ESARPGValidType::Invalid;
	return CombatComponent;
}

bool USARPGFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
	check(QueryPawn && TargetPawn);

	IGenericTeamAgentInterface* QueryTeamAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent && TargetTeamAgent) {
		return QueryTeamAgent->GetGenericTeamId() != TargetTeamAgent->GetGenericTeamId();
	}

	return false;
}

float USARPGFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag USARPGFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);
	if (CrossResult.Z < 0.0f) {
		OutAngleDifference *= -1.0f;
	}

	if (OutAngleDifference >= -45.0f && OutAngleDifference <= 45.0f) {
		return SARPGGameplayTags::Shared_Status_HitReact_Front;
	}
	else if (OutAngleDifference < -45.0f && OutAngleDifference >= -135.0f) {
		return SARPGGameplayTags::Shared_Status_HitReact_Left;
	}
	else if (OutAngleDifference < -135.0f || OutAngleDifference > 135.0f) {
		return SARPGGameplayTags::Shared_Status_HitReact_Back;
	}
	else if (OutAngleDifference > 45.0f && OutAngleDifference <= 135.0f) {
		return SARPGGameplayTags::Shared_Status_HitReact_Right;
	}

	return SARPGGameplayTags::Shared_Status_HitReact_Front;
}

bool USARPGFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
	check(InAttacker && InDefender);

	const float DotResult = FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());

	return DotResult < -0.1f; // Is attacker in front of defender ? i.e Defender forward vector is parallel to attacker forward vector ?
}

bool USARPGFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InEffectSpecHandle)
{
	USARPGAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
	USARPGAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*InEffectSpecHandle.Data, TargetASC);

	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

void USARPGFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutTimeRemaining, ESARPGCountDownActionInput CountDownInput, UPARAM(DisplayName = "Output") ESARPGCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{
	UWorld* World = nullptr;
	if (GEngine) {
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World) {
		return;
	}

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	FSARPGCountDownAction* FoundAction = LatentActionManager.FindExistingAction<FSARPGCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);

	if (CountDownInput == ESARPGCountDownActionInput::Start) {
		if (!FoundAction) {
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FSARPGCountDownAction(TotalTime, UpdateInterval, OutTimeRemaining, CountDownOutput, LatentInfo));
		}
	}

	else if (CountDownInput == ESARPGCountDownActionInput::Cancel) {
		if (FoundAction) {
			FoundAction->CancelAction();
		}
	}
}

USARPGGameInstance* USARPGFunctionLibrary::GetSARPGGameInstance(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			return World->GetGameInstance<USARPGGameInstance>();
		}
	}

	return nullptr;
}

void USARPGFunctionLibrary::ToggleInputMode(ESARPGInputMode InInputMode, const UObject* WorldContextObject)
{
	APlayerController* PlayerController = nullptr;
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			if (ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController())
			{
				PlayerController = LocalPlayer->GetPlayerController(World);
			}
			//PlayerController = World->GetFirstPlayerController();
			// Better use GetController() from correct pawn reference
		}
	}

	if(!PlayerController)
	{
		return;
	}

	switch(InInputMode){
		case ESARPGInputMode::GameOnly:
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->bShowMouseCursor = false;
			break;
		case ESARPGInputMode::UIOnly:
			PlayerController->SetInputMode(FInputModeUIOnly());
			PlayerController->bShowMouseCursor = true;
			break;
		default:
			break;
	}
}

void USARPGFunctionLibrary::SaveCurrentGameDifficulty(ESARPGGameDifficulty InGameDifficultyToSave)
{
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(USARPGSaveGame::StaticClass());

	if(USARPGSaveGame * SARPGSaveGame = Cast<USARPGSaveGame>(SaveGameObject))
	{
		SARPGSaveGame->SavedCurrentGameDifficulty = InGameDifficultyToSave;
		const bool bWasSaved = UGameplayStatics::SaveGameToSlot(SARPGSaveGame, SARPGGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);
	}
}

bool USARPGFunctionLibrary::TryLoadSavedGameDifficulty(ESARPGGameDifficulty& OutSavedGameDifficulty)
{
	if (UGameplayStatics::DoesSaveGameExist(SARPGGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0))
	{
		USaveGame* LoadGameObject = UGameplayStatics::LoadGameFromSlot(SARPGGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);
		if (USARPGSaveGame* SARPGSaveGame = Cast<USARPGSaveGame>(LoadGameObject))
		{
			OutSavedGameDifficulty = SARPGSaveGame->SavedCurrentGameDifficulty;
			return true;
		}
	}

	return false;
}
