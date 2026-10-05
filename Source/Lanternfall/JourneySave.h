#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "JourneySave.generated.h"

UCLASS()
class LANTERNFALL_API UJourneySave : public USaveGame
{
 GENERATED_BODY()
public:
 UPROPERTY(SaveGame) int32 Schema=1;
 UPROPERTY(SaveGame) int32 Stage=0;
 UPROPERTY(SaveGame) int32 Route=0;
 UPROPERTY(SaveGame) TMap<FString,int32> Inventory;
 UPROPERTY(SaveGame) TArray<FString> Pickups;
 UPROPERTY(SaveGame) FVector Position=FVector(-500,0,180);
};

