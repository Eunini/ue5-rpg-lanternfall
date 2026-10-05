#include "../Source/Lanternfall/Core/Journey.h"
#include <iostream>
#include <stdexcept>
using namespace Lantern;
int Count=0;
void Expect(bool Value,const char* Name){++Count;if (!Value) throw std::runtime_error(Name);}
int main()
{
 try
 {
  for (Route RouteNow:{Route::Clinic,Route::Relay})
  {
   Journey J;Expect(Journey::Valid(J.Get()),"initial snapshot");
   Expect(J.Deliver(RouteNow)==Result::WrongStage,"route required");
   Expect(J.Choose(RouteNow)==Result::Ok,"choose route");
   Expect(J.Deliver(RouteNow)==Result::MissingCells,"cell requirement");
   Expect(J.Collect("Cell_A")==Result::Ok,"first cell");
   Expect(J.Collect("Cell_A")==Result::AlreadyCollected,"unique pickup");
   Expect(J.Collect("unknown")==Result::UnknownPickup,"known pickup IDs");
   Expect(J.Collect("Cell_B")==Result::Ok,"second cell");
   const auto Opposite=RouteNow==Route::Clinic?Route::Relay:Route::Clinic;
   Expect(J.Deliver(Opposite)==Result::WrongTerminal,"destination enforced");
   Expect(J.Count("EnergyCell")==2,"failed delivery is atomic");
   Expect(J.Deliver(RouteNow)==Result::Ok,"delivery");
   Expect(J.Count("EnergyCell")==0,"exact consumption");
   Expect(J.Deliver(RouteNow)==Result::WrongStage,"single reward");
   Expect(J.Choose(Opposite)==Result::WrongStage,"committed decision");
   Expect(Journey::Valid(J.Get()),"powered invariant");
   Journey Restored;Expect(Restored.Restore(J.Get()),"restore progress");
   Expect(Restored.Resolve()==Result::Ok,"finish narrative");
   Expect(Journey::Valid(Restored.Get()),"resolved invariant");
   auto Invalid=Restored.Get();Invalid.Items["EnergyCell"]=-1;
   Expect(!Restored.Restore(Invalid),"negative inventory rejected");
   Expect(Restored.Get().Progress==Stage::Resolved,"invalid restore is atomic");
   Invalid=Restored.Get();Invalid.Pickups.insert("invented");
   Expect(!Restored.Restore(Invalid),"unknown pickup rejected");
   Invalid=Restored.Get();Invalid.Schema=999;
   Expect(!Restored.Restore(Invalid),"schema enforced");
  }
  Journey J;J.Collect("Cell_A");J.Collect("Cell_B");J.Collect("Cell_C");
  Expect(J.Count("EnergyCell")==3,"exploration before dialogue");
  J.Choose(Route::Clinic);J.Choose(Route::Relay);
  Expect(J.Deliver(Route::Relay)==Result::Ok,"reroute before connection");
  Expect(J.Count("EnergyCell")==1,"surplus inventory preserved");
  Expect(Journey::Valid(J.Get()),"surplus invariant");
  std::cout<<Count<<" journey checks passed\n";return 0;
 }
 catch (const std::exception& E){std::cerr<<E.what()<<"\n";return 1;}
}

