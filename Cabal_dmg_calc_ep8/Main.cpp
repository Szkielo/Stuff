#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <deque>


class CharacterStats {
public:
	double attack, 
		amp, 
		cd, 
		cr, 
		add_dmg;

	void displayStats() {
		std::cout << "Attack: " << attack << "\n"
			<< "Amp: " << amp << "\n"
			<< "CD: " << cd << "\n"
			<< "CR: " << cr << "\n"
			<< "Add DMG: " << add_dmg << "\n\n";
	}

	void loadStatsFromFile() {
		std::fstream file;
		file.open("CharacterStats.txt");
		
		std::string temp;
		std::getline(file, temp);
		attack = std::stod(temp);
		std::getline(file, temp);
		amp = std::stod(temp);
		std::getline(file, temp);
		cd = std::stod(temp);
		std::getline(file, temp);
		cr = std::stod(temp);
		std::getline(file, temp);
		add_dmg = std::stod(temp);

		file.close();
	}

	void saveStatsToFile() {
		std::fstream file;
		file.open("CharacterStats.txt");
		file << attack << "\n"
			<< amp << "\n"
			<< cd << "\n"
			<< cr << "\n"
			<< add_dmg << "\n";
		file.close();
	}
};

class AlterationInstance {
public:
	std::string instance_name;
	double attack = 0,
		add_attack = 0,
		amp = 0,
		skill_amp = 0,
		cd = 0,
		cr = 0,
		add_dmg = 0,

		enemy_def = 0;

	AlterationInstance(CharacterStats character, std::string name) {
		instance_name = name;
		attack += character.attack;
		amp += character.amp;
		cd += character.cd;
		cr += character.cr;
		add_dmg += character.add_dmg;

		double temp;
		std::cout << "\nPlease specify monster defense (4500 for Large Guardian): ";
		std::cin >> temp;
	}

	double calcNonCrit() {
		return ((attack + add_attack) * (amp + skill_amp + 1)/100 - enemy_def) + add_dmg;
	}
	double calcCrit() {
		return ((attack + add_attack) * (amp + skill_amp + 1)/100 - enemy_def) * (cd + 1)/100 + add_dmg;
	}
	double calcAvgHit() {
		return ((attack + add_attack) * (amp + skill_amp + 1)/100 - enemy_def) * (((cd + 1)/100 * cr/100) - (1 - cr)/100) + add_dmg;
	}
	
	void displayInstance() {
		std::cout << "\nName: " << instance_name << "\n\n"
			<< "Attack: " << attack << "\n"
			<< "Add attack: " << add_attack << "\n"
			<< "Amp: " << amp << "\n"
			<< "CD: " << cd << "\n"
			<< "CR: " << cr << "\n"
			<< "Add DMG: " << add_dmg << "\n\n"

			<< "Enemy defense: " << enemy_def << "\n\n"

			<< "Non-crit hit: " << calcNonCrit() << "\n"
			<< "Critical hit: " << calcCrit() << "\n\n"
			<< "Average damage: " << calcNonCrit() << "\n";
	}
};



int main()
{
	std::cout << "Calculator created by Mikele\n";
	CharacterStats player;
	std::cout << "\nEnter attack: ";
	std::cin >> player.attack;
	std::cout << "\nEnter amp: ";
	std::cin >> player.amp;
	std::cout << "\nEnter crit dmg: ";
	std::cin >> player.cd;
	std::cout << "\nEnter crit rate: ";
	std::cin >> player.cr;
	std::cout << "\nEnter add dmg: ";
	std::cin >> player.add_dmg;

	std::deque<AlterationInstance*> instance_list;

	int select_option;
	while (true) {
		system("cls");
		player.displayStats();

		for (int i = 0; i < instance_list.size(); i++) {
			instance_list[i]->displayInstance();
		}

		std::cout << "\n\nSelect one of the options: "
			<< "\n1. Create instance"
			<< "\n2. Edit instance"
			<< "\n3. Delete instance"
			<< "\n4. Edit character stats"
			<< "\n5. Exit\n";
		std::cin >> select_option;
		
		std::string temp_string;
		double temp_double;

		if (select_option == 1) {
			std::cout << "\nEnter instance name: ";
			std::cin >> temp_string;
			AlterationInstance* a = new AlterationInstance(player, temp_string);
			instance_list.push_back(a);
		}
		else if (select_option == 2) {
			std::cout << "\nEnter instance name: ";
			std::cin >> temp_string;
			for (int i = 0; i < instance_list.size(); i++) {
				if (instance_list[i]->instance_name == temp_string) {
					std::cout << "Instance found! You can now alter values by +x or -x, if you don't want to change them pass 0 in.";

					std::cout << "\nAlter attack by: ";
					std::cin >> temp_double;
					instance_list[i]->attack += temp_double;

					std::cout << "\nAlter add attack by: ";
					std::cin >> temp_double;
					instance_list[i]->add_attack += temp_double;

					std::cout << "\nAlter amp by: ";
					std::cin >> temp_double;
					instance_list[i]->amp += temp_double;

					std::cout << "\nAlter crit damage by: ";
					std::cin >> temp_double;
					instance_list[i]->cd += temp_double;

					std::cout << "\nAlter crit rate by: ";
					std::cin >> temp_double;
					instance_list[i]->cr += temp_double;

					std::cout << "\nAlter add dmg by: ";
					std::cin >> temp_double;
					instance_list[i]->add_dmg += temp_double;

					std::cout << "\nAlter monster defense by: ";
					std::cin >> temp_double;
					instance_list[i]->enemy_def += temp_double;
				}
			}
		}
		else if (select_option == 3) {
		}
		else if (select_option == 4) {
		}
		else if (select_option == 5) {
			for (int i = 0; i < instance_list.size(); i++) {
				delete instance_list[i];
			}
			return 0;
		}
	}

	
}