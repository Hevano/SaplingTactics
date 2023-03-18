#include "UnitAction.h"
#include "Unit.h"

Status UnitAction::tick(Unit& u)
{
  status = effect(u, *this);
  if (status == Status::Failure || status == Status::Success) data.clear();
  return status;
}
