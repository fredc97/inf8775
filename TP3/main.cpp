#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <float.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <vector>
#include <iterator>
#include <algorithm>
#include "game.h"
#include "gen.h"
using namespace std;

/*
std::ostream &operator<<(std::ostream &str, vector<Pack> &p)
{
  for (int i = 0; i < p.size(); i++)
  {
    Pack pack = p.at(i);
    for (int j = 0; j < pack.cards.size(); j++) {
      str << to_string(pack.synergy.at(j)) + " ";
    }
    str<<"\n";
  }
  return str;
}
*/
struct Solution
{

};

int main(int argc, char *argv[])
{
  srand (time(NULL));
  char *fn = nullptr;
  bool show_p = false;
  bool show_t = false;
  bool show_c = false;
  Game game; 
  Solution resultat;
  double timeElapsed = 0;
  vector<Pack> gen;
  Population pop; 
  Individual t;
 // int size = 0;
 // int elem = 0;

  for (int i = 1; i < argc; i++)
  {

    if (!strcmp(argv[i], "-e"))
    {
      i++;
      fn = argv[i];
      game = initialize(fn);
      pop = generateInitialPop(game);
      //evolvePopulation(pop, game);
      resolve(pop,game);
      //Individual winner1, winner2; 
      //selection(pop, game, winner1, winner2);
       //t =  crossover(winner1, winner2, game);
     // gen = pop.paquets;
      
      
    }
    else if (!strcmp(argv[i], "-p"))
    {

      show_p = true;
    }
    else if (!strcmp(argv[i], "-c"))
    {

      show_c = true;
    }
    else if (!strcmp(argv[i], "-t"))
    {

      show_t = true;
    }
    else if (!strcmp(argv[i], "-a"))
    {
      i++;
      if (!strcmp(argv[i], "glouton"))
      {
         chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		    //resultat = glouton(roll);
        //
         chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        timeElapsed = (double)std::chrono::duration<double>(t2 - t1).count();
      }
      else
      {
        cout << "Erreur: Algo inconnu" << endl;

        return -1;
      }
    }
  }

  if (show_p)
  {
  // cout << t.packs;
  //cout << fitnessIndividual(t, game);


  }
  if (show_t)
  {
    cout <<timeElapsed * 1000 << endl;
  }
  if (show_c)
  {
    cout << 'c' << endl;
  }

  return 0;
}
