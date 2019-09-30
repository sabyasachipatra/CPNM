#!/bin/bash
echo $PWD
workDir=$PWD
inputNetwork="$workDir"/ecoli_intr.txt""
inputGraph="$workDir"/ecoli_intr_enum.txt""
dictionary="$workDir"/ecoli_dictionary.txt""
./conversion.exe $inputNetwork $inputGraph $dictionary

echo $inputGraph
cd modet
for i in {3..5}
do
    featureFile="$workDir"/feature"$i".txt""
    echo $featureFile
    ./run -m modet -s $i -g $inputGraph -f $featureFile
done
cd ..
cp feature3.txt feature34.txt
rm temp.txt
./concatenate.exe "feature34.txt" "feature4.txt"
rm temp.txt

./normalization.exe feature34.txt norm_feature34.txt

featureFile="$workDir"/norm_feature34.txt""
clusterFile="$workDir"/ecoli_clusters.txt""
cd cpnm
./run -g $inputGraph -d $dictionary -f $featureFile -c $clusterFile -wa 50
cd ..
