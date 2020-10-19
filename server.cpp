
#include "Connection.hpp"

#include "hex_dump.hpp"

#include <chrono>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <unordered_map>

int main(int argc, char **argv) {
#ifdef _WIN32
	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif

	//------------ argument parsing ------------

	if (argc != 2) {
		std::cerr << "Usage:\n\t./server <port>" << std::endl;
		return 1;
	}

	//------------ initialization ------------

	Server server(argv[1]);


	//------------ main loop ------------
	constexpr float ServerTick = 1.0f / 10.0f; //TODO: set a server tick that makes sense for your game

	//server state:

	//per-client state:
	struct PlayerInfo {
		PlayerInfo() {
			static uint32_t next_player_id = 1;
			name = "Player" + std::to_string(next_player_id);
			next_player_id += 1;
		}
		std::string name;

		uint32_t left_presses = 0;
		uint32_t right_presses = 0;
		uint32_t up_presses = 0;
		uint32_t down_presses = 0;

		int32_t total = 0;

	};
	std::unordered_map< Connection *, PlayerInfo > players;

	while (true) {
		static auto next_tick = std::chrono::steady_clock::now() + std::chrono::duration< double >(ServerTick);
		//process incoming data from clients until a tick has elapsed:
		while (true) {
			auto now = std::chrono::steady_clock::now();
			double remain = std::chrono::duration< double >(next_tick - now).count();
			if (remain < 0.0) {
				next_tick += std::chrono::duration< double >(ServerTick);
				break;
			}
			server.poll([&](Connection *c, Connection::Event evt){
				if (evt == Connection::OnOpen) {
					//client connected:

					//create some player info for them:
					players.emplace(c, PlayerInfo());


				} else if (evt == Connection::OnClose) {
					//client disconnected:

					//remove them from the players list:
					auto f = players.find(c);
					assert(f != players.end());
					players.erase(f);


				} else { assert(evt == Connection::OnRecv);
					//got data from client:
					std::cout << "got bytes:\n" << hex_dump(c->recv_buffer); std::cout.flush();

					//look up in players list:
					auto f = players.find(c);
					assert(f != players.end());
					PlayerInfo &player = f->second;

					//handle messages from client:
					//TODO: update for the sorts of messages your clients send
					while (c->recv_buffer.size() >= 5) {
						//expecting five-byte messages 'b' (left count) (right count) (down count) (up count)
						char type = c->recv_buffer[0];
						if (type != 'b') {
							std::cout << " message of non-'b' type received from client!" << std::endl;
							//shut down client connection:
							c->close();
							return;
						}
						uint8_t left_count = c->recv_buffer[1];
						uint8_t right_count = c->recv_buffer[2];
						uint8_t down_count = c->recv_buffer[3];
						uint8_t up_count = c->recv_buffer[4];

						player.left_presses += left_count;
						player.right_presses += right_count;
						player.down_presses += down_count;
						player.up_presses += up_count;

						c->recv_buffer.erase(c->recv_buffer.begin(), c->recv_buffer.begin() + 5);
					}
				}
			}, remain);
		}

		//update current game state
		//TODO: replace with *your* game state update
		std::string status_message = "";
		int32_t overall_sum = 0;
		for (auto &[c, player] : players) {
			for (; player.left_presses > 0; --player.left_presses) {
				player.total -= 1;
			}
			for (; player.right_presses > 0; --player.right_presses) {
				player.total += 1;
			}
			for (; player.down_presses > 0; --player.down_presses) {
				player.total -= 10;
			}
			for (; player.up_presses > 0; --player.up_presses) {
				player.total += 10;
			}
			if (status_message != "") status_message += " + ";
			status_message += std::to_string(player.total) + " (" + player.name + ")";

			overall_sum += player.total;
		}
		status_message += " = " + std::to_string(overall_sum);
		//std::cout << status_message << std::endl; //DEBUG

		//send updated game state to all clients
		//TODO: update for your game state
		for (auto &[c, player] : players) {
			//send an update starting with 'm', a 24-bit size, and a blob of text:
			c->send('m');
			c->send(uint8_t(status_message.size() >> 16));
			c->send(uint8_t((status_message.size() >> 8) % 256));
			c->send(uint8_t(status_message.size() % 256));
			c->send_buffer.insert(c->send_buffer.end(), status_message.begin(), status_message.end());
		}

	}


	return 0;

#ifdef _WIN32
	} catch (std::exception const &e) {
		std::cerr << "Unhandled exception:\n" << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception (unknown type)." << std::endl;
		throw;
	}
#endif
}
