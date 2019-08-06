#include "Data.C"
#include "StageData.C"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TGTab.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"


Int_t gPodID = 0;
Data* gData = 0;
StageData* gStageData = 0;
TGNumberEntryField* gEventNumberEntry = 0;
TGLabel* gEventLabel = 0;
TGLabel* gChannelLabel = 0;

//Forward declaration
void load_event();

//---------------------------------------------------------------------
class EvNavHandler
{
public:
  void Fwd()
  {
    if (gPodID < gData->fChain->GetEntries() - 1) {
      ++gPodID;
      load_event();
    } else {
      printf("At last POD.\n");
    }
  }
  void Bck()
  {
    if (gPodID > 0) {
      --gPodID;
      load_event();
    } else {
      printf("At first POD.\n");
    }
  }

  void GotoEvent()
  {
    gPodID = gEventNumberEntry->GetIntNumber();
    if (gPodID < 1) {
      std::cout << "POD ID too low! Going to first POD." << std::endl;
      gPodID = 0;
    }
    else if (gPodID > gData->fChain->GetEntries()) {
      std::cout << "POD ID beyond file! Going to last POD." << std::endl;
      gPodID = gData->fChain->GetEntries()-1;
    }
    load_event();
  }
};



//---------------------------------------------------------------------
void make_gui()
{
  // Create minimal GUI for event navigation.
  
  TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);

  EvNavHandler* fh = new EvNavHandler;
  
  TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  frmMain->SetWindowName("XX GUI");
  frmMain->SetCleanup(kDeepCleanup);


  //TString icondir(TString::Format("%s/share/root/icons/", gSystem->Getenv("ROOTSYS")));
  TString icondir(TString::Format("%s/icons/", gSystem->Getenv("ROOTSYS")));
  TGLayoutHints* lh = new TGLayoutHints(kLHintsNormal, 0,0,4,0);
  
  TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain);
  {

    TGPictureButton* b = 0;
    TGLabel* blabel = 0;
    
    blabel = new TGLabel(hf, " Next ");
    blabel->SetTextJustify(kTextCenterY);
    b = new TGPictureButton(hf, gClient->GetPicture(icondir+"tb_forw.xpm"));
    hf->AddFrame(blabel,lh);
    hf->AddFrame(b);
    b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain);
  {
    TGPictureButton* b = 0;
    TGLabel* blabel = 0;

    blabel = new TGLabel(hf, " Prev ");
    blabel->SetTextJustify(kTextCenterY);
    b = new TGPictureButton(hf, gClient->GetPicture(icondir+"tb_back.xpm"));
    hf->AddFrame(blabel, lh);
    hf->AddFrame(b);
    b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");
  }
  frmMain->AddFrame(hf);
   

  hf = new TGHorizontalFrame(frmMain);
  {
    // ability to jump to event
    TGLabel* eventNbrLabel = new TGLabel(hf, " Go to POD # " );
    gEventNumberEntry = new TGNumberEntryField(hf, 0, 0,
                                               TGNumberFormat::kNESInteger,
                                               TGNumberFormat::kNEANonNegative,
                                               TGNumberFormat::kNELLimitMinMax,
                                               0, 10000000);
    hf->AddFrame(eventNbrLabel,lh);
    hf->AddFrame(gEventNumberEntry);
    gEventNumberEntry->Connect("ReturnPressed()", "EvNavHandler", fh, "GotoEvent()");
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain);
  {
    // Show event number
    TGLabel* evtLabel = new TGLabel(hf, " Event: " );
    gEventLabel = new TGLabel(hf, "-1");
    gEventLabel->SetTextJustify(kTextBottom);

    hf->AddFrame(evtLabel, lh);
    hf->AddFrame(gEventLabel, new TGLayoutHints(kLHintsBottom|kLHintsExpandX));
 
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain);
  {
    // Show channel number
    TGLabel* channelLabel = new TGLabel(hf, " Channel: " );
    gChannelLabel = new TGLabel(hf, "-1");
    gChannelLabel->SetTextJustify(kTextBottom);
    gChannelLabel->SetTextJustify(kTextLeft);
    gChannelLabel->SetMargins(0,20);

    hf->AddFrame(channelLabel, lh);
    hf->AddFrame(gChannelLabel, new TGLayoutHints(kLHintsBottom|kLHintsExpandX)); 
  }
  frmMain->AddFrame(hf);
  
  frmMain->MapSubwindows();
  frmMain->Resize();
  frmMain->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("POD Control", 0);

}



