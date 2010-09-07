/*
 * cxViewWrapper3D.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: christiana
 */

#include "cxViewWrapper3D.h"
#include <vector>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <vtkTransform.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTool2DRep.h"
#include "sscOrientationAnnotationRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "sscToolManager.h"
#include "sscSlicePlanes3DRep.h"
#include "cxLandmarkRep.h"
#include "cxRepManager.h"
#include "sscDataManager.h"
#include "sscMesh.h"

namespace cx
{


ViewWrapper3D::ViewWrapper3D(int startIndex, ssc::View* view)
{
  mView = view;
  this->connectContextMenu(mView);
  std::string index = QString::number(startIndex).toStdString();

  view->getRenderer()->GetActiveCamera()->SetParallelProjection(true);

  mLandmarkRep = repManager()->getLandmarkRep("LandmarkRep_"+index);
  mProbeRep = repManager()->getProbeRep("ProbeRep_"+index);

  // plane type text rep
  mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_"+mView->getName(), "");
  mPlaneTypeText->addText(ssc::Vector3D(0,1,0), "3D", ssc::Vector3D(0.98, 0.02, 0.0));
  mView->addRep(mPlaneTypeText);

  //data name text rep
  mDataNameText = ssc::DisplayTextRep::New("dataNameText_"+mView->getName(), "");
  mDataNameText->addText(ssc::Vector3D(0,1,0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
  mView->addRep(mDataNameText);

  connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(toolsAvailableSlot()));
  this->toolsAvailableSlot();
}

ViewWrapper3D::~ViewWrapper3D()
{
  if (mView)
    mView->removeReps();
}

void ViewWrapper3D::appendToContextMenu(QMenu& contextMenu)
{
  QAction* slicePlanesAction = new QAction("Show Slice Planes", &contextMenu);
  slicePlanesAction->setCheckable(true);
  slicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
  connect(slicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(showSlicePlanesActionSlot(bool)));

  QAction* fillSlicePlanesAction = new QAction("Fill Slice Planes", &contextMenu);
  fillSlicePlanesAction->setCheckable(true);
  fillSlicePlanesAction->setEnabled(mSlicePlanes3DRep->getProxy()->getVisible());
  fillSlicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getDrawPlanes());
  connect(fillSlicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(fillSlicePlanesActionSlot(bool)));

  QAction* resetCameraAction = new QAction("Reset Camera (r)", &contextMenu);
  //resetCameraAction->setCheckable(true);
  //resetCameraAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
  connect(resetCameraAction, SIGNAL(triggered()), this, SLOT(resetCameraActionSlot()));

  contextMenu.addSeparator();
  contextMenu.addAction(resetCameraAction);
  contextMenu.addSeparator();
  contextMenu.addAction(slicePlanesAction);
  contextMenu.addAction(fillSlicePlanesAction);
}

void ViewWrapper3D::resetCameraActionSlot()
{
  mView->getRenderer()->ResetCamera();
}

void ViewWrapper3D::showSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setVisible(checked);
}
void ViewWrapper3D::fillSlicePlanesActionSlot(bool checked)
{
  mSlicePlanes3DRep->getProxy()->setDrawPlanes(checked);
}

void ViewWrapper3D::imageAdded(ssc::ImagePtr image)
{
  if (!mVolumetricReps.count(image->getUid()))
  {
    ssc::VolumetricRepPtr rep = repManager()->getVolumetricRep(image);

    mVolumetricReps[image->getUid()] = rep;
    mView->addRep(rep);
  }

  mProbeRep->setImage(image);
  mLandmarkRep->setImage(image);

  updateView();

  mView->getRenderer()->ResetCamera();
}

void ViewWrapper3D::updateView()
{
  std::vector<ssc::ImagePtr> images = mViewGroup->getImages();

  //update data name text rep
  QStringList text;
  for (unsigned i = 0; i < images.size(); ++i)
  {
    text << qstring_cast(images[i]->getName());
  }
  mDataNameText->setText(0, string_cast(text.join("\n")));
}

void ViewWrapper3D::imageRemoved(const QString& uid)
{
  std::string suid = string_cast(uid);

  if (!mVolumetricReps.count(suid))
    return;

  ssc::messageManager()->sendDebug("Remove image from view group 3d: "+suid);
  mView->removeRep(mVolumetricReps[suid]);
  mVolumetricReps.erase(suid);

  if (mProbeRep->getImage() && mProbeRep->getImage()->getUid()==suid)
    mProbeRep->setImage(ssc::ImagePtr());
  if (mLandmarkRep->getImage() && mLandmarkRep->getImage()->getUid()==suid)
    mLandmarkRep->setImage(ssc::ImagePtr());

  this->updateView();
}

void ViewWrapper3D::meshAdded(ssc::MeshPtr data)
{
  ssc::GeometricRepPtr rep = ssc::GeometricRep::New(data->getUid()+"_geom_rep");
  rep->setMesh(data);
  mGeometricReps[data->getUid()] = rep;
  mView->addRep(rep);
  this->updateView();

  mView->getRenderer()->ResetCamera();
}

void ViewWrapper3D::meshRemoved(const QString& uid)
{
  std::string suid = string_cast(uid);

  if (!mGeometricReps.count(suid))
    return;

  mView->removeRep(mGeometricReps[suid]);
  mGeometricReps.erase(suid);
  this->updateView();
}
  
ssc::View* ViewWrapper3D::getView()
{
  return mView;
}

void ViewWrapper3D::dominantToolChangedSlot()
{
  ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
  mProbeRep->setTool(dominantTool);
}


void ViewWrapper3D::toolsAvailableSlot()
{
 // std::cout <<"void ViewWrapper3D::toolsAvailableSlot() " << std::endl;
  // we want to do this also when nonconfigured and manual tool is present
//  if (!toolManager()->isConfigured())
//    return;

  ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
  ssc::ToolManager::ToolMapPtr::value_type::iterator iter;
  for (iter=tools->begin(); iter!=tools->end(); ++iter)
  {
    if(iter->second->getType() == ssc::Tool::TOOL_REFERENCE)
      continue;

    std::string uid = iter->second->getUid()+"_rep3d_"+this->mView->getUid();
    if (!mToolReps.count(uid))
    {
      mToolReps[uid] = ssc::ToolRep3D::New(uid);
      repManager()->addToolRep3D(mToolReps[uid]);
    }
    ssc::ToolRep3DPtr toolRep = mToolReps[uid];
//    std::cout << "setting 3D tool rep for " << iter->second->getName() << std::endl;
    toolRep->setTool(iter->second);
    toolRep->setOffsetPointVisibleAtZeroOffset(true);
    mView->addRep(toolRep);
    ssc::messageManager()->sendDebug("ToolRep3D for tool "+iter->second->getName()+" added to view "+mView->getName()+".");
  }

//
//  ToolRep3DMap* toolRep3DMap = repManager()->getToolRep3DReps();
//  ToolRep3DMap::iterator repIt = toolRep3DMap->begin();
//  ssc::ToolManager::ToolMapPtr tools = toolManager()->getTools();
//  ssc::ToolManager::ToolMap::iterator toolIt = tools->begin();
//
//  while((toolIt != tools->end()) && (repIt != toolRep3DMap->end()))
//  {
//    if(toolIt->second->getType() != ssc::Tool::TOOL_REFERENCE)
//    {
//      std::cout << "setting tool rep " << toolIt->second->getName() << std::endl;
//      repIt->second->setTool(toolIt->second);
//      mView->addRep(repIt->second);
//      repIt++;
//    }
//    toolIt++;
//  }
}


void ViewWrapper3D::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
  if (mode==ssc::rsNOT_REGISTRATED)
  {
    mView->removeRep(mLandmarkRep);
    mView->removeRep(mProbeRep);
    
    disconnect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
  }
  if (mode==ssc::rsIMAGE_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
    mView->addRep(mProbeRep);

    connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const std::string&)), this, SLOT(dominantToolChangedSlot()));
    this->dominantToolChangedSlot();
  }
  if (mode==ssc::rsPATIENT_REGISTRATED)
  {
    mView->addRep(mLandmarkRep);
  }
}

void ViewWrapper3D::setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy)
{
  mSlicePlanes3DRep = ssc::SlicePlanes3DRep::New("uid");
  mSlicePlanes3DRep->setProxy(proxy);
  mView->addRep(mSlicePlanes3DRep);
}


//------------------------------------------------------------------------------


}
