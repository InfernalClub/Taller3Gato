#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <mutex>
#include <thread>

using namespace std;

struct Player {
    int index;
    bool WINNER;

    // Default constructor
    Player() : index(0), WINNER(false) {}

    Player(int playerIndex) : index(playerIndex), WINNER(false) {}
};

class Tournament {
private:
    vector<Player> players;
    bool gameOver;
    mutex mutex;
    int data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int size = sizeof(data) / sizeof(data[0]);

public:
    Tournament(int playerCount) : players(playerCount), gameOver(false) {
        for (int i = 0; i < playerCount; ++i) {
            players[i] = Player(i + 1); // Start indexing from 1
            cout << "Player " << players[i].index << " information\n";
        }
    }

    int overlyComplicatedCheck(int results[3][3]) {
        // Check rows, columns, and diagonals
        int sum;

        // Check rows
        for (int row = 0; row < 3; ++row) {
            sum = results[row][0] + results[row][1] + results[row][2];
            if (sum == 3 || sum == -3) {
                return (sum == 3) ? 1 : 2;
            }
        }

        // Check columns
        for (int col = 0; col < 3; ++col) {
            sum = results[0][col] + results[1][col] + results[2][col];
            if (sum == 3 || sum == -3) {
                return (sum == 3) ? 1 : 2;
            }
        }

        // Check main diagonal
        sum = results[0][0] + results[1][1] + results[2][2];
        if (sum == 3 || sum == -3) {
            return (sum == 3) ? 1 : 2;
        }

        // Check anti-diagonal
        sum = results[0][2] + results[1][1] + results[2][0];
        if (sum == 3 || sum == -3) {
            return (sum == 3) ? 1 : 2;
        }

        return 0; // If no winner found, return 0
    }

    void runGame(int matchId) {
        const int initialPlayerCount = players.size();

        // Run matches until there is only one player left
        while (players.size() > 1) {
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(data, data + size, default_random_engine(seed));

            int results[3][3];
            for (int i = 0; i < size; ++i) {
                int row = i / 3;
                int col = i % 3;
                results[row][col] = (data[i] % 2 == 0) ? -1 : 1;
            }

            cout << "Results of Match " << matchId << ":\n";
            for (int row = 0; row < 3; ++row) {
                for (int col = 0; col < 3; ++col) {
                    cout << results[row][col] << " ";
                }
                cout << "\n";
            }

            int winnerIndex = overlyComplicatedCheck(results);
            if (winnerIndex != 0) {
                lock_guard<std::mutex> lock(mutex);
                Player& winner = players[winnerIndex - 1];
                winner.WINNER = true;
                cout << "Player " << winner.index << " is a winner!\n";

                players.erase(remove_if(players.begin(), players.end(),
                                        [](const Player& p) { return !p.WINNER; }), players.end());
            } else {
                cout << "No winner yet. Running the match again.\n";
            }
        }

        // Proceed to the next round if there is more than one player left
        if (initialPlayerCount > 1) {
            cout << "Proceeding to the next round with " << initialPlayerCount / 2 << " players.\n";
        }
    }

    void playRound(int gamesPerThread) {
        int roundId = 1;

        while (players.size() > 1) {
            vector<thread> threads;

            int remainingPlayers = players.size();
            int matchesPerThread = min(gamesPerThread, remainingPlayers / 2);

            cout << "--- Round " << roundId << ", Matches: " << remainingPlayers / 2 << " ---\n";

            for (int i = 0; i < remainingPlayers; i += 2) {
                int matchId = i / 2 + 1;
                threads.emplace_back(&Tournament::runGame, this, matchId);
            }

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }
            threads.clear();

            // Update remaining players for the next round
            remainingPlayers /= 2;
            matchesPerThread = min(gamesPerThread, remainingPlayers / 2);

            // Increment round index
            ++roundId;
        }
    }

    void runTournament() {
        int gamesPerThread = 2;
        playRound(gamesPerThread);

        // Declare overall winner when there's only one player left
        if (!players.empty()) {
            cout << "Player " << players[0].index << " is the overall winner!\n";
        }
    }
};

int main() {
    const int playerCount = 8;
    Tournament tournament(playerCount);
    tournament.runTournament();

    return 0;
}
