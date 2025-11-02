// Clement Lecoeur All Rights Reserved


#include "SARPGGameInstance.h"
#include "MoviePlayer.h"

void USARPGGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ThisClass::OnPreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnDestinationWorldLoaded);
}

void USARPGGameInstance::OnPreLoadMap(const FString& MapName)
{
	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreenAttributes.MinimumLoadingScreenDisplayTime = 2.0f;
	LoadingScreenAttributes.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);
}

void USARPGGameInstance::OnDestinationWorldLoaded(UWorld* LoadedWorld)
{
	GetMoviePlayer()->StopMovie();
}

TSoftObjectPtr<UWorld> USARPGGameInstance::GetGameLevelByTag(FGameplayTag InLevelTag) const
{
	for(const FSARPGGameLevelSet& GameLevelSet : GameLevelSets) {
		if (!GameLevelSet.IsValid()) {
			continue;
		}

		if (GameLevelSet.LevelTag == InLevelTag) {
			return GameLevelSet.Level;
		}
	}

	return TSoftObjectPtr<UWorld>();
}
