#ifndef SSCSLICERREPSW_H_
#define SSCSLICERREPSW_H_

#include <vtkSmartPointer.h>
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "sscTool.h"
#include "sscRepImpl.h"
#include "sscImage.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"

typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<class vtkImageActor> vtkImageActorPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageBlend> vtkImageBlendPtr;


namespace ssc
{
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;

class SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const std::string& uid);	
	virtual ~SliceRepSW();
	virtual std::string getType() const { return "ssc::SliceRepSW"; };
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	void setInput(vtkImageDataPtr input);
	ImagePtr getImage() { return mImage; };
	vtkImageActorPtr getActor(){ return mImageActor; };
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	std::string getImageUid()const;
	void update();
	//PLANE_TYPE getPlane(){ return mType; }
//	void addIamges(ImagePtr image);
//	void createOverlay();
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	SliceRepSW(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View* view) ;

private slots:
	void sliceTransformChangedSlot(Transform3D sMr); 
		
private:
	//PLANE_TYPE mType;
	ImagePtr mImage;

	Transform3D rMs_debug;
	
	
	SliceProxyPtr mSlicer;
	//std::string mImageUid;
	vtkWindowLevelLookupTablePtr mLookupTable;
	//vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageMapToColorsPtr mWindowLevel;
	vtkImageReslicePtr mReslicer;
	vtkMatrix4x4Ptr mMatrixAxes;
	vtkImageActorPtr mImageActor;
};	

}// namespace ssc

#endif /*SSCSLICERREPSW_H_*/
