#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStateMachine>

enum types{
    NONE,
    HOTCHOCOLATE,
    CAPPUCCINO,
    ESPRESSO
};

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
    void S_InitialiseSystems_OnEntry(void);
    void S_Configure_onEntry(void);
    void S_Controlpanel_onEntry(void);
    void S_Controlpanel_onExit(void);
    void S_HotchocolateSelected_onEntry(void);
    void S_CappuccinoSelected_onEntry(void);
    void S_EspressoSelected_onEntry(void);
    void S_WaitForCoins_onEntry(void);
    void S_GiveBackCoins_onEntry(void);
    void S_Process_20C_onEntry(void);
    void S_Process_50C_onEntry(void);
    void S_GiveChange_onEntry(void);
    void S_WaitForCoffee_onEntry(void);
    void S_MakeSelectedDrink_onEntry(void);
    void S_Done_onEntry(void);
    void readFile(void);
    void writeFile(int change, int coffee, int amount);
    bool checkChange(void);
    bool checkIngredients(int coffeeNeeded);
    bool handleCoin(int coin);
    void makeDrink();

private:
    Ui::MainWindow *ui;

    QStateMachine statemachine;


};
#endif // MAINWINDOW_H
