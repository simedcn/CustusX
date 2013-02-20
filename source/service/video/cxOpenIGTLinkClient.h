// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXOPENIGTLINKCLIENT_H_
#define CXOPENIGTLINKCLIENT_H_

#include <vector>
#include <QtCore>
#include <QTcpSocket>
#include "boost/shared_ptr.hpp"
class QTcpSocket;
#include "igtlMessageHeader.h"
#include "igtlClientSocket.h"
#include "igtlImageMessage.h"
#include "cxRenderTimer.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkClientBase.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

typedef boost::shared_ptr<class IGTLinkClient> IGTLinkClientPtr;

/**\brief Client thread for OpenIGTLink messaging.
 * \ingroup cxServiceVideo
 *
 *
 */
class IGTLinkClient: public IGTLinkClientBase
{
Q_OBJECT
public:
	IGTLinkClient(QString address, int port, QObject* parent = NULL);
	virtual QString hostDescription() const; // threadsafe
	~IGTLinkClient() {}

protected:
	virtual void run();

private slots:
	void readyReadSlot();

	void hostFoundSlot();
	void connectedSlot();
	void disconnectedSlot();
	void errorSlot(QAbstractSocket::SocketError);

private:
	bool ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
	bool ReceiveSonixStatus(QTcpSocket* socket, igtl::MessageHeader::Pointer& header);
	bool readOneMessage();

	bool mHeadingReceived;
	QString mAddress;
	int mPort;
	QTcpSocket* mSocket;
	igtl::MessageHeader::Pointer mHeaderMsg;
};

/**
 * @}
 */
} //end namespace cx

#endif /* CXOPENIGTLINKCLIENT_H_ */
