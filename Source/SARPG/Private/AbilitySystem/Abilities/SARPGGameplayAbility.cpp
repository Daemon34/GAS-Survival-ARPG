// Clement Lecoeur All Rights Reserved

#include "AbilitySystem/Abilities/SARPGGameplayAbility.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "SARPGFunctionLibrary.h"
#include "SARPGGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"

void USARPGGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (AbilityActivationPolicy == ESARPGAbilityActivationPolicy::OnGiven) {
		if (ActorInfo && !Spec.IsActive()) {
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void USARPGGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (AbilityActivationPolicy == ESARPGAbilityActivationPolicy::OnGiven) {
		if (ActorInfo) {
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
		}
	}
}

UPawnCombatComponent* USARPGGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

USARPGAbilitySystemComponent* USARPGGameplayAbility::GetSARPGAbilitySystemComponentFromActorInfo() const
{
	return Cast<USARPGAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle USARPGGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	check(TargetASC && InSpecHandle.IsValid());

	return GetSARPGAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle USARPGGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, ESARPGSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? ESARPGSuccessType::Successfull : ESARPGSuccessType::Failed;

	return ActiveGameplayEffectHandle;
}

void USARPGGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty()){
		return;
	}

	APawn* OwningPawn = CastChecked<APawn>(GetAvatarActorFromActorInfo());

	for(const FHitResult& Hit : InHitResults){
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor())) {
			if (USARPGFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn)) {
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);

				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied()) {
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitPawn, SARPGGameplayTags::Shared_Event_HitReact, Data);
				}
			}
		}
	}
}
