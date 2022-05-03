#ifndef MAIN_FRAME_H_INCLUDED
#define MAIN_FRAME_H_INCLUDED

#include <wx/frame.h>
#include <wx/aui/aui.h>

class wxAuiToolBar;
class wxAuiToolBarItem;
class wxAuiNotebook;
class wxOccPanel;

class MainFrame: public wxFrame
{
    public:
        MainFrame(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxString& title = wxEmptyString,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxSize(800,600),
                  long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);

        ~MainFrame();

    protected:
        void OnPaint(wxPaintEvent &evt);
        void OnToolButtonClick(wxCommandEvent &event);

        wxAuiToolBar* m_main_toolbar;
		wxAuiToolBarItem* m_tool1;
		wxAuiToolBarItem* m_tool2;
		wxAuiToolBarItem* m_tool3;
		wxAuiNotebook* m_auinotebook2;
		wxPanel* draw_panel;

		wxAuiManager m_mgr;

    private:
        void Draw(void);

    DECLARE_EVENT_TABLE()
};

#endif // MAIN_FRAME_H_INCLUDED
