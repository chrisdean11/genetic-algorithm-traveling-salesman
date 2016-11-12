/*
Chris Dean
November 1, 2016
CS 541
Lab 2

My algorithm utilizes this breeding/culling strategy:
Keep the best route unchanged.
Perform mutations on the rest of the top 50% of ranked routes.
Randomize the lower half ranked routes (basically throw them away and replace)

I decided against crossover because I don't think it's justified given the nature of the 
problem: order is all that matters. If you look at the sample problem given, crossover usually
completely messes up the order such that neither segment resembles its parent.

*/

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <cstring>

#define 	MAX_CITIES 	25 	// Maximum/default number of cities allowed
#define 	GRID_X 			200 // Grid width
#define 	GRID_Y 			200 // Grid height
#define		SIZE 	 			100 // Total number of routes in the population

using namespace std;

// This map holds all the info about the city layout
struct Map{
	int cityNum; 														// Number of cities
	int start;															// Starting city number
	int xCoord[MAX_CITIES]; 								// x-coordinate of each city
	int yCoord[MAX_CITIES]; 								// y-coordinate of each city
	int distances[MAX_CITIES][MAX_CITIES]; 	// LUT holding the distance between any two cities
};

// Helper Functions
bool checkCoordinates(Map map, int x, int y, int city);
void printCities(Map map);

int main(int argc, char* argv[]){
	Map map; 						// Struct containing all of the map info
	int response = 0;		// Controls repetitions in the main loop
	srand(time(NULL));

	// Greet user, explain usage
	cout << "\n\n\nWelcome to my Genetic Algorithm for the Traveling Salesman problem!\n";
	cout << "\nUSAGE: ./a.out <textfile>";
	cout << "  (If you did not supply a textfile argument, a randomized list of 25 cities will ";
	cout << "be created for you.)\n\n\n";
	cout << "First this program will build the Map and display an overview of the City locations, and then begin finding a solution.\n";
	cout << "Press enter to begin\n";
	cin.ignore();

	/*
	 *
	 * (1) SETUP: Initialize the city map and original population
	 *
	 */

	// If there's a second argument, check if it's a valid file and get city coordinates
	if(argc >= 2){
		// Read the file into Map. Error message and quit if it's not in a valid format.
		char * f = argv[1]; 				// Filename containing city info
		char ch;
		int index, x, y;
		ifstream fs;
		fs.open(f);
		fs >> map.cityNum;

		// Loop through the list of cities
		for(int i = 0; i < map.cityNum; i++){
			fs >> index >> x >>  y;
			map.xCoord[index-1] = x;
			map.yCoord[index-1] = y;
		}
		fs >> map.start;
		fs.close();
	}

	// Otherwise, create a randomized set of city coordinates
	else{
		int x;
		int y;
		map.cityNum = MAX_CITIES;
		for(int i = 0; i < map.cityNum; i++){
			do{
				x = rand()%GRID_X;
				y = rand()%GRID_Y;
			}while(checkCoordinates(map, x, y, i));
			map.start = 1;
			map.xCoord[i] = x;
			map.yCoord[i] = y;
		}
	}

	// Build the LUT for all the distances between each pair of cities
	for( int i = 0; i < map.cityNum; i++){
		for(int j = 0; j < map.cityNum; j++) map.distances[i][j] = abs(map.xCoord[i] - map.xCoord[j]) + abs(map.yCoord[i] - map.yCoord[j]);
	}

	// Print out the details of the map
	printCities(map);

	// Initialize a population of randomized routes
	// It was simpler to pass 1D arrays to random_shuffle() so this randomizes a dummy array called randomOrder.
	int route[SIZE][map.cityNum];
	for(int i = 0; i < SIZE; i++){
		int randomOrder[map.cityNum];
		for(int j = 0; j < map.cityNum; j++) randomOrder[j] = j + 1;
		// Need to put the start city in the first index. So we must make a switch if it's not already in the first position.
		if(randomOrder[0] != map.start) {
			int buf = randomOrder[0];
			randomOrder[0] = map.start;
			int i = 1;
			while(randomOrder[i] != map.start) i++;
			randomOrder[i] = buf;
		}
		random_shuffle(&randomOrder[1], &randomOrder[map.cityNum]);
		for(int j = 0; j < map.cityNum; j++) route[i][j] = randomOrder[j];
		
		// Print out all of the routes (Debug only)
		//for(int j = 0; j < map.cityNum; j++) cout<<route[i][j]<<" ";
		//cout<<endl;
	}

	/*
	 *
	 * (2) MAIN LOOP: Fitness Eval->Elitism/Mutation/Randomize->Report Best Route->Repeat
	 *
	 */

	do{
		int fitness[SIZE];
		int best = 20000;			// Fitness value of best route
		int bestindex;				// Location of best route in population
		int median;						// The median value in the population

		// Obtain the fitness value for each route
		for(int i = 0; i < SIZE; i++){
			fitness[i] = 0;
			for(int j = 0; j < map.cityNum; j++) {
				int cityOne = route[i][j];
				int cityTwo = route[i][(j+1)%(map.cityNum)];
				fitness[i] += map.distances[cityOne-1][cityTwo-1];
			}
			//fitness[i] += map.distances[start-1][cityTwo-1]
			//cout << "Fitness " << i << ": " << fitness[i]<<endl;
			if(fitness[i] < best){ best = fitness[i]; bestindex = i;}
		}
		
		// Get the median value
		int buf[SIZE];
		copy(&fitness[0], &fitness[SIZE], buf);
		sort(&buf[0], &buf[SIZE]);
		median = buf[SIZE/2];

		// Depending on each route's fitness value, either Mutate, Randomize, or Keep it.
		for(int i = 0; i < SIZE; i++){
			// Best Route - Leave it alone
			if(fitness[i] == best) ; 
			// Mutate - switch two cities in the route, but make sure not to move the first city
			else if(fitness[i] < median){
				int cityOne = rand()%(map.cityNum-1) + 1;
				int cityTwo = rand()%(map.cityNum-1) + 1; 
				int buf = route[i][cityOne];
				route[i][cityOne] = route[i][cityTwo];
				route[i][cityTwo] = buf;
			}
			// Random - this route is poor. Generate a new random one
			else {
				int random[map.cityNum];
				for(int j = 0; j < map.cityNum; j++) random[j] = route[i][j];
				random_shuffle(&random[1], &random[map.cityNum]);
				for(int j = 1; j < map.cityNum; j++) route[i][j] = random[j];
			}
		}

		// Print info and check if the user wants to repeat
		if(response <= 0){
				// Print out all of the routes (Debug only)
				//for(int j = 0; j < map.cityNum; j++) cout<<route[i][j]<<" ";
				//cout<<endl;
				//cout <<"\nBest Route Index: " << bestindex; 
			cout << "\nBest route found: ";
			for(int j = 0; j < map.cityNum; j++) cout << route[bestindex][j] << " ";
		  cout << route[bestindex][0] << " ";
			cout << "\nTotal Distance of this route: " << best;
			cout << "\n\nHow many generations would you like to move forward? (0 exits program): ";
			cin >> response;
			if(response > 10000) {cout<<"\n That's too many! Let's do 10000.\n"; response = 10000;}
		}
		response--;
	}while(response >= 0);

	return 0;
}

// Checks if there's a city already at these coordinates. Assumes the "city" number you put in
// is the max index so far.
// Returns true if the spot is taken, false if not.
bool checkCoordinates(Map map, int x, int y, int city){
	for(int i = 0; i < city; i++){
		if( (map.xCoord[i] == x) && (map.yCoord[i] == y) ) return true;
	}

	return false;
}

// Prints out the distance matrix
void printCities(Map map){
	
	// Print out the list of cities
	cout << "\nList of City Locations:\n";
	for( int i = 0; i < map.cityNum; i++){
		cout << "City " << i + 1 << ": (" << map.xCoord[i] << ", " << map.yCoord[i] << ")\n"; 
	}

	cout << "Start at City " << map.start << endl;

	// Print out the distance matrix
	cout << "\n\nTable of Distances Between Cities:\n  ";
	for(int i = 0; i < map.cityNum; i++) {if(i < 9) cout << " "; cout << "  " << i + 1;}
	cout << endl;
	for( int i = 0; i < map.cityNum; i++){
		if(i < 9) cout << " ";
		cout << i + 1;
		for(int j = 0; j < map.cityNum; j++){
			cout << " ";
			int d = map.distances[i][j];
			if( d < 100) cout << " ";
			if( d < 10 ) cout << " ";
			cout << d;
		}
		cout << endl;
	}

}