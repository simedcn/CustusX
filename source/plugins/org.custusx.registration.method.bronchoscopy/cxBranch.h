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
#ifndef BRANCH_H_
#define BRANCH_H_

#include <vector>
#include "cxDataManager.h"
#include "cxMesh.h"
#include "cxVector3D.h"

typedef std::vector<double> dVector;
typedef std::vector<dVector> dVectors;


namespace cx
{

class Branch;
typedef std::vector<Branch*> branchVector;

class Branch
{
	Eigen::MatrixXd positions;
	Eigen::MatrixXd orientations;
	branchVector childBranches;
	Branch* parentBranch;
public:
	Branch();
	virtual ~Branch();
	void setPositions(Eigen::MatrixXd pos);
	Eigen::MatrixXd getPositions();
	void setOrientations(Eigen::MatrixXd orient);
	Eigen::MatrixXd getOrientations();
	void addChildBranch(Branch* child);
	void setChildBranches(branchVector children);
	void deleteChildBranches();
	branchVector getChildBranches();
	void setParentBranch(Branch* parent);
	Branch* getParentBranch();

};


}//namespace cx

#endif /* BRANCH_H_ */


