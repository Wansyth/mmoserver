/*
---------------------------------------------------------------------------------------
This source file is part of swgANH (Star Wars Galaxies - A New Hope - Server Emulator)
For more information, see http://www.swganh.org


Copyright (c) 2006 - 2010 The swgANH Team

---------------------------------------------------------------------------------------
*/
#include "UIInputBox.h"
#include "UIButton.h"
#include "UICallback.h"
#include "UIDropdownBox.h"
#include "UIInputField.h"
#include "UIManager.h"
#include "UIOpcodes.h"
#include "PlayerObject.h"

#include "LogManager/LogManager.h"
#include "Common/Message.h"
#include "Common/MessageFactory.h"

//================================================================================

UIInputBox::UIInputBox(UICallback* callback,uint32 id,uint8 windowType,const int8* eventStr,const int8* caption,const int8* text,const BStringVector dropdownElements
					   ,PlayerObject* playerObject,uint8 ibType,uint16 maxInputLength)
: UIWindow(callback,id,windowType,"Script.inputBox",eventStr),
mIbType(ibType),
mMaxInputLength(maxInputLength)
{
	mOwner		= playerObject;
	mCaption	= caption;
	mPrompt		= text;
	mCaption.convert(BSTRType_Unicode16);
	mPrompt.convert(BSTRType_Unicode16);

	_initChildren(dropdownElements);
}

//================================================================================

UIInputBox::~UIInputBox()
{
}

//================================================================================

void UIInputBox::handleEvent(Message* message)
{
	uint32	action				= message->getUint32();
	uint32	items				= message->getUint32();
	string	inputStr;

	if(items)
	{
		message->getUint32(); // item count again
		message->getStringUnicode16(inputStr);
	}

	if(mUICallback != NULL)
		mUICallback->handleUIEvent(action,0,inputStr,this);

	mOwner->removeUIWindow(mId);
	gUIManager->destroyUIWindow(mId);
}

//================================================================================

void UIInputBox::sendCreate()
{
	if(!mOwner || mOwner->getConnectionState() != PlayerConnState_Connected)
		return;

	Message*	newMessage;

	gMessageFactory->StartMessage();             
	gMessageFactory->addUint32(opSuiCreatePageMessage);  

	gMessageFactory->addUint32(mId);

	gMessageFactory->addString(mWindowTypeStr);

	uint32 propertyCount = 4 + getChildrenPropertyCount();
	
	gMessageFactory->addUint32(propertyCount);

	// main window properties
	gMessageFactory->addUint8(5);
	gMessageFactory->addUint32(0);
	gMessageFactory->addUint32(7);
	gMessageFactory->addUint16(0);
	gMessageFactory->addUint16(1);
	gMessageFactory->addUint8(9);
	gMessageFactory->addString(mEventStr);
	gMessageFactory->addString(BString("txtInput"));
	gMessageFactory->addString(BString("LocalText"));
	gMessageFactory->addString(BString("cmbInput"));
	gMessageFactory->addString(BString("SelectedText"));

	gMessageFactory->addUint8(5);
	gMessageFactory->addUint32(0);
	gMessageFactory->addUint32(7);
	gMessageFactory->addUint16(0);
	gMessageFactory->addUint16(1);
	gMessageFactory->addUint8(10);
	gMessageFactory->addString(mEventStr);
	gMessageFactory->addString(BString("txtInput"));
	gMessageFactory->addString(BString("LocalText"));
	gMessageFactory->addString(BString("cmbInput"));
	gMessageFactory->addString(BString("SelectedText"));

	// prompt
	gMessageFactory->addUint8(3);
	gMessageFactory->addUint32(1);
	gMessageFactory->addString(mPrompt);
	gMessageFactory->addUint32(2);
	gMessageFactory->addString(BString("Prompt.lblPrompt"));
	gMessageFactory->addString(BString("Text"));

	// caption
	gMessageFactory->addUint8(3);
	gMessageFactory->addUint32(1);
	gMessageFactory->addString(mCaption);
	gMessageFactory->addUint32(2);
	gMessageFactory->addString(BString("bg.caption.lblTitle"));
	gMessageFactory->addString(BString("Text"));

	// child elements
	Children::iterator childrenIt = mChildElements.begin();

	while(childrenIt != mChildElements.end())
	{
		(*childrenIt)->addMessageData();
		++childrenIt;
	}
	
	// unknown
	gMessageFactory->addUint64(0);
	gMessageFactory->addUint64(0);
	gMessageFactory->addUint32(0);

	newMessage = gMessageFactory->EndMessage();

	(mOwner->getClient())->SendChannelA(newMessage,mOwner->getAccountId(),CR_Client,2);
}

//================================================================================

void UIInputBox::_initChildren(BStringVector dropdownElements)
{
	switch(mIbType)
	{
		case SUI_IB_NODROPDOWN_OK:
		{
			mChildElements.push_back(new UIButton(0,"btnCancel",false));
			mChildElements.push_back(new UIButton(0,"btnOk",true,"@ok"));
			mChildElements.push_back(new UIDropdownBox(0,"cmbInput",false,dropdownElements,mMaxInputLength));
			mChildElements.push_back(new UIInputField(0,"txtInput",true,mMaxInputLength));
		}
		break;

		case SUI_IB_DROPDOWN_OK:
		{
			mChildElements.push_back(new UIButton(0,"btnCancel",false));
			mChildElements.push_back(new UIButton(0,"btnOk",true,"@ok"));
			mChildElements.push_back(new UIDropdownBox(0,"cmbInput",true,dropdownElements,mMaxInputLength));
			mChildElements.push_back(new UIInputField(0,"txtInput",true,mMaxInputLength));
		}
		break;

		case SUI_IB_NODROPDOWN_OKCANCEL:
		{
			mChildElements.push_back(new UIButton(0,"btnCancel",true,"@cancel"));
			mChildElements.push_back(new UIButton(0,"btnOk",true,"@ok"));
			mChildElements.push_back(new UIDropdownBox(0,"cmbInput",false,dropdownElements,mMaxInputLength));
			mChildElements.push_back(new UIInputField(0,"txtInput",true,mMaxInputLength));
		}
		break;

		case SUI_IB_DROPDOWN_OKCANCEL:
		{
			mChildElements.push_back(new UIButton(0,"btnCancel",true,"@cancel"));
			mChildElements.push_back(new UIButton(0,"btnOk",true,"@ok"));
			mChildElements.push_back(new UIDropdownBox(0,"cmbInput",true,dropdownElements,mMaxInputLength));
			mChildElements.push_back(new UIInputField(0,"txtInput",true,mMaxInputLength));
		}
		break;

		default:break;
	}
}

//================================================================================



