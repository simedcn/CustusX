/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxBasicVideoSource.h"

#include <QTimer>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "cxImage.h"
#include "cxBoundingBox3D.h"
#include "cxLogger.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"


namespace cx
{

BasicVideoSource::BasicVideoSource(QString uid) :
	mStreaming(false)
{
	mStatus = "USE_DEFAULT";
	mRedirecter = vtkSmartPointer<vtkImageChangeInformation>::New(); // used for forwarding only.


	vtkImageDataPtr emptyImage = generateVtkImageData(Eigen::Array3i(3,3,1),
														   Vector3D(1,1,1),
														   0);
	mEmptyImage.reset(new Image(uid, emptyImage));
	mReceivedImage = mEmptyImage;
	mRedirecter->SetInputData(mEmptyImage->getBaseVtkImageData());

	mTimeout = true; // must start invalid
	mTimeoutTimer = new QTimer(this);
	mTimeoutTimer->setInterval(1000);
	connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

BasicVideoSource::~BasicVideoSource()
{
	stop();
}

void BasicVideoSource::overrideTimeout(bool timeout)
{
	if (mTimeoutTimer)
	{
		mTimeoutTimer->setParent(NULL);
		delete mTimeoutTimer;
		mTimeoutTimer = NULL;
	}

	mTimeout = timeout;
}

QString BasicVideoSource::getUid()
{
	return mReceivedImage->getUid();
}
QString BasicVideoSource::getName()
{
	return mReceivedImage->getName();
}

void BasicVideoSource::timeout()
{
	if (mTimeout)
		return;

	reportWarning("Timeout!");
	mTimeout = true;
	emit newFrame();
}

bool BasicVideoSource::validData() const
{
	return this->isConnected() && !mTimeout;
}

double BasicVideoSource::getTimestamp()
{
	return mReceivedImage->getAcquisitionTime().toMSecsSinceEpoch();
}

TimeInfo BasicVideoSource::getAdvancedTimeInfo()
{
	return mReceivedImage->getAdvancedTimeInfo();
}

bool BasicVideoSource::isConnected() const
{
	return (mReceivedImage!=mEmptyImage);
}

bool BasicVideoSource::isStreaming() const
{
	return this->isConnected() && mStreaming;
}

void BasicVideoSource::setResolution(double resolution)
{
	mRedirecter->SetOutputSpacing(resolution, resolution, resolution);
}

vtkImageDataPtr BasicVideoSource::getVtkImageData()
{
	mRedirecter->Update();
	return mRedirecter->GetOutput();
}

void BasicVideoSource::start()
{
	if (mStreaming)
		return;

	mStreaming = true;

	if (mTimeoutTimer)
	{
		mTimeoutTimer->start();
	}

	if (!this->isConnected())
		return;

	emit streaming(true);
	emit newFrame();
}

void BasicVideoSource::stop()
{
	if (!mStreaming)
		return;

	mStreaming = false;
	if (mTimeoutTimer)
	{
		mTimeoutTimer->stop();
	}

	emit streaming(false);
	emit newFrame();
}

QString BasicVideoSource::getStatusString() const
{
	if (mStatus!="USE_DEFAULT")
		return mStatus;

//	 { return mStatus; }
	if (!this->isConnected())
		return "Not connected";
	if (!this->isStreaming())
		return "Not streaming";
	if (!this->validData())
		return "Timeout";
//	return "Running";
	return "";
}


void BasicVideoSource::setInput(ImagePtr input)
{
	bool wasConnected = this->isConnected();

	if (input)
	{
		mReceivedImage = input;
	}
	else
	{
		if (mReceivedImage)
		{
			// create an empty image with the same uid as the stream.
			mEmptyImage.reset(new Image(mReceivedImage->getUid(), mEmptyImage->getBaseVtkImageData()));
		}
		mReceivedImage = mEmptyImage;
	}
	mRedirecter->SetInputData(mReceivedImage->getBaseVtkImageData());
	mRedirecter->Update();

	if (mTimeoutTimer)
	{
		mTimeout = false;
		mTimeoutTimer->start();
	}

	if (this->isConnected() != wasConnected)
		emit connected(this->isConnected());

	emit newFrame();
}

} // namespace cx
