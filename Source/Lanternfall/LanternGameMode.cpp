#include "LanternGameMode.h"
#include "LanternPlayer.h"
#include "LanternHUD.h"
#include "LanternWorld.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
ALanternGameMode::ALanternGameMode()
{
 DefaultPawnClass=ALanternPlayer::StaticClass();HUDClass=ALanternHUD::StaticClass();
 static ConstructorHelpers::FClassFinder<APawn> Blueprint(TEXT("/Game/Lanternfall/Blueprints/BP_Courier"));
 if (Blueprint.Succeeded()) DefaultPawnClass=Blueprint.Class;
}
void ALanternGameMode::InitGame(const FString& MapName,const FString& Options,FString& ErrorMessage)
{
 Super::InitGame(MapName,Options,ErrorMessage);
 if (!TActorIterator<ALanternWorld>(GetWorld())) GetWorld()->SpawnActor<ALanternWorld>();
}

