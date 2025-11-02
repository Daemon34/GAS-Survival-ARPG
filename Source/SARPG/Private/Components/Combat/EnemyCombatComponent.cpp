// Clement Lecoeur All Rights Reserved


#include "Components/Combat/EnemyCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SARPGGameplayTags.h"
#include "SARPGFunctionLibrary.h"
#include "Characters/SARPGEnemyCharacter.h"
#include "Components/BoxComponent.h"
#include "SARPGDebugHelper.h"

void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlappedActors.Contains(HitActor)) {
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	// TODO : Implement block check
	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = USARPGFunctionLibrary::NativeDoesActorHaveTag(HitActor, SARPGGameplayTags::Player_Status_Blocking);
	const bool bIsMyAttackUnblockable = USARPGFunctionLibrary::NativeDoesActorHaveTag(GetOwningPawn(), SARPGGameplayTags::Enemy_Status_Unblockable);

	if(bIsPlayerBlocking && !bIsMyAttackUnblockable) {
		bIsValidBlock = USARPGFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;

	if (bIsValidBlock) {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			SARPGGameplayTags::Player_Event_SuccessfulBlock,
			EventData
		);
	}
	else {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			SARPGGameplayTags::Shared_Event_MeleeHit,
			EventData
		);
	}
}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	ASARPGEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<ASARPGEnemyCharacter>();
	check(OwningEnemyCharacter);

	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter->GetRightHandCollisionBox();

	check(LeftHandCollisionBox && RightHandCollisionBox);

	switch(ToggleDamageType)
	{
		case EToggleDamageType::LeftHand:
			LeftHandCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
			break;
		case EToggleDamageType::RightHand:
			RightHandCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
			break;
	}

	if (!bShouldEnable) {
		OverlappedActors.Empty();
	}
}
