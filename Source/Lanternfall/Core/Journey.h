#pragma once
#include <map>
#include <set>
#include <string>

namespace Lantern
{
 enum class Route { None, Clinic, Relay };
 enum class Stage { Offered, Accepted, Powered, Resolved };
 enum class Result { Ok, WrongStage, WrongTerminal, MissingCells, AlreadyCollected, UnknownPickup };
 struct Snapshot
 {
  int Schema=1;
  Route Destination=Route::None;
  Stage Progress=Stage::Offered;
  std::map<std::string,int> Items;
  std::set<std::string> Pickups;
 };
 class Journey
 {
 public:
  const Snapshot& Get() const { return State; }
  int Count(const std::string& Item) const
  { const auto It=State.Items.find(Item); return It==State.Items.end()?0:It->second; }
  Result Choose(Route Destination);
  Result Collect(const std::string& Pickup);
  Result Deliver(Route Terminal);
  Result Resolve();
  bool Restore(const Snapshot& Candidate);
  static bool Valid(const Snapshot& Candidate);
 private:
  Snapshot State;
 };
}

