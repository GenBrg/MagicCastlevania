#include "Mode.hpp"
#include <iostream>
std::shared_ptr< Mode > Mode::current;

void Mode::set_current(std::shared_ptr< Mode > const &new_current) {
	if (current) {
		current->on_leave();
	}
	current = new_current;
	current->on_enter();
	//NOTE: may wish to, e.g., trigger resize events on new current mode.
}
