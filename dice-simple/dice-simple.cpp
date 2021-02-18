
#include <stdlib.h>  
#include <time.h>  
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
using namespace std;

class Dice {

private:
	int SIDES;
public:
	Dice(int sides) :SIDES(sides)
	{
	}

	int roll()
	{
		srand(GetTickCount());
		return (rand() % ((SIDES + 1) - 1) + 1);
	}
};



class Player {

protected:
	int playerID;
	int playerTurn;
	unsigned long int score;
	bool inGame;

public:
	Player(int id) {
		playerID = id;
		playerTurn = id;
		score = 0;
		inGame = true;
	}

	virtual int  updateScore(int x) = 0;
	virtual bool getPenalty() = 0;
	virtual void setPenalty(bool penalty) = 0;
	virtual void setSkipRoll(bool skip) = 0;
	virtual void setLastRoll(int rand) = 0;
	virtual int getLastRoll() = 0;
	virtual bool metTarget(int target) = 0;

	int getID() {
		return playerID;
	}

	int  getTurn() {
		return playerTurn;
	}

	void setTurn(int turn) {
		playerTurn = turn;
	}

	int getScore() {
		return score;
	}

	bool getInGame() { return inGame; };
	void setInGame(bool status) { inGame = status; };
};

class SimplePlayer : public Player {
private:

	int lastRoll;
	bool skipRoll;

public:
	SimplePlayer(int id) :Player(id) {
		lastRoll = 0;
		skipRoll = false;
	}

	bool getPenalty() { return skipRoll; };
	void setPenalty(bool penalty) { skipRoll = penalty; };
	int getLastRoll() { return lastRoll; }
	void setLastRoll(int rand) { lastRoll = rand; }
	void setSkipRoll(bool skip) { skipRoll = skip; }

	bool metTarget(int target) {
		if (getInGame() == true && (getScore() < target))
			return false;
		else
			return true;
	}


	int updateScore(int x) {
		if (LONG_MAX - score > x) {
			score += x;
			return score;
		}
		else {
			//overflow error
			cout << "overflow error\n";
			return -1;
		}
	};
};

class Game {

protected:
	Dice* dice;
	Player** gPlayers;
	Player* currentTurnPlayer;
	int nPlayers, nCurrentPlayers, status, targetScore;

public:

	Game(int numPlayers, int ptargetScore, Dice* dice, Player** players) {

		//set initial conditions
		nCurrentPlayers = numPlayers;
		nPlayers = numPlayers;
		targetScore = ptargetScore;
		this->dice = dice;
		gPlayers = players;
		randomizePlayerTurns();
		currentTurnPlayer = getPlayer(1);
	}

	~Game() {

		for (int i = 0; i < nPlayers; i++) {
			delete gPlayers[i];
		}
		delete gPlayers;
		delete dice;
	}

	bool allPlayersFinished() {
		return (nCurrentPlayers == 0);
	}

	Player* getPlayer(int turn) {
		for (int i = 0; i < nPlayers; i++) {
			if (gPlayers[i]->getTurn() == turn)
				return gPlayers[i];
		}
		return NULL;
	}


	Player** randomizePlayerTurns()
	{
		//seed
		srand(time(NULL));

		// Start from the last element and swap one by one.
		for (int i = nPlayers - 1; i > 0; i--)
		{
			// Pick a random index from 0 to i 
			int j = rand() % (i + 1);

			// Swap gPlayers[i] with the element at random index 
			int temp = gPlayers[i]->getTurn();
			gPlayers[i]->setTurn(gPlayers[j]->getTurn());
			gPlayers[j]->setTurn(temp);

		}
		return gPlayers;
	}



	void showAllScores()
	{
		for (int turn = 0; turn < nPlayers; turn++) {
			cout << "\tturn-" << turn + 1 << " playerID-" << getPlayer(turn + 1)->getID() << " score:" << getPlayer(turn + 1)->getScore() << "\n";
		}

	}

	//rules of game
	virtual void play() = 0;

};

class SimpleGame : public Game {

public:
	SimpleGame(int nPlayers, int playerTargetScore, Dice* dice, Player** players) :
		Game(nPlayers, playerTargetScore, dice, players) {};

	void setEvents(Player* player, int rand, int& turn) {
		switch (rand) {
		case 6: player->setLastRoll(rand); --turn;
			break;
		case 1: if (player->getLastRoll() == 1) {
			player->setSkipRoll(true);
		};
		default: player->setLastRoll(rand);
		}
	}

	void play()
	{
		showAllScores();

		while (!allPlayersFinished()) {
			for (int turn = 0; turn < nPlayers; turn++)
			{
				currentTurnPlayer = getPlayer(turn + 1);
				int rand = 0;
				if (currentTurnPlayer->metTarget(targetScore) == false) {
					if (currentTurnPlayer->getPenalty() == false) {
						cout << "PlayerID-" << currentTurnPlayer->getID() << ", its your turn(press r to roll the dice)";
						while (_getch() != 'r');
						cout << " rolling dice";
						rand = dice->roll();
						cout << "roll->" << rand << "\n";
						currentTurnPlayer->updateScore(rand);
						showAllScores();
						this->setEvents(currentTurnPlayer, rand, turn);
					}
					else {
						//penalized, so skip play, clear penalty.
						currentTurnPlayer->setPenalty(false);
						rand = 0;
					}
				}
				else {
					//quit the game
					if (currentTurnPlayer->getInGame() == true) {
						nCurrentPlayers--;
						currentTurnPlayer->setInGame(false);
					}
					rand = 0;
				}
			}
		}

	}




};



int main(int argc, int argv[])
{
	int nPlayers, playerTargetScore, sides;
	cout << "Enter number of players->";
	cin >> nPlayers;

	cout << "Enter Target score->";
	cin >> playerTargetScore;

	if ((nPlayers <= 0) || (playerTargetScore <= 0)) {
		cout << "incorrect inputs!\n";
		return -1;
	}

	sides = 6;

	Player** players;
	players = new Player * [nPlayers];

	for (int i = 0; i < nPlayers; i++) {
		players[i] = new SimplePlayer(i + 1);
	}

	cout << "hello";

	(new SimpleGame(nPlayers, playerTargetScore, new Dice(sides), players))->play();

}

