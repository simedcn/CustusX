#ifndef CXFILEPREVIEWWIDGET_H_
#define CXFILEPREVIEWWIDGET_H_

#include "cxWhatsThisWidget.h"

class QTextDocument;
class QTextEdit;

namespace cx
{
/**
 *\class FilePreviewWidget
 *
 * \brief
 *
 * \date Mar 22, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class FilePreviewWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  FilePreviewWidget(QWidget* parent);
  virtual ~FilePreviewWidget();

  virtual QString defaultWhatsThis() const;

  void previewFile(QString absoluteFilePath);

private:
  QTextDocument* mTextDocument;
  QTextEdit*     mTextEdit;
};

}

#endif /* CXFILEPREVIEWWIDGET_H_ */
