// Clement Lecoeur All Rights Reserved


#include "AbilitySystem/SARPGAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "SARPGFunctionLibrary.h"
#include "SARPGGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/PawnUIComponent.h"
#include "Components/UI/HeroUIComponent.h"

#include "SARPGDebugHelper.h"

USARPGAttributeSet::USARPGAttributeSet()
{
	InitCurrentHealth(1.0f);
	InitMaxHealth(1.0f);
	InitCurrentRage(1.0f);
	InitMaxRage(1.0f);
	InitAttackPower(1.0f);
	InitDefensePower(1.0f);
	InitDamageTaken(0.0f);
}

void USARPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid()) {
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}

	checkf(CachedPawnUIInterface.IsValid(), TEXT("CachedPawnUIInterface is not valid. Make sure the actor %s implements IPawnUIInterface."), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();

	checkf(PawnUIComponent, TEXT("PawnUIComponent is not valid. Make sure the actor %s has a PawnUIComponent attached."), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute()) {
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.0f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute()) {
		const float NewCurrentRage = FMath::Clamp(GetCurrentRage(), 0.0f, GetMaxRage());

		SetCurrentRage(NewCurrentRage);

		if(GetCurrentRage() == GetMaxRage()) {
			USARPGFunctionLibrary::AddGameplayTagToActorIfNone(
				Data.Target.GetAvatarActor(),
				SARPGGameplayTags::Player_Status_Rage_Full
			);
		}
		else if(GetCurrentRage() == 0.0){
			USARPGFunctionLibrary::AddGameplayTagToActorIfNone(
				Data.Target.GetAvatarActor(),
				SARPGGameplayTags::Player_Status_Rage_None
			);
		}
		else {
			USARPGFunctionLibrary::RemoveGameplayTagFromActorIfFound(
				Data.Target.GetAvatarActor(),
				SARPGGameplayTags::Player_Status_Rage_Full
			);

			USARPGFunctionLibrary::RemoveGameplayTagFromActorIfFound(
				Data.Target.GetAvatarActor(),
				SARPGGameplayTags::Player_Status_Rage_None
			);
		}

		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent()) {
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute()) {
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();
		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.0f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);

		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());

		if (GetCurrentHealth() == 0.0f) {
			USARPGFunctionLibrary::AddGameplayTagToActorIfNone(
				Data.Target.GetAvatarActor(),
				SARPGGameplayTags::Shared_Status_Dead
			);
		}
	}
}
