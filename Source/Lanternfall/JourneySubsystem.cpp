#include "JourneySubsystem.h"
#include "JourneySave.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

namespace { FText Txt(const TCHAR* Value){return FText::FromString(Value);} }
void UJourneySubsystem::OpenDialogue(){bDialogue=true;OnJourneyChanged.Broadcast();}
void UJourneySubsystem::CloseDialogue(){bDialogue=false;OnJourneyChanged.Broadcast();}
void UJourneySubsystem::Changed()
{
 SaveJourney(); OnJourneyChanged.Broadcast();
}
bool UJourneySubsystem::IsClaimed(FName Id) const {return Journey.Get().Pickups.count(TCHAR_TO_UTF8(*Id.ToString()))!=0;}
bool UJourneySubsystem::IsPowered() const {return static_cast<int>(Journey.Get().Progress)>=2;}
void UJourneySubsystem::ChooseDialogue(int32 Index)
{
 if (!bDialogue) return;
 using namespace Lantern;
 const Stage StageNow=Journey.Get().Progress;
 if (StageNow==Stage::Offered || StageNow==Stage::Accepted)
 {
  if (Index==0 || Index==1)
  {
   Journey.Choose(Index==0?Route::Clinic:Route::Relay); bDialogue=false; Changed();
  }
  else if (Index==2) CloseDialogue();
 }
 else if (StageNow==Stage::Powered && Index==0)
 {
  Journey.Resolve(); bDialogue=false; Changed();
 }
 else if (Index==0) CloseDialogue();
}
bool UJourneySubsystem::CollectCell(FName Id)
{
 const bool Success=Journey.Collect(TCHAR_TO_UTF8(*Id.ToString()))==Lantern::Result::Ok;
 if (Success) {Status=Txt(TEXT("Energy cell secured."));Changed();}
 return Success;
}
bool UJourneySubsystem::DeliverPower(int32 Route)
{
 const auto Result=Journey.Deliver(static_cast<Lantern::Route>(Route));
 if (Result==Lantern::Result::Ok) {Status=Txt(TEXT("Power restored. Return to Nera."));Changed();return true;}
 Status=Txt(Result==Lantern::Result::MissingCells?TEXT("Two energy cells are required."):
            Result==Lantern::Result::WrongTerminal?TEXT("Your chosen destination is the other terminal."):
            TEXT("Speak with Nera to choose a destination."));
 OnJourneyChanged.Broadcast(); return false;
}
FText UJourneySubsystem::GetObjective() const
{
 using namespace Lantern;
 switch (Journey.Get().Progress)
 {
  case Stage::Offered:return Txt(TEXT("Find Nera at the courtyard. Decide who receives the reserve power."));
  case Stage::Accepted:return Txt(Journey.Get().Destination==Route::Clinic?
   TEXT("Gather two cells. Restore the clinic at the amber terminal."):
   TEXT("Gather two cells. Restore the relay at the blue terminal."));
  case Stage::Powered:return Txt(TEXT("Return to Nera. See the consequence of your decision."));
  default:return Txt(Journey.Get().Destination==Route::Clinic?
   TEXT("Clinic secured. The town survives tonight; the outside world remains silent."):
   TEXT("Relay secured. Help is on its way; the clinic faces one more night in the dark."));
 }
}
FText UJourneySubsystem::GetDialogue() const
{
 using namespace Lantern;
 if (Journey.Get().Progress==Stage::Offered || Journey.Get().Progress==Stage::Accepted)
  return Txt(TEXT("NERA / FIELD ENGINEER\nWe have one reserve circuit. Two cells can power the clinic or the radio relay.\nThe clinic keeps people alive tonight. The relay could bring help tomorrow.\nChoose carefully. You can change your mind before the cells are connected."));
 return Txt(Journey.Get().Destination==Route::Clinic?
  TEXT("NERA\nThe clinic lights are back. Mara can keep the oxygen running.\nThe relay stays quiet. We will have to face tomorrow ourselves."):
  TEXT("NERA\nA relief convoy answered the relay. They will arrive at dawn.\nMara is holding the clinic together by lanternlight until then."));
}
TArray<FText> UJourneySubsystem::GetChoices() const
{
 const auto Stage=Journey.Get().Progress;
 if (Stage==Lantern::Stage::Offered || Stage==Lantern::Stage::Accepted)
  return {Txt(TEXT("Power the clinic. Protect the people already here.")),
          Txt(TEXT("Power the relay. Call for outside help.")),Txt(TEXT("I need more time."))};
 return {Txt(Stage==Lantern::Stage::Powered?TEXT("We live with this choice."):TEXT("Until tomorrow, Nera."))};
}
bool UJourneySubsystem::SaveJourney()
{
 UJourneySave* Save=Cast<UJourneySave>(UGameplayStatics::CreateSaveGameObject(UJourneySave::StaticClass()));
 const auto& S=Journey.Get();
 if (!Save || !Lantern::Journey::Valid(S)) return false;
 Save->Stage=static_cast<int32>(S.Progress); Save->Route=static_cast<int32>(S.Destination);
 for (const auto& Item:S.Items) Save->Inventory.Add(UTF8_TO_TCHAR(Item.first.c_str()),Item.second);
 for (const auto& Pickup:S.Pickups) Save->Pickups.Add(UTF8_TO_TCHAR(Pickup.c_str()));
 if (const APawn* Player=UGameplayStatics::GetPlayerPawn(GetWorld(),0)) Save->Position=Player->GetActorLocation();
 const bool Success=UGameplayStatics::SaveGameToSlot(Save,TEXT("Lanternfall_Journey"),0);
 if (!Success) Status=Txt(TEXT("The save could not be written. Your current journey remains open."));
 return Success;
}
bool UJourneySubsystem::LoadJourney()
{
 const auto* Save=Cast<UJourneySave>(UGameplayStatics::LoadGameFromSlot(TEXT("Lanternfall_Journey"),0));
 if (!Save) {Status=Txt(TEXT("No saved journey is available."));return false;}
 Lantern::Snapshot Candidate;
 Candidate.Schema=Save->Schema;Candidate.Progress=static_cast<Lantern::Stage>(Save->Stage);
 Candidate.Destination=static_cast<Lantern::Route>(Save->Route);
 for (const auto& Item:Save->Inventory) Candidate.Items.emplace(TCHAR_TO_UTF8(*Item.Key),Item.Value);
 for (const auto& Id:Save->Pickups) Candidate.Pickups.emplace(TCHAR_TO_UTF8(*Id));
 if (Save->Position.ContainsNaN() || Save->Position.SizeSquared()>FMath::Square(20000.f) || !Journey.Restore(Candidate))
 {Status=Txt(TEXT("The saved journey could not be restored."));return false;}
 if (APawn* Player=UGameplayStatics::GetPlayerPawn(GetWorld(),0)) Player->SetActorLocation(Save->Position);
 bDialogue=false;Status=Txt(TEXT("Journey restored."));OnJourneyChanged.Broadcast();return true;
}

