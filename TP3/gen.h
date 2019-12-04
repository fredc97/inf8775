#include <vector>
#include <string>
#include <tuple>
#include "game.h"
using namespace std;



const int POPULATION_SIZE = 25;
const int TOURNAMENT_ROUND = 5;
const int SURVIVANTS = 20;
const int MUTATIONS = 10;
const int MAX_SEARCH_MUTATES = 50;
const int WAVE_VIRUS = 1000;
const int PROPORTION_VIRUS = 0.1;
const int EQUALITY_MAX = 2;
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