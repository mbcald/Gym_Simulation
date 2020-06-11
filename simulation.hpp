// Madison Caldwell: University of Michigan Class of 2020
// Purpose: Simulate one day at the IMSB under the proposed UM COVID gym plan by predicting the behavior of each gym user.

#include <deque>
#include <random>
#include <cmath>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;

struct User {
    // DOUBLE: amount of time spent in gym
    double gymTime = 0;
    // DOUBLE: amount of time spent waiting
    double waitTime = 0;
    // DOUBLE: amount of time willing to wait in line before leaving
    double waitLimit = 0;
};

class Gym {
public:
    Gym() {
        // DOUBLE: current rate of influx of students
        double rate = 0;
        // INT: index of the rates vector
        int rateIndex = -1;
        // DOUBLE: takes into account fractions of Users that will officially enter the gym during the next minute
        double userPercentage = 0;
        // DOUBLE: the amount of time the person at the front of the line has been waiting
        double longestWait = 0;

        // FOR LOOP: loops through each minute of the day
        for (int i = 0; i < MINUTES_OPEN; i++) {
            // IF: Every 60 mintues change the rate to the next index of the rates vector
            if (i % 60 == 0) {
                rateIndex++;
                rate = rates[rateIndex];
            }

            // Updates the logest wait variable
            // IF: make longestWait = 0 if the line is empty, otherwise set it equal to the wait time of the person at the front of the queue
            if (wait.empty()) {
                longestWait = 0;
            }
            else {
                longestWait = wait.front().waitTime;
            }
            avgMin.push_back(longestWait);

            // FOR: update gymTime for all members in gym, if their time is less than zero, remove them from gym
            for (size_t u = 0; u < gym.size(); u++) {
                gym[u].gymTime--;
                if (gym[u].gymTime <= 0) {
                    gym.erase(gym.begin() + u, gym.begin() + u + 1);
                }
            }

            // FOR: increment wait times for every User in the line
                // If their wait limit is less than 0, remove them from the line
            for (size_t u = 0; u < wait.size(); u++) {
                wait[u].waitTime++;
                wait[u].waitLimit--;
                if (wait[u].waitLimit <= 0) {
                    wait.erase(wait.begin() + u, wait.begin() + u + 1);
                    numLeft++;
                }
            }

            // add Users to the gym based on the influx rate
            userPercentage += rate;

            // WHILE: create new users if necessary, accounting for fractions of users
            while (userPercentage >= 1) {
                User temp;
                temp.gymTime = dGYM(gen);
                temp.waitLimit = dWAIT(gen);
                wait.push_back(temp);
                userPercentage -= 1;
            }

            // WHILE: take users off of the queue to fill the gym to capacity or empty the line
            while (gym.size() < MAX_CAPACITY && !wait.empty()) {
                gym.push_back(wait.front());
                avgWait *= numUsers;
                avgWait += wait.front().waitTime;
                numUsers++;
                avgWait /= numUsers;
                wait.pop_front();
            }

            inLine.push_back((double)wait.size());
            numPeople.push_back(gym.size());

            cout << "Minute " << i << ": \n\tWait Time: " << longestWait << "\n\tLine Size: " << wait.size()
            << "\n\tPeople in Gym: " << gym.size() << "\n";
        }
        cout << "Total average wait time: " << avgWait << "\nNumber of people" <<
        " who left the line: " << numLeft << "\n";
    }

    int getAvgWait() {
        return avgWait;
    }

    size_t getQueueSize() {
        return wait.size();
    }

    vector<double>& getAvgMin() {
        return avgMin;
    }

    vector<double>& getInLine() {
        return inLine;
    }

    vector<double>& getNumPeople() {
        return numPeople;
    }

    int getNumLeft() {
        return numLeft;
    }

    int getNumUsers() {
        return numUsers;
    }

private:
    // QUEUE: contains Users who are waiting in line
    deque<User> wait;
    // VECTOR: contains Users who are inside of the gym
    vector<User> gym;
    // INT: maximum capacity of the gym
    static const int MAX_CAPACITY = 100;
    // INT: number of minutes the gym is open (17 hours)
    static const int MINUTES_OPEN = 1020;
    // DOUBLE: average wait time over the course of the day
    double avgWait = 0;
    // VECTOR: contains the instantaneous wait time for each minute of the day
    vector<double> avgMin;
    // VECTOR: contains the number of people in line for each minute of the day
    vector<double> inLine;
    // VECTOR: contains the number of people in the gym for each minute of the day
    vector<double> numPeople;
    // INT: the total number of people who left the line because the wait was too long
    int numLeft = 0;
    // INT: the total number of people who used the gym throughout the day
    int numUsers = 0;

    // normal distribution random number generator to decide how long someone stays in the gym
    random_device rd{};
    mt19937 gen{rd()};
    // NORMAL DISTRIBUTION: normal distribution that represents time spent in gym
    normal_distribution<> dGYM{60,20};
    // NORMAL DISTRIBUTION: normal distribution that represents time spent in line before leaving
    normal_distribution<> dWAIT{30,15};

    // VECTOR: rate of influx of students for each hour of the day (6AM to 11PM)
    const vector<double> rates = {1.376, 1.216, 1.392, 1.408, 1.509, 1.533,
        1.393, 1.395, 1.676, 2.078, 3.004, 3.405, 2.880, 2.394, 2.116,
        1.774, 0.488};
};
