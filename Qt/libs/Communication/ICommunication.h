/**
 * @file   ICommunication.h
 * @author Alex (616c6578@protonmail.com)
 * @date   July, 2017
 * @brief  Communication interface
 * @mainpage ICommunication
 *
 * All the communications are realized through this interface ^_^
 */
#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

#include <QObject>
#include <QString>

/**
 * @name ICommunication
 * @brief The ICommunication class is a interface that defines the methods for reading and writing
 * data intro streams. The type of data streams is defined in the class which
 * defines this interface as it vary from one protocol to another.
 */
class ICommunication
{

public:
    /**
     * Interface virtual destructor. Don't forget to clean garbage!!1
     */
    virtual ~ICommunication() = 0;

    /**
     * @brief This method is used to connect to implemented coommunication protocol
     * E.g. Connecting to TCP Server or to Serial port, etc. Connecting to GUI means
     * just starting it. Also, connecting to TCP may have a deep meaning: like creating
     * a TCP Server and then listen for incoming connections.
     *
     * @return Boolean value which indicate the success of operation:
     *  @retval true    connection succesfully established;
     *  @retval false   failed to connect;
     */
    virtual bool connect() = 0;

    /**
     * @brief Close active connections and then disconnect from the protocol which
     * was implemented.
     *
     * @return  Info regarding operation
     *  @retval true    succesfully disconnected;
     *  @retval false   forcibly disconnected;
     */
    virtual bool disconnect() = 0;

    /**
     * @brief Check if you are connected to protocol and ready to read/write. Use this
     * everytime before trying to read/write.
     *
     * @return Whether you are connected or not.
     *  @retval true    connected - protocol is readable/writable;
     *  @retval false   not connected. You may want to attempt to reconnect in this case
     * in order to be able to read/write.
     */
    virtual bool isConnected() = 0;

    /**
     * @brief Read all data being received from protocol. This function will block
     * thread until all data is readed.
     *
     * @return A string which contain all data received from protocol input stream.
     */
    virtual QString readAll() = 0;

    /**
     * @brief Write data back to protocol input stream.
     * @param[in] data is the variable which contains data to be written.
     * @return Return the number of bytes written.
     */
    virtual qint64 write(QString data) = 0;

    /**
     * @brief Use this to retreive last error description.
     * @return A string describing last error if any.
     */
    virtual QString getLastError() = 0;

signals:
    /**
     * @brief This is a signal which is triggered every
     * time connection to protocol changes. There are two possibilities: either connection
     * is true either is false. This stands for "connected" or "not connected". This is
     * usefull to know when connection status changes.
     *
     * @param[in] newStatus is the status of connection. <b>TRUE</b> means connection
     * is established and usable and <b>FALSE</b> means that connection to communication
     * protocol changes status to disconnected is it is lost.
     */
    virtual void connectionStatusChanged(bool newStatus) = 0;

    /**
     * @brief This is a SIGNAL triggered when protocol incoming stream is available
     * and have intormation to be read.
     * Use this signal to know when to call method readAll() and read incoming information.
     */
    virtual void readyRead() = 0;

};

#endif  // ICOMMUNICATION_H
