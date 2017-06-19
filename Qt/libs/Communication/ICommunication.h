#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

#include <QObject>
#include <QString>

/**
 * Interface {@code ICommunication} defines the methods for reading and writing
 * data intro streams. The kind of data streams is defined in the class which
 * defines this interface.
 * <br/><br/>
 */

class ICommunication
{
    Q_OBJECT
public:
    /**
     * Interface virtual destructor
     */
    virtual ~ICommunication() = 0;

    /**
     * @brief This method is used to connect to implemented coommunication protocol
     * E.g. Connecting to TCP Server or to Serial port, etc. Connecting to GUI means
     * just starting it. Also, connecting to TCP may have a deep meaning: like creating
     * a TCP Server and then listen for incoming connections.
     * <br/>
     * @return {@code true} in case of success; {@code false} in case it fail to
     * connect to implemented protocol.
     */
    virtual bool connect() = 0;

    /**
     * @brief Close active connections and then disconnect from the protocol which
     * was implemented.
     * <br/>
     * @return {@code true} in case of success; {@code false} in case it fail to
     * connect to implemented protocol.
     */
    virtual bool disconnect() = 0;

    /**
     * @brief Read all data being received from protocol. This function will block
     * thread until all data is readed.
     * <br/>
     * @return A variable of type {@code QString} which contain all data received
     * from protocol input stream.
     */
    virtual QString readAll() = 0;

    /**
     * @brief Write data back to protocol.
     * <br/>
     * @param {@code data} QString variable which contains data to be written.
     * <br/>
     * @return Return the number of bytes written.
     */
    virtual qint64 write(QString data) = 0;

signals:
    virtual void connectionStatusChanged(bool newStatus) = 0;
    virtual void readyRead() = 0;

};

#endif  // ICOMMUNICATION_H
