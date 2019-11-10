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
#include <climits>
#include <algorithm>
using namespace std;

struct Roll
{
  int sizeRoll;
  vector<int> sizeCuts;
  vector<int> priceCuts;
};

struct Solution
{
  vector<int> sizeCuts;
  int profit = 0;
};

struct Rentability
{
  int cutProfit;
  int cutSize;
  double ratio;
};

vector<Rentability> RemoveCut(vector<Rentability> r, int cutSize, int profit)
{
  r.erase(
      std::remove_if(r.begin(), r.end(), [&](Rentability const &r2) {
        return (r2.cutSize == cutSize && r2.cutProfit == profit);
      }),
      r.end());
  return r;
}

bool cmp(const Rentability &lhs, const Rentability &rhs)
{
  return lhs.ratio < rhs.ratio;
}

std::ostream &operator<<(std::ostream &str, const Solution &p)
{
  str << "[ ";
  for (int i = 0; i < p.sizeCuts.size(); i++)
  {
    str << to_string(p.sizeCuts.at(i)) + " ";
  }
  str << "]";
  return str;
}

/**
 * ----------------------------
 * ---------------------
 * Algorithme à programmation dynamqiue
 * ---------------------
 * ----------------------------
**/

Solution progDyn(Roll roll)
{
  
  vector<int> sizeCuts = roll.sizeCuts;
  vector<int> priceCuts = roll.priceCuts;
  Solution sol[roll.sizeRoll + 1];
  sol[0].profit = 0;

  for (int i = 1; i <= roll.sizeRoll; i++)
  {
    int max_profit = -1;
    vector<int> sizeCutsStep;
    for (int j = 0; j < i; j++)
    {
      int new_profit = priceCuts[j] + sol[i-j-1].profit;
      max_profit = max(max_profit, new_profit);

      if (max_profit == new_profit && j > 0) {
        sizeCutsStep.push_back(j);
      }     

    }
    sol[i].profit = max_profit;
    sol[i].sizeCuts = sizeCutsStep;
  }
  return sol[roll.sizeRoll];
}

/**
 * ----------------------------
 * ---------------------
 * Algorithme glouton
 * ---------------------
 * ----------------------------
**/

Solution glouton(Roll roll)
{
  Solution sol;
  int sum = 0;

  // Rentability vector
  vector<Rentability> rentabilities;
  for (int i = 0; i < roll.sizeCuts.size(); i++)
  {
    Rentability ri;
    ri.cutSize = roll.sizeCuts.at(i);
    ri.cutProfit = roll.priceCuts.at(i);
    ri.ratio = (double)ri.cutProfit / (double)ri.cutSize;
    rentabilities.push_back(ri);
  }

  // algorithme glouton
  while (rentabilities.size() > 0 && sum <= roll.sizeRoll)
  {
    Rentability r = *max_element(rentabilities.begin(), rentabilities.end(), cmp);
    //cout << r.ratio << " : " << r.cutSize << " : " << r.cutProfit << " : " << sol.profit << " \n";

    int sumTemp = sum;

    if (sumTemp + r.cutSize <= roll.sizeRoll)
    {
      sum += r.cutSize;
      sol.sizeCuts.push_back(r.cutSize);
      sol.profit += r.cutProfit;
    }
    else
    {
      rentabilities = RemoveCut(rentabilities, r.cutSize, r.cutProfit);
    }
  }

  return sol;
}

int main(int argc, char *argv[])
{
  char *fn = nullptr;
  bool show_p = false;
  bool show_t = false;

  Solution resultat;
  double timeElapsed = 0;
  int size = 0;
  int elem = 0;

  Roll roll;

  for (int i = 1; i < argc; i++)
  {

    if (!strcmp(argv[i], "-e"))
    {
      i++;
      fn = argv[i];
      std::ifstream file;
      file.open(fn);

      if (!file)
      {
        cerr << "Error with file";
      }

      file >> roll.sizeRoll;
      int totalCuts = 0;
      while (totalCuts < roll.sizeRoll)
      {
        int cutTemp = 0;
        file >> cutTemp;
        roll.sizeCuts.push_back(cutTemp);
        totalCuts++;
      }

      int totalPrice = 0;
      while (totalPrice < roll.sizeRoll)
      {
        int priceTemp = 0;
        file >> priceTemp;
        roll.priceCuts.push_back(priceTemp);
        totalPrice++;
      }
      /*
      cout << "Cut size: \n";
      for (int s = 0; s < roll.sizeCuts.size(); s++)
      {
        cout << roll.sizeCuts.at(s);
        cout << "\n";
      }

      cout << "Cut price : \n";
      for (int s = 0; s < roll.priceCuts.size(); s++)
      {
        cout << roll.priceCuts.at(s);
        cout << "\n";
      }
*/
    }
    else if (!strcmp(argv[i], "-p"))
    {

      show_p = true;
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
        auto t1 = std::chrono::high_resolution_clock::now();
        resultat = glouton(roll);
        auto t2 = std::chrono::high_resolution_clock::now();
        timeElapsed = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
      }
      else if (!strcmp(argv[i], "dynamique"))
      {
        auto t1 = std::chrono::high_resolution_clock::now();
        resultat = progDyn(roll);
        auto t2 = std::chrono::high_resolution_clock::now();
        timeElapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
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
    cout << resultat.profit << " with : " << resultat << endl;
  }
  if (show_t)
  {
    cout << "Time elapsed : " << (double)timeElapsed << endl;
  }

  return 0;
}
