#include "main_frame.h"
#include "classes/include/wxoccpanel.h"
#include <wx/artprov.h>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_Line.hxx>
#include <Geom_CartesianPoint.hxx>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_PAINT(MainFrame::OnPaint)
    EVT_TOOL(wxID_ANY, MainFrame::OnToolButtonClick)
wxEND_EVENT_TABLE()

const wxWindowID TOOL_ID_1 = wxNewId();
const wxWindowID TOOL_ID_2 = wxNewId();
const wxWindowID TOOL_ID_3 = wxNewId();


MainFrame::MainFrame(wxWindow* parent, wxWindowID id, const wxString& title,
          const wxPoint& pos, const wxSize& size, long style)
          :wxFrame(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);


	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	m_main_toolbar = new wxAuiToolBar(this,
                        wxID_ANY, wxDefaultPosition,
                        wxDefaultSize, wxAUI_TB_HORZ_LAYOUT);

	m_tool1 = m_main_toolbar->AddTool(wxNewId(), wxT("tool1"),
                            wxArtProvider::GetBitmap(wxART_FILE_SAVE),
                            wxNullBitmap, wxITEM_NORMAL,
                            wxEmptyString, wxEmptyString, NULL);

	m_tool2 = m_main_toolbar->AddTool(wxNewId(), wxT("tool2"),
                            wxArtProvider::GetBitmap(wxART_FILE_SAVE),
                            wxNullBitmap, wxITEM_NORMAL, wxEmptyString,
                            wxEmptyString, NULL);

	m_tool3 = m_main_toolbar->AddTool(wxNewId(), wxT("tool3"),
                            wxArtProvider::GetBitmap(wxART_FILE_SAVE),
                            wxNullBitmap, wxITEM_NORMAL, wxEmptyString,
                            wxEmptyString, NULL );


	m_mgr.AddPane(m_main_toolbar, wxAuiPaneInfo().Name(_("main toolbar"))
               .ToolbarPane().Caption(_("menu")).Top().Gripper());

	m_auinotebook2 = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition,
                                    wxDefaultSize, wxAUI_NB_DEFAULT_STYLE);

	m_mgr.AddPane(m_auinotebook2, wxAuiPaneInfo().Left().PinButton(true)
               .Dock().Center().CaptionVisible(false).Resizable().FloatingSize(wxDefaultSize));

	draw_panel = new wxOccPanel(m_auinotebook2, wxID_ANY, wxDefaultPosition,
                        /*wxDefaultSize*/ wxSize(800,600), wxTAB_TRAVERSAL);

	m_auinotebook2->AddPage(draw_panel, wxT("a page"), false, wxNullBitmap);

	m_main_toolbar->Realize();

	m_mgr.Update();
	/**/
	this->Centre(wxBOTH);

	//Draw();
}

MainFrame::~MainFrame()
{
    m_mgr.UnInit();
}

#include <wx/msgdlg.h>
void MainFrame::OnToolButtonClick(wxCommandEvent &event)
{
    int id = event.GetId();
    wxAuiToolBarItem *item = m_main_toolbar->FindTool(id);
    const wxString &cmd = item->GetLabel();

    if(cmd=="tool1")
    {
        wxOccPanel *panel = dynamic_cast<wxOccPanel*>(draw_panel);
        //wxMessageBox(cmd);
        BRepPrimAPI_MakeCylinder cylinder(10., 50.);
        const TopoDS_Shape &shape = cylinder.Shape();
        Handle(AIS_Shape) object = new AIS_Shape(shape);
        panel->AddShape(object);
    }
    else if(cmd=="tool2")
    {
        wxOccPanel *panel = dynamic_cast<wxOccPanel*>(draw_panel);
        panel->Test();
    }
    else if(cmd=="tool3")
    {
        wxOccPanel *panel = dynamic_cast<wxOccPanel*>(draw_panel);
        BRepPrimAPI_MakeBox box(gp_Pnt(10,10,10), 10,10,10);
        const TopoDS_Shape &shape = box.Shape();
        Handle(AIS_Shape) object = new AIS_Shape(shape);
        panel->AddShape(object);

        Handle(Geom_Point) point1 = new Geom_CartesianPoint(100,50,10);
        Handle(Geom_Point) point2 = new Geom_CartesianPoint(200,150,0);
        Handle(AIS_Line) line = new AIS_Line(point1, point2);
        panel->AddShape(line);
    }
}
