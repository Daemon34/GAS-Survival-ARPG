// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SARPGTypes/SARPGEnumTypes.h"
#include "SARPGFunctionLibrary.generated.h"

class USARPGAbilitySystemComponent;
class UPawnCombatComponent;
struct FScalableFloat;
class USARPGGameInstance;
/**
 * 
 */
UCLASS()
class SARPG_API USARPGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static USARPGAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);

	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, ESARPGConfirmType& OutConfirmType);

	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary", meta = (DisplayName = "Get Pawn Combat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, ESARPGValidType& OutValidType);

	UFUNCTION(BlueprintPure, Category = "SARPG|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);

	UFUNCTION(BlueprintPure, Category = "SARPG|FunctionLibrary", meta = (CompactNodeTitle = "Get Value At Level"))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel = 1.0f);

	UFUNCTION(BlueprintPure, Category = "SARPG|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference);

	UFUNCTION(BlueprintPure, Category = "SARPG|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InEffectSpecHandle);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo", ExpandEnumAsExecs = "CountDownInput|CountDownOutput", TotalTime = "1.0", UpdateInterval = "0.1"))
	static void CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval, float& OutTimeRemaining, ESARPGCountDownActionInput CountDownInput, UPARAM(DisplayName = "Output") ESARPGCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintPure, Category = "SARPG|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static USARPGGameInstance* GetSARPGGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static void ToggleInputMode(ESARPGInputMode InInputMode, const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary")
	static void SaveCurrentGameDifficulty(ESARPGGameDifficulty InGameDifficultyToSave);

	UFUNCTION(BlueprintCallable, Category = "SARPG|FunctionLibrary")
	static bool TryLoadSavedGameDifficulty(ESARPGGameDifficulty& OutSavedGameDifficulty);
};