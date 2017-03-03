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


#include "catch.hpp"
#include <vtkOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>

#include "vtkForwardDeclarations.h"
#include "cxSharedOpenGLContext.h"
#include "cxImage.h"
#include "cxRenderWindowFactory.h"


namespace cxtest
{

cx::ImagePtr createDummyImage(int number = 0, int axisSize = 10)
{
	vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(axisSize, 255);
	QString uid = QString("dummyImageUid%1").arg(number);
	QString name = QString("dummyImageName%1").arg(number);
	cx::ImagePtr image = cx::ImagePtr(new cx::Image(uid, dummyImageData, name));
	return image;
}

TEST_CASE("SharedOpenGLContext init", "[opengl][resource][visualization][integration][jb]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	CHECK(renderWindowFactory->getRenderWindow("TestWindowUid"));
	CHECK(renderWindowFactory->getSharedRenderWindow());
	CHECK(renderWindowFactory->getSharedOpenGLContext());
}

TEST_CASE("SharedOpenGLContext render", "[opengl][resource][visualization][integration][jb]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	vtkRenderWindowPtr renderWindow  = renderWindowFactory->getRenderWindow("TestWindowUid");
	REQUIRE(renderWindow);
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);

	renderWindow->Render();
}

TEST_CASE("SharedOpenGLContext upload texture", "[opengl][resource][visualization][integration][jb]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	REQUIRE(renderWindowFactory->getRenderWindow("TestWindowUid"));
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);

	cx::ImagePtr image0 = createDummyImage(0);
	cx::ImagePtr image1 = createDummyImage(1);

	REQUIRE(sharedOpenGLContext->uploadImage(image0));
	REQUIRE(sharedOpenGLContext->uploadImage(image1));

	REQUIRE(sharedOpenGLContext->hasUploadedImage(image0->getUid()));
	REQUIRE(sharedOpenGLContext->hasUploadedImage(image1->getUid()));
}

TEST_CASE("SharedOpenGLContext download texture", "[opengl][resource][visualization][integration][jb]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	vtkRenderWindowPtr renderWindow1 = renderWindowFactory->getRenderWindow("TestWindowUid");
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);

	cx::ImagePtr image0 = createDummyImage(0);
	REQUIRE(sharedOpenGLContext->uploadImage(image0));
	REQUIRE(sharedOpenGLContext->hasUploadedImage(image0->getUid()));


	vtkImageDataPtr imageData = sharedOpenGLContext->downloadImageFromTextureBuffer(image0->getUid());
	REQUIRE(imageData);

	vtkImageDataPtr imageData0 = image0->getBaseVtkImageData();

	char* imagePtr = static_cast<char*>(imageData->GetScalarPointer());
	char* imagePtr0 = static_cast<char*>(imageData0->GetScalarPointer());
	Eigen::Array3i dims(imageData->GetDimensions());
	int size = dims[0]*dims[1]*dims[2];

	for (int i = 0; i < size; ++i)
	{
		INFO(i);
		REQUIRE(imagePtr[i] == imagePtr0[i]);
	}
}

TEST_CASE("SharedOpenGLContext upload many textures", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	REQUIRE(renderWindowFactory->getRenderWindow("TestWindowUid"));
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);

	int num = 1000;

	for (int i = 0; i < num; ++i)
	{
		std::cout << "Upload texture: " << i << std::endl;
		cx::ImagePtr image = createDummyImage(i);
		REQUIRE(sharedOpenGLContext->uploadImage(image));
		REQUIRE(sharedOpenGLContext->hasUploadedImage(image->getUid()));
	}
}

TEST_CASE("SharedOpenGLContext upload many large textures", "[opengl][resource][visualization][manual]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	REQUIRE(renderWindowFactory->getRenderWindow("TestWindowUid"));
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);

	int num = 1000;

	for (int i = 0; i < num; ++i)
	{
		std::cout << "Upload texture: " << i << std::endl;
		cx::ImagePtr image = createDummyImage(i, 500);
		REQUIRE(sharedOpenGLContext->uploadImage(image));
		REQUIRE(sharedOpenGLContext->hasUploadedImage(image->getUid()));
	}
}

TEST_CASE("SharedOpenGLContext upload and download many textures", "[opengl][resource][visualization][manual]")
{
	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	//vtkRenderWindowPtr renderWindow1 = renderWindowFactory->getRenderWindow("TestWindowUid");
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);

	int num = 1000;

	for (int i = 0; i < num; ++i)
	{
		std::cout << "Upload texture: " << i << std::endl;
		cx::ImagePtr image = createDummyImage(i);
		REQUIRE(sharedOpenGLContext->uploadImage(image));
		REQUIRE(sharedOpenGLContext->hasUploadedImage(image->getUid()));

		vtkImageDataPtr downloadedImageData = sharedOpenGLContext->downloadImageFromTextureBuffer(image->getUid());
		REQUIRE(downloadedImageData);

		vtkImageDataPtr originalImageData = image->getBaseVtkImageData();

		char* imagePtr = static_cast<char*>(downloadedImageData->GetScalarPointer());
		char* originalImagePtr = static_cast<char*>(originalImageData->GetScalarPointer());
		Eigen::Array3i dims(originalImageData->GetDimensions());
		int size = dims[0]*dims[1]*dims[2];

		for (int i = 0; i < size; ++i)
		{
			INFO(i);
			REQUIRE(imagePtr[i] == originalImagePtr[i]);
		}
	}
}


TEST_CASE("SharedOpenGLContext can delete uploaded resources.", "[opengl][integration][shared_context][jb]")
{

	cx::RenderWindowFactoryPtr renderWindowFactory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::SharedOpenGLContextPtr sharedOpenGLContext = renderWindowFactory->getSharedOpenGLContext();
	REQUIRE(sharedOpenGLContext);
	REQUIRE(sharedOpenGLContext->getNumberOfTexturesInUse() == 0);

	cx::ImagePtr image = createDummyImage(1);
	QString imageUid = image->getUid();

	REQUIRE(sharedOpenGLContext->uploadImage(image));
	REQUIRE(sharedOpenGLContext->hasUploadedImage(imageUid));
	REQUIRE(sharedOpenGLContext->getNumberOfTexturesInUse() == 1);

	REQUIRE(sharedOpenGLContext->delete3DTextureForImage(imageUid));
	REQUIRE(sharedOpenGLContext->getNumberOfTexturesInUse() == 0);
}

}//cxtest
