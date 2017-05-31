#include "cxFileManagerServiceNull.h"

#include "cxLogger.h"

namespace cx
{
FileManagerServiceNull::FileManagerServiceNull()
{

}

FileManagerServiceNull::~FileManagerServiceNull()
{}

bool FileManagerServiceNull::isNull()
{
	return true;
}

bool FileManagerServiceNull::canLoad(const QString &type, const QString &filename)
{
	printWarning();
	return false;
}

DataPtr FileManagerServiceNull::load(const QString &uid, const QString &filename)
{
	printWarning();
	return DataPtr();
}

QString FileManagerServiceNull::canLoadDataType() const
{
	printWarning();
	return "";
}

bool FileManagerServiceNull::readInto(DataPtr data, QString path)
{
	printWarning();
	return false;
}

QString FileManagerServiceNull::findDataTypeFromFile(QString filename)
{
	printWarning();
	return "";
}

vtkPolyDataPtr FileManagerServiceNull::loadVtkPolyData(QString filename)
{
	printWarning();
	return vtkPolyDataPtr();
}

vtkImageDataPtr FileManagerServiceNull::loadVtkImageData(QString filename)
{
	printWarning();
	return vtkImageDataPtr();
}

void FileManagerServiceNull::save(DataPtr data, const QString &filename)
{
	printWarning();printWarning();
}

void FileManagerServiceNull::addFileReaderWriter(FileReaderWriterService *service)
{
	printWarning();
}

void FileManagerServiceNull::removeFileReaderWriter(FileReaderWriterService *service)
{
	printWarning();
}

void FileManagerServiceNull::printWarning() const
{
	reportWarning("Trying to use FileManagerServiceNull.");
}


}
