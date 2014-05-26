/*
 * cxToolTipSampleWidget.cpp
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#include <cxToolTipSampleWidget.h>

#include <QPushButton>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxToolManager.h"
#include "cxDataManager.h"
#include "cxVector3D.h"
#include "cxDefinitionStrings.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDataLocations.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"

namespace cx
{

ToolTipSampleWidget::ToolTipSampleWidget(QWidget* parent) :
    BaseWidget(parent, "ToolTipSampleWidget", "ToolTip Sample"),
    mSampleButton(new QPushButton("Sample")),
    mSaveToFileNameLabel(new QLabel("<font color=red> No file selected </font>")),
    mSaveFileButton(new QPushButton("Save to...")),
    mTruncateFile(false)
{
  QVBoxLayout* toplayout = new QVBoxLayout(this);

  mCoordinateSystems = SelectCoordinateSystemStringDataAdapter::New();
  mTools = SelectToolStringDataAdapter::New();
  mData = SelectDataStringDataAdapter::New();

  mCoordinateSystemComboBox = new LabeledComboBoxWidget(this, mCoordinateSystems);
  mToolComboBox = new LabeledComboBoxWidget(this, mTools);
  mDataComboBox = new LabeledComboBoxWidget(this, mData);

  toplayout->addWidget(new QLabel("<b>Select coordinate system to sample in: </b>"));
  toplayout->addWidget(mCoordinateSystemComboBox);
  toplayout->addWidget(mToolComboBox);
  toplayout->addWidget(mDataComboBox);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mSampleButton);
  toplayout->addWidget(this->createHorizontalLine());
  toplayout->addWidget(mSaveFileButton);
  toplayout->addWidget(mSaveToFileNameLabel);
  toplayout->addStretch();

  connect(mSaveFileButton, SIGNAL(clicked()), this, SLOT(saveFileSlot()));
  connect(mSampleButton, SIGNAL(clicked()), this, SLOT(sampleSlot()));
  connect(mCoordinateSystems.get(), SIGNAL(changed()), this, SLOT(coordinateSystemChanged()));

  //setting initial state
  this->coordinateSystemChanged();
}

ToolTipSampleWidget::~ToolTipSampleWidget()
{}

QString ToolTipSampleWidget::defaultWhatsThis() const
{
  return "<html>"
     "<h3>Tool tip sampling.</h3>"
     "<p>You can sample the dominant(active) tools tooltip in any coordinate system and get the results written to file.</p>"
     "</html>";
}

void ToolTipSampleWidget::saveFileSlot()
{
  QString configPath = DataLocations::getRootConfigPath();
  if(patientService()->getPatientData()->isPatientValid())
    configPath = patientService()->getPatientData()->getActivePatientFolder();

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                             configPath+"/SampledPoints.txt",
                             tr("Text (*.txt)"));
  if(fileName.isEmpty())
    return;
  else if(QFile::exists(fileName))
    mTruncateFile = true;

  mSaveToFileNameLabel->setText(fileName);
}

void ToolTipSampleWidget::sampleSlot()
{
  QFile samplingFile(mSaveToFileNameLabel->text());

  CoordinateSystem to = this->getSelectedCoordinateSystem();
  Vector3D toolPoint = spaceProvider()->getDominantToolTipPoint(to, false);

  if(!samplingFile.open(QIODevice::WriteOnly | (mTruncateFile ? QIODevice::Truncate : QIODevice::Append)))
  {
    reportWarning("Could not open "+samplingFile.fileName());
    report("Sampled point: "+qstring_cast(toolPoint));
    return;
  }
  else
  {
    if(mTruncateFile)
      mTruncateFile = false;
  }

  QString sampledPoint = qstring_cast(toolPoint);

  QTextStream streamer(&samplingFile);
  streamer << sampledPoint;
  streamer << endl;

  reporter()->playSampleSound();
  report("Sampled point in "+qstring_cast(to.mId)+" ("+to.mRefObject+") space, result: "+sampledPoint);
}

void ToolTipSampleWidget::coordinateSystemChanged()
{
  switch (string2enum<COORDINATE_SYSTEM>(mCoordinateSystems->getValue()))
  {
  case csDATA:
    mDataComboBox->show();
    mToolComboBox->hide();
    break;
  case csTOOL:
    mToolComboBox->show();
    mDataComboBox->hide();
    break;
  case csSENSOR:
    mToolComboBox->show();
    mDataComboBox->hide();
    break;
  default:
    mDataComboBox->hide();
    mToolComboBox->hide();
    break;
  };
}

CoordinateSystem ToolTipSampleWidget::getSelectedCoordinateSystem()
{
  CoordinateSystem retval(csCOUNT);

  retval.mId = string2enum<COORDINATE_SYSTEM>(mCoordinateSystems->getValue());

  switch (retval.mId)
  {
  case csDATA:
	retval = spaceProvider()->getD(mData->getData());
    break;
  case csTOOL:
	retval = spaceProvider()->getT(mTools->getTool());
    break;
  case csSENSOR:
	retval = spaceProvider()->getT(mTools->getTool());
    break;
  default:
    retval.mRefObject = "";
    break;
  };

  return retval;
}
//------------------------------------------------------------------------------


}