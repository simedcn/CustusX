#include "cxPatientRegistrationDockWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <vtkDoubleArray.h>

#include "sscDataManager.h"
#include "sscVector3D.h"
#include "cxVolumetricRep.h"
#include "cxLandmarkRep.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"

/**
 * cxPatientRegistrationDockWidget.cpp
 *
 * \brief
 *
 * \date Feb 3, 2009
 * \author: Janne Beate Bakeng, SINTEF
 * \author Geir Arne Tangen, SINTEF
 */

namespace cx
{
PatientRegistrationDockWidget::PatientRegistrationDockWidget() :
  mGuiContainer(new QWidget(this)),
  mVerticalLayout(new QVBoxLayout(mGuiContainer)),
  mImagesComboBox(new QComboBox(mGuiContainer)),
  mLandmarkTableWidget(new QTableWidget(mGuiContainer)),
  mToolSampleButton(new QPushButton("Sample Tool", mGuiContainer)),
  mDoRegistrationButton(new QPushButton("Do Registration", mGuiContainer)),
  mAccuracyLabel(new QLabel(QString(" "),mGuiContainer)),
  mDataManager(DataManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mCurrentRow(-1),
  mCurrentColumn(-1)
{
  //Dock widget
  this->setWindowTitle("Patient Registration");
  this->setWidget(mGuiContainer);
  connect(this, SIGNAL(visibilityChanged(bool)),
          this, SLOT(visibilityOfDockWidgetChangedSlot(bool)));

  //combobox
  mImagesComboBox->setEditable(false);
  mImagesComboBox->setEnabled(false);
  connect(mImagesComboBox, SIGNAL(currentIndexChanged(const QString& )),
          this, SLOT(imageSelectedSlot(const QString& )));

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int, int)),
          this, SLOT(cellChangedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(rowSelectedSlot(int, int)));

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()),
          this, SLOT(toolSampleButtonClickedSlot()));
  mDoRegistrationButton->setDisabled(true);
  connect(mDoRegistrationButton, SIGNAL(clicked()),
          this, SLOT(doRegistrationButtonClickedSlot()));

  //toolmanager
  connect(mToolManager, SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
  connect(mToolManager, SIGNAL(toolSampleAdded(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));
  connect(mToolManager, SIGNAL(toolSampleRemoved(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));

  //layout
  mVerticalLayout->addWidget(mImagesComboBox);
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mDoRegistrationButton);
  mVerticalLayout->addWidget(mAccuracyLabel);

  ssc::ToolPtr dominantTool = mToolManager->getDominantTool();
  if(dominantTool.get() != NULL)
    this->dominantToolChangedSlot(dominantTool->getUid());
}
PatientRegistrationDockWidget::~PatientRegistrationDockWidget()
{}
void PatientRegistrationDockWidget::imageSelectedSlot(const QString& comboBoxText)
{
  if(comboBoxText.isEmpty() || comboBoxText.endsWith("..."))
    return;

  std::string imageId = comboBoxText.toStdString();

  //find the image
  ssc::ImagePtr image = mDataManager->getImage(imageId);
  if(image.get() == NULL)
  {
    mMessageManager->sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }

  //Set new current image
  mCurrentImage = image;
  mLandmarkActiveMap = mRegistrationManager->getActivePointsMap();

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(image);

  //view3D
  View3D* view3D_1 = mViewManager->get3DView("View3D_1");
  VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  volumetricRep->setImage(mCurrentImage);
  landmarkRep->setImage(mCurrentImage);
  view3D_1->setRep(volumetricRep);
  view3D_1->addRep(landmarkRep);

  //view2D
  View2D* view2D_1 = mViewManager->get2DView("View2D_1");
  View2D* view2D_2 = mViewManager->get2DView("View2D_2");
  View2D* view2D_3 = mViewManager->get2DView("View2D_3");
  InriaRep2DPtr inriaRep2D_1 = mRepManager->getInria2DRep("InriaRep2D_1");
  InriaRep2DPtr inriaRep2D_2 = mRepManager->getInria2DRep("InriaRep2D_2");
  InriaRep2DPtr inriaRep2D_3 = mRepManager->getInria2DRep("InriaRep2D_3");
  view2D_1->setRep(inriaRep2D_1);
  view2D_2->setRep(inriaRep2D_2);
  view2D_3->setRep(inriaRep2D_3);
  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  //TODO: ...or getBaseVtkImageData()???
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(mCurrentImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();

  //link volumetricRep and inriaReps
  connect(volumetricRep.get(), SIGNAL(pointPicked(double,double,double)),
          inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
  connect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),
          volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),
          volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),
          volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
}
void PatientRegistrationDockWidget::visibilityOfDockWidgetChangedSlot(bool visible)
{
  if(visible)
  {
    connect(mDataManager, SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBox()));
    this->populateTheImageComboBox();
  }
  else
  {
    disconnect(mDataManager, SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBox()));

    //update the active vector in registration manager
    mRegistrationManager->setActivePointsMap(mLandmarkActiveMap);
  }
}
void PatientRegistrationDockWidget::toolSampledUpdateSlot(double notUsedX, double notUsedY, double notUsedZ,unsigned int notUsedIndex)
{
  int numberOfToolSamples = mToolManager->getToolSamples()->GetNumberOfTuples();
  int numberOfActiveToolSamples = 0;
  std::map<int, bool>::iterator it = mLandmarkActiveMap.begin();
  while(it != mLandmarkActiveMap.end())
  {
    if(it->second)
      numberOfActiveToolSamples++;
    it++;
  }
  if(numberOfActiveToolSamples >= 3 && numberOfToolSamples >= 3)
  {
    this->doPatientRegistration();
    this->updateAccuracy();
  }
}
void PatientRegistrationDockWidget::toolVisibleSlot(bool visible)
{
  if(visible)
    mToolSampleButton->setEnabled(true);
  else
    mToolSampleButton->setEnabled(false);
}
void PatientRegistrationDockWidget::toolSampleButtonClickedSlot()
{
  ssc::Transform3DPtr lastTransform = mToolToSample->getLastTransform();
  if(lastTransform.get() == NULL)
  {
    std::cout << "if(lastTransform.get() == NULL)" << std::endl;
    return;
  }
  vtkMatrix4x4Ptr lastTransformMatrix = lastTransform->matrix();
  double x = lastTransformMatrix->GetElement(0,3);
  double y = lastTransformMatrix->GetElement(1,3);
  double z = lastTransformMatrix->GetElement(2,3);

  if(mCurrentRow == -1)
    mCurrentRow = 0;
  unsigned int index = mCurrentRow+1;
  mToolManager->addToolSampleSlot(x, y, z, index);
}
void PatientRegistrationDockWidget::doRegistrationButtonClickedSlot()
{
  this->doPatientRegistration();
  this->updateAccuracy();
}
void PatientRegistrationDockWidget::rowSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;
}
void PatientRegistrationDockWidget::populateTheImageComboBox()
{
  mImagesComboBox->clear();

  //find out if the master image is set
  ssc::ImagePtr masterImage = mRegistrationManager->getMasterImage();

  //get a list of images from the datamanager
  std::map<std::string, ssc::ImagePtr> images = mDataManager->getImages();
  if(images.size() == 0 || masterImage.get() == NULL)
  {
    mImagesComboBox->insertItem(1, QString("First do Image Registration..."));
    mImagesComboBox->setEnabled(false);
    return;
  }

  //add these to the combobox
  typedef std::map<std::string, ssc::ImagePtr>::iterator iterator;
  int listPosition = 1;
  for(iterator i = images.begin(); i != images.end(); ++i)
  {
    mImagesComboBox->insertItem(listPosition, QString(i->first.c_str()));
    listPosition++;
  }

  //set the master image as the selected on
  std::string uid = masterImage->getUid();
  int comboboxIndex = mImagesComboBox->findText(QString(uid.c_str()));
  if (comboboxIndex < 0)
    return;

  mImagesComboBox->setCurrentIndex(comboboxIndex);
}
void PatientRegistrationDockWidget::cellChangedSlot(int row, int column)
{
  if (column!=0)
    return;

  Qt::CheckState state = mLandmarkTableWidget->item(row,column)->checkState();
  mLandmarkActiveMap[row] = state;

}
void PatientRegistrationDockWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample.get() != NULL && mToolToSample->getUid() == uid)
    return;

  //ToolPtr newTool = ToolPtr(dynamic_cast<Tool*>(mToolManager->getTool(uid).get()));
  ToolPtr newTool = ToolPtr(dynamic_cast<Tool*>(mToolManager->getDominantTool().get()));
  if(mToolToSample.get() != NULL)
  {
    if(newTool.get() == NULL)
      return;

    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)),
                this, SLOT(toolVisibleSlot(bool)));
  }

  mToolToSample = newTool;
  connect(mToolToSample.get(), SIGNAL(toolVisible(bool)),
              this, SLOT(toolVisibleSlot(bool)));

  //TODO: REMOVE
  ssc::ToolRep3DPtr toolRep3D_1 = mRepManager->getToolRep3DRep("ToolRep3D_1");
  toolRep3D_1->setTool(mToolToSample);
  View3D* view = mViewManager->get3DView("View3D_1");
  view->addRep(toolRep3D_1);

  //update button
  mToolSampleButton->setEnabled(mToolToSample->getVisible());
}
void PatientRegistrationDockWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(3);
  QStringList headerItems(QStringList() << "Active" << "Name" << "Accuracy");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  //fill the table widget with rows for the landmarks
  int row = 0;
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* landmark = landmarks->GetTuple(i);
    std::map<int, double>::iterator it = mLandmarkRegistrationAccuracyMap.find(landmark[3]);
    double landmarkRegistrationAccuracy = it->second;

    if(landmark[3] > mLandmarkTableWidget->rowCount())
      mLandmarkTableWidget->setRowCount(landmark[3]);
    QTableWidgetItem* columnOne;
    QTableWidgetItem* columnTwo;
    QTableWidgetItem* columnThree;

    int rowToInsert = landmark[3]-1;
    int tempRow = -1;
    if(rowToInsert < row)
    {
      tempRow = row;
      row = rowToInsert;
    }
    for(; row <= rowToInsert; row++)
    {
      if(row == rowToInsert)
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem(tr("(%1, %2, %3)").arg(landmark[0]).arg(landmark[1]).arg(landmark[2]));
        columnThree = new QTableWidgetItem(tr("%1").arg(landmarkRegistrationAccuracy));
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
      }
      //check the mLandmarkActiveVector...
      std::map<int, bool>::iterator it = mLandmarkActiveMap.find(row);
      if(it != mLandmarkActiveMap.end())
      {
        if(!it->second)
          columnOne->setCheckState(Qt::Unchecked);
        else
          columnOne->setCheckState(Qt::Checked);
      }
      else
      {
        mLandmarkActiveMap[row] = true;
        columnOne->setCheckState(Qt::Checked);
      }
      columnTwo->setFlags(Qt::ItemIsSelectable);
      columnThree->setFlags(Qt::ItemIsSelectable);
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
      mLandmarkTableWidget->setItem(row, 2, columnThree);
    }
    if(tempRow != -1)
      row = tempRow;
  }
  //fill in names
  typedef RegistrationManager::NameListType::iterator Iterator;
  for(Iterator it = nameList.begin(); it != nameList.end(); ++it)
  {
    std::string name = it->second.first;
    int index = it->first;
    int row = index-1;
    QTableWidgetItem* columnTwo;

    if(index > mLandmarkTableWidget->rowCount())
    {
      mLandmarkTableWidget->setRowCount(index);
      columnTwo = new QTableWidgetItem();
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
    }
    else
    {
      columnTwo = mLandmarkTableWidget->item(row, 1);
      if(columnTwo == NULL) //TODO: remove
        std::cout << "columnTwo == NULL!!!" << std::endl;
    }
    columnTwo->setText(QString(name.c_str()));
  }
}
void PatientRegistrationDockWidget::updateAccuracy()
{
  //ssc:Image masterImage = mRegistrationManager->getMasterImage();
  vtkDoubleArrayPtr globalPointset = mRegistrationManager->getGlobalPointSet();
  vtkDoubleArrayPtr toolSamplePointset = mToolManager->getToolSamples();

  ssc::Transform3DPtr rMpr = mToolManager->get_rMpr();

  int numberOfGlobalImagePoints = globalPointset->GetNumberOfTuples();
  int numberOfToolSamplePoints = toolSamplePointset->GetNumberOfTuples();

  // First reset the accuracy table
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    double* imagePoint = globalPointset->GetTuple(i);
    mLandmarkRegistrationAccuracyMap[imagePoint[3]] = 1000;
  }

  // Calculate and fill the accuracy table
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    for(int j=0; j < numberOfToolSamplePoints; j++)
    {
      double* targetPoint = globalPointset->GetTuple(i);
      double* sourcePoint = toolSamplePointset->GetTuple(j);
      if(sourcePoint[3] == targetPoint[3])
      {
          // Calculate accuracy - Set mLandmarkAccuracy
          //Vector3DPtr sourcePointVector =
          //  Vector3DPtr(new ssc::Vector3D(sourcePoint[0],
          //                                sourcePoint[1], sourcePoint[2]));
          //Vector3DPtr transformedPointVector = rMpr->coord(sourcePointVector);
          ssc::Vector3D sourcePointVector(sourcePoint[0],
                                          sourcePoint[1], sourcePoint[2]);
          ssc::Vector3D transformedPointVector = rMpr->coord(sourcePointVector);
          mLandmarkRegistrationAccuracyMap[sourcePoint[3]] =
              sqrt(pow(transformedPointVector[0],2) +
                    pow(transformedPointVector[1],2) +
                    pow(transformedPointVector[2],2));

      }
    }
  }

  // Calculate total registration accuracy
  mTotalRegistrationAccuracy = 0;
  std::map<int, bool>::iterator it = mLandmarkActiveMap.begin();

  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    if(it->second)
    {
      mTotalRegistrationAccuracy = mTotalRegistrationAccuracy +
                                    mLandmarkRegistrationAccuracyMap[i];
      it++;
    }
  }

  //make sure the accuracy is filled in the table widget
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void PatientRegistrationDockWidget::doPatientRegistration()
{
  mRegistrationManager->doPatientRegistration();
}
}//namespace cx
