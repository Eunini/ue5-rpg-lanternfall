#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LanternGameMode.generated.h"
UCLASS()
class LANTERNFALL_API ALanternGameMode : public AGameModeBase
{
 GENERATED_BODY()
public:
 ALanternGameMode();
 virtual void InitGame(const FString& MapName,const FString& Options,FString& ErrorMessage) override;
};

