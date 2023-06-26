#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <unistd.h>
#include <QDebug>
#include <QThread>


FILE *fp;
char storageFile[255] = "storage.txt";
int totalAmountPaid;
int change;
int coffee;
int coffeeNeeded = 0;
int insertedMoney = 0;
int typeSelected = NONE;
int priceNeeded = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    QState *S_InitialiseSystems = new QState();
    QState *S_Configure = new QState();
    QState *S_Controlpanel = new QState();
    QState *S_HotchocolateSelected = new QState();
    QState *S_CappuccinoSelected = new QState();
    QState *S_EspressoSelected = new QState();
    QState *S_WaitForCoins = new QState();
    QState *S_GiveBackCoins = new QState();
    QState *S_Process_20C = new QState();
    QState *S_Process_50C = new QState();
    QState *S_GiveChange = new QState();
    QState *S_WaitForCoffee = new QState();
    QState *S_MakeSelectedDrink = new QState();
    QState *S_Done = new QState();
    QState *S_WaitForExtra = new QState();

    S_InitialiseSystems->addTransition(ui->pbYes, &QPushButton::clicked, S_Configure);
    S_Configure->addTransition(ui->pbHotchocolate, &QPushButton::clicked, S_HotchocolateSelected);
    S_Configure->addTransition(ui->pbEspresso, &QPushButton::clicked, S_EspressoSelected);
    S_Configure->addTransition(ui->pbCappuccino, &QPushButton::clicked, S_CappuccinoSelected);
    S_Configure->addTransition(ui->pbNo, &QPushButton::clicked, S_Controlpanel);
    S_Controlpanel->addTransition(ui->pbRefill, &QPushButton::clicked, S_Configure);
    S_HotchocolateSelected->addTransition(ui->pbYes, &QPushButton::clicked, S_WaitForCoins);
    S_HotchocolateSelected->addTransition(ui->pbNo, &QPushButton::clicked, S_Controlpanel);
    S_EspressoSelected->addTransition(ui->pbYes, &QPushButton::clicked, S_WaitForCoins);
    S_EspressoSelected->addTransition(ui->pbNo, &QPushButton::clicked, S_Controlpanel);
    S_CappuccinoSelected->addTransition(ui->pbYes, &QPushButton::clicked, S_WaitForCoins);
    S_CappuccinoSelected->addTransition(ui->pbNo, &QPushButton::clicked, S_Controlpanel);
    S_WaitForCoins->addTransition(ui->pb20c, &QPushButton::clicked, S_Process_20C);
    S_WaitForCoins->addTransition(ui->pb50c, &QPushButton::clicked, S_Process_50C);
    S_WaitForCoins->addTransition(ui->pbCancel, &QPushButton::clicked, S_GiveBackCoins);
    S_GiveBackCoins->addTransition(ui->pbYes, &QPushButton::clicked, S_Configure);
    S_Process_20C->addTransition(ui->pbYes, &QPushButton::clicked, S_GiveChange);
    S_Process_20C->addTransition(ui->pbNo, &QPushButton::clicked, S_WaitForCoins);
    S_Process_50C->addTransition(ui->pbYes, &QPushButton::clicked, S_GiveChange);
    S_Process_50C->addTransition(ui->pbNo, &QPushButton::clicked, S_WaitForCoins);
    S_GiveChange->addTransition(ui->pbYes, &QPushButton::clicked, S_WaitForCoffee);
    S_WaitForCoffee->addTransition(ui->pbCup, &QPushButton::clicked, S_MakeSelectedDrink);
    S_MakeSelectedDrink->addTransition(ui->pbYes, &QPushButton::clicked, S_Done);
    S_Done->addTransition(ui->pbCup, &QPushButton::clicked, S_Configure);

    statemachine.addState(S_InitialiseSystems);
    statemachine.addState(S_Configure);
    statemachine.addState(S_Controlpanel);
    statemachine.addState(S_HotchocolateSelected);
    statemachine.addState(S_CappuccinoSelected);
    statemachine.addState(S_EspressoSelected);
    statemachine.addState(S_WaitForCoins);
    statemachine.addState(S_GiveBackCoins);
    statemachine.addState(S_Process_20C);
    statemachine.addState(S_Process_50C);
    statemachine.addState(S_GiveChange);
    statemachine.addState(S_WaitForCoffee);
    statemachine.addState(S_MakeSelectedDrink);
    statemachine.addState(S_Done);
    statemachine.addState(S_WaitForExtra);
    statemachine.setInitialState(S_InitialiseSystems);

    connect(S_InitialiseSystems, &QState::entered, this, &MainWindow::S_InitialiseSystems_OnEntry);
    connect(S_Configure, &QState::entered, this, &MainWindow::S_Configure_onEntry);
    connect(S_Controlpanel, &QState::entered, this, &MainWindow::S_Controlpanel_onEntry);
    connect(S_Controlpanel, &QState::exited, this, &MainWindow::S_Controlpanel_onExit);
    connect(S_HotchocolateSelected, &QState::entered, this, &MainWindow::S_CappuccinoSelected_onEntry);
    connect(S_CappuccinoSelected, &QState::entered, this, &MainWindow::S_CappuccinoSelected_onEntry);
    connect(S_EspressoSelected, &QState::entered, this, &MainWindow::S_EspressoSelected_onEntry);
    connect(S_WaitForCoins, &QState::entered, this, &MainWindow::S_WaitForCoins_onEntry);
    connect(S_GiveBackCoins, &QState::entered, this, &MainWindow::S_GiveBackCoins_onEntry);
    connect(S_Process_20C, &QState::entered, this, &MainWindow::S_Process_20C_onEntry);
    connect(S_Process_50C, &QState::entered, this, &MainWindow::S_Process_50C_onEntry);
    connect(S_GiveChange, &QState::entered, this, &MainWindow::S_GiveChange_onEntry);
    connect(S_WaitForCoffee, &QState::entered, this, &MainWindow::S_WaitForCoffee_onEntry);
    connect(S_MakeSelectedDrink, &QState::entered, this, &MainWindow::S_MakeSelectedDrink_onEntry);
    connect(S_Done, &QState::entered, this, &MainWindow::S_Done);

    statemachine.start();

    qDebug() << "MainWindow started";
    qDebug() << "Statemachine started started";
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow closed";
    delete ui;
}

void MainWindow::S_InitialiseSystems_OnEntry(void)
{
    ui->gbHidden->setVisible(false);
    ui->gbMoney->setVisible(false);
    ui->gbMake->setVisible(false);
    ui->gbControlpanel->setVisible(false);
    ui->gbType->setVisible(true);

    auto palette = ui->lcdNeeded->palette();
    palette.setColor(palette.WindowText, Qt::black);
    palette.setColor(palette.Light, Qt::black);
    palette.setColor(palette.Dark, Qt::black);
    ui->lcdNeeded->setPalette(palette);

    palette = ui->lcdInserted->palette();
    palette.setColor(palette.WindowText, Qt::black);
    palette.setColor(palette.Light, Qt::black);
    palette.setColor(palette.Dark, Qt::black);
    ui->lcdInserted->setPalette(palette);

    ui->pbYes->animateClick();

}

void MainWindow::S_Configure_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_Configure");
    ui->userInfo->appendPlainText("Please select type of coffee");
    ui->gbMoney->setVisible(false);
    ui->gbMake->setVisible(false);
    ui->gbControlpanel->setVisible(false);
    ui->gbType->setVisible(true);
    ui->userInfo->clear();
    if(!checkChange())
    {
        ui->pbNo->animateClick();
    }
}

void MainWindow::S_Controlpanel_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_Controlpanel");
    ui->userInfo->appendPlainText("Press refill to refill system and go back to main window");
    ui->gbMoney->setVisible(false);
    ui->gbMake->setVisible(false);
    ui->gbControlpanel->setVisible(true);
    ui->gbType->setVisible(false);
}

void MainWindow::S_Controlpanel_onExit(void)
{
    int maxChange = 2000;
    int maxCoffee = 10;


    writeFile(maxChange, maxCoffee, totalAmountPaid);
}

void MainWindow::S_HotchocolateSelected_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_HotchocolateSelected");
    coffeeNeeded = 1;
    if(checkIngredients(coffeeNeeded))
    {
        typeSelected = HOTCHOCOLATE;
        priceNeeded = 100;

        ui->userInfo->appendPlainText("Selected drink = Hot Chocolate");
        ui->lcdNeeded->display(priceNeeded);
        ui->lcdInserted->display(insertedMoney);
        ui->userInfo->appendPlainText("Please select the coin you want to use");

        ui->pbYes->animateClick();
    }else{
        ui->pbNo->animateClick();
    }
}

void MainWindow::S_CappuccinoSelected_onEntry(void)
{
     ui->stateLogger->appendPlainText("Entered state S_CappuccinoSelected");
     coffeeNeeded = 1;
     if(checkIngredients(coffeeNeeded))
     {
         typeSelected = CAPPUCCINO;
         priceNeeded = 120;

         ui->userInfo->appendPlainText("Selected drink = Cappuccino");
         ui->lcdNeeded->display(priceNeeded);
         ui->lcdInserted->display(insertedMoney);
         ui->userInfo->appendPlainText("Please select the coin you want to use");

         ui->pbYes->animateClick();
     }else{
         ui->pbNo->animateClick();
     }
}

void MainWindow::S_EspressoSelected_onEntry(void)
{
     ui->stateLogger->appendPlainText("Entered state S_EspressoSelected");
     coffeeNeeded = 1;

     if(checkIngredients(coffeeNeeded))
     {
         typeSelected = ESPRESSO;
         priceNeeded = 100;

         ui->userInfo->appendPlainText("Selected drink = Espresso");
         ui->lcdNeeded->display(priceNeeded);
         ui->lcdInserted->display(insertedMoney);
         ui->userInfo->appendPlainText("Please select the coin you want to use");

         ui->pbYes->animateClick();
     }else{
         ui->pbNo->animateClick();
     }
}


void MainWindow::S_WaitForCoins_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_WaitForCoins");
    ui->gbMoney->setVisible(true);
    ui->gbMake->setVisible(false);
    ui->gbType->setVisible(false);
    ui->gbControlpanel->setVisible(false);
}


void MainWindow::S_GiveBackCoins_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_GiveBackCoins");
    ui->userInfo->appendPlainText(QString::number(insertedMoney) = "change have been dispenced");
    insertedMoney = 0;
    ui->lcdInserted->display(insertedMoney);
    ui->pbYes->animateClick();
}

void MainWindow::S_Process_20C_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_Proccess_20C");
    int coin = 20;
    if (handleCoin(coin)){
        ui->pbYes->animateClick();
    }else{
        ui->pbNo->animateClick();
    }
}

void MainWindow::S_Process_50C_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_Proccess_50C");
    int coin = 50;
    if (handleCoin(coin)){
        ui->pbYes->animateClick();
    }else{
        ui->pbNo->animateClick();
    }
}


void MainWindow::S_GiveChange_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_GiveChange");
    if(insertedMoney > 0){
        ui->userInfo->appendPlainText(QString::number(insertedMoney) + "change have been dispenced");
        writeFile(change-insertedMoney, coffee, totalAmountPaid);
        insertedMoney = 0;
        ui->lcdInserted->display(insertedMoney);
    }else{
        ui->userInfo->appendPlainText("No change to give");
    }
    ui->pbYes->animateClick(); // software trigger to jump to the next state
}

void MainWindow::S_WaitForCoffee_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_WaitingForCup");
    ui->gbMoney->setVisible(false);
    ui->gbMake->setVisible(true);
    ui->gbType->setVisible(false);
    ui->gbControlpanel->setVisible(false);
    ui->pbCup->setText("Place cup");
    ui->userInfo->appendPlainText("Please place cup");
    ui->progressBar->setValue(0);
}

void MainWindow::S_MakeSelectedDrink_onEntry(void)
{
    ui->stateLogger->appendPlainText("Entered state S_MakeSelectedDrink");
    makeDrink();

    writeFile(change, coffee - coffeeNeeded,totalAmountPaid);
    coffeeNeeded = 0;

    ui->pbYes->animateClick();
}

void MainWindow::S_Done(void)
{
    ui->stateLogger->appendPlainText("Entered state S_Done");
    ui->userInfo->appendPlainText("your coffee is done");
    ui->pbCup->setText("Remove cup");
    ui->userInfo->appendPlainText("Please remove cup");
}


void MainWindow::readFile()
{
    char dataString[255];
    int data[] = {0,0,0};

    FILE *fp;
    fp = fopen(storageFile, "r");
    if (fp != NULL)
    {
        fgets(dataString, 254,fp);
        fclose(fp);
    }else{
        ui->userInfo->appendPlainText("Error while reading file");
    }
    QString sData = "data from file = ";
    sData =+ dataString;
    qDebug() << sData;

    char *token = strtok(dataString, ",");

    int i =0;
    while(token != NULL)
    {
        data[i] = atoi(token);
        token = strtok(NULL, ",");
        i++;
    }

    change = data[0];
    coffee = data[1];
    totalAmountPaid = data[2];

}

void MainWindow::writeFile(int change, int coffee, int amount)
{
    char data[255];
    sprintf(data,"%d,%d,%d\n", change, coffee, amount);
    FILE *fp;
    fp = fopen(storageFile, "w");
    if (fp != NULL)
    {
        fprintf(fp,"%s", data);
        fclose(fp);
    }else{
        ui->userInfo->appendPlainText("Error while writing file");
    }
}

bool MainWindow::checkChange()
{
    readFile();
    int checkchange = 100;

    if(change < checkchange)
    {
        return false;
    }else
    {
        return true;
    }
}

bool MainWindow::checkIngredients(int coffeeNeeded)
{
    readFile();
    if(coffee >= coffeeNeeded){
        return true;
    }
    return false;
}

bool MainWindow::handleCoin(int coin)
{
    readFile();
    insertedMoney = insertedMoney + coin;
    ui->lcdInserted->display(insertedMoney);
    if (insertedMoney >= priceNeeded)
    {
        ui->userInfo->appendPlainText("Enough money is inserted");
        insertedMoney = insertedMoney - priceNeeded;
        ui->userInfo->appendPlainText("total = " + QString::number(totalAmountPaid + priceNeeded));
        totalAmountPaid = totalAmountPaid + priceNeeded;
        writeFile(change, coffee, totalAmountPaid);
        return true;
    }
    else {
        ui->userInfo->appendPlainText("Not enough money inserted. Insert coin");
        return false;
    }
}

void MainWindow::makeDrink()
{
    switch(typeSelected)
    {
    case HOTCHOCOLATE:
        ui->userInfo->appendPlainText("Please wait while your Hot Chocolate is made");
        break;
    case CAPPUCCINO:
        ui->userInfo->appendPlainText("Please wait while your Cappuccino is made");
        break;
    case ESPRESSO:
        ui->userInfo->appendPlainText("Please wait while your Espresso is made");
        break;
    }
    int totalMillis = 2000;
    for(int i = 1; i <= 100; i++)
    {
        ui->progressBar->setValue(i);
        QThread::msleep(totalMillis/100);
    }

}
