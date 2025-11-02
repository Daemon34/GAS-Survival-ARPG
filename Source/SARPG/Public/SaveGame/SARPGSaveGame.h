// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SARPGTypes/SARPGEnumTypes.h"
#include "SARPGSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SARPG_API USARPGSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	ESARPGGameDifficulty SavedCurrentGameDifficulty;
	
};
