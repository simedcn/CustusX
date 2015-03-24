/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxImageStreamerOpenCV.h"
#include "cxConfig.h"

#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QHostAddress>
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlServerSocket.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkMetaImageReader.h"
#include "vtkImageImport.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkMetaImageWriter.h"

#include "cxTypeConversions.h"
#include "cxCommandlineImageStreamerFactory.h"
#include "cxStringHelpers.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"

#ifdef CX_USE_OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif

namespace cx
{

std::vector<PropertyPtr> ImageStreamerOpenCVArguments::getSettings(QDomElement root)
{
	std::vector<PropertyPtr> retval;
	retval.push_back(this->getVideoPortOption(root));
	retval.push_back(this->getPrintPropertiesOption(root));
	return retval;
}

DoublePropertyBasePtr ImageStreamerOpenCVArguments::getVideoPortOption(QDomElement root)
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("videoport", "Video Port", "Select video source as an integer from 0 and up.", 0, DoubleRange(0, 10, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSPINBOX);
	retval->setGroup("OpenCV");
	return retval;
}

BoolPropertyBasePtr ImageStreamerOpenCVArguments::getPrintPropertiesOption(QDomElement root)
{
	BoolPropertyPtr retval;
	bool defaultValue = false;
	retval = BoolProperty::initialize("properties", "Print Properties",
											"When starting OpenCV, print properties to console",
											defaultValue, root);
	retval->setAdvanced(true);
	retval->setGroup("OpenCV");
	return retval;
}

StringMap ImageStreamerOpenCVArguments::convertToCommandLineArguments(QDomElement root)
{
	StringMap retval;
	retval["--type"] = "OpenCV";
	retval["--videoport"] = this->getVideoPortOption(root)->getValueAsVariant().toString();
	if (this->getPrintPropertiesOption(root)->getValue())
		retval["--properties"] = "1";
	return retval;
}

QStringList ImageStreamerOpenCVArguments::getArgumentDescription()
{
	QStringList retval;
#ifdef CX_USE_OpenCV
	retval << "--videoport:		video id,     default=0";
	retval << "--in_width:		width of incoming image, default=camera";
	retval << "--in_height:		height of incoming image, default=camera";
	retval << "--out_width:		width of outgoing image, default=camera";
	retval << "--out_height:		width of outgoing image, default=camera";
	retval << "--properties:		dump image properties";
#endif
	return retval;
}

} // namespace cx

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

namespace
{

//------------------------------------------------------------
// Function to generate random matrix.
void GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
	//float position[3];
	//float orientation[4];

	matrix[0][0] = 1.0;
	matrix[1][0] = 0.0;
	matrix[2][0] = 0.0;
	matrix[3][0] = 0.0;
	matrix[0][1] = 0.0;
	matrix[1][1] = -1.0;
	matrix[2][1] = 0.0;
	matrix[3][1] = 0.0;
	matrix[0][2] = 0.0;
	matrix[1][2] = 0.0;
	matrix[2][2] = 1.0;
	matrix[3][2] = 0.0;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
	matrix[3][3] = 1.0;
}

}

namespace cx
{
ImageStreamerOpenCV::ImageStreamerOpenCV()
//	ImageStreamer(parent),
//	mSendTimer(0),
//	mGrabTimer(0)
{
	mGrabbing = false;
	mAvailableImage = false;
	setSendInterval(40);

#ifdef CX_USE_OpenCV
	mVideoCapture.reset(new cv::VideoCapture());
#endif
//	mGrabTimer = new QTimer(this);
//	connect(mGrabTimer, SIGNAL(timeout()), this, SLOT(grab())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mSendTimer = new QTimer(this);
	connect(mSendTimer, SIGNAL(timeout()), this, SLOT(send())); // this signal will be executed in the thread of THIS, i.e. the main thread.
}

ImageStreamerOpenCV::~ImageStreamerOpenCV()
{
	this->deinitialize_local();
}

QString ImageStreamerOpenCV::getType()
{
	return "OpenCV";
}

QStringList ImageStreamerOpenCV::getArgumentDescription()
{
	return ImageStreamerOpenCVArguments().getArgumentDescription();
}


void ImageStreamerOpenCV::initialize(StringMap arguments)
{
	CommandLineStreamer::initialize(arguments);
}

void ImageStreamerOpenCV::deinitialize_local()
{
#ifdef CX_USE_OpenCV
	while (mGrabbing) // grab() method seems to call processmessages itself...
		qApp->processEvents();
	mVideoCapture->release();
	mVideoCapture.reset(new cv::VideoCapture());
#endif
}

void ImageStreamerOpenCV::initialize_local()
{
//	for (StringMap::iterator i=mArguments.begin(); i!=mArguments.end(); ++i)
//	{
//		std::cout << "A: " << i->first << " = " << i->second << std::endl;
//	}

	if (!mArguments.count("videoport"))
		mArguments["videoport"] = "0";
	if (!mArguments.count("out_width"))
		mArguments["out_width"] = "";
	if (!mArguments.count("out_height"))
		mArguments["out_height"] = "";
    if (!mArguments.count("in_width"))
        mArguments["in_width"] = "";
    if (!mArguments.count("in_height"))
        mArguments["in_height"] = "";

	QString videoSource = mArguments["videoport"];
	int videoport = convertStringWithDefault(mArguments["videoport"], 0);

	bool sourceIsInt = false;
	videoSource.toInt(&sourceIsInt);

#ifdef CX_USE_OpenCV
	if (sourceIsInt){
	    // open device (camera)
		mVideoCapture->open(videoport);
	}
	else{
        // open file
		mVideoCapture->open(videoSource.toStdString().c_str());
	}

	if (!mVideoCapture->isOpened())
	{
		cerr << "ImageStreamerOpenCV: Failed to open a video device or video file!\n" << endl;
		return;
	}
	else
	{
		//determine default values
		int default_width = mVideoCapture->get(CV_CAP_PROP_FRAME_WIDTH);
		int default_height = mVideoCapture->get(CV_CAP_PROP_FRAME_HEIGHT);

		//set input size
		int in_width = convertStringWithDefault(mArguments["in_width"], default_width);
		int in_height = convertStringWithDefault(mArguments["in_height"], default_height);
		mVideoCapture->set(CV_CAP_PROP_FRAME_WIDTH, in_width);
		mVideoCapture->set(CV_CAP_PROP_FRAME_HEIGHT, in_height);

		//set output size (resize)
		int out_width = convertStringWithDefault(mArguments["out_width"], in_width);
		int out_height = convertStringWithDefault(mArguments["out_height"], in_height);
		mRescaleSize.setWidth(out_width);
		mRescaleSize.setHeight(out_height);

		if (mArguments.count("properties"))
			this->dumpProperties();

		std::cout << "ImageStreamerOpenCV: Started streaming from openCV device "
			<< videoSource.toStdString()
			<< ", size=(" << in_width << "," << in_height << ")";
		if (( in_width!=mRescaleSize.width() )|| (in_height!=mRescaleSize.height()))
			std::cout << ". Scaled to (" << mRescaleSize.width() << "," << mRescaleSize.height() << ")";

		std::cout << std::endl;
	}
#endif
}

bool ImageStreamerOpenCV::startStreaming(SenderPtr sender)
{
	this->initialize_local();

	if (!mSendTimer)
//	if (!mGrabTimer || !mSendTimer)
	{
		std::cout << "ImageStreamerOpenCV: Failed to start streaming: Not initialized." << std::endl;
		return false;
	}

	mSender = sender;
//	mGrabTimer->start(0);
	mSendTimer->start(getSendInterval());
	this->continousGrabEvent(); // instead of grabtimer
//	mCounter.start();
//	std::cout << "*** ImageStreamerOpenCV: Started" << std::endl;

	return true;
}

void ImageStreamerOpenCV::stopStreaming()
{
	if (!mSendTimer)
//	if (!mGrabTimer || !mSendTimer)
		return;
//	mGrabTimer->stop();
	mSendTimer->stop();
	mSender.reset();

	this->deinitialize_local();
}

void ImageStreamerOpenCV::dumpProperties()
{
#ifdef CX_USE_OpenCV
	this->dumpProperty(CV_CAP_PROP_POS_MSEC, "CV_CAP_PROP_POS_MSEC");
	this->dumpProperty(CV_CAP_PROP_POS_FRAMES, "CV_CAP_PROP_POS_FRAMES");
	this->dumpProperty(CV_CAP_PROP_POS_AVI_RATIO, "CV_CAP_PROP_POS_AVI_RATIO");
	this->dumpProperty(CV_CAP_PROP_FRAME_WIDTH, "CV_CAP_PROP_FRAME_WIDTH");
	this->dumpProperty(CV_CAP_PROP_FRAME_HEIGHT, "CV_CAP_PROP_FRAME_HEIGHT");
	this->dumpProperty(CV_CAP_PROP_FPS, "CV_CAP_PROP_FPS");
	this->dumpProperty(CV_CAP_PROP_FOURCC, "CV_CAP_PROP_FOURCC");
	this->dumpProperty(CV_CAP_PROP_FRAME_COUNT, "CV_CAP_PROP_FRAME_COUNT");
	this->dumpProperty(CV_CAP_PROP_FORMAT, "CV_CAP_PROP_FORMAT");
	this->dumpProperty(CV_CAP_PROP_MODE, "CV_CAP_PROP_MODE");
	this->dumpProperty(CV_CAP_PROP_BRIGHTNESS, "CV_CAP_PROP_BRIGHTNESS");
	this->dumpProperty(CV_CAP_PROP_CONTRAST, "CV_CAP_PROP_CONTRAST");
	this->dumpProperty(CV_CAP_PROP_SATURATION, "CV_CAP_PROP_SATURATION");
	this->dumpProperty(CV_CAP_PROP_HUE, "CV_CAP_PROP_HUE");
	this->dumpProperty(CV_CAP_PROP_GAIN, "CV_CAP_PROP_GAIN");
	this->dumpProperty(CV_CAP_PROP_EXPOSURE, "CV_CAP_PROP_EXPOSURE");
	this->dumpProperty(CV_CAP_PROP_CONVERT_RGB, "CV_CAP_PROP_CONVERT_RGB");
	//  this->dumpProperty(CV_CAP_PROP_WHITE_BALANCE, "CV_CAP_PROP_WHITE_BALANCE");
	this->dumpProperty(CV_CAP_PROP_RECTIFICATION, "CV_CAP_PROP_RECTIFICATION");
#endif
}

void ImageStreamerOpenCV::dumpProperty(int val, QString name)
{
#ifdef CX_USE_OpenCV
	double value = mVideoCapture->get(val);
	if (value != -1)
		std::cout << "Property " << name.toStdString() << " : " << mVideoCapture->get(val) << std::endl;
#endif
}

/**
 * Grab image, then post an event asking for a new grab.
 *
 * This is an alternative to using a QTimer with timeout 0.
 */
void ImageStreamerOpenCV::continousGrabEvent()
{
	if (!mSendTimer->isActive())
		return;
	this->grab();
	QMetaObject::invokeMethod(this, "continousGrabEvent", Qt::QueuedConnection);
}

void ImageStreamerOpenCV::grab()
{
#ifdef CX_USE_OpenCV
	if (!mVideoCapture->isOpened())
	{
		return;
	}
	mGrabbing = true;
	// grab images from camera to opencv internal buffer, do not process
	mVideoCapture->grab();
	mLastGrabTime = QDateTime::currentDateTime();
	mAvailableImage = true;
	mGrabbing = false;
#endif
}

void ImageStreamerOpenCV::send()
{
	if (!mSender || !mSender->isReady())
		return;
	if (!mAvailableImage)
	{
//		reportDebug("dropped resend of frame");
		return;
	}
	PackagePtr package(new Package());
	package->mIgtLinkImageMessage = this->getImageMessage();
	mSender->send(package);
	mAvailableImage = false;

//	static int counter=0;
//	if (++counter%50==0)
//		std::cout << "=== ImageStreamerOpenCV   send: " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
}

IGTLinkImageMessage::Pointer ImageStreamerOpenCV::getImageMessage()
{
	IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();

#ifdef CX_USE_OpenCV
	if (!mVideoCapture->isOpened())
		return IGTLinkImageMessage::Pointer();

	QTime start = QTime::currentTime();

	cv::Mat frame_source;
	//  mVideoCapture >> frame_source;
	if (!mVideoCapture->retrieve(frame_source, 0))
		return IGTLinkImageMessage::Pointer();

	if (this->thread() == QCoreApplication::instance()->thread() && !mSender)
	{
		cv::imshow("ImageStreamerOpenCV", frame_source);
	}

	//  std::cout << "grab" << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
	//  return igtl::ImageMessage::Pointer();

	igtl::TimeStamp::Pointer timestamp;
	timestamp = igtl::TimeStamp::New();
	//  double now = 1.0/1000*(double)QDateTime::currentDateTime().toMSecsSinceEpoch();
	double grabTime = 1.0 / 1000 * (double) mLastGrabTime.toMSecsSinceEpoch();
	timestamp->SetTime(grabTime);
	static QDateTime lastlastGrabTime = mLastGrabTime;
//	std::cout << "OpenCV stamp:\t" << mLastGrabTime.toString("hh:mm:ss.zzz").toStdString() << std::endl;
//	std::cout << "OpenCV diff:\t" <<lastlastGrabTime.msecsTo(mLastGrabTime) << "\tdelay:\t" << mLastGrabTime.msecsTo(QDateTime::currentDateTime()) << std::endl;
	lastlastGrabTime = mLastGrabTime;

	cv::Mat frame = frame_source;
	if (( frame.cols!=mRescaleSize.width() )|| (frame.rows!=mRescaleSize.height()))
	{
		cv::resize(frame_source, frame, cv::Size(mRescaleSize.width(), mRescaleSize.height()), 0, 0, CV_INTER_LINEAR);
	}

	//  std::cout << "grab " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
//	std::cout << "WH=("<< frame.cols << "," << frame.rows << ")" << ", Channels,Depth=("<< frame.channels() << "," << frame.depth() << ")" << std::endl;


	int size[] =
	{ 1.0, 1.0, 1.0 }; // spacing (mm/pixel)
	size[0] = frame.cols;
	size[1] = frame.rows;

	float spacingF[] =
	{ 1.0, 1.0, 1.0 }; // spacing (mm/pixel)
	int* svsize = size;
	int svoffset[] =
	{ 0, 0, 0 }; // sub-volume offset
	int scalarType = -1;

	if (frame.channels() == 3 || frame.channels() == 4)
	{
		scalarType = IGTLinkImageMessage::TYPE_UINT32;// scalar type
	}
	else if (frame.channels() == 1)
	{
		if (frame.depth() == 16)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT16;// scalar type
		}
		else if (frame.depth() == 8)
		{
			scalarType = IGTLinkImageMessage::TYPE_UINT8;// scalar type
		}
	}

	if (scalarType == -1)
	{
		std::cerr << "unknown image type" << std::endl;
		return IGTLinkImageMessage::Pointer();
	}
	//------------------------------------------------------------
	// Create a new IMAGE type message
//	IGTLinkImageMessage::Pointer imgMsg = IGTLinkImageMessage::New();
	imgMsg->SetDimensions(size);
	imgMsg->SetSpacing(spacingF);
	imgMsg->SetScalarType(scalarType);
	imgMsg->SetSubVolume(svsize, svoffset);
	imgMsg->AllocateScalars();
	imgMsg->SetTimeStamp(timestamp);

	unsigned char* destPtr = reinterpret_cast<unsigned char*> (imgMsg->GetScalarPointer());
	uchar* src = frame.data;
	//  std::cout << "pre copy " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
	int N = size[0] * size[1];
	QString colorFormat;

	if (frame.channels() >= 3)
	{
		if (frame.isContinuous())
		{
			// 3-channel continous colors
			for (int i = 0; i < N; ++i)
			{
				*destPtr++ = 255;
				*destPtr++ = src[2];
				*destPtr++ = src[1];
				*destPtr++ = src[0];
				src += 3;
			}
		}
		else
		{
//			std::cout << "noncontinous conversion, rows=" << size[1] << std::endl;
			for (int i=0; i<size[1]; ++i)
			{
				 const uchar* src = frame.ptr<uchar>(i);
				 for (int j=0; j<size[0]; ++j)
				 {
						*destPtr++ = 255;
						*destPtr++ = src[2];
						*destPtr++ = src[1];
						*destPtr++ = src[0];
						src += 3;
				 }
			}
		}
		colorFormat = "ARGB";
	}
	if (frame.channels() == 1)
	{
		if (!frame.isContinuous())
		{
			std::cout << "Error: Non-continous frame data." << std::endl;
			return IGTLinkImageMessage::Pointer();
		}

		// BW camera
		for (int i = 0; i < N; ++i)
		{
			*destPtr++ = *src++;
		}
		colorFormat = "R";
	}

	imgMsg->SetDeviceName(cstring_cast(QString("cxOpenCV [%1]").arg(colorFormat)));

	//------------------------------------------------------------
	// Get randome orientation matrix and set it.
	igtl::Matrix4x4 matrix;
	GetRandomTestMatrix(matrix);
	imgMsg->SetMatrix(matrix);

//	  std::cout << "   grab+process " << start.msecsTo(QTime::currentTime()) << " ms" << std::endl;
#endif
	return imgMsg;
}

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

}
