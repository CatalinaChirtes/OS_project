# OS_project

prima oara mergi unde ii amplasat proiectu in calculator

cd ~/CLionProjects/terminalOS/

ca sa rulezi mainul dai astea 2 comenzi

g++ -o main main.cpp -lreadline

./main


daca faci o modificare in unul din fisiere din cpp, cum ar fi cp, prima oara rulezi comanda de la fisierul ala si apoi rulezi cele 2 de main

g++ tee.cpp -o bin/tee

g++ dirname.cpp -o bin/dirname

g++ cp.cpp -o bin/cp


execmplu pentru cand lucrezi cu cp


cd ~/CLionProjects/terminalOS/

g++ cp.cpp -o bin/cp

g++ -o main main.cpp -lreadline

./main
