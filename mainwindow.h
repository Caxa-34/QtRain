#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <tile.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_sbWidth_valueChanged(int arg1);

    void on_sbHeight_valueChanged(int arg1);

    void drawTable();
    void fillTable();

    void on_btnStart_clicked();

    bool atBorder(Tile* tile);

    void calculate();
    bool calculateWater(Tile* tile);

    void showResult(int numTest);

    bool inputTiles();

    void on_btnRandom_clicked();

private:

    int w = 1;
    int h = 1;
    int cntTests = 1;
    int resultWater;

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
