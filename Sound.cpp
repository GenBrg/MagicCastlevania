#include "Sound.hpp"
#include "load_wav.hpp"
#include "load_opus.hpp"

#include <SDL.h>

#include <list>
#include <cassert>
#include <exception>
#include <iostream>
#include <algorithm>

//local (to this file) data used by the audio system:
namespace {

	//handy constants:
	constexpr uint32_t const AUDIO_RATE = 48000; //sampling rate
	constexpr uint32_t const MIX_SAMPLES = 1024; //number of samples to mix per call of mix_audio callback; n.b. SDL requires this to be a power of two

	//The audio device:
	SDL_AudioDeviceID device = 0;

	//list of all currently playing samples:
	std::list< std::shared_ptr< Sound::PlayingSample > > playing_samples;

}

//public-facing data:

//global volume control:
Sound::Ramp< float > Sound::volume = Sound::Ramp< float >(1.0f);

//global listener information:
Sound::Listener Sound::listener;

//This audio-mixing callback is defined below:
void mix_audio(void *, Uint8 *buffer_, int len);

//------------------------ public-facing --------------------------------

Sound::Sample::Sample(std::string const &filename) {
	if (filename.size() >= 4 && filename.substr(filename.size()-4) == ".wav") {
		load_wav(filename, &data);
	} else if (filename.size() >= 5 && filename.substr(filename.size()-5) == ".opus") {
		load_opus(filename, &data);
	} else {
		throw std::runtime_error("Sample '" + filename + "' doesn't end in either \".png\" or \".opus\" -- unsure how to load.");
	}
}

Sound::Sample::Sample(std::vector< float > const &data_) : data(data_) {
}



void Sound::init() {
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		std::cerr << "Failed to initialize SDL audio subsytem:\n" << SDL_GetError() << std::endl;
		std::cerr << "  (Will continue without audio.)\n" << std::endl;
		return;
	}

	//Based on the example on https://wiki.libsdl.org/SDL_OpenAudioDevice
	SDL_AudioSpec want, have;
	SDL_zero(want);
	want.freq = AUDIO_RATE;
	want.format = AUDIO_F32SYS;
	want.channels = 2;
	want.samples = MIX_SAMPLES;
	want.callback = mix_audio;

	device = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
	if (device == 0) {
		std::cerr << "Failed to open audio device:\n" << SDL_GetError() << std::endl;
		std::cerr << "  (Will continue without audio.)\n" << std::endl;
	} else {
		//start audio playback:
		SDL_PauseAudioDevice(device, 0);
		std::cout << "Audio output initialized." << std::endl;
	}
}


void Sound::shutdown() {
	if (device != 0) {
		//stop audio playback:
		SDL_PauseAudioDevice(device, 1);
		SDL_CloseAudioDevice(device);
		device = 0;
	}
}


void Sound::lock() {
	if (device) SDL_LockAudioDevice(device);
}

void Sound::unlock() {
	if (device) SDL_UnlockAudioDevice(device);
}

std::shared_ptr< Sound::PlayingSample > Sound::play(Sample const &sample, float volume, float pan) {
	std::shared_ptr< Sound::PlayingSample > playing_sample = std::make_shared< Sound::PlayingSample >(sample, volume, pan, false);
	lock();
	playing_samples.emplace_back(playing_sample);
	unlock();
	return playing_sample;
}

std::shared_ptr< Sound::PlayingSample > Sound::play_3D(Sample const &sample, float volume, glm::vec3 const &position, float half_volume_radius) {
	std::shared_ptr< Sound::PlayingSample > playing_sample = std::make_shared< Sound::PlayingSample >(sample, volume, position, half_volume_radius, false);
	lock();
	playing_samples.emplace_back(playing_sample);
	unlock();
	return playing_sample;
}

std::shared_ptr< Sound::PlayingSample > Sound::loop(Sample const &sample, float volume, float pan) {
	std::shared_ptr< Sound::PlayingSample > playing_sample = std::make_shared< Sound::PlayingSample >(sample, volume, pan, true);
	lock();
	playing_samples.emplace_back(playing_sample);
	unlock();
	return playing_sample;
}



std::shared_ptr< Sound::PlayingSample > Sound::loop_3D(Sample const &sample, float volume, glm::vec3 const &position, float half_volume_radius) {
	std::shared_ptr< Sound::PlayingSample > playing_sample = std::make_shared< Sound::PlayingSample >(sample, volume, position, half_volume_radius, true);
	lock();
	playing_samples.emplace_back(playing_sample);
	unlock();
	return playing_sample;
}


void Sound::stop_all_samples() {
	lock();
	for (auto &s : playing_samples) {
		s->stop();
	}
	unlock();
}

void Sound::set_volume(float new_volume, float ramp) {
	lock();
	volume.set(new_volume, ramp);
	unlock();
}

//------------------

void Sound::PlayingSample::set_volume(float new_volume, float ramp) {
	Sound::lock();
	if (!stopping) {
		volume.set(new_volume, ramp);
	}
	Sound::unlock();
}

void Sound::PlayingSample::set_pan(float new_pan, float ramp) {
	if (!(pan.value == pan.value)) return; //ignore if not in '2D' mode
	Sound::lock();
	pan.set(new_pan, ramp);
	Sound::unlock();
}

void Sound::PlayingSample::set_position(glm::vec3 const &new_position, float ramp) {
	if (pan.value == pan.value) return; //ignore if not in '3D' mode
	Sound::lock();
	position.set(new_position, ramp);
	Sound::unlock();
}

void Sound::PlayingSample::set_half_volume_radius(float new_radius, float ramp) {
	if (pan.value == pan.value) return; //ignore if not in '3D' mode
	Sound::lock();
	half_volume_radius.set(new_radius, ramp);
	Sound::unlock();
}

void Sound::PlayingSample::stop(float ramp) {
	Sound::lock();
	if (!(stopping || stopped)) {
		stopping = true;
		volume.target = 0.0f;
		volume.ramp = ramp;
	} else {
		volume.ramp = std::min(volume.ramp, ramp);
	}
	Sound::unlock();
}

//------------------

void Sound::Listener::set_position_right(glm::vec3 const &new_position, glm::vec3 const &new_right, float ramp) {
	Sound::lock();
	position.set(new_position, ramp);
	//some extra code to make sure right is always a unit vector:
	if (new_right == glm::vec3(0.0f)) {
		right.set(glm::vec3(1.0f, 0.0f, 0.0f), ramp);
	} else {
		right.set(glm::normalize(new_right), ramp);
	}
	Sound::unlock();
}

//------------------------ internals --------------------------------


//helper: equal-power panning
inline void compute_pan_weights(float pan, float *left, float *right) {
	//clamp pan to -1 to 1 range:
	pan = std::max(-1.0f, std::min(1.0f, pan));

	//want left^2 + right^2 = 1.0, so use angles:
	float ang = 0.5f * 3.1415926f * (0.5f * (pan + 1.0f));
	*left = std::cos(ang);
	*right = std::sin(ang);
}

//helper: 3D audio panning
void compute_pan_from_listener_and_position(
	glm::vec3 const &listener_position,
	glm::vec3 const &listener_right,
	glm::vec3 const &source_position,
	float source_half_radius,
	float *left, float *right
	) {
	glm::vec3 to = source_position - listener_position;
	float distance = glm::length(to);
	//start by panning based on direction.
	//note that for a LR fade to sound uniform, sound power (squared magnitude) should remain constant.
	if (distance == 0.0f) {
		*left = *right = std::sqrt(2.0f);
	} else {
		//amt ranges from -1 (most left) to 1 (most right):
		float amt = glm::dot(listener_right, to) / distance;
		//turn into an angle from 0.0f (most left) to pi/2 (most right):
		float ang = 0.5f * 3.1415926f * (0.5f * (amt + 1.0f));
		*left = std::cos(ang);
		*right = std::sin(ang);

		//squared distance attenuation is realistic if there are no walls,
		// but I'm going to use linear because it's sounds better to me.
		// (feel free to change it, of course)
		//want att = 0.5f at distance == half_volume_radius
		float att = 1.0f / (1.0f + (distance / source_half_radius));
		*left *= att;
		*right *= att;
	}
}

//helper: ramp updates...
constexpr float const RAMP_STEP = float(MIX_SAMPLES) / float(AUDIO_RATE);

//helper: ...for single values:
void step_value_ramp(Sound::Ramp< float > &ramp) {
	if (ramp.ramp < RAMP_STEP) {
		ramp.value = ramp.target;
		ramp.ramp = 0.0f;
	} else {
		ramp.value += (RAMP_STEP / ramp.ramp) * (ramp.target - ramp.value);
		ramp.ramp -= RAMP_STEP;
	}
}

//helper: ...for 3D positions:
void step_position_ramp(Sound::Ramp< glm::vec3 > &ramp) {
	if (ramp.ramp < RAMP_STEP) {
		ramp.value = ramp.target;
		ramp.ramp = 0.0f;
	} else {
		ramp.value = glm::mix(ramp.value, ramp.target, RAMP_STEP / ramp.ramp);
		ramp.ramp -= RAMP_STEP;
	}
}

//helper: ...for 3D directions:
void step_direction_ramp(Sound::Ramp< glm::vec3 > &ramp) {
	if (ramp.ramp < RAMP_STEP) {
		ramp.value = ramp.target;
		ramp.ramp = 0.0f;
	} else {
		//find normal to the plane containing value and target:
		glm::vec3 norm = glm::cross(ramp.value, ramp.target);
		if (norm == glm::vec3(0.0f)) {
			if (ramp.target.x <= ramp.target.y && ramp.target.x <= ramp.target.z) {
				norm = glm::vec3(1.0f, 0.0f, 0.0f);
			} else if (ramp.target.y <= ramp.target.z) {
				norm = glm::vec3(0.0f, 1.0f, 0.0f);
			} else {
				norm = glm::vec3(0.0f, 0.0f, 1.0f);
			}
			norm -= ramp.target * glm::dot(ramp.target, norm);
		}
		norm = glm::normalize(norm);
		//find perpendicular to target in this plane:
		glm::vec3 perp = glm::cross(norm, ramp.target);

		//find angle from target to value:
		float angle = std::acos(glm::clamp(glm::dot(ramp.value, ramp.target), -1.0f, 1.0f));

		//figure out new target value by moving angle toward target:
		angle *= (ramp.ramp - RAMP_STEP) / ramp.ramp;

		ramp.value = ramp.target * std::cos(angle) + perp * std::sin(angle);
		ramp.ramp -= RAMP_STEP;
	}
}


//The audio callback -- invoked by SDL when it needs more sound to play:
void mix_audio(void *, Uint8 *buffer_, int len) {
	assert(buffer_); //should always have some audio buffer

	struct LR {
		float l;
		float r;
	};
	static_assert(sizeof(LR) == 8, "Sample is packed");
	assert(len == MIX_SAMPLES * sizeof(LR)); //should always have the expected number of samples
	LR *buffer = reinterpret_cast< LR * >(buffer_);

	//zero the output buffer:
	for (uint32_t s = 0; s < MIX_SAMPLES; ++s) {
		buffer[s].l = 0.0f;
		buffer[s].r = 0.0f;
	}

	//update global values:
	float start_volume = Sound::volume.value;
	glm::vec3 start_position =  Sound::listener.position.value;
	glm::vec3 start_right =  Sound::listener.right.value;

	step_value_ramp(Sound::volume);
	step_position_ramp( Sound::listener.position);
	step_direction_ramp( Sound::listener.right);

	float end_volume = Sound::volume.value;
	glm::vec3 end_position =  Sound::listener.position.value;
	glm::vec3 end_right =  Sound::listener.right.value;

	//add audio from each playing sample into the buffer:
	for (auto si = playing_samples.begin(); si != playing_samples.end(); /* later */) {
		Sound::PlayingSample &playing_sample = **si; //much more convenient than writing ** everywhere.

		//Figure out sample panning/volume at start...
		LR start_pan;
		if (!(playing_sample.pan.value == playing_sample.pan.value)) {
			//3D panning
			compute_pan_from_listener_and_position(
				start_position, start_right,
				playing_sample.position.value,
				playing_sample.half_volume_radius.value,
				&start_pan.l, &start_pan.r);

			step_position_ramp(playing_sample.position);
			step_value_ramp(playing_sample.half_volume_radius);
		} else {
			//2D panning
			compute_pan_weights(playing_sample.pan.value, &start_pan.l, &start_pan.r);

			step_value_ramp(playing_sample.pan);
		}
		start_pan.l *= start_volume * playing_sample.volume.value;
		start_pan.r *= start_volume * playing_sample.volume.value;

		step_value_ramp(playing_sample.volume);

		//..and end of the mix period:
		LR end_pan;
		if (!(playing_sample.pan.value == playing_sample.pan.value)) {
			//3D panning
			compute_pan_from_listener_and_position(
				end_position, end_right,
				playing_sample.position.value,
				playing_sample.half_volume_radius.value,
				&end_pan.l, &end_pan.r);
		} else {
			//2D panning
			compute_pan_weights(playing_sample.pan.value, &end_pan.l, &end_pan.r);
		}

		end_pan.l *= end_volume * playing_sample.volume.value;
		end_pan.r *= end_volume * playing_sample.volume.value;

		//figure out a step to add at each sample so that pan will move smoothly from start to end:
		LR pan = start_pan;
		LR pan_step;
		pan_step.l = (end_pan.l - start_pan.l) / MIX_SAMPLES;
		pan_step.r = (end_pan.r - start_pan.r) / MIX_SAMPLES;

		assert(playing_sample.i < playing_sample.data.size());

		for (uint32_t i = 0; i < MIX_SAMPLES; ++i) {
			//mix one sample based on current pan values:
			buffer[i].l += pan.l * playing_sample.data[playing_sample.i];
			buffer[i].r += pan.r * playing_sample.data[playing_sample.i];

			//update position in sample:
			playing_sample.i += 1;
			if (playing_sample.i == playing_sample.data.size()) {
				if (playing_sample.loop) {
					playing_sample.i = 0;
				} else {
					break;
				}
			}

			//update pan values:
			pan.l += pan_step.l;
			pan.r += pan_step.r;
		}

		if (playing_sample.i >= playing_sample.data.size()
		 || (playing_sample.stopping && playing_sample.volume.value == 0.0f)) { //sample has finished
		 	playing_sample.stopped = true;
			//erase from list:
			auto old = si;
			++si;
			playing_samples.erase(old);
		} else {
			++si;
		}
	}

	/*//DEBUG: report output power:
	float max_power = 0.0f;
	for (uint32_t s = 0; s < MIX_SAMPLES; ++s) {
		max_power = std::max(max_power, (buffer[s].l * buffer[s].l + buffer[s].r * buffer[s].r));
	}
	std::cout << "Max Power: " << std::sqrt(max_power) << "; playing samples: " << playing_samples.size() << std::endl; //DEBUG
	*/

}


