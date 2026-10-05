#include "Journey.h"
namespace Lantern
{
 Result Journey::Choose(Route Destination)
 {
  if ((Destination!=Route::Clinic && Destination!=Route::Relay) ||
      (State.Progress!=Stage::Offered && State.Progress!=Stage::Accepted)) return Result::WrongStage;
  State.Destination=Destination; State.Progress=Stage::Accepted; return Result::Ok;
 }
 Result Journey::Collect(const std::string& Pickup)
 {
  if (Pickup!="Cell_A" && Pickup!="Cell_B" && Pickup!="Cell_C") return Result::UnknownPickup;
  if (State.Pickups.count(Pickup)) return Result::AlreadyCollected;
  State.Pickups.insert(Pickup); ++State.Items["EnergyCell"]; return Result::Ok;
 }
 Result Journey::Deliver(Route Terminal)
 {
  if (State.Progress!=Stage::Accepted) return Result::WrongStage;
  if (Terminal!=State.Destination) return Result::WrongTerminal;
  if (Count("EnergyCell")<2) return Result::MissingCells;
  State.Items["EnergyCell"]-=2;
  State.Items[Terminal==Route::Clinic?"MedicBadge":"SignalBadge"]=1;
  State.Progress=Stage::Powered; return Result::Ok;
 }
 Result Journey::Resolve()
 {
  if (State.Progress!=Stage::Powered) return Result::WrongStage;
  State.Progress=Stage::Resolved; return Result::Ok;
 }
 bool Journey::Valid(const Snapshot& S)
 {
  if (S.Schema!=1 || static_cast<int>(S.Progress)<0 || static_cast<int>(S.Progress)>3 ||
      static_cast<int>(S.Destination)<0 || static_cast<int>(S.Destination)>2) return false;
  if ((S.Progress==Stage::Offered)!=(S.Destination==Route::None)) return false;
  for (const auto& Pickup:S.Pickups)
   if (Pickup!="Cell_A" && Pickup!="Cell_B" && Pickup!="Cell_C") return false;
  for (const auto& Item:S.Items)
   if ((Item.first!="EnergyCell" && Item.first!="MedicBadge" && Item.first!="SignalBadge") ||
       Item.second<0 || Item.second>3) return false;
  const auto Count=[&S](const char* Key){const auto It=S.Items.find(Key);return It==S.Items.end()?0:It->second;};
  const bool Powered=static_cast<int>(S.Progress)>=static_cast<int>(Stage::Powered);
  if (Count("EnergyCell")!=static_cast<int>(S.Pickups.size())-(Powered?2:0)) return false;
  if (Count("MedicBadge")!=(Powered && S.Destination==Route::Clinic?1:0)) return false;
  if (Count("SignalBadge")!=(Powered && S.Destination==Route::Relay?1:0)) return false;
  return true;
 }
 bool Journey::Restore(const Snapshot& Candidate)
 {
  if (!Valid(Candidate)) return false;
  State=Candidate; return true;
 }
}

