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

#include "cxFastImageRegistrationWidget.h"

#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxRegistrationManager.h"

namespace cx
{

FastImageRegistrationWidget::FastImageRegistrationWidget(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, QWidget* parent, QString objectName, QString windowTitle) :
		LandmarkImageRegistrationWidget(registrationService, patientModelService, parent, objectName, windowTitle)
{
}

FastImageRegistrationWidget::~FastImageRegistrationWidget()
{

}

QString FastImageRegistrationWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Fast image registration.</h3>"
			"<p>Select landmarks in the data set that you want to use for performing fast registration.</p>"
			"<p><i>Click in the dataset and push the add or resample button.</i></p>"
			"</html>";
}

void FastImageRegistrationWidget::performRegistration()
{
  //make sure the masterImage is set
	DataPtr fixedData = mRegistrationService->getFixedData();
  if(!fixedData)
		mRegistrationService->setFixedData(dataManager()->getActiveImage());

  this->updateAvarageAccuracyLabel();
}
//------------------------------------------------------------------------------
PlateImageRegistrationWidget::PlateImageRegistrationWidget(RegistrationServicePtr registrationService, PatientModelServicePtr patientModelService, QWidget* parent) :
		FastImageRegistrationWidget(registrationService, patientModelService, parent, "PlateImageRegistrationWidget", "Plate Image Registration")
{
}

PlateImageRegistrationWidget::~PlateImageRegistrationWidget()
{}

QString PlateImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Image registration.</h3>"
      "<p>Select landmarks in the data set that you want to use for performing plate registration.</p>"
      "<p><i>Click in the dataset and push the add or resample button.</i></p>"
      "</html>";
}

void PlateImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  dataManager()->setLandmarkActive(mActiveLandmark, true);
  LandmarkImageRegistrationWidget::editLandmarkButtonClickedSlot();
}

void PlateImageRegistrationWidget::performRegistration()
{
  FastImageRegistrationWidget::performRegistration();
	mRegistrationService->doFastRegistration_Translation();
}
//------------------------------------------------------------------------------

}//namespace cx
