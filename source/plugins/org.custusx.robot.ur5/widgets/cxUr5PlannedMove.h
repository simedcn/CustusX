#ifndef UR5PLANNEDMOVE_H
#define UR5PLANNEDMOVE_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTextEdit>


namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5PlannedMoveTab : public QWidget
{
    Q_OBJECT
public:
    Ur5PlannedMoveTab(Ur5RobotPtr Ur5Robot, QWidget *parent = 0);
    virtual ~Ur5PlannedMoveTab();


private slots:
    void runVTKfileSlot();
    void openVTKfileSlot();
    void blendRadiusChangedSlot();
    void sendMessageSlot();
    void runVelocityVTKSlot();

    void startLoggingSlot();
    void stopLoggingSlot();
    void clearPoseQueueSlot();
    void moveToInitialPositionButtonSlot();

private:
    QPushButton *runVTKButton, *openVTKButton, *runVelocityVTKButton;
    QPushButton *sendMessageButton;
    QPushButton *startLoggingButton, *stopLoggingButton;
    QPushButton *clearPoseQueueButton;
    QPushButton *moveToInitialPositionButton;

    QLineEdit *vtkLineEdit;

    QLineEdit *accelerationLineEdit, *velocityLineEdit, *timeLineEdit, *blendRadiusLineEdit;


    void setupUi(QWidget *parent);
    Ur5RobotPtr mUr5Robot;

    void setMoveVTKWidget(QVBoxLayout *parent);
    void setMoveSettingsWidget(QVBoxLayout *parent);
    void setTextEditorWidget(QVBoxLayout *parent);

    QTextEdit *textEditor;
};

} // cx

#endif // UR5PLANNEDMOVE_H
