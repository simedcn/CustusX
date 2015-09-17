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
#include "cxRMPCWidget.h"

#include <vtkPolyData.h>
#include "cxTransform3D.h"
#include "cxDataSelectWidget.h"
#include "cxTrackingService.h"
#include "cxMesh.h"
#include "cxSelectDataStringProperty.h"
#include "cxRecordSessionWidget.h"
#include "cxRecordSession.h"
#include "cxView.h"
#include "cxToolRep3D.h"
#include "cxToolTracer.h"
//#include "cxBronchoscopyRegistration.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxViewService.h"
#include "cxStringProperty.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxTrackingService.h"
#include "cxDoubleProperty.h"
#include "cxProfile.h"
#include "cxHelperWidgets.h"
#include "cxBoolProperty.h"
#include "cxCheckBoxWidget.h"
#include "cxRepContainer.h"
#include "cxWidgetObscuredListener.h"
#include "cxViewGroupData.h"
#include "cxStringPropertySelectTool.h"
#include "cxHelperWidgets.h"
#include "cxAcquisitionService.h"
#include "cxRegServices.h"
#include "cxRecordTrackingWidget.h"
#include <QGroupBox>
#include "cxRegistrationProperties.h"
#include "cxPC_ICP_Widget.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxMeshHelpers.h"
#include "vtkPointData.h"
#include "cxICPWidget.h"
#include "cxMeshInView.h"
#include "cxSpaceListener.h"
#include "cxSpaceProvider.h"

namespace cx
{
RMPCWidget::RMPCWidget(RegServices services, QWidget* parent) :
	ICPRegistrationBaseWidget(services, parent, "org_custusx_registration_method_pointcloud_widget",
						   "Point Cloud Registration")
{
}

void RMPCWidget::setup()
{
	mSpaceListenerMoving = mServices.spaceProvider->createListener();
	mSpaceListenerFixed = mServices.spaceProvider->createListener();
	mSpaceListenerMoving->setSpace(mServices.spaceProvider->getPr());
	connect(mSpaceListenerMoving.get(), &SpaceListener::changed, this, &RMPCWidget::onSpacesChanged);
	connect(mSpaceListenerFixed.get(), &SpaceListener::changed, this, &RMPCWidget::onSpacesChanged);

	mFixedImage.reset(new StringPropertyRegistrationFixedImage(mServices.registrationService, mServices.patientModelService));
	mMovingImage.reset(new StringPropertyRegistrationMovingImage(mServices.registrationService, mServices.patientModelService));

	connect(mServices.registrationService.get(), &RegistrationService::fixedDataChanged,
			this, &RMPCWidget::inputChanged);
	connect(mServices.registrationService.get(), &RegistrationService::movingDataChanged,
			this, &RMPCWidget::inputChanged);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->addWidget(new LabeledComboBoxWidget(this, mFixedImage));
	layout->addWidget(new LabeledComboBoxWidget(this, mMovingImage));

	layout->addWidget(mICPWidget);
	layout->addStretch();

	this->inputChanged();
	this->onSettingsChanged();
}


void RMPCWidget::initializeRegistrator()
{
	DataPtr fixed = mServices.registrationService->getFixedData();
	DataPtr moving = mServices.registrationService->getMovingData();
	QString logPath = mServices.patientModelService->getActivePatientFolder() + "/Logs/";

	mRegistrator->initialize(moving, fixed, logPath);
}

void RMPCWidget::inputChanged()
{
	if (mObscuredListener->isObscured())
		return;

	DataPtr fixed = mServices.registrationService->getFixedData();
	mSpaceListenerFixed->setSpace(mServices.spaceProvider->getD(fixed));

	this->onSpacesChanged();
}

void RMPCWidget::applyRegistration(Transform3D delta)
{
	Transform3D rMpr = mServices.patientModelService->get_rMpr();
	Transform3D new_rMpr = delta*rMpr;//output
	mServices.registrationService->applyPatientRegistration(new_rMpr, "I2P Surface to Surface");

	mServices.registrationService->applyImage2ImageRegistration(delta, "I2P Surface to Surface - correction");

}

void RMPCWidget::onShown()
{
	this->inputChanged();
}


} //namespace cx
