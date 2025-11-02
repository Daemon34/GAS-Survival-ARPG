// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "SARPGHeroController.generated.h"

/**
 * 
 */
UCLASS()
class SARPG_API ASARPGHeroController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ASARPGHeroController();

	//~ Begin IGenericTeamAgentInterface Interface.
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface Interface.

private:
	FGenericTeamId HeroTeamId;
};
