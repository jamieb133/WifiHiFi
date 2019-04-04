/**
 * @file MainWindow.h
 * @author Jamie Brown
 * @brief 
 * @version 0.1
 * @date 2019-04-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QWidget>
#include <QGroupBox>
#include <QDial>
#include <QPushButton>
#include <QPointer>
#include <QSlider>
#include <QListView>
#include <QVector>
#include <QUdpSocket>

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Main Window object
     * 
     */
    explicit MainWindow();

    /**
     * @brief 
     * 
     */
    bool inRegistry(char id);
public slots:
    /**
     * @brief 
     * 
     */
    void readyRead();

private slots:
    /**
     * @brief 
     * 
     */
    void OnConnectClicked();

    /**
     * @brief 
     * 
     */
    void OnMidMoved(int val);

    /**
     * @brief 
     * 
     */
    void OnTrebleMoved(int val);

    /**
     * @brief 
     * 
     */
    void OnBassMoved(int val);

    /**
     * @brief 
     * 
     * @param val 
     */
    void OnVolMoved(int val);

signals:

private:

    QPointer<QPushButton> m_connectButton;
    QPointer<QListView> m_listView;
    QStringList m_list;
    QPointer<QStringListModel> m_listModel;
    QPointer<QSlider> m_volumeSlider;
    QPointer<QDial> m_bassDial;
    QPointer<QDial> m_midDial;
    QPointer<QDial> m_trebleDial;

    QPointer<QUdpSocket> m_socket;
    QPointer<QHostAddress> m_destIP;
    QVector<char> m_registry;

    int m_speakerCnt = 0;

};
#endif //MAINWINDOW_H 