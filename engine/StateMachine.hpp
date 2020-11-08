#pragma once

#include "IState.hpp"

#include <cassert>

// Idea from book Programming Game AI by Example
template <typename EntityType>
class StateMachine {
public:
	StateMachine(EntityType* owner) :
	owner_(owner)
	{}

	void SetCurrentState(IState<EntityType>* state) { current_state_ = state; }
	void SetGlobalState(IState<EntityType>* state) { global_state_ = state; }
	void SetPreviousState(IState<EntityType>* state) { previous_state_ = state; }

	void Update(float elapsed) 
	{
		if (global_state_) {
			global_state_->Execute(owner_, elapsed);
		}

		if (current_state_) {
			current_state_->Execute(owner_, elapsed);
		}
	}

	void ChangeState(State<EntityType*> new_state) 
	{
		assert(new_state);
		previous_state_ = current_state_;
		current_state_->Exit(owner_);
		current_state_ = new_state;
		current_state_->Enter(owner_);
	}

	void RevertToPreviousState()
	{
		ChangeState(previous_state_);
	}

private:
	EntityType* owner_;
	IState<EntityType>* current_state_ { nullptr };
	IState<EntityType>* previous_state_ { nullptr };
	IState<EntityType>* global_state_ { nullptr };
};
