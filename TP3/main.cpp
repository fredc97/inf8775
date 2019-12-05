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

int main(int argc, char *argv[])
{
  srand (time(NULL));
  char *fn = nullptr;
  bool show_p = false;
  Game game; 
  Population pop; 


  for (int i = 1; i < argc; i++)
  {

    if (!strcmp(argv[i], "-e"))
    {
      i++;
      fn = argv[i];
      game = initialize(fn);
      pop = generateInitialPop(game);
      
    }
    else if (!strcmp(argv[i], "-p"))
    {
      show_p = true;
    }
  }
    resolve(pop,game, show_p);
    
  return 0;
}
