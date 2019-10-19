cd ~/Desktop/INF8775/TP1/TP1/code/
ex_folder=dataset
# Mettons toutes nos mesures dans un fichier format csv
echo "algo,serie,taille,temps" > ./results.csv

for algo in {"brute","seuil"}; do
    for testset_folder in $(ls $ex_folder); do
        for ex in $(ls ${ex_folder}/${testset_folder}); do
            size=$(echo $ex | cut -d_ -f2)
            t=$(./tp.sh -e ./${ex_folder}/${testset_folder}/${ex} -a $algo -t)
            echo $algo,$testset_folder,$size,$t
        done
    done
done >> results.csv
