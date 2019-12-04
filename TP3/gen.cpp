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
#include <unistd.h>
using namespace std;

// vecteur des positions dans le fichier
// melange

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

            // cout<<i<<" - Index : "<<randomNumber<<" >> "<<cards.at(randomNumber)<<"\n";
            if (std::find(global.begin(), global.end(), randomNumber) == global.end())
            {
                pack.cards.push_back(randomNumber);
                pack.synergy.push_back(randomNumber);
                global.push_back(randomNumber);
            }
            else
            {
                //cout<<"Already exist "<<i<<"\n";
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

void evolvePopulation(Population &population, Game &game)
{
    Population pop;
    pop = population;
    Individual ind1, ind2, newInd;
    int count = 0;
    int bestSolution = 0;
    int iterationWithSameSolution = 0;
    int nbVirusPop = 1;
    while (true)
    {
         if (iterationWithSameSolution > WAVE_VIRUS)
        {
            //cout<<"Virus - "<<pop.individuals.size()<<"\n";
            pop = virus(pop, game, nbVirusPop);
            iterationWithSameSolution = 0;
            if (nbVirusPop != 10) {
            nbVirusPop++; 
            }
        }
        //cout<<"Next"<<count<<" \n";
       // cout<<"On mutate - "<<pop.individuals.size()<<"\n";
        pop = mutation(pop, game);
        // cout<<"Sélection des meilleurs - "<<pop.individuals.size()<<"\n";
        selection(pop, game, ind1, ind2);
        //cout<<"Crossover ! - "<<pop.individuals.size()<<"\n";
        newInd = crossover(ind1, ind2, game);
        //cout<<"On push le new individu - "<<pop.individuals.size()<<"\n";
        pop.individuals.push_back(newInd);
        pop.individuals.push_back(ind1);
        pop.individuals.push_back(ind2);

        //cout<<"Survivants - "<<pop.individuals.size()<<"\n";
        pop = selectBestOnes(pop, game.n_packs, game);

        int actualSol = fitnessIndividual(pop.individuals.at(0), game);
        if (actualSol > bestSolution)
        {
            bestSolution = actualSol;
            iterationWithSameSolution = 0;
            //cout << pop.individuals.at(0).packs;
            cout << bestSolution << "\n";
            nbVirusPop = 1;
        }
        else
        {
            iterationWithSameSolution++;
        }
        //cout << pop.individuals.at(0).packs << "\n";
        //cout << bestSolution << "\n";
        //count++;

        //usleep(200000);
    }
}

void resolve(Population &population, Game &game)
{
    cout << "Start evolution \n";

    evolvePopulation(population, game);
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

    //cout<<"On sort les survivants! \n";
    sort(population.individuals.begin(), population.individuals.end(), compareFitnessIndividual(game));

    //cout<<"On prend les 50 meilleurs que l'on gardera \n";
    while (survivant < SURVIVANTS)
    {
        //cout<<"HI!!!"<<" survivant = "<<survivant<<" AND pop size : "<<population.individuals.size()<<"\n";
        //cout<<fitnessIndividual(population.individuals.at(survivant), game)<<"\n";
        //if (fitnessIndividual(population.individuals.at(count), game) > 0) {
        //       cout<<"HI!!!"<<" Count = "<<count<<" AND pop size : "<<population.individuals.size()<<"\n";

        newPop.individuals.push_back(population.individuals.at(survivant));
        survivant++;
        //        }
    }
    //cout<<fitnessIndividual(newPop.individuals.at(0), game)<<" avant \n";
    sort(newPop.individuals.begin(), newPop.individuals.end(), compareFitnessIndividual(game));
    //cout<<fitnessIndividual(newPop.individuals.at(0), game)<<"après \n";

    return newPop;
}

Population virus(Population &pop, Game &game, int prop)
{
    Population popTemp = pop;
    sort(popTemp.individuals.begin(), popTemp.individuals.end(), compareFitnessIndividual(game));
    Individual best = popTemp.individuals.at(0);
    popTemp.individuals.erase(popTemp.individuals.begin());

    int victims = popTemp.individuals.size() * (PROPORTION_VIRUS * prop);
    for (int v = 0; v < victims; v++)
    {
        int victim = rand() % popTemp.individuals.size();
        popTemp.individuals.erase(popTemp.individuals.begin() + victim);
    }
    //  Generate news individuals
    popTemp.individuals.push_back(best);
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
        //cout<<"New"<<"\n";
        newInd = mutationIndividual(popTemp.individuals.at(ind), game);
        popTemp.individuals.at(ind) = newInd;
        mutations++;

    }
    popTemp.individuals.push_back(best);
    return popTemp;
}

Individual mutationIndividual(Individual &ind, Game &game)
{
    /*
    int scoreByCards[game.n_packs];
    int scoreBySynergy[game.n_packs];

    for (int i = 0; i < ind.packs.size(); i++) {
        int scoreActualByCard;
        int scoreActualBySynergy;
        Pack pack;
        for (int j = 0; j < pack.cards.size(); j++) {
            scoreActualByCard += game.cards.at(pack.cards.at(j)); 
            for (int y = j; j < pack.cards.size(); y++) {
                scoreActualBySynergy += game.synergy[pack.synergy.at(j)][pack.synergy.at(y)];
            }
        }
        scoreByCards[i] = scoreActualByCard;
        scoreBySynergy[i] = scoreActualBySynergy;
    }
    
        for (int i = 0; i < ind.packs.size(); i++ ) {
              //  cout<<"AVANT - PACK "<<i<<" : "<<scorePack(ind.packs.at(i), game)<<"\n";
            }
*/
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
        //cout<<"ici : "<<search<<"\n";
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
            //cout<<"trouvé \n";

        }

        search++;
    }
     //   cout<<"on sort \n";

    if (search < max_search) {
        pack1.totalValue = scorePack1;
        pack2.totalValue = scorePack2;

        ind.packs.at(pack_1) = pack1;
        ind.packs.at(pack_2) = pack2;
    }
    /*for (int i = 0; i < ind.packs.size(); i++ ) {
      //  cout<<"APRÈS - PACK "<<i<<" : "<<scorePack(ind.packs.at(i), game)<<"\n";
        }*/
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
    //cout<<genesPreserved<<"\n";

    for (int i = 0; i < genesPreserved; i++)
    {
        //cout<<"1 : "<<individual1.packs.at(i).totalValue <<"\n";
        //cout<<"2 : "<<individual2.packs.at(i).totalValue <<"\n";

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
                //cout << cardsToAdd<<" already exist... \n";
                bool tryInsert = true;
                while (tryInsert)
                {
                    int random = rand() % game.cards.size();
                    //cout<<"No! Again..."<<random<<"\n";
                    if (std::find(cardsNewInd.begin(), cardsNewInd.end(), random) == cardsNewInd.end())
                    {
                        cardsNewInd.push_back(random);
                        tryInsert = false;
                        //  cout<<"Find! Ok bye! "<<random<< "\n";
                        pack.cards.at(c) = random;
                        pack.synergy.at(c) = random;
                        int value = scorePack(pack, game);
                        pack.totalValue = value;
                        //  cout<<"Should be random "<<pack.cards.at(c)<<"\n";
                    }
                }
            }
            newIndividual.packs.at(x) = pack;
        }

        //cardsNewInd.insert(cardsNewInd.end(), newIndividual.packs.at(x).cards.begin(), newIndividual.packs.at(x).cards.end());
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

    //cout<<newIndividual.packs.size()<<"\n";
    return newIndividual;
}

void selection(Population &population, Game &game, Individual &winner_1, Individual &winner_2)
{
    // First selection
    int ind1 = rand() % population.individuals.size();
    int ind2 = rand() % population.individuals.size();

    //Individual winner_1;
    //Individual winner_2;

    int winner_index_1 = tournament(ind1, ind2, game, population);
    winner_1 = population.individuals.at(winner_index_1);

    //Population popTemp = population;
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
        //cout<<"Round "<<i<<" : "<<ind1<<" VS "<<ind2<<"\n";
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
                // cout<<"Winner ! : "<<scoreInd1<<" -> "<< index_winner<<"\n";
                // cout<<"Loser ! : "<<scoreInd2<<" -> "<<ind2<<"\n";
                ind2 = rand() % population.individuals.size();
            }
            else
            {
                if (scoreInd1 != scoreInd2)
                {
                    index_winner = ind2;
                    ind1 = rand() % population.individuals.size();
                    // cout<<"Winner ! : "<<scoreInd2<<" -> "<< index_winner<<"\n";
                    //cout<<"Loser ! : "<<scoreInd1<<" -> "<<ind1<<"\n";
                }
                else
                {
                    // no winner, equality
                    if (equality_count < equality_max)
                    {
                        //cout<<"Equality, rematch with differents opponents"<<"\n";
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
