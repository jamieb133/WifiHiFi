#ifndef TYPES_H
#define TYPES_H

#include <QObject>
#include <QUdpSocket>


typedef enum {
    MASTER,
    SLAVE
} SyncServer_t;

typedef enum {
    SYNC,
    DELAY_REQ,
    DELAY_RESP
} SyncMsg_t;















#endif //TYPES_H