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


#include "cxRegistrationTransform.h"
//#include <QtXML>
#include <QDomElement>
#include "cxTime.h"
#include "cxTypeConversions.h"

namespace cx
{

// --------------------------------------------------------
RegistrationHistoryPtr RegistrationHistory::mNull; ///< static member
// --------------------------------------------------------


/**NULL implementation of a registration transform
 */
class RegistrationHistoryNull: public RegistrationHistory
{
public:
	virtual void addXml(QDomNode& parentNode) const
	{
	}
	virtual void parseXml(QDomNode& dataNode)
	{
	}

	virtual void addRegistration(const RegistrationTransform& transform)
	{
	}
	virtual void setRegistration(const Transform3D& transform)
	{
	}
	virtual void updateRegistration(const QDateTime& oldTime, const RegistrationTransform& newTransform)
	{
	}

	virtual void setParentSpace(const QString& newParent)
	{
	}
	virtual void addParentSpace(const QString& newParent)
	{
	}
	virtual void addParentSpace(const ParentSpace& newParent)
	{
	}
	virtual void updateParentSpace(const QDateTime& oldTime, const ParentSpace& newParent)
	{
	}

	virtual std::vector<RegistrationTransform> getData() const
	{
		return std::vector<RegistrationTransform>();
	}
	virtual std::vector<ParentSpace> getParentSpaces() const
	{
		return std::vector<ParentSpace>();
	}
	virtual void removeNewerThan(const QDateTime& timestamp)
	{
	}
	virtual void setActiveTime(const QDateTime& timestamp)
	{
	}
	virtual QDateTime getActiveTime() const
	{
		return QDateTime();
	}
	virtual RegistrationTransform getCurrentRegistration() const
	{
		return RegistrationTransform();
	}
	virtual ParentSpace getCurrentParentSpace()
	{
		return ParentSpace();
	}
	virtual void clear()
	{
	}
	virtual bool isNull() const
	{
		return true;
	}
};

//---------------------------------------------------------
//-------  RegistrationTransform  -------------------------
//---------------------------------------------------------

RegistrationTransform::RegistrationTransform() :
	mValue(Transform3D::Identity())
{
}

RegistrationTransform::RegistrationTransform(const Transform3D& value, const QDateTime& timestamp, const QString& type)
{
	mValue = value;
	mTimestamp = timestamp;
	mType = type;
}

void RegistrationTransform::addXml(QDomNode& parentNode) const ///< write internal state to node
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement base = doc.createElement("registrationTransform");
	parentNode.appendChild(base);

	base.setAttribute("timestamp", mTimestamp.toString(timestampSecondsFormat()));
	base.setAttribute("type", mType);

	if (!mFixed.isEmpty())
		base.setAttribute("fixed", mFixed);
	if (!mMoving.isEmpty())
		base.setAttribute("moving", mMoving);

	base.appendChild(doc.createTextNode("\n" + qstring_cast(mValue)));
}

void RegistrationTransform::parseXml(QDomNode& dataNode)///< read internal state from node
{
	if (dataNode.isNull())
		return;

	QDomElement base = dataNode.toElement();

	mTimestamp = QDateTime::fromString(base.attribute("timestamp"), timestampSecondsFormat());
	mType = base.attribute("type");
	mValue = Transform3D::fromString(base.text());
	mFixed = base.attribute("fixed");
	mMoving = base.attribute("moving");
}

bool operator<(const RegistrationTransform& lhs, const RegistrationTransform& rhs)
{
	return lhs.mTimestamp < rhs.mTimestamp;
}

bool operator==(const RegistrationTransform& lhs, const RegistrationTransform& rhs)
{
	return similar(lhs.mValue, rhs.mValue, 1.0E-3) && (lhs.mTimestamp == rhs.mTimestamp) && (lhs.mType == rhs.mType);
}

//---------------------------------------------------------
//-------  RegistrationTransform  -------------------------
//---------------------------------------------------------

ParentSpace::ParentSpace()
{

}

ParentSpace::ParentSpace(const QString& uid, const QDateTime& timestamp, const QString& type)
{
	mValue = uid;
	mTimestamp = timestamp;
	mType = type;
}

void ParentSpace::addXml(QDomNode& parentNode) const ///< write internal state to node
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement base = doc.createElement("parentFrame");
	parentNode.appendChild(base);

	base.setAttribute("value", mValue);
	base.setAttribute("timestamp", mTimestamp.toString(timestampSecondsFormat()));
	base.setAttribute("type", mType);
}

void ParentSpace::parseXml(QDomNode& dataNode)///< read internal state from node
{
	if (dataNode.isNull())
		return;

	QDomElement base = dataNode.toElement();

	mTimestamp = QDateTime::fromString(base.attribute("timestamp"), timestampSecondsFormat());
	mType = base.attribute("type");
	mValue = base.attribute("value");
}

bool operator<(const ParentSpace& lhs, const ParentSpace& rhs)
{
	return lhs.mTimestamp < rhs.mTimestamp;
}

bool operator==(const ParentSpace& lhs, const ParentSpace& rhs)
{
	return (lhs.mValue == rhs.mValue) && (lhs.mTimestamp == rhs.mTimestamp) && (lhs.mType == rhs.mType);
}

//---------------------------------------------------------
//-------  RegistrationHistory    -------------------------
//---------------------------------------------------------

RegistrationHistoryPtr RegistrationHistory::getNullObject()
{
	if (!mNull)
		mNull.reset(new RegistrationHistoryNull);
	return mNull;
}

void RegistrationHistory::addXml(QDomNode& parentNode) const ///< write internal state to node
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement base = doc.createElement("registrationHistory");
	parentNode.appendChild(base);

	QDomElement currentTime = doc.createElement("currentTime");
	currentTime.appendChild(doc.createTextNode(mCurrentTime.toString(timestampSecondsFormat())));
	base.appendChild(currentTime);

	for (unsigned i = 0; i < mData.size(); ++i)
	{
		mData[i].addXml(base);
	}
	for (unsigned i = 0; i < mParentSpaces.size(); ++i)
	{
		mParentSpaces[i].addXml(base);
	}
}

void RegistrationHistory::parseXml(QDomNode& dataNode)///< read internal state from node
{
	if (dataNode.isNull())
		return;

	mData.clear();
	QString currentTimeRaw = dataNode.namedItem("currentTime").toElement().text();
	QDateTime currentTime = QDateTime::fromString(currentTimeRaw, timestampSecondsFormat());

	// iterate over all data elements
	QDomElement currentElem = dataNode.firstChildElement("registrationTransform");
	for (; !currentElem.isNull(); currentElem = currentElem.nextSiblingElement("registrationTransform"))
	{
		RegistrationTransform transform;
		transform.parseXml(currentElem);
		mData.push_back(transform);
	}

	// iterate over all data elements
	currentElem = dataNode.firstChildElement("parentFrame");
	for (; !currentElem.isNull(); currentElem = currentElem.nextSiblingElement("parentFrame"))
	{
		ParentSpace transform;
		transform.parseXml(currentElem);
		mParentSpaces.push_back(transform);
	}

	std::sort(mData.begin(), mData.end());
	std::sort(mParentSpaces.begin(), mParentSpaces.end());
	setActiveTime(currentTime); // update cache
}

void RegistrationHistory::clear()
{
	mData.clear();
	mCurrentTime = QDateTime();
	mTransformCache = RegistrationTransform();
}

/**Add one registration transform to the history.
 */
void RegistrationHistory::addRegistration(const RegistrationTransform& transform)
{
	if (std::count(mData.begin(), mData.end(), transform)) // ignore if already present
		return;

	mData.push_back(transform);
	std::sort(mData.begin(), mData.end());
	setActiveTime(QDateTime()); // reset to last registration when reregistering.
}

/**Replace the registration performed at oldTime with the new one.
 *
 */
void RegistrationHistory::updateRegistration(const QDateTime& oldTime, const RegistrationTransform& newTransform)
{
	for (std::vector<RegistrationTransform>::iterator iter = mData.begin(); iter != mData.end(); ++iter)
	{
		if ((iter->mTimestamp == oldTime)
			&& oldTime.isValid()
			&& (iter->mType == newTransform.mType))
		{
			mData.erase(iter);
			break;
		}
	}
	this->addRegistration(newTransform);
}

/**Set a registration transform, overwriting all history.
 * Use this when registration history is not needed, or when defining a relationship for the first time.
 */
void RegistrationHistory::setRegistration(const Transform3D& transform)
{
	bool changed = true;
	if (similar(getCurrentRegistration().mValue, transform))
	{
		changed = false;
	}
	mData.clear();
	this->addRegistration(RegistrationTransform(transform));
	if (changed)
	{
		emit currentChanged();
	}
}

/**Set a parent fram, overwriting all history.
 * Use this when registration history is not needed, or when defining a relationship for the first time.
 */
void RegistrationHistory::setParentSpace(const QString& newParent)
{
	mParentSpaces.clear();
	this->addParentSpace(ParentSpace(newParent));
}

void RegistrationHistory::addParentSpace(const QString& newParent)
{
	this->addParentSpace(ParentSpace(newParent, QDateTime::currentDateTime(), "Set Parent Frame"));
}

void RegistrationHistory::addParentSpace(const ParentSpace& newParent)
{
	if (std::count(mParentSpaces.begin(), mParentSpaces.end(), newParent)) // ignore if already present
		return;

	mParentSpaces.push_back(newParent);
	std::sort(mParentSpaces.begin(), mParentSpaces.end());
	setActiveTime(QDateTime()); // reset to last registration when reregistering.
}

void RegistrationHistory::updateParentSpace(const QDateTime& oldTime, const ParentSpace& newParent)
{
	for (std::vector<ParentSpace>::iterator iter = mParentSpaces.begin(); iter != mParentSpaces.end(); ++iter)
	{
		if (iter->mTimestamp != oldTime)
			continue;
		mParentSpaces.erase(iter);
		break;
	}
	this->addParentSpace(newParent);
}

ParentSpace RegistrationHistory::getCurrentParentSpace()
{
	return mParentSpaceCache;
}

std::vector<RegistrationTransform> RegistrationHistory::getData() const
{
	return mData;
}

std::vector<ParentSpace> RegistrationHistory::getParentSpaces() const
{
	return mParentSpaces;
}

void RegistrationHistory::removeNewerThan(const QDateTime& timestamp)
{
	if (!timestamp.isValid())
		return;

	for (std::vector<RegistrationTransform>::iterator iter = mData.begin(); iter != mData.end();)
	{
		if (iter->mTimestamp > timestamp)
		{
			std::cout << "RegistrationHistory::removeNewerThan(" << timestamp.toString(timestampSecondsFormatNice())
				<< "): removed [" << iter->mTimestamp.toString(timestampSecondsFormatNice()) << ", " << iter->mType
				<< "]" << std::endl;
			iter = mData.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	for (std::vector<ParentSpace>::iterator iter = mParentSpaces.begin(); iter != mParentSpaces.end();)
	{
		if (iter->mTimestamp > timestamp)
		{
			std::cout << "RegistrationHistory::removeNewerThan(" << timestamp.toString(timestampSecondsFormatNice())
				<< "): removed parent frame [" << iter->mTimestamp.toString(timestampSecondsFormatNice()) << ", "
				<< iter->mType << "]" << std::endl;
			iter = mParentSpaces.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	setActiveTime(QDateTime());
}

void RegistrationHistory::setCache(const RegistrationTransform& val, const ParentSpace& parent,
	const QDateTime& timestamp)
{
	if ((mTransformCache == val) && (mParentSpaceCache == parent) && (mCurrentTime == timestamp))
		return;

	mCurrentTime = timestamp;
	mTransformCache = val;
	mParentSpaceCache = parent;

	emit currentChanged();
}

/**set the active time. Use only registrations performed at or prior
 * to this time.
 */
void RegistrationHistory::setActiveTime(const QDateTime& timestamp)
{
	RegistrationTransform val;
	ParentSpace parent;
	// set to specified time
	if (timestamp.isValid())
	{
		for (std::vector<RegistrationTransform>::iterator iter = mData.begin(); iter != mData.end(); ++iter)
		{
			if (iter->mTimestamp <= timestamp)
				val = *iter;
		}
		for (std::vector<ParentSpace>::iterator iter = mParentSpaces.begin(); iter != mParentSpaces.end(); ++iter)
		{
			if (iter->mTimestamp <= timestamp)
				parent = *iter;
		}
	}
	else
	{
		if (!mData.empty())
			val = mData.back();
		if (!mParentSpaces.empty())
			parent = mParentSpaces.back();
	}

	setCache(val, parent, timestamp);
}

QDateTime RegistrationHistory::getActiveTime() const
{
	return mCurrentTime;
}

RegistrationTransform RegistrationHistory::getCurrentRegistration() const
{
	return mTransformCache;
}

} // end namespace cx

