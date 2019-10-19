echo $ls
echo "algo,serie,taille,temps" > ./results.csv
for algo in {"brute","brute"}; do
    for testset_folder in $(ls $ex_folder); do
        for ex in $(ls ${ex_folder}/${testset_folder}); do
            size=$(echo $ex | cut -d_ -f2)
            t=$(./tp.sh -e ./${ex_folder}/${testset_folder}/${ex} -a $algo -t)
            echo $algo,$testset_folder,$size,$t
        done
    done
done >> results.csv
