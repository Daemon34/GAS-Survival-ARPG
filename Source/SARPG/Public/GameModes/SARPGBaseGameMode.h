// Clement Lecoeur All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SARPGTypes/SARPGEnumTypes.h"
#include "SARPGBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SARPG_API ASARPGBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASARPGBaseGameMode();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	ESARPGGameDifficulty CurrentGameDifficulty;

public:
	FORCEINLINE ESARPGGameDifficulty GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }
};
