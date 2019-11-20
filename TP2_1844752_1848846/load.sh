cd ~/Desktop/INF8775/TP1/TP2/
ex_folder=exemplaires
# Mettons toutes nos mesures dans un fichier format csv
echo "algo,taille,temps,profit" > ./results.csv

for algo in {"dynamique","glouton"}; do
    for ex in $(ls ${ex_folder}); do
        size=$(echo $ex | cut -d'-' -f1)
        t=$(./tp.sh -e ./${ex_folder}/${ex} -a $algo -t -p)
        read -ra test <<< $t
        echo $algo,$size,${test[1]},${test[0]} 
    done
done >> results.csv
