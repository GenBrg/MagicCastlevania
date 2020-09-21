#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string>
#include <cmath>

//Game audio system. Simplified from f18-base3.
//Uses 48kHz sampling rate.

namespace Sound {

//Sample objects hold mono (one-channel) audio.
struct Sample {
	//Load from a '.wav' or '.opus' file.
	//  will warn and convert if sound is not already 48kHz mono:
	Sample(std::string const &filename);
	
	//Directly supply an audio buffer:
	Sample(std::vector< float > const &data);

	//sample data is stored as 48kHz, mono, floating-point:
	std::vector< float > data;
};

//Ramp<> manages values that should be smoothly interpolated
//  to a target over a certain amount of time:
template< typename T >
struct Ramp {
	//This variadic template passes the arguments to the constructor of 'value':
	template< typename... Args >
	Ramp(Args&&... args) : value(std::forward< Args >(args)...), target(value) { }

	//The 'set()' function sets a new value with an included ramp time:
	void set(T const &value_, float ramp_) {
		if (ramp_ <= 0.0f) {
			value = target = value_;
			ramp = 0.0f;
		} else {
			target = value_;
			ramp = ramp_;
		}
	}
	T value;
	T target;
	float ramp = 0.0f;
};

// 'PlayingSample' objects book-keep samples that are currently playing:
struct PlayingSample {
	//change the panning or volume of a playing sample (and do proper locking);
	// value will change over 'ramp' seconds to avoid creating audible artifacts:
	void set_volume(float new_volume, float ramp = 1.0f / 60.0f);
	//set the panning of a sample (use only on samples in "2D" mode; no effect on "3D" samples):
	void set_pan(float new_pan, float ramp = 1.0f / 60.0f);
	//set the position of a sample (use only on samples in "3D" mode; no effect on "2D" samples):
	void set_position(glm::vec3 const &new_position, float ramp = 1.0f / 60.0f);
	//set the half-volume radius (use only on "3D" playing sounds):
	void set_half_volume_radius(float new_radius, float ramp = 1.0f / 60.0f);

	//'stop' will fade sample out over 'ramp' seconds and then remove it from the active samples:
	void stop(float ramp = 1.0f / 60.0f);

	//internals:
	//NOTE: PlayingSample is used in a separate thread; so setting these values directly
	// may result in bad results. Instead, use the functions above, which perform locking!
	std::vector< float > const &data; //reference to sample data being played
	uint32_t i = 0; //next data value to read
	bool loop = false; //should playback loop after data runs out?
	bool stopping = false; //is playing stopping?
	bool stopped = false; //was playback stopped (either by running out of sample, or by stop())?

	Ramp< float > volume = Ramp< float >(1.0f);

	//2D playback panning control: ('NaN' if sound played in 3D mode)
	Ramp< float > pan = Ramp< float >(std::numeric_limits< float >::quiet_NaN());

	//3D playback panning control: ('NaN' if sound played in 2D mode)
	Ramp< glm::vec3 > position = Ramp< glm::vec3 >(std::numeric_limits< float >::quiet_NaN());
	Ramp< float > half_volume_radius = std::numeric_limits< float >::quiet_NaN();

	PlayingSample(Sample const &sample_, float volume_, float pan_, bool loop_)
		: data(sample_.data), loop(loop_), volume(volume_), pan(pan_) { }
	PlayingSample(Sample const &sample_, float volume_, glm::vec3 const &position_, float half_volume_radius_, bool loop_)
		: data(sample_.data), loop(loop_), volume(volume_), position(position_), half_volume_radius(half_volume_radius_) { }
};

// ------- global functions -------

void init(); //call Sound::init() from main.cpp before using any member functions

void shutdown(); //call Sound::shutdown() from main.cpp to gracefully(-ish) exit

//Call 'Sound::play' to play a sample once.
//  if you hang on to the return value, you can change the panning, volume, or stop playback early.
std::shared_ptr< PlayingSample > play(
	Sample const &sample,
	float volume = 1.0f,
	float pan = 0.0f //-1.0f == hard left, 1.0f == hard right
);
//The play_3D version will play a sample in '3D' mode (that is, panning determined by listener position):
std::shared_ptr< PlayingSample > play_3D(
	Sample const &sample,
	float volume,
	glm::vec3 const &position,
	float half_volume_radius = std::numeric_limits< float >::infinity()
);

//Call 'Sound::loop' to play a sample ~forever~.
//  if you hang on to the return value, you can change the panning, volume, or stop playback.
std::shared_ptr< PlayingSample > loop(
	Sample const &sample,
	float volume = 1.0f,
	float pan = 0.0f //-1.0f == hard left, 1.0f == hard right
);
//The loop_3D version will loop a sample in '3D' mode (that is, panning determined by listener position):
std::shared_ptr< PlayingSample > loop_3D(
	Sample const &sample,
	float volume,
	glm::vec3 const &position,
	float half_volume_radius = std::numeric_limits< float >::infinity()
);

//Listener controls the panning of "3D" samples (ones played using the "position" version of the play functions):
struct Listener {
	void set_position(glm::vec3 const &new_position, float ramp = 1.0f / 60.0f);
	void set_right(glm::vec3 const &new_right, float ramp = 1.0f / 60.0f);

	//internals:
	Ramp< glm::vec3 > position = Ramp< glm::vec3 >(0.0f); //listener's location
	Ramp< glm::vec3 > right = Ramp< glm::vec3 >(1.0f, 0.0f, 0.0f); //unit vector pointing to listener's right
};
extern struct Listener listener;

//"panic button" to shut off all currently playing sounds:
void stop_all_samples();

//set global volume:
void set_volume(float new_volume, float ramp = 1.0f / 60.0f);
extern Ramp< float > volume;

//the audio callback doesn't run between Sound::lock() and Sound::unlock()
// the set_*/stop/play/... functions already use these helpers, so you shouldn't need
// to call them unless your code is modifying values directly:
void lock();
void unlock();

} //namespace Sound
