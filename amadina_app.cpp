#include "amadina_app.h"
#include "main_frame.h"

IMPLEMENT_APP(AmadinaApp);

bool AmadinaApp::OnInit()
{
    wxInitAllImageHandlers();
    MainFrame* frame = new MainFrame(0);
    frame->Show();
    SetTopWindow(frame);
    return true;
}
