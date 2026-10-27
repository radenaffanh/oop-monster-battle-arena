#include <bits/stdc++.h>
using namespace std;

class Monster {
private:
    string name;
    int health;
    int attackPower;
    int resourcePoints;
    int dotDamage; //damage over time (dot)
    int dotTurns;
    int stunTurns;
    
protected:
    int reflectDamage;
    int damageReductionPercent;
    int RPreduction;
    int getAttackPower() const { return attackPower; }

public:
    Monster(string n, int hp, int atk){
        name = n;
        health = hp;
        attackPower = atk;
        resourcePoints = 0;
        dotDamage = 0;
        dotTurns = 0;
        stunTurns = 0;
        damageReductionPercent = 0;
        reflectDamage = 0;
     }
        

    virtual ~Monster() {}

    string getName() const { return name; }
    int getHP() const { return health; }
    int getRP() const { return resourcePoints; }
    bool isAlive() const { return health > 0; }

    void regenRP(int amount) { resourcePoints += amount; }

    void useRP(int amount) {
        if (resourcePoints < amount)
            throw runtime_error("RP not enough");
        resourcePoints -= amount;
    }

    void takeDamage(int dmg, Monster* attacker = nullptr) {
        if (damageReductionPercent > 0) {
            dmg = dmg * (100 - damageReductionPercent) / 100;
            cout << name << " reduces damage by " << damageReductionPercent << "%!\n";
        }
        
        health -= dmg;
        if (health < 0) health = 0;
        
        
        if (reflectDamage > 0 && attacker != nullptr) {
            cout << name << " reflects " << reflectDamage << " damage back to " << attacker->getName() << "!\n";
            attacker->takeDamage(reflectDamage);
        }

        if (RPreduction > 0 && attacker != nullptr) {
            cout << name << " drains " << RPreduction << " RP from " << attacker->getName() << "!\n";
            attacker->useRP(min(RPreduction, attacker->getRP()));
        }

        //Reset efek setelah giliran
        damageReductionPercent = 0;
        reflectDamage = 0;
        RPreduction = 0;
    }

    void heal(int amount) {
        health += amount;
    }

    void applyDOT(int dmg, int turns) {
        dotDamage = dmg;
        dotTurns = turns;
    }

    void clearDOT() {
        dotDamage = 0;
        dotTurns = 0;
    }

    void applyStun(int turns) {
    stunTurns = turns;
    }

    bool isStunned() const {
        return stunTurns > 0;
    }

    void processStun() {
        if (stunTurns > 0) {
            cout << name << " is stunned and loses a turn!\n";
            stunTurns--;
        }
    }

    void processDOT() {
        if (dotTurns > 0) {
            cout << name << " takes " << dotDamage << " DOT damage\n";
            takeDamage(dotDamage);
            dotTurns--;
        }
    }

    // Overloading
    int attack() const { return attackPower; }
    int attack(int bonus) const { return attackPower + bonus; }

    // Interface abstrak
    virtual void basicAttack(Monster& target) = 0;
    virtual void defend() = 0;
    virtual void ultimate(Monster& target) = 0;
};

class FireMonster : public Monster {
public:
    FireMonster() : Monster("FireMonster", 90, 25) {}

    void basicAttack(Monster& target) override {
        target.takeDamage(attack(), this);
        regenRP(10);
    }

    void defend() override {
        useRP(8);
        cout << getName() << " uses defenses: Flame Shield! (reduce next attack damage by 50%)" << endl;   
        damageReductionPercent = 50;
        reflectDamage = 5;
        regenRP(10);
    }

    void ultimate(Monster& target) override {
        useRP(30);
        cout << getName() << " uses Firestorm\n";
        target.takeDamage(getAttackPower() * 4);
        target.applyDOT(5, 2);
        regenRP(10);
    }
};

class IceMonster : public Monster {
public:
    IceMonster() : Monster("IceMonster", 130, 12) {}

    void basicAttack(Monster& target) override {
        target.takeDamage(attack(), this);
        regenRP(8);
    }

   void defend() override {
        useRP(12);
        cout << getName() << " uses defenses: Ice Barrier! (block all damage this turn)" << endl;
        damageReductionPercent = 100;
        heal(10);
        regenRP(8);

    }

    void ultimate(Monster& target) override {
        useRP(20);
        cout << getName() << " uses Ultimate: Absolute Zero!\n";
        target.takeDamage(getAttackPower() * 2); //damage sedang
        clearDOT();                              //menghapus efek dot
        heal(15);                         //healing sedang  
        regenRP(10);                               
    }
};

class EarthMonster : public Monster {
public:
    EarthMonster() : Monster("EarthMonster", 100, 18) {}

    void basicAttack(Monster& target) override {
        target.takeDamage(attack(), this);
        regenRP(15);
    }

    void defend() override {
        useRP(10);
        cout << getName() << " uses Rock Solid\n";
        damageReductionPercent = 30;
        RPreduction = 5;
        regenRP(15);

    }

    void ultimate(Monster& target) override {
        useRP(25);
        cout << getName() << " uses Tectonic Shift\n";
        target.takeDamage(getAttackPower() * 2);
        target.applyStun(1);
        regenRP(15); 
    }
};

void showStatus(Monster* player1, Monster* player2) {
    cout << "\n=== STATUS ===\n";
    cout << player1->getName() << " | HP: " << player1->getHP() << " | RP: " << player1->getRP() << endl;
    cout << player2->getName() << " | HP: " << player2->getHP() << " | RP: " << player2->getRP() << endl;
}

Monster* pilihMonster(int player) {
    int choice;

    while (true) {
        try {
            cout << "\nPlayer " << player << " choose a monster:\n";
            cout << "1. FireMonster\n";
            cout << "2. IceMonster\n";
            cout << "3. EarthMonster\n";
            cout << "Which monster do you choose? ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Input must be a number.");
            }

            if (choice < 1 || choice > 3) {
                throw out_of_range("Invalid monster choice.");
            }

            switch (choice) {
                case 1: return new FireMonster();
                case 2: return new IceMonster();
                case 3: return new EarthMonster();
            }
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please choose again.\n";
        }
    }
}


void playerTurn(Monster* current, Monster* target, int playerNum) {
    int choice;
    int action;

    cout << "\n--- Player " << playerNum << "'s Turn ---\n";

    current->processDOT();
    current->processStun();
    
    if (current->isStunned()) {
        return; // Lewati giliran jika stun
    }
    target->processDOT();

    
    cout << current->getName()
         << " | HP: " << current->getHP()
         << " | RP: " << current->getRP() << endl;

    cout << target->getName()
         << " | HP: " << target->getHP()
         << " | RP: " << target->getRP() << endl;



    cout << "\nChoose Action:\n";
    cout << "1. Basic Attack\n";
    cout << "2. Defense\n";
    cout << "3. Ultimate\n";
    cout << ">> ";
    cin >> choice;

    try {
        switch (choice) {
            case 1:
                current->basicAttack(*target);
                break;
            case 2:
                current->defend();
                break;
            case 3:
                current->ultimate(*target);
                break;
            default:
                cout << "Not valid.\n";
        }
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
        cout << "Please choose again.\n";
    }
}

void aiTurn(Monster* ai, Monster* player) {
    cout << "\nAI turn (" << ai->getName() << ")\n";

    ai->processDOT();
    ai->processStun();

    if (ai->isStunned()) {
        return;
    }

    player->processDOT();

    try {
        if (ai->getHP() <= 30 && ai->getRP() >= 8) {
            cout << "AI chooses DEFENSE\n";
            ai->defend();
        }
        else if (ai->getRP() >= 25) {
            cout << "AI chooses ULTIMATE\n";
            ai->ultimate(*player);
        }
        else {
            cout << "AI chooses BASIC ATTACK\n";
            ai->basicAttack(*player);
        }
    }
    catch (...) {
        ai->basicAttack(*player);
    }
}



int main() {
    int mode;

    cout << "=== MONSTER BATTLE ARENA ===\n";

    cout << "Instruksi Bermain:\n";
    cout << "1. Setiap monster memiliki HP, Attack, dan RP\n";
    cout << "2. RP digunakan untuk skill Defend dan Ultimate\n";
    cout << "3. Basic Attack tidak membutuhkan RP\n";
    cout << "4. Setiap giliran monster dapat:\n";
    cout << "   - Basic Attack\n";
    cout << "   - Defend (jika RP cukup)\n";
    cout << "   - Ultimate (jika RP cukup)\n";
    cout << "5. Monster akan meregenerasi RP setiap giliran\n";
    cout << "=====================================\n\n";

    while (true) {
        try {
            cout << "Choose Mode:\n";
            cout << "1. Player vs Player (PvP)\n";
            cout << "2. Player vs AI\n";
            cout << ">> ";

            if (!(cin >> mode)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Input must be a number.");
            }

            if (mode != 1 && mode != 2) {
                throw out_of_range("Invalid game mode.");
            }

            break; // keluar loop jika valid
        }
        catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cout << "Please choose again    .\n\n";
        }
    }

    switch (mode) {

    // PvP
    case 1: {
        Monster* p1 = pilihMonster(1);
        Monster* p2 = pilihMonster(2);

        bool turnP1 = true;

        while (p1->isAlive() && p2->isAlive()) {
            if (turnP1)
                playerTurn(p1, p2, 1);
            else
                playerTurn(p2, p1, 2);

            turnP1 = !turnP1;
        }

        cout << "\n=== GAME OVER ===\n";
        cout << (p1->isAlive() ? "PLAYER 1 MENANG\n" : "PLAYER 2 MENANG\n");

        delete p1;
        delete p2;
        break;
    }

    // VS AI
    case 2: {
        Monster* player = pilihMonster(1);
        Monster* ai = new FireMonster(); // AI default

        while (player->isAlive() && ai->isAlive()) {
            playerTurn(player, ai, 1);
            if (ai->isAlive())
                aiTurn(ai, player);
        }

        cout << "\n=== GAME OVER ===\n";
        cout << (player->isAlive() ? "KAMU MENANG\n" : "KAMU KALAH\n");

        delete player;
        delete ai;
        break;
    }
    }

    return 0;
}
