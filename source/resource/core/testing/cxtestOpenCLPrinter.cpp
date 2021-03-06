/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxOpenCLPrinter.h"

namespace cxtest
{

TEST_CASE("OpenCLPrinter: print info about platform and devices", "[unit][OpenCL][OpenCLPrinter]")
{
	cx::OpenCLPrinter::printPlatformAndDeviceInfo();
	CHECK(true);
}

}
