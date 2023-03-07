#include "RangedUnit.h"

void RangedUnit::move()
{
	if (!active) return;
	Vector2 move = Vector2MoveTowards(rect.GetPosition(), movement, stats[Stat::Speed] * GetFrameTime());
	rect.SetPosition(move);

	for (auto& p : projectiles) {
          p.update();    
	}
}
