#pragma once
#include <vector>
#include <string>
using namespace std;

struct Game
{
  int n_cards;
  int n_packs;
  vector<int> cards;
  int synergy[1000][1000];
};

Game initialize(std::string file);
