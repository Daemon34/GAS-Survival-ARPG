// Clement Lecoeur All Rights Reserved


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStones.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Characters/SARPGHeroCharacter.h"
#include "Items/PickUps/SARPGStoneBase.h"
#include "Components/UI/HeroUIComponent.h"

void UHeroGameplayAbility_PickUpStones::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(true);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_PickUpStones::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetHeroUIComponentFromActorInfo()->OnStoneInteracted.Broadcast(false);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_PickUpStones::CollectStones()
{
	CollectedStones.Empty();
	TArray<FHitResult> TraceHits;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorUpVector() * -1.0f * BoxTraceDistance,
		TraceBoxSize / 2.0f,
		(GetHeroCharacterFromActorInfo()->GetActorUpVector() * -1.0f).ToOrientationRotator(),
		StoneTraceChannel,
		false,
		TArray<AActor*>(),
		bDrawDebugShape ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None,
		TraceHits,
		true
	);

	for (const FHitResult& TraceHit : TraceHits) {
		if (ASARPGStoneBase* FoundStone = Cast<ASARPGStoneBase>(TraceHit.GetActor())) {
			CollectedStones.AddUnique(FoundStone);
		}
	}

	if (CollectedStones.IsEmpty()) {
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
}

void UHeroGameplayAbility_PickUpStones::ConsumeStones()
{
	if (CollectedStones.IsEmpty()) {
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return;
	}

	for (ASARPGStoneBase* CollectedStone : CollectedStones) {
		if (CollectedStone) {
			CollectedStone->Consume(GetSARPGAbilitySystemComponentFromActorInfo(), GetAbilityLevel());
		}
	}
}
