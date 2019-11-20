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

bool compareRentability(Rentability r1, Rentability r2)
{
	return (r1.ratio > r2.ratio);
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
	int sizeCutsStep[roll.sizeRoll + 1];
	std::fill_n(sizeCutsStep, roll.sizeRoll + 1, 1);
	int t = 0;
	int s = 0;
	sol[0].profit = 0;
	int max_profit = -1;
	for (int i = 1; i <= roll.sizeRoll; i++)
	{
		for (int j = 0; j < i; j++)
		{
			int new_profit = priceCuts[j] + sol[i - j - 1].profit;
			if (new_profit > max_profit) {
				max_profit = new_profit;
				sizeCutsStep[i] = j + 1;
			}
		}
		sol[i].profit = max_profit;
	}

	int total_lenght = roll.sizeRoll;
	vector<int> cutsSolution;
	while (total_lenght > 0) {
		cutsSolution.push_back(sizeCutsStep[total_lenght]);
		total_lenght = total_lenght - sizeCutsStep[total_lenght];
	}
	sol[roll.sizeRoll].sizeCuts = cutsSolution;
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
	sort(rentabilities.begin(), rentabilities.end(), compareRentability);


	int i = 0;
	// algorithme glouton
	while (i < rentabilities.size())
	{
		Rentability r = rentabilities.at(i);

		int sumTemp = sum;
		if (sumTemp + r.cutSize <= roll.sizeRoll)
		{
			sum += r.cutSize;
			sol.sizeCuts.push_back(r.cutSize);
			sol.profit += r.cutProfit;
		}
		else
		{
			i++;
		}
	}
	return sol;
}

/**
 * ----------------------------
 * ---------------------
 * Algorithme backtrack
 * ---------------------
 * ----------------------------
**/

int profitmax = 0;
vector<Rentability> cutfinal;

void cutSet(vector<Rentability>& rentabilities, int sum,
	vector<Rentability>& rentabilitiesTemp, int node, Solution& sol)
{
	if (sum < 0)
		return;
	if (sum == 0)
	{
		int sommeTot = 0;
		for (int i = 0; i < rentabilitiesTemp.size(); i++) {
			sommeTot += rentabilitiesTemp[i].cutProfit;
		}
		if (sommeTot > profitmax) {
			profitmax = sommeTot;
			sol.profit = sommeTot;
			cutfinal = rentabilitiesTemp;
		}
		return;
	}
	for (int i = node; i < rentabilities.size() && sum - rentabilities[i].cutSize >= 0; i++)
	{
		rentabilitiesTemp.push_back(rentabilities[i]);
		cutSet(rentabilities, sum - rentabilities[i].cutSize, rentabilitiesTemp, i, sol);
		rentabilitiesTemp.pop_back();
	}
}

Solution backtrack(Roll roll)
{
	Solution sol;

	vector<Rentability> rentabilities;
	for (int i = 0; i < roll.sizeCuts.size(); i++)
	{
		Rentability ri;
		ri.cutSize = roll.sizeCuts.at(i);
		ri.cutProfit = roll.priceCuts.at(i);
		ri.ratio = (double)ri.cutProfit / (double)ri.cutSize;
		rentabilities.push_back(ri);
	}
	sort(rentabilities.begin(), rentabilities.end(), compareRentability);


	vector<Rentability> r;
	cutSet(rentabilities, roll.sizeRoll, r, 0, sol);

	for (int i = 0; i < cutfinal.size(); i++) {
		sol.sizeCuts.push_back(cutfinal[i].cutSize);
	}
	return sol;

}
int main(int argc, char *argv[])
{
	char *fn = nullptr;
	bool show_p = false;
	bool show_t = false;
	bool show_c = false;

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
				resultat = glouton(roll);
				chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
				timeElapsed = (double)std::chrono::duration<double>(t2 - t1).count();
			}
			else if (!strcmp(argv[i], "dynamique"))
			{
				chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
				resultat = progDyn(roll);
				chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

				timeElapsed = (double)std::chrono::duration<double>(t2 - t1).count();
			}
			else if (!strcmp(argv[i], "backtrack"))
			{
				chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
				resultat = backtrack(roll);
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
		cout << resultat.profit << " ";
	}
	if (show_t)
	{
		cout << timeElapsed * 1000 << endl;
	}
	if (show_c)
	{
		cout << resultat << endl;
	}

	return 0;
}
