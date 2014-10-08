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


/*
 * sscStringDataAdapterXml.cpp
 *
 *  Created on: Jun 27, 2010
 *      Author: christiana
 */
#include "cxStringDataAdapterXml.h"

#include <iostream>
#include <QDomElement>
#include <QStringList>

namespace cx
{

StringDataAdapterXml::StringDataAdapterXml() : 	mIsReadOnly(false)
{

}

/** Make sure one given option exists witin root.
 * If not present, fill inn the input defaults.
 */
StringDataAdapterXmlPtr StringDataAdapterXml::initialize(const QString& uid, QString name, QString help, QString value, QStringList range, QDomNode root)
{
	StringDataAdapterXmlPtr retval(new StringDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(value);
	retval->mAllowOnlyValuesInRange = true;
	return retval;
}

StringDataAdapterXmlPtr StringDataAdapterXml::initialize(const QString& uid, QString name, QString help, QString value, QDomNode root)
{
	StringDataAdapterXmlPtr retval(new StringDataAdapterXml());
	retval->mUid = uid;
	retval->mName = name.isEmpty() ? uid : name;
	retval->mHelp = help;
	//retval->mRange = range;
	retval->mStore = XmlOptionItem(uid, root.toElement());
	retval->mValue = retval->mStore.readValue(value);
	retval->mAllowOnlyValuesInRange = false;
	return retval;
}

void StringDataAdapterXml::setReadOnly(bool val)
{
	mIsReadOnly = val;
	emit changed();
}

QString StringDataAdapterXml::getDisplayName() const
{
	return mName;
}

QString StringDataAdapterXml::getValueAsString() const
{
	return mValue;
}

void StringDataAdapterXml::setValueFromString(QString value)
{
	this->setValue(value);
}

QString StringDataAdapterXml::getUid() const
{
	return mUid;
}

QString StringDataAdapterXml::getHelp() const
{
	return mHelp;
}

void StringDataAdapterXml::setHelp(QString val)
{
    if (val == mHelp)
        return;

    mHelp = val;
    emit changed();
}


QString StringDataAdapterXml::getValue() const
{
	return mValue;
}

bool StringDataAdapterXml::setValue(const QString& val)
{
	if (val == mValue)
		return false;

	mValue = val;
	mStore.writeValue(val);
	int pos = this->getValueRange().indexOf(val);
	emit valueWasSet(pos);
	emit changed();
	return true;
}

QStringList StringDataAdapterXml::getValueRange() const
{
	return mRange;
}

void StringDataAdapterXml::setValueRange(QStringList range)
{
	mRange = range;
	emit changed();
}

/**If a mapping from internal name to display name has been set, use it.
 * Otherwise return the input.
 */
QString StringDataAdapterXml::convertInternal2Display(QString internal)
{
	if (mDisplayNames.count(internal))
		return mDisplayNames[internal];
	return internal;
}

void StringDataAdapterXml::setDisplayNames(std::map<QString, QString> names)
{
	mDisplayNames = names;
	emit changed();
}

} // namespace cx
