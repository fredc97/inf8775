#include "gen.h"
#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <tuple>
#include <map>
using namespace std;

Individual generateInitialInd(Game &game)
{

    std::random_device random_device;
    std::mt19937 random_engine(random_device());
    std::uniform_int_distribution<int> distribution_1_100(0, game.cards.size() - 1);
    Individual individual;
    vector<Pack> packs;
    vector<int> global;
    for (int j = 0; j < game.n_packs; j++)
    {
        Pack pack;
        for (int i = 0; i < game.n_cards; i++)
        {
            int randomNumber = distribution_1_100(random_engine);

            if (std::find(global.begin(), global.end(), randomNumber) == global.end())
            {
                pack.cards.push_back(randomNumber);
                pack.synergy.push_back(randomNumber);
                global.push_back(randomNumber);
            }
            else
            {
                i--;
                continue;
            }
        }

        int value = scorePack(pack, game);
        pack.totalValue = value;

        packs.push_back(pack);
    }

    individual.packs = packs;
    return individual;
}

int scorePack(const Pack &pack, Game &game)
{
    int value = 0;
    for (int t = 0; t < pack.cards.size(); t++)
    {
        value += game.cards.at(pack.cards.at(t));
        for (int y = t; y < pack.cards.size(); y++)
        {
            value += game.synergy[pack.synergy.at(t)][pack.synergy.at(y)];
        }
    }
    return value;
}

int fitnessIndividual(Individual &ind, Game &game)
{
    int min_score = scorePack(ind.packs.at(0), game);
    for (int i = 1; i < ind.packs.size(); i++)
    {
        int new_score = scorePack(ind.packs.at(i), game);
        if (new_score < min_score)
        {
            min_score = new_score;
        }
    }

    return min_score;
}

Population generateInitialPop(Game &game)
{
    Population pop;
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        // emplace_back
        pop.individuals.push_back(generateInitialInd(game));
    }
    return pop;
}

std::ostream &operator<<(std::ostream &str, vector<Pack> &p)
{
    for (int i = 0; i < p.size(); i++)
    {
        Pack pack = p.at(i);
        for (int j = 0; j < pack.cards.size(); j++)
        {
            str << to_string(pack.synergy.at(j)) + " ";
        }
        str << "\n";
    }
    return str;
}

void evolvePopulation(Population &population, Game &game, bool printPack)
{
    Population pop;
    pop = population;
    Individual ind1, ind2, newInd;
    int count = 0;
    int bestSolution = 0;
    int iterationWithSameSolution = 0;
    int nbVirusPop = 1;
    int apocalyps = 0;
    while (true)
    {
        if (apocalyps > APOCALYPS)
        {
            pop = annihilation(pop, game);
            iterationWithSameSolution = 0;
            apocalyps = 0;
        }
        if (iterationWithSameSolution > WAVE_VIRUS)
        {

            pop = virus(pop, game, nbVirusPop);
            iterationWithSameSolution = 0;

            if (nbVirusPop != 5)
            {
                nbVirusPop++;
            }
            else
            {
                nbVirusPop = 1;
            }
        }

        pop = mutation(pop, game);

        selection(pop, game, ind1, ind2);

        newInd = crossover(ind1, ind2, game);

        pop.individuals.push_back(newInd);
        pop.individuals.push_back(ind1);
        pop.individuals.push_back(ind2);

        pop = selectBestOnes(pop, game.n_packs, game);


        int actualSol = fitnessIndividual(pop.individuals.at(0), game);
        if (actualSol > bestSolution)
        {
            bestSolution = actualSol;
            iterationWithSameSolution = 0;
            if (printPack)
            {
                cout << pop.individuals.at(0).packs<<flush;
            }
            else
            {
                cout << bestSolution << "\n";
            }
            nbVirusPop = 1;
        }
        else
        {
            iterationWithSameSolution++;
        }

        apocalyps++;
    }
}


void resolve(Population &population, Game &game, bool printPack)
{
    evolvePopulation(population, game, printPack);
}

class compareFitnessIndividual
{
public:
    compareFitnessIndividual(Game &gameGen) : game(gameGen)
    {
    }
    Game &game;
    bool operator()(Individual &a, Individual &b)
    {
        int scoreA = fitnessIndividual(a, game);
        int scoreB = fitnessIndividual(b, game);
        return (scoreA > scoreB);
    }
};

Population selectBestOnes(Population &population, int n_best, Game &game)
{
    Population newPop;
    int count = 0;
    int survivant = 0;

    sort(population.individuals.begin(), population.individuals.end(), compareFitnessIndividual(game));

    while (survivant < SURVIVANTS)
    {
        newPop.individuals.push_back(population.individuals.at(survivant));
        survivant++;
    }
    sort(newPop.individuals.begin(), newPop.individuals.end(), compareFitnessIndividual(game));

    return newPop;
}

Population virus(Population &pop, Game &game, int prop)
{
    Population popTemp = pop;

    int victims = popTemp.individuals.size() * (PROPORTION_VIRUS * prop);
    for (int v = 0; v < victims; v++)
    {
        int victim = rand() % popTemp.individuals.size();
        popTemp.individuals.erase(popTemp.individuals.begin() + victim);
    }

    //  Generate news individuals
    for (int n = 0; n < victims; n++)
    {
        popTemp.individuals.push_back(generateInitialInd(game));
    }

    return popTemp;
}

Population annihilation(Population &pop, Game &game)
{
    Population popTemp = pop;

    int victims = popTemp.individuals.size();
    for (int v = 0; v < victims; v++)
    {
        popTemp.individuals.erase(popTemp.individuals.begin() + v);
    }

    //  Generate news individuals
    for (int n = 0; n < victims; n++)
    {
        popTemp.individuals.push_back(generateInitialInd(game));
    }

    return popTemp;
}

Population mutation(Population &pop, Game &game)
{
    Population popTemp = pop;
    sort(popTemp.individuals.begin(), popTemp.individuals.end(), compareFitnessIndividual(game));
    Individual best = popTemp.individuals.at(0);
    popTemp.individuals.erase(popTemp.individuals.begin());

    int mutations = 0;
    vector<int> mutated;
    while (mutations < MUTATIONS)
    {
        int ind = rand() % popTemp.individuals.size();
        mutated.push_back(ind);

        Individual newInd;
        newInd = mutationIndividual(popTemp.individuals.at(ind), game);
        popTemp.individuals.at(ind) = newInd;
        mutations++;
    }
    popTemp.individuals.push_back(best);
    return popTemp;
}

Individual mutationIndividual(Individual &ind, Game &game)
{
    int pack_1, pack_2;
    do
    {
        pack_1 = rand() % ind.packs.size();
        pack_2 = rand() % ind.packs.size();
    } while (pack_1 == pack_2);

    Pack pack1 = ind.packs.at(pack_1);
    Pack pack2 = ind.packs.at(pack_2);
    int scorePack1, scorePack2;
    int scoreBeforePack1 = scorePack(pack1, game);
    int scoreBeforePack2 = scorePack(pack2, game);
    int found = 0;
    int max_search = MAX_SEARCH_MUTATES;
    int search = 0;
    while (!found && search < max_search)
    {
        int cards = rand() % game.n_cards;
        vector<int> cardsToSwitch;
        for (int p = 0; p < cards; p++)
        {
            int card_pack1 = pack1.cards.at(p);
            int card_pack2 = pack2.cards.at(p);
            pack1.cards.at(p) = card_pack2;
            pack1.synergy.at(p) = card_pack2;
            pack2.cards.at(p) = card_pack1;
            pack2.synergy.at(p) = card_pack1;
        }

        scorePack1 = scorePack(pack1, game);
        scorePack2 = scorePack(pack2, game);
        if (scorePack1 >= scoreBeforePack1 && scorePack2 >= scoreBeforePack2)
        {
            found = 1;
        }

        search++;
    }

    if (search < max_search)
    {
        pack1.totalValue = scorePack1;
        pack2.totalValue = scorePack2;

        ind.packs.at(pack_1) = pack1;
        ind.packs.at(pack_2) = pack2;
    }

    return ind;
}

class compareScorePack
{
public:
    compareScorePack(Game &gameGen) : game(gameGen)
    {
    }
    Game &game;
    bool operator()(const Pack &a, const Pack &b)
    {
        int scoreA = scorePack(a, game);
        int scoreB = scorePack(b, game);
        return (scoreA > scoreB);
    }
};

void sortIndividualByScore(Individual &individual, Game &game)
{
    sort(individual.packs.begin(), individual.packs.end(), compareScorePack(game));
}

Individual crossover(Individual &individual1, Individual &individual2, Game &game)
{
    sortIndividualByScore(individual1, game);
    sortIndividualByScore(individual2, game);
    Individual newIndividual;

    int genesPreserved = rand() % individual1.packs.size();

    for (int i = 0; i < genesPreserved; i++)
    {

        if (individual1.packs.at(i).totalValue > individual2.packs.at(i).totalValue)
        {

            newIndividual.packs.push_back(individual1.packs.at(i));
        }
        else
        {
            newIndividual.packs.push_back(individual2.packs.at(i));
        }
    }

    // *****
    // ********
    // Verify the unicity of the cards in the chosen packs
    // ********
    // *****
    vector<int> cardsNewInd;
    for (int x = 0; x < newIndividual.packs.size(); x++)
    {
        Pack pack = newIndividual.packs.at(x);
        for (int c = 0; c < pack.cards.size(); c++)
        {
            int cardsToAdd = pack.cards.at(c);
            if (std::find(cardsNewInd.begin(), cardsNewInd.end(), cardsToAdd) == cardsNewInd.end())
            {
                cardsNewInd.push_back(cardsToAdd);
            }
            else
            {
                // If already exist, remplace it by random card...
                bool tryInsert = true;
                while (tryInsert)
                {
                    int random = rand() % game.cards.size();
                    if (std::find(cardsNewInd.begin(), cardsNewInd.end(), random) == cardsNewInd.end())
                    {
                        cardsNewInd.push_back(random);
                        tryInsert = false;
                        pack.cards.at(c) = random;
                        pack.synergy.at(c) = random;
                        int value = scorePack(pack, game);
                        pack.totalValue = value;
                    }
                }
            }
            newIndividual.packs.at(x) = pack;
        }
    }

    // Fill the Individual with other random pack
    int packToFill = game.n_packs - newIndividual.packs.size();
    while (packToFill != 0)
    {
        Pack pack;
        for (int p = 0; p < game.n_cards; p++)
        {
            int random = rand() % game.cards.size();
            if (std::find(cardsNewInd.begin(), cardsNewInd.end(), random) == cardsNewInd.end())
            {
                cardsNewInd.push_back(random);

                pack.cards.push_back(random);
                pack.synergy.push_back(random);
                int value = scorePack(pack, game);
                pack.totalValue = value;
            }
            else
            {
                p--;
            }
        }
        newIndividual.packs.push_back(pack);
        packToFill--;
    }

    return newIndividual;
}

void selection(Population &population, Game &game, Individual &winner_1, Individual &winner_2)
{
    // First selection
    int ind1 = rand() % population.individuals.size();
    int ind2 = rand() % population.individuals.size();

    int winner_index_1 = tournament(ind1, ind2, game, population);
    winner_1 = population.individuals.at(winner_index_1);

    population.individuals.erase(population.individuals.begin() + winner_index_1);

    ind1 = rand() % population.individuals.size();
    ind2 = rand() % population.individuals.size();
    int winner_index_2 = tournament(ind1, ind2, game, population);
    winner_2 = population.individuals.at(winner_index_2);
    population.individuals.erase(population.individuals.begin() + winner_index_2);
}

int tournament(int ind1, int ind2, Game &game, Population &population)
{
    int index_winner = -9999;
    int equality_max = EQUALITY_MAX;
    int equality_count = 0;
    for (int i = 0; i < TOURNAMENT_ROUND; i++)
    {
        if (ind1 == ind2)
        {
            ind1 = rand() % population.individuals.size();
            ind2 = rand() % population.individuals.size();
            i--;
            continue;
        }
        else
        {
            Individual individual_1 = population.individuals.at(ind1);
            Individual individual_2 = population.individuals.at(ind2);

            int scoreInd1 = fitnessIndividual(individual_1, game);
            int scoreInd2 = fitnessIndividual(individual_2, game);

            if (scoreInd1 > scoreInd2)
            {
                index_winner = ind1;
                ind2 = rand() % population.individuals.size();
            }
            else
            {
                if (scoreInd1 != scoreInd2)
                {
                    index_winner = ind2;
                    ind1 = rand() % population.individuals.size();
                }
                else
                {
                    // no winner, equality
                    if (equality_count < equality_max)
                    {
                        ind1 = rand() % population.individuals.size();
                        ind2 = rand() % population.individuals.size();
                        equality_count++;
                        i--;
                        continue;
                    }
                    else
                    {
                        bool coin = rand() % 2;
                        if (coin)
                        {
                            index_winner = ind1;
                        }
                        else
                        {
                            index_winner = ind2;
                        }
                    }
                }
            }
        }
    }

    return index_winner;
}
