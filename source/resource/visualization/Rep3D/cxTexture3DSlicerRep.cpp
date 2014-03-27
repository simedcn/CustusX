// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "cxTexture3DSlicerRep.h"

#include <vtkRenderer.h>
#include <vtkFloatArray.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>
#include <vtkImageData.h>
#include <vtkPainterPolyDataMapper.h>
#include <vtkLookupTable.h>

#include "cxImage.h"
#include "cxView.h"
#include "cxImageLUT2D.h"
#include "cxSliceProxy.h"
#include "cxTypeConversions.h"
#include "cxGPUImageBuffer.h"
#include "cxTexture3DSlicerProxy.h"

//---------------------------------------------------------
namespace cx
{
//---------------------------------------------------------

void Texture3DSlicerRep::setTargetSpaceToR()
{
	mProxy->setTargetSpaceToR();
}

Texture3DSlicerRep::Texture3DSlicerRep(const QString& uid) :
	RepImpl(uid)
{
	mProxy = Texture3DSlicerProxy::New();
	mView = NULL;
}

Texture3DSlicerRep::~Texture3DSlicerRep()
{
}

Texture3DSlicerRepPtr Texture3DSlicerRep::New(const QString& uid)
{
	Texture3DSlicerRepPtr retval(new Texture3DSlicerRep(uid));
	retval->mSelf = retval;
	return retval;
}

void Texture3DSlicerRep::setShaderPath(QString path)
{
	mProxy->setShaderPath(path);
}

void Texture3DSlicerRep::viewChanged()
{
	if (!mView)
		return;
	if (mView->getZoomFactor() < 0)
		return; // ignore if zoom is invalid
	mProxy->setViewportData(mView->get_vpMs(), mView->getViewport());
}

void Texture3DSlicerRep::setImages(std::vector<ImagePtr> images)
{
	mProxy->setImages(images);
}

std::vector<ImagePtr> Texture3DSlicerRep::getImages()
{
	return mProxy->getImages();
}

void Texture3DSlicerRep::setSliceProxy(SliceProxyPtr slicer)
{
	mProxy->setSliceProxy(slicer);
}

void Texture3DSlicerRep::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mProxy->getActor());
	mView = view;
	connect(dynamic_cast<QObject *>(view), SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
	this->viewChanged();
}

void Texture3DSlicerRep::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveActor(mProxy->getActor());
	disconnect(dynamic_cast<QObject *>(view), SIGNAL(resized(QSize)), this, SLOT(viewChanged()));
	mView = NULL;
}

void Texture3DSlicerRep::update()
{
	mProxy->update();
}

void Texture3DSlicerRep::printSelf(std::ostream & os, Indent indent)
{

}

void Texture3DSlicerRep::setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp)
{
	mProxy->setViewportData(vpMs, vp);
}

bool Texture3DSlicerRep::isSupported(vtkRenderWindowPtr window)
{
	return Texture3DSlicerProxy::isSupported(window);
}

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------