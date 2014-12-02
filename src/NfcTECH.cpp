//============================================================================
// Copyright [2013] <COS>
// Name        : NfcTECH.cpp
// Author      : TriggerHuang@viatech.com.cn
// Description : Example Application of COS Project
//============================================================================
#define DEBUG_LEVEL 0
#define LOG_TAG "Trigger"

#include <gaia/base/Array.h>
#include <gaia/base/ByteArray.h>
#include <gaia/base/String.h>
#include <gaia/base/StringArray.h>
#include <gaia/base/Vector.h>
#include <gaia/base/Vector2D.h>
#include <gaia/base/gloger.h>
#include <gaia/core/Context.h>
#include <gaia/core/Message.h>
#include <gaia/core/Handler.h>
#include <gaia/core/ContentResolver.h>
#include <gaia/core/Persistence.h>
#include <gaia/core/event/OnClickListener.h>
#include <gaia/core/vision/Widget.h>
#include <gaia/core/vision/Scene.h>
#include <gaia/graphics/Color.h>
#include <gaia/ui/Button.h>
#include <gaia/ui/EditWidget.h>
#include <gaia/ui/LinearController.h>
#include <gaia/ui/TextWidget.h>
#include <gaia/ui/ScrollWidget.h>

#include <gaia/connectivity/nfc/NfcAdapter.h>
#include <gaia/connectivity/nfc/NfcManager.h>
#include <gaia/connectivity/nfc/NdefRecord.h>
#include <gaia/connectivity/nfc/NdefMessage.h>
#include <gaia/connectivity/nfc/NfcEvent.h>
#include <gaia/connectivity/nfc/MifareClassic.h>
#include <gaia/connectivity/nfc/Tag.h>
#include <gaia/connectivity/nfc/CreateNdefMessageCallback.h>
#include <gaia/core/Subscriber.h>
#include <gaia/core/Demand.h>
#include <gaia/core/DemandFilter.h>
#include <gaia/core/ComponentName.h>
#include <gaia/core/PendingDemand.h>
#include <gaia/net/Uri.h>
#include <gaia/storage/File.h>
#include <gaia/core/Parcelable.h>
#include <gaia/base/List.h>
#include <string>


#include "NfcTECH.h"
#include "nfctech/R.h"

#define safe_delete(ptr) \
    if ((ptr) != 0) \
    { \
        delete ptr; \
        ptr = 0; \
    }


using namespace gaia::base;
using namespace gaia::core;
using namespace gaia::graphics;
using namespace gaia::content;
using namespace gaia::ui;

using gaia::storage::File;
using gaia::net::Uri;
using gaia::base::String;
using gaia::base::Vector;
using gaia::base::Vector2D;
using gaia::core::Widget;
using gaia::core::Context;
using gaia::core::Demand;
using gaia::core::DemandFilter;
using gaia::core::Message;
using gaia::core::Handler;
using gaia::base::ByteArray;


using gaia::connectivity::NfcAdapter;
using gaia::connectivity::NfcManager;
using gaia::connectivity::NdefMessage;
using gaia::connectivity::NdefRecord;
using gaia::connectivity::NfcEvent;
using gaia::connectivity::CreateNdefMessageCallback;
using gaia::connectivity::Tag;
using gaia::connectivity::MifareClassic;


  NfcTECH::NfcTECH() :
    mpCtrl(NULL),
    mpTextNotice(NULL),
    mpTextWarning(NULL),
    mpScrollView(NULL),
    mpNfcAdapter(NULL),
    mNfcEnabled(false),
    mWriteTag(false),
    mpWrite_btn(NULL),
    mpNfcTECHBtnOnClickListener(NULL),
    mpContext(NULL) {
}

  NfcTECH::~NfcTECH() {
}

void NfcTECH::onInit(Persistence* const p) {
	mpContext = new gaia::core::Context(this);
    Scene* pScene = Scene::SceneFactory(this);
    mpCtrl = new LinearController(this);
    mpCtrl->setOrientation(LinearController::HORIZONTAL);


    mpTextNotice = new TextWidget(this);
    mpTextNotice->setTextColor(Color::BLUE);
    mpTextNotice->setTextSize(15);
    mpTextNotice->setText("Notice: ");

    mpScrollView = new ScrollWidget(this);
    mpScrollView->addWidget(mpTextNotice);
    mpCtrl->addWidget(mpScrollView);

    mpTextWarning = new TextWidget(this);
    mpTextWarning->setTextColor(Color::RED);
    mpTextWarning->setTextSize(15);
    mpTextWarning->setText("Warning: ");
    mpCtrl->addWidget(mpTextWarning);

    mpNfcTECHBtnOnClickListener = new NfcTECHBtnOnClickListener(this);
    mpWrite_btn = new Button(this);
    mpWrite_btn->setText("(点击)写入");
    mpWrite_btn->setId(NfcTECH::Write_button);
    mpWrite_btn->setOnClickListener(mpNfcTECHBtnOnClickListener);
    mpCtrl->addWidget(mpWrite_btn);


    mpNfcAdapter = NfcAdapter::getDefaultAdapterN(*mpContext);
    mNfcEnabled = getEnablerState();
    if (mNfcEnabled) {
    	showMessage("NFC is enabled!");
    }
    else {
    	showMessage("NFC is not enabled!");
    	goto init_error;
    }
    GLOG(LOG_TAG, LOGINFO, "%s:%d ", __FUNCTION__, __LINE__);
    pScene->attachController(mpCtrl);
    pScene->setupSceneLayout();

	//const Demand demand(getDemand());
    ///GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand demand = %s", __FUNCTION__, __LINE__, demand.toString().string());
    //showMessage(demand.toString().string());
    //resolveDemand(demand);
    return;

 init_error:
	safe_delete(mpCtrl);
	safe_delete(mpContext);
	safe_delete(mpTextNotice);
	safe_delete(mpTextWarning);
	safe_delete(mpScrollView);
	safe_delete(mpNfcAdapter);
	safe_delete(mpWrite_btn);
	safe_delete(mpNfcTECHBtnOnClickListener);
}

void NfcTECH::onTear() {
	if (mNfcEnabled) {
		safe_delete(mpCtrl);
		safe_delete(mpContext);
		safe_delete(mpTextNotice);
		safe_delete(mpTextWarning);
		safe_delete(mpScrollView);
		safe_delete(mpNfcAdapter);
		safe_delete(mpWrite_btn);
		safe_delete(mpNfcTECHBtnOnClickListener);
		getSceneSingleton()->SceneDestroy();
	}
	mNfcEnabled = false;
	mWriteTag = false;
}

void NfcTECH::onResume() {
	const Demand demand(getDemand());
    GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand demand = %s", __FUNCTION__, __LINE__, demand.toString().string());
    //showMessage(demand.toString().string());
    showWarning(demand.toString().string());
    resolveDemand(demand);

}

void NfcTECH::onRestart() {
	const Demand demand(getDemand());
    GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand demand = %s", __FUNCTION__, __LINE__, demand.toString().string());
    showMessage(demand.toString().string());
    resolveDemand(demand);

}

void NfcTECH::showMessage(const gaia::base::String& msg) {
    if (mpTextNotice == NULL) return;

    mpTextNotice->setText(msg);
}

void NfcTECH::showWarning(const gaia::base::String& msg) {
    if (mpTextWarning == NULL) return;

    mpTextWarning->setText(msg);
}


bool NfcTECH::getEnablerState() {
    if (mpNfcAdapter == NULL) {
        GLOG(LOG_TAG, LOGINFO, "mpNfcAdapter is NULL");
        showMessage("NfcAdapter is NULL");
        return false;
    }

    mNfcEnabled = mpNfcAdapter->isEnabled();
    return mNfcEnabled;
}

void NfcTECH::resolveDemand(const Demand& demand) {
    String action = demand.getAction();
    GLOG(LOG_TAG, LOGINFO, "action = %s resolveDemand", action.string());
    if (NfcAdapter::ACTION_TECH_DISCOVERED().equals(action)) {
    //TODO
        Parcelable parcel = demand.getParcelableExtra(NfcAdapter::EXTRA_TAG());
        Tag* tag = parcel.getN<Tag>();
        if (mWriteTag) {
        	GLOG(LOG_TAG, LOGINFO, "mWriteTag true");
        	mWriteTag = false;
        	bool auth = false;
        	MifareClassic* mfc = MifareClassic::getN(*tag);
        	String metaInfo = "";
        	mfc->connect();
        	String typeS = "MifareClassic";
        	int32_t sector_index = 0;
        	int32_t block_index = 0;
        	auth = mfc->authenticateSectorWithKeyA(sector_index, MifareClassic::KEY_DEFAULT());
        	if (auth) {
        	    showWarning("authentic OK");
        	    ByteArray data_write(16);
        	    for (int i = 0; i < 16; i++) {
        	      data_write[i] = i;
        	     }
        	     //void writeBlock(int32_t blockIndex, const gaia::base::ByteArray& data);
        	     mfc->writeBlock(1, data_write);
        	     showWarning(" write finished!");
        	     GLOG(LOG_TAG, LOGINFO, " write finished!");
        	     showMessage("");

        	  }else {
        	        		showWarning("authentic failed!");
        	        		GLOG(LOG_TAG, LOGINFO, " authentic failed! in write");
        	        	}


        }else {
        	GLOG(LOG_TAG, LOGINFO, "mWriteTag false");
        	bool auth = false;
        	// Get an instance of the Mifare classic card from this TAG intent
        	MifareClassic* mfc = MifareClassic::getN(*tag);
        	String metaInfo = "";
        	//Enable I/O operations to the tag from this TagTechnology object.
        	mfc->connect();
        	int32_t type = mfc->getType();//获取TAG的类型
        	int32_t sectorCount = mfc->getSectorCount();//获取TAG中包含的扇区数
        	String typeS = "";
        	typeS = "MifareClassic";

/*  //TODO
        switch (type) {
            case MifareClassic.TYPE_CLASSIC:
                typeS = "TYPE_CLASSIC";
                break;
            case MifareClassic.TYPE_PLUS:
                typeS = "TYPE_PLUS";
                break;
            case MifareClassic.TYPE_PRO:
                typeS = "TYPE_PRO";
                break;
            case MifareClassic.TYPE_UNKNOWN:
                typeS = "TYPE_UNKNOWN";
                break;
        }
*/
        	metaInfo = metaInfo + "卡片类型：" + typeS + "\n共" + String::valueOf(sectorCount) + "个扇区\n共"
                    + String::valueOf(mfc->getBlockCount()) + "个块\n存储空间: " + String::valueOf(mfc->getSize()) + "B\n";

        	for (int32_t j = 0; j < sectorCount; j++) {
                //Authenticate a sector with key A.
               auth = mfc->authenticateSectorWithKeyA(j, MifareClassic::KEY_DEFAULT());
                int32_t bCount;
                int32_t bIndex;
                if (auth) {
                    metaInfo = metaInfo + "Sector " +String::valueOf(j) + ":验证成功\n";
                    // 读取扇区中的块
                    bCount = mfc->getBlockCountInSector(j);
                    bIndex = mfc->sectorToBlock(j);
                    for (int32_t i = 0; i < bCount; i++) {
                        ByteArray data = mfc->readBlock(bIndex);
                        if(data.size() == 0)
                            metaInfo = metaInfo + "Block " + String::valueOf(bIndex) + " : "
                                       + "读取失败\n";
                        else
                        metaInfo = metaInfo + "Block " + String::valueOf(bIndex) + " : "
                                + bytesToString(data) + "\n";
                        bIndex++;
                    }
                } else {
                    metaInfo = metaInfo + "Sector " + String::valueOf(j) + ":验证失败\n";
                }
            }
            showMessage(metaInfo);
        }

          }
#if 0
    if (NfcAdapter::ACTION_TAG_DISCOVERED().equals(action)) {
        if (demand.hasExtra(NfcAdapter::EXTRA_NDEF_MESSAGES())) {
            // StringArray ms = demand.getStringArrayExtra(NfcAdapter::EXTRA_NDEF_MESSAGES());
            // showMessage(ms[0]);
#if 1
            Array<Parcelable> rawMsgs = demand.getParcelableArrayExtra(NfcAdapter::EXTRA_NDEF_MESSAGES());
            Array<NdefMessage *> msgs(rawMsgs.length());
            for (uint32_t i = 0; i < rawMsgs.length(); i++) {
                bool isNull  = rawMsgs[i].isNull();
                NdefMessage *msg = rawMsgs[i].getN<NdefMessage>();
                msgs[i] = msg;
            }
           /* Parcelable rawMsg = demand.getParcelableExtra(NfcAdapter::EXTRA_NDEF_MESSAGES());

            NdefMessage * msg = rawMsg.get<NdefMessage>();*/
            buildTagViews(msgs[0]);
#endif
        } else if (demand.hasExtra(NfcAdapter::EXTRA_TAG())) {
            String rawMsg = demand.getStringExtra(NfcAdapter::EXTRA_TAG());
            showMessage(rawMsg);
        }
    } else {
        GLOG(LOG_TAG, LOGINFO, "Unknow demand : %s", demand.toString().string());
    }
#endif
}

void NfcTECH::onNewDemand(Demand* const demand) {
        // String action = demand->getAction();
        // GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand action = %s", __FUNCTION__, __LINE__, action.string());

        //GLOG(LOG_TAG, LOGINFO, "%s:%d, onNewDemand demand = %s", __FUNCTION__, __LINE__, demand->toString().string());
        //showMessage(demand->toString().string());
        //resolveDemand(*demand);
    }

String NfcTECH::bytesToString(const ByteArray& bs) {
    String s = "";
    byte_t b;
    // for (byte b : bs) {
    for (uint32_t i = 0; i < bs.size(); i++) {
        b = bs[i];
        s= s + String::format("%02X", b);
    }
    return s;
}

NfcTECHBtnOnClickListener::NfcTECHBtnOnClickListener(NfcTECH* pNfcTECH):mpNfcTECH(pNfcTECH) {
    GLOGENTRY(LOG_TAG);
}

NfcTECHBtnOnClickListener::~NfcTECHBtnOnClickListener() {
	mpNfcTECH = NULL;
    GLOGENTRY(LOG_TAG);
}

void NfcTECHBtnOnClickListener::onClick(Widget* v){
    GLOGENTRY(LOG_TAG);
    switch(v->getId()) {
    case NfcTECH::Write_button:
    	mpNfcTECH->mWriteTag = true;
    	mpNfcTECH->showWarning("Write TAG");
    	GLOG(LOG_TAG, LOGINFO, "set mWriteTag true");
            break;
    default:
            GLOG(LOG_TAG,LOGER,"unknown button");
            break;
    }

}




template class Export<NfcTECH, Page>;

