/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-04-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "MainWindow.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow* window = new MainWindow;
    window->setWindowTitle("WifiHifi Controller");
    window->setFixedSize(350, 400);
    window->show();

    return app.exec();
}