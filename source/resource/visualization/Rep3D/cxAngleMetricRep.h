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

#ifndef CXANGLEMETRICREP_H_
#define CXANGLEMETRICREP_H_

#include "cxDataMetricRep.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

typedef vtkSmartPointer<class vtkTextActor> vtkTextActorPtr;

namespace cx
{
typedef boost::shared_ptr<class CaptionText3D> CaptionText3DPtr;
typedef boost::shared_ptr<class GraphicalArc3D> GraphicalArc3DPtr;
}

namespace cx
{

typedef boost::shared_ptr<class AngleMetricRep> AngleMetricRepPtr;
typedef boost::shared_ptr<class AngleMetric> AngleMetricPtr;

/** Rep for visualizing a AngleMetric.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 *
 * \date Jul 27, 2011
 * \author Christian Askeland, SINTEF
 */
class AngleMetricRep: public DataMetricRep
{
Q_OBJECT
public:
	static AngleMetricRepPtr New(const QString& uid, const QString& name = ""); ///constructor
	virtual ~AngleMetricRep() {}

	virtual QString getType() const { return "AngleMetricRep"; }

protected:
    virtual void clear();
	virtual void onModifiedStartRender();

private:
	AngleMetricRep(const QString& uid, const QString& name = "");
	AngleMetricRep(); ///< not implemented
    AngleMetricPtr getAngleMetric();

	GraphicalLine3DPtr mLine0;
	GraphicalLine3DPtr mLine1;
	GraphicalArc3DPtr mArc;
};

}

#endif /* CXANGLEMETRICREP_H_ */