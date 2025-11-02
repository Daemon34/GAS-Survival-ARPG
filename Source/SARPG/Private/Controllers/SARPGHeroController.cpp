// Clement Lecoeur All Rights Reserved


#include "Controllers/SARPGHeroController.h"

ASARPGHeroController::ASARPGHeroController()
{
    HeroTeamId = FGenericTeamId(0);
}

FGenericTeamId ASARPGHeroController::GetGenericTeamId() const
{
    return HeroTeamId;
}
