// Clement Lecoeur All Rights Reserved


#include "AbilitySystem/SARPGAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SARPGHeroGameplayAbility.h"
#include "SARPGGameplayTags.h"

void USARPGAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;

		if (InInputTag.MatchesTag(SARPGGameplayTags::InputTag_Toggleable) && AbilitySpec.IsActive()) {
			CancelAbilityHandle(AbilitySpec.Handle);
		}
		else {
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void USARPGAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid() || !InInputTag.MatchesTag(SARPGGameplayTags::InputTag_MustBeHeld)) {
		return;
	}

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) {
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag) && AbilitySpec.IsActive()) {
			CancelAbilityHandle(AbilitySpec.Handle);
		}
	}
}

void USARPGAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FSARPGHeroAbilitySet>& InDefaultWeaponAbilities, const TArray<FSARPGHeroSpecialAbilitySet>& InDefaultSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty()) {
		return;
	}

	for (const FSARPGHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities) {
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}

	for (const FSARPGHeroSpecialAbilitySet& AbilitySet : InDefaultSpecialWeaponAbilities) {
		if (!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void USARPGAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty()) {
		return;
	}

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove) {
		if (SpecHandle.IsValid()) {
			ClearAbility(SpecHandle);
		}
	}
	InSpecHandlesToRemove.Empty();
}

bool USARPGAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if (!FoundAbilitySpecs.IsEmpty()) {
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* SpecToActivate = FoundAbilitySpecs[RandomAbilityIndex];
		
		check(SpecToActivate);

		if (!SpecToActivate->IsActive()) {
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}

	return false;
}
