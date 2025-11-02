// Clement Lecoeur All Rights Reserved


#include "Widgets/SARPGWidgetBase.h"
#include "Interfaces/PawnUIInterface.h"

void USARPGWidgetBase::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn())) {
        if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent()) {
            BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
        }
    }
}

void USARPGWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
    if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor)) {
        UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();

		checkf(EnemyUIComponent, TEXT("Failed to extract an EnemyUIComponent from %s"), *OwningEnemyActor->GetActorNameOrLabel());

		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
    }
}
