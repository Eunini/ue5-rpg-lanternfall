#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/Journey.h"
#include "JourneySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJourneyChanged);
UCLASS()
class LANTERNFALL_API UJourneySubsystem : public UGameInstanceSubsystem
{
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) void OpenDialogue();
 UFUNCTION(BlueprintCallable) void CloseDialogue();
 UFUNCTION(BlueprintCallable) void ChooseDialogue(int32 Index);
 UFUNCTION(BlueprintCallable) bool CollectCell(FName Id);
 UFUNCTION(BlueprintCallable) bool DeliverPower(int32 Route);
 UFUNCTION(BlueprintCallable) bool SaveJourney();
 UFUNCTION(BlueprintCallable) bool LoadJourney();
 UFUNCTION(BlueprintPure) FText GetObjective() const;
 UFUNCTION(BlueprintPure) FText GetDialogue() const;
 UFUNCTION(BlueprintPure) TArray<FText> GetChoices() const;
 UFUNCTION(BlueprintPure) int32 GetCells() const { return Journey.Count("EnergyCell"); }
 UFUNCTION(BlueprintPure) int32 GetRoute() const { return static_cast<int32>(Journey.Get().Destination); }
 UFUNCTION(BlueprintPure) bool IsDialogueOpen() const { return bDialogue; }
 UPROPERTY(BlueprintAssignable) FJourneyChanged OnJourneyChanged;
 bool IsClaimed(FName Id) const;
 bool IsPowered() const;
 FText Status;
 const Lantern::Snapshot& GetSnapshot() const { return Journey.Get(); }
private:
 Lantern::Journey Journey;
 bool bDialogue=false;
 void Changed();
};

