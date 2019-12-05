################################################################################
#
# Ce script vérifie si votre solution est valide. C'est le script qui sera
# utilisé pour la correction, donc assurez-vous que la sortie de votre
# script tp.sh est compatible avec ce script-ci.
#
# Argument 1 : Path vers l'exemplaire
# Argument 2 : Path vers la solution de cet exemplaire
#
# Exemple d'utilisation :
#
#   1. Vous exécutez votre algorithme avec tp.sh et vous envoyez son résultat
#      vers un fichier texte :
#
#      ./tp.sh -e /home/pholi/INF8775/TP3/MTG_20_20 -p > sol_20_20
#
#   2. Vous vérifiez si votre solution est valide avec ce script-ci :
#
#      python3 sol_check.py MTG_20_20 sol_20_20
#
#
# Contactez-moi en cas de problème (philippe.olivier@polymtl.ca).
#
################################################################################


import pathlib
import sys


# Initial sanity checks
if (len(sys.argv) != 3):
    exit("ERREUR : Ce script de vérification de solution prend deux " + \
         "arguments en entrée. Voir le code source pour un exemple.")
if (not pathlib.Path(sys.argv[1]).is_file()):
    exit("ERREUR : Fichier " + sys.argv[1] + " inexistant.")
if (not pathlib.Path(sys.argv[2]).is_file()):
    exit("ERREUR : Fichier " + sys.argv[2] + " inexistant.")

# Parse instance
try:
    raw_instance = open(sys.argv[1]).readlines()
    n = int(raw_instance[0])
    m = int(raw_instance[1])
    base_strength = [int(x) for x in raw_instance[2].strip().split()]
    synergy = []
    for line in range(3, n*m+3):
        synergy.append([int(x) for x in raw_instance[line].strip().split()])
except:
    exit("ERREUR : Problème avec le format de l'instance.")

# Parse solution
try:
    raw_solution = open(sys.argv[2]).readlines()
    decks = []
    for i in raw_solution:
        decks.append([int(x) for x in i.strip().split()])
    decks = decks[-m:]
except:
    exit("ERREUR : Problème avec le format de la solution.")

# Ensure correct card numbers
cards = [card for deck in decks for card in deck]
if (min(cards) < 0 or max(cards) >= n*m):
    print("ERREUR: Les cartes doivent être numérotées de 0 à n*m-1.")
    exit(0)
    
# Ensure exactly n*m cards are used
if (len(cards) < n*m):
    print("ERREUR: Vous n'utilisez que", len(cards), "cartes parmi les", n*m, "disponibles.")
    exit(0)
if (len(cards) > n*m):
    print("ERREUR: Vous utilisez", len(cards), "cartes alors que seulement", n*m, "cartes sont disponibles.")
    exit(0)
    
# Ensure no card is used twice
if (len(cards) != len(set(cards))):
    print("ERREUR: Au moins une carte est utilisée plus d'une fois.")
    exit(0)
    
# Ensure that all decks are constructed
if (len(decks) != m):
    print("ERREUR: Votre solution utilise", len(decks), "paquets, alors qu'elle doit utiliser", m, "paquets.")
    exit(0)

# Ensure that all decks have the correct number of cards
for deck in decks:
    if len(deck) != n:
        print("ERREUR: Un paquet utilise", len(deck), "cartes. Vos paquets doivent tous avoir", n, "cartes.")
        exit(0)

deck_values = [0 for _ in range(m)]
for i in range(m):
    for j in range(n):
        deck_values[i] += base_strength[decks[i][j]]
    for j in range(0, n-1):
        for k in range(j+1, n):
            deck_values[i] += synergy[decks[i][j]][decks[i][k]]
        
# Print the solution
print("Votre solution est valide et a une valeur de", min(deck_values))
