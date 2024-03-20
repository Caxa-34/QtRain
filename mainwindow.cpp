#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QColor>
#include <tile.h>
#include <random>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    drawTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}


std::vector<std::vector<Tile*>> tiles; //тайлы ввода
std::vector<std::vector<Tile*>> tilesWithWater; //тайлы вычислений
std::vector<Tile*> tilesPath; //тайлы пути воды

int w = 1, h = 1, cntTests = 1,
resultWater = 0,
maxHeight = 0, minHeight = 999999, tilesHeight = 0; //максимальная разница в длине тайла

void MainWindow::drawTable(){
    ui->tbResult->setRowCount(w);
    ui->tbResult->setColumnCount(h);
    int side = 50; //разиер ячейки
    for (int i = 0; i < ui->tbResult->columnCount(); ++i) {
        ui->tbResult->setColumnWidth(i, side);
    }

    for (int i = 0; i < ui->tbResult->rowCount(); ++i) {
        ui->tbResult->setRowHeight(i, side);
    }
}

void MainWindow::on_sbWidth_valueChanged(int arg1)
{
    w = arg1;
    drawTable();
}

void MainWindow::on_sbHeight_valueChanged(int arg1)
{
    h = arg1;
    drawTable();
}

void MainWindow::showResult(int numTest) {
    //заполнение по умолчанию
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(tiles[i][j]->height));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tbResult->setItem(i, j, item);
        }
    }
    //заполнение рассчётов
    int heightRange = maxHeight / 5; //расчёт уровня высоты для закраски
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            QTableWidgetItem *item = ui->tbResult->item(i, j);
            if (tilesWithWater[i][j]->heightWater > 0) { //отображение тайлов заполненных водой
                item->setBackgroundColor(Qt::cyan);
                QString str = item->text() + " + " + QString::number(tilesWithWater[i][j]->heightWater);
                item->setText(str);
            }
            else { //отображение сухих тайлов учитывая высоту
                int level = tilesWithWater[i][j]->height / heightRange + 1;
                QColor color(256-128/10*level, 256-128/10*level, 256-128/10*level);
                item->setBackgroundColor(color);
            }

        }
    }
    QMessageBox::information(this, "Result, test " + QString::number(numTest+1), "On a construction: " + QString::number(resultWater) + " cube inch of water");
}

bool MainWindow::inputTiles() {
    resultWater = 0;
    maxHeight = 0;
    minHeight = 999999;

    //Инициализация двумерного массива
    tiles.resize(w);
    for (int i = 0; i < w; ++i) {
        tiles[i].resize(h);
        for (int j = 0; j < h; ++j) {
            QTableWidgetItem *item = ui->tbResult->item(i, j);

            if (!item || item->text().isEmpty()) { //если не заполнили
                QMessageBox::warning(this, "Error", "Cell is null!");
                return false;
            }
            bool isInt;
            int value = item->text().toInt(&isInt);
            if (!isInt || value < 1 || value > 10000) { //если неверно заполнили
                QMessageBox::warning(this, "Error", "Invalid data!");
                return false;
            }
            tiles[i][j] = new Tile(i, j, value);
            if (value > maxHeight) maxHeight = value;
            if (value < minHeight) minHeight = value;
        }
    }
    tilesHeight = maxHeight - minHeight;

    //Вывод в таблицу и создание связей с соседними тайлами
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            if (i != w - 1) tiles[i][j]->neighbors.push_back(tiles[i+1][j]); //низ
            if (i != 0) tiles[i][j]->neighbors.push_back(tiles[i-1][j]); //верх
            if (j != h - 1) tiles[i][j]->neighbors.push_back(tiles[i][j+1]); //право
            if (j != 0) tiles[i][j]->neighbors.push_back(tiles[i][j-1]); //лево

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(tiles[i][j]->height));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tbResult->setItem(i, j, item);
        }
    }

    //Дублирование тайлов для расчётов
    tilesWithWater = std::vector<std::vector<Tile*>>(tiles);
    return true;
}

void MainWindow::fillTable() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 8);

    //Инициализация двумерного массива
    tiles.resize(w);
    for (int i = 0; i < w; ++i) {
        tiles[i].resize(h);
        for (int j = 0; j < h; ++j) {
            int randomHeight = dis(gen);
            tiles[i][j] = new Tile(i, j, randomHeight);
        }
    }

    //Вывод в таблицу и создание связей с соседними тайлами
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(tiles[i][j]->height));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tbResult->setItem(i, j, item);
        }
    }
}

bool MainWindow::atBorder(Tile* tile) { //Проверка на нахождение тайла с краю (где вода 100% стекает)
    if (tile->x == 0) return true;
    if (tile->y == 0) return true;
    if (tile->x == w-1) return true;
    if (tile->y == h-1) return true;
    return false;
}

int minInTiles(std::vector<Tile*> tiles) {
    int min = 999999;
    for (int i = 0; i < tiles.size(); i++) {
        if (min > tiles[i]->height) min = tiles[i]->height;
    }
    return min;
}

void MainWindow::calculate() { //имитация создания блоков воды
    for (int numWater = 0; numWater < tilesHeight; numWater++) {
        for (int xTile = 1; xTile < w-1; xTile++)
            for (int yTile = 1; yTile < h-1; yTile++) {
                tilesPath = std::vector<Tile*>();
                bool path = calculateWater(tilesWithWater[xTile][yTile]);
                if (!path) {
                    int minInPath = minInTiles(tilesPath);
                    for (int i = 0; i < tilesPath.size(); i++) {
                        if (tilesPath[i]->height == minInPath) {
                            tilesPath[i]->height++;
                            tilesPath[i]->heightWater++;
                            resultWater++;
                        }
                    }
                }
            }
    }

}

bool MainWindow::calculateWater(Tile* currentTile) { //поведение блока воды
    if (atBorder(currentTile)) return true;
    int heightWater = currentTile->height + 1;
    tilesPath.push_back(currentTile);

    /*
    if (heightWater <= currentTile->neighbors[0]->height &&
        heightWater <= currentTile->neighbors[1]->height &&
        heightWater <= currentTile->neighbors[2]->height &&
        heightWater <= currentTile->neighbors[3]->height) {

        currentTile->height++;
        currentTile->heightWater++;
        resultWater++;
        return false;
    }
    */

    bool path1 = false, path2 = false, path3 = false, path4 = false;
    if (std::find(tilesPath.begin(), tilesPath.end(), currentTile->neighbors[0]) == tilesPath.end()
            && currentTile->neighbors[0]->height < heightWater) {
        path1 = calculateWater(currentTile->neighbors[0]);
    }
    if (std::find(tilesPath.begin(), tilesPath.end(), currentTile->neighbors[1]) == tilesPath.end()
            && currentTile->neighbors[1]->height < heightWater) {
        path2 = calculateWater(currentTile->neighbors[1]);
    }
    if (std::find(tilesPath.begin(), tilesPath.end(), currentTile->neighbors[2]) == tilesPath.end()
            && currentTile->neighbors[2]->height < heightWater) {
        path3 = calculateWater(currentTile->neighbors[2]);
    }
    if (std::find(tilesPath.begin(), tilesPath.end(), currentTile->neighbors[3]) == tilesPath.end()
            && currentTile->neighbors[3]->height < heightWater) {
        path4 = calculateWater(currentTile->neighbors[3]);
    }

    /*
    if (heightWater > currentTile->neighbors[2]->height) {
        calculateWater(currentTile->neighbors[2]);
    } else if (heightWater > currentTile->neighbors[1]->height) {
        calculateWater(currentTile->neighbors[1]);
    } else if (heightWater > currentTile->neighbors[3]->height) {
        calculateWater(currentTile->neighbors[3]);
    } else if (heightWater > currentTile->neighbors[0]->height) {
        calculateWater(currentTile->neighbors[0]);
    }
    */

    if (path1 || path2 || path3 || path4) return true;
    return false;
}

void MainWindow::on_btnStart_clicked()
{
    if (inputTiles()) {
        for (int i = 0; i < ui->sbCountTests->value(); i++) {
            calculate();
            showResult(i);
        }
    }
}


void MainWindow::on_btnRandom_clicked()
{
    fillTable();
}
