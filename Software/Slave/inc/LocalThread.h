#ifndef LOCALTHREAD_H
#define LOCALTHREAD_H

#include <QThread>

class LocalThread : public QThread
{
    Q_OBJECT

public:
    explicit LocalThread();

private:
    void run();
};

#endif //LOCALTHREAD_H