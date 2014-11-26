//============================================================================
// Copyright [2013] <COS>
// Name        : NfcTECH.h
// Author      : TriggerHuang@viatech.com.cn
// Description : Example Application of COS Project
//============================================================================


#pragma once
#include <gaia/core/Page.h>

namespace gaia {
    namespace connectivity {
        class NfcManager;
        class NdefMessage;
        class NdefRecord;
        class CreateNdefMessageCallback;
    }  // connectivity

    namespace core {
        class OnClickListener;
        class Persistence;
        class Scene;
        class widget;
        class Context;
        class Handler;
        class Message;
        class Subscriber;
        class DemandFilter;
        class Demand;
        class PendingDemand;
    }  // core

    namespace ui {
        class Button;
        class EditWidget;
        class LinearController;
        class TextWidget;
        class ListWidget;
        class ScrollWidget;
    }  // ui
    namespace base {
        class String;
        template<typename T> class Vector;
        template <class TYPE> class Vector2D;
        class ByteArray;
    }  // base

    namespace net {
        class Uri;
    }

    namespace locale {
        class Locale;
    }
}  // gaia


class NfcTECH: public gaia::core::Page {
  public:
    NfcTECH();
    ~NfcTECH();

  protected:
    virtual void onInit(gaia::core::Persistence* const p);
    virtual void onTear();
    virtual void onNewDemand(gaia::core::Demand* const demand);
    virtual void onResume();
  public:
    void resolveDemand(const gaia::core::Demand& demand);
    bool getEnablerState();
    void showMessage(const gaia::base::String& msg);
    void showWarning(const gaia::base::String& msg);
    gaia::base::String NfcTECH::bytesToString(const gaia::base::ByteArray& bs);


  private:
    gaia::ui::LinearController* mpCtrl;

    gaia::core::Context* mpContext;
    gaia::ui::TextWidget* mpTextNotice;
    gaia::ui::TextWidget* mpTextWarning;
    gaia::ui::ScrollWidget* mpScrollView;
    gaia::connectivity::NfcAdapter* mpNfcAdapter;
    bool mNfcEnabled;
};

