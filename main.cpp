#include <iostream>
#include <ctime>
#include <cstdlib>   // rand and srand

using namespace std;





int gato ()
{
    int g[3][3];

}

int randomfill(){
    int random = 1 + (rand() % 2);

}

int main() {
    unsigned seed = time(0);
    srand(seed);
    int g[3][3];
    for(int i (0); i != 3;i++)
        for(int j (0);j !=3;j++)
            g[i][j] = randomfill();




    for(int i(0); i != 3; ++i){
        for(int j(0); j != 3; ++j)
            std::cout << g[i][j] << ", ";
        std::cout << std::endl;
    }

}