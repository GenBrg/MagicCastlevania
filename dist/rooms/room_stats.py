import json

LEVEL_NUM = 3
MAX_ROOM_IDX = 18

MONSTER_DIR = "../monsters/"

if __name__ == "__main__":
	# Load monster informations
	monster_infos = {}
	with open(MONSTER_DIR + "monster.list", "r") as f:
		for line in f.readlines():
			if line[-1] == '\n':
				line = line[:-1]
			with open(MONSTER_DIR + line + ".json", 'r') as json_f:
				monster_infos[line] = json.load(json_f)

	# Load rooms
	for i in range(1, LEVEL_NUM + 1):
		monster_count = {}
		total_exp = 0
		total_coin = 0

		for j in range(3, MAX_ROOM_IDX + 1):
			room_file_name = "room%d-%d.json" % (i, j)
			with open(room_file_name, "r") as f:
				room = json.load(f)
				for monster in room["monsters"]:
					monster_info = monster_infos[monster["name"]]
					monster_count[monster["name"]] = monster_count.get(monster["name"], 0) + 1
					total_coin += monster_info["coin"]
					total_exp += monster_info["exp"]

		with open("boss_room%d.json" % i, "r") as f:
			boss = monster_infos[json.load(f)["monsters"][0]["name"]]
			total_coin += boss["coin"]
			total_exp += boss["exp"]

		print("Level %d:" % i)
		print(monster_count)
		print("total exp: %d" % total_exp)
		print("total coin: %d" % total_coin)