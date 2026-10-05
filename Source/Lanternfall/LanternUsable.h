#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LanternUsable.generated.h"

UENUM(BlueprintType)
enum class ELanternAction : uint8 { Nera, Cell, Clinic, Relay };
UCLASS()
class LANTERNFALL_API ALanternUsable : public AActor
{
 GENERATED_BODY()
public:
 ALanternUsable();
 virtual void OnConstruction(const FTransform& Transform) override;
 virtual void Tick(float DeltaTime) override;
 virtual void BeginPlay() override;
 UFUNCTION(BlueprintNativeEvent) void Interact(APawn* Interactor);
 virtual void Interact_Implementation(APawn* Interactor);
 UFUNCTION(BlueprintCallable) void PerformAction(APawn* Interactor);
 void Configure(ELanternAction Action,FName Id=NAME_None);
 FText GetPrompt() const;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) ELanternAction Action=ELanternAction::Nera;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FName PickupId;
private:
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Body;
 UPROPERTY() TObjectPtr<class UPointLightComponent> PowerLight;
 UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> PowerIndicator;
};
