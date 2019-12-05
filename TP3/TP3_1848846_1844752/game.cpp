
#include "game.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

Game initialize(string fn)
{
    Game game;
    std::ifstream file;
    file.open(fn);
    
    if (!file)
    {
        cerr << "Error with file";

    }

    file >> game.n_cards;
    file >> game.n_packs;
    int cards = 0;
    while (cards < (game.n_cards * game.n_packs))
    {
        int cardTemp = 0;
        file >> cardTemp;
        game.cards.push_back(cardTemp);
        cards++;
    }

   // cout << "Cards: \n";
  //  cout << game.cards.size() << "\n";
    // for (int s = 0; s < game.cards.size(); s++)
    // {
    //   cout << game.cards.at(s);
    //   cout << "\n";
    // }

    // Synergy
    int space = game.n_cards * game.n_packs;
    //int synergy[space][space];
    for (int i = 0; i < space; i++)
    {
        for (int j = 0; j < space; j++)
        {
            int cardTemp = 0;
            file >> cardTemp;
            game.synergy[i][j] = cardTemp;
        }

    
    }
    //game.synergy = synergy;
    return game;
}
