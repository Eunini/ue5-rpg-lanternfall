#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LanternPlayer.generated.h"

UCLASS()
class LANTERNFALL_API ALanternPlayer : public ACharacter
{
 GENERATED_BODY()
public:
 ALanternPlayer();
 virtual void Tick(float Delta) override;
 virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
 UFUNCTION(BlueprintCallable) void BeginJourney();
 class ALanternUsable* Focused() const;
 bool bJournal=false;
 bool WasDemoSaveRestored() const {return bDemoRestored;}
private:
 UPROPERTY() TObjectPtr<class UCameraComponent> Camera;
 float DemoAge=0.f,StepAge=0.f;
 int32 DemoStep=0;
 bool bDemoRestored=false;
 void AdvanceDemo(float Delta);
 void Forward(float Value);void Right(float Value);void Yaw(float Value);void Pitch(float Value);
 void Use();void Choice1();void Choice2();void Choice3();void Save();void Load();void Journal();
};
