#include "RawData.C"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TGTab.h"
#include "TGButton.h"
#include "TGLabel.h"
#include "TGNumberEntry.h"


Int_t gEventID = 0;
RawData* gRawData = 0;
TGNumberEntryField* gEventNumberEntry = 0;
TGLabel* gEventLabel = 0;
TGLabel* gPmtLabel = 0;

//Forward declaration
void load_event();

//---------------------------------------------------------------------
class EvNavHandler
{
public:
  void Fwd()
  {
    if (gEventID < gRawData->fChain->GetEntries() - 1) {
      ++gEventID;
      load_event();
    } else {
      printf("At last event.\n");
    }
  }
  void Bck()
  {
    if (gEventID > 0) {
      --gEventID;
      load_event();
    } else {
      printf("At first event.\n");
    }
  }

  void GotoEvent()
  {
    gEventID = gEventNumberEntry->GetIntNumber();
    if (gEventID < 1) {
      std::cout << "Event ID too low! Going to first event." << std::endl;
      gEventID = 0;
    }
    else if (gEventID > gRawData->fChain->GetEntries()) {
      std::cout << "Event ID beyond file! Going to last event." << std::endl;
      gEventID = gRawData->fChain->GetEntries()-1;
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
    TGLabel* eventNbrLabel = new TGLabel(hf, " Go to event # " );
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
    TGLabel* pmtLabel = new TGLabel(hf, " PMT: " );
    gPmtLabel = new TGLabel(hf, "-1");
    gPmtLabel->SetTextJustify(kTextBottom);
    gPmtLabel->SetTextJustify(kTextLeft);
    gPmtLabel->SetMargins(0,20);

    hf->AddFrame(pmtLabel, lh);
    hf->AddFrame(gPmtLabel, new TGLayoutHints(kLHintsBottom|kLHintsExpandX)); 
  }
  frmMain->AddFrame(hf);

  frmMain->MapSubwindows();
  frmMain->Resize();
  frmMain->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("Event Control", 0);

}



