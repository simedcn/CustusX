#ifndef CXPOINTSAMPLINGWIDGET_H_
#define CXPOINTSAMPLINGWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtGui>
#include "sscForwardDeclarations.h"
#include "sscLandmark.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{

/**
 * \class PointSamplingWidget
 *
 * \date 2010.05.05
 * \author: Christian Askeland, SINTEF
 */
class PointSamplingWidget : public BaseWidget
{
  Q_OBJECT

public:
  PointSamplingWidget(QWidget* parent);
  virtual ~PointSamplingWidget();

  virtual QString defaultWhatsThis() const;

signals:

protected slots:
	void updateSlot();
	void itemSelectionChanged();

  void addButtonClickedSlot();
  void editButtonClickedSlot();
  void removeButtonClickedSlot();
  void gotoButtonClickedSlot();
  void loadReferencePointsSlot();
  void testSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void setManualTool(const ssc::Vector3D& p_r);
  ssc::Vector3D getSample() const;
  void enablebuttons();
  void addPoint(ssc::Vector3D point);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
  typedef std::vector<ssc::Landmark> LandmarkVector;
  LandmarkVector mSamples;
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  QPushButton* mAddButton; ///< the Add Landmark button
  QPushButton* mEditButton; ///< the Edit Landmark button
  QPushButton* mRemoveButton; ///< the Remove Landmark button
  QPushButton* mTestButton;
  QPushButton* mLoadReferencePointsButton; ///< button for loading a reference tools reference points

//private:
//  PointSamplingWidget();

};

}//end namespace cx


#endif /* CXPOINTSAMPLINGWIDGET_H_ */
