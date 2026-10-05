#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LanternWorld.generated.h"
UCLASS()
class LANTERNFALL_API ALanternWorld : public AActor
{
 GENERATED_BODY()
public:
 ALanternWorld();
 virtual void OnConstruction(const FTransform& Transform) override;
 virtual void BeginPlay() override;
private:
 UPROPERTY() TArray<TObjectPtr<AActor>> Actors;
};

