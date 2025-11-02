// Clement Lecoeur All Rights Reserved


#include "Items/PickUps/SARPGStoneBase.h"
#include "Characters/SARPGHeroCharacter.h"
#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "SARPGGameplayTags.h"

void ASARPGStoneBase::Consume(USARPGAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);

	UGameplayEffect* EffectCDO = StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();

	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		EffectCDO,
		ApplyLevel,
		AbilitySystemComponent->MakeEffectContext()
	);

	BP_OnStoneConsumed();
}

void ASARPGStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASARPGHeroCharacter* OverlappedHeroCharacter = Cast<ASARPGHeroCharacter>(OtherActor)) {
		OverlappedHeroCharacter->GetSARPGAbilitySystemComponent()->TryActivateAbilityByTag(SARPGGameplayTags::Player_Ability_PickUp_Stones);
	}
}
