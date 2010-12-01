#ifndef SSCOPENIGTLINKCLIENT_H_
#define SSCOPENIGTLINKCLIENT_H_

#include <vector>
#include <QtCore>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
class QTcpSocket;
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "cxRenderTimer.h"

namespace ssc
{
typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

class IGTLinkClient : public QThread
{
  Q_OBJECT
public:
  IGTLinkClient(QString address, int port, QObject* parent = NULL);
  igtl::ImageMessage::Pointer getLastImageMessage(); // threadsafe
  QString hostDescription() const; // threadsafe

signals:
  void imageReceived();
  void fps(double);

protected:
  virtual void run();

private slots:
//  void tick();
  void readyReadSlot();

  void hostFoundSlot();
  void connectedSlot();
  void disconnectedSlot();
  void errorSlot(QAbstractSocket::SocketError);

private:
  cx::RenderTimer mFPSTimer;
  bool ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
  void addImageToQueue(igtl::ImageMessage::Pointer imgMsg);

  bool mHeadingReceived;
  QString mAddress;
  int mPort;
  QTcpSocket* mSocket;
//  igtl::ClientSocket::Pointer mSocket;
  igtl::MessageHeader::Pointer mHeaderMsg;

  QMutex mImageMutex;
  std::list<igtl::ImageMessage::Pointer> mMutexedImageMessageQueue;

};

}//end namespace ssc

#endif /* SSCOPENIGTLINKCLIENT_H_ */
