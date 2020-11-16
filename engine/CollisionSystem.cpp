#include "CollisionSystem.hpp"

void CollisionSystem::Update(float elapsed)
{
	// Collision detection
	for (auto& [channel, colliders] : colliders_) {
		for (auto& collider : colliders) {
			for (const auto& checking_channel : kChannelProperty.at(channel)) {
				for (auto& other_collider : colliders_[checking_channel]) {

				}
			}
		}
	}

	// Garbage collection
	for (auto& [_, colliders] : colliders_) {
		colliders.erase(std::remove_if(colliders.begin(), colliders.end(), [&](Collider* collider){
			if (collider->IsDestroyed()) {
				delete collider;
				return true;
			}
			return false;
		}), colliders.end());
	}
}

void CollisionSystem::AddCollider(Collider* collider)
{
	colliders_[collider->GetChannel()].push_back(collider);
}