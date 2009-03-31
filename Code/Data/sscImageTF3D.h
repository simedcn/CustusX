/*
 * sscImageTransferFunctions3D.h
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#ifndef SSCIMAGETRANSFERFUNCTIONS3D_H_
#define SSCIMAGETRANSFERFUNCTIONS3D_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkScalarsToColors> vtkScalarsToColorsPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;


namespace ssc
{

/**Handler for the transfer functions used in 3d image volumes.
 * Used by Image.
 */
class ImageTF3D
{
public:
	ImageTF3D(vtkImageDataPtr base);

	void setOpacityTF(vtkPiecewiseFunctionPtr tf);
	vtkPiecewiseFunctionPtr getOpacityTF();
	void setColorTF(vtkColorTransferFunctionPtr tf);
	vtkColorTransferFunctionPtr getColorTF();

	void setLLR(double val);
	double getLLR() const;
	void setWindow(double val);
	double getWindow() const;
	void setLevel(double val);
	double getLevel() const;
	void setLut(vtkLookupTablePtr lut);
	vtkLookupTablePtr getLut() const;
	double getScalarMax() const;
	void setTable(vtkUnsignedCharArrayPtr table);
	
private:
	void refreshColorTF();
	void refreshOpacityTF();

	//vtkPiecewiseFunctionPtr mGradientOpacityTF; // implement when needed.
	vtkPiecewiseFunctionPtr mOpacityTF;
	vtkColorTransferFunctionPtr mColorTF;
	vtkVolumePropertyPtr mVolumeProperty;
	
	vtkImageDataPtr mBase;
	double mLLR;
	double mWindow;
	double mLevel;
	vtkLookupTablePtr mLut;
};


}

#endif /* SSCIMAGETRANSFERFUNCTIONS3D_H_ */
