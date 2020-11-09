#pragma once

template <typename EntityType>
class IState {
public:
	virtual void Enter(EntityType*) = 0;
	virtual void Execute(EntityType*, float elapsed) = 0;
	virtual void Exit(EntityType*) = 0;
	virtual ~State() {}
};