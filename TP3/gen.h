#include <vector>
#include <string>
#include <tuple>
#include "game.h"
using namespace std;


const int POPULATION_SIZE = 25;
const int TOURNAMENT_ROUND = 2;
const int SURVIVANTS = 15;
const int MUTATIONS = 10; // 10 good
const int MAX_SEARCH_MUTATES = 50;
const int WAVE_VIRUS = 7000;
const int APOCALYPS = 50000;
const double PROPORTION_VIRUS = 0.1;
const int EQUALITY_MAX = 2;

/*
const int POPULATION_SIZE = 25;
const int TOURNAMENT_ROUND = 1;
const int SURVIVANTS = 20;
const int MUTATIONS = 15;
const int MAX_SEARCH_MUTATES = 60;
const int WAVE_VIRUS = 2000;
const double PROPORTION_VIRUS = 0.1;
const int EQUALITY_MAX = 2;
*/
struct Pack
{
  vector<int> cards; 
  vector<int> synergy;
  int totalValue;
};


struct Individual
{
  vector<Pack> packs; 
};


struct Population {
	vector<Individual> individuals;
};




Population generateInitialPop(Game &game);
Individual generateInitialInd(Game &game);
void evolvePopulation(Population &population, Game &game);
int fitnessIndividual(Individual &individual, Game &game);
int scorePack(const Pack &pack, Game &game);
int tournament(int ind1, int ind2, Game &game, Population &population);
void selection(Population &population, Game &game, Individual &winner1, Individual &winner2);
void sortIndividualByScore(Individual &individual, Game &game);
Individual crossover(Individual &individual1, Individual &individual2, Game &game);
Population selectBestOnes(Population &population, int n_best, Game &game);
void resolve(Population &population, Game &game);
Population mutation(Population &pop, Game &game);
Individual mutationIndividual(Individual &ind, Game &game);
Population virus(Population &pop, Game &game, int proportion);
void affichage(Population &pop, Game &game);
Population annihilation(Population &pop, Game &game);