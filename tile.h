#ifndef TILE_H
#define TILE_H
#include <vector>

class Tile
{
public:
    int height = 0;
    int heightWater = 0;
    std::vector<Tile*> neighbors;


    int x = 0;
    int y = 0;

    Tile();
    Tile(int x, int y, int h) : height(h), x(x), y(y) {}

    ~Tile()
    {
        // Освобождаем память для всех соседних тайлов
        for (auto& neighbor : neighbors)
        {
            delete neighbor;
        }
    }
};

#endif // TILE_H
