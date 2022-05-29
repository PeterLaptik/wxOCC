#include "../include/wxoccpanel.h"
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <AIS_Line.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>

// Nominal mouse wheel amount for one click (known as a 'detent')
// See description in MSDN
const double WHEEL_DELTA = 120.0;

const char *DEFAULT_NAME = "wxOCCPanel";

wxBEGIN_EVENT_TABLE(wxOccPanel, wxPanel)
    EVT_PAINT(wxOccPanel::OnPaint)
    EVT_SIZE(wxOccPanel::OnResize)
    EVT_MOTION(wxOccPanel::OnMouseMove)
    EVT_MOUSEWHEEL(wxOccPanel::OnMouseWheel)
    EVT_LEFT_DOWN(wxOccPanel::OnLeftMouseButtonDown)
    EVT_LEFT_UP(wxOccPanel::OnLeftMouseButtonUp)
    EVT_RIGHT_DOWN(wxOccPanel::OnRightMouseButtonDown)
//    EVT_RIGHT_DOWN(wxOccPanel::OnMouseButtonDown)
//    EVT_RIGHT_UP(wxOccPanel::OnMouseButtonUp)
wxEND_EVENT_TABLE()

wxOccPanel::wxOccPanel(wxWindow *parent,
                     wxWindowID winid,
                    const wxPoint &pos,
                    const wxSize &size,
                    long style,
                    const wxString &name)
            : wxPanel(parent, winid, pos, size, style, name),
            m_scale_factor(WHEEL_DELTA),
            m_mouse_lb_clicked(false),
            m_last_x(-1), m_last_y(-1),
            m_panel_name(DEFAULT_NAME)
{
    HWND wnd = this->GetHandle();
    m_display_connection = new Aspect_DisplayConnection();
    m_graphic_driver = new OpenGl_GraphicDriver(m_display_connection);
    m_window = new WNT_Window((Aspect_Handle) wnd);

    m_viewer = new V3d_Viewer(m_graphic_driver); // manages views
    // view is the orientation, mapping etc of your actual display
    m_view = m_viewer->CreateView();
    // attach the view to the window
    m_view->SetWindow(m_window);
    if(!m_window->IsMapped())
        m_window->Map();
    // for selection management i.e neutral point or open local context
    m_context = new AIS_InteractiveContext(m_viewer);
    // makes everything come alive
    m_viewer->SetDefaultLights();
    // makes everything come alive
    m_viewer->SetLightOn();

    m_view->SetBackgroundColor(Quantity_NOC_BLACK);
    m_view->MustBeResized();
    m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.1, V3d_ZBUFFER);
    m_context->DisplayAll(true);
    m_view->Redraw();

    m_view_cube = new AIS_ViewCube();
    AddShape(m_view_cube);

    // Set for sketch mode
    //m_context->Activate(TopAbs_FACE, Standard_True);
    //AIS_Shape::SelectionType(TopAbs_EDGE);
}

wxOccPanel::~wxOccPanel()
{

}

void wxOccPanel::AddShape(Handle(AIS_InteractiveObject) shape)
{
    m_object_pool.AppendObject(shape);
    m_context->Display(shape, AIS_Shaded, 0, true);
    m_context->DisplayAll(true);
    m_view->Redraw();
}

#include<wx/msgdlg.h>
void wxOccPanel::DeleteSelected()
{

}

void wxOccPanel::OnPaint(wxPaintEvent &event)
{
    m_view->Redraw();
}

void wxOccPanel::OnResize(wxSizeEvent &event)
{
    m_view->MustBeResized();
}

#include <typeinfo>
void wxOccPanel::Test()
{
    /*
    const Handle(AIS_Selection) &selection = m_context->Selection();
    if(selection->IsEmpty())
        return;

    const AIS_NListOfEntityOwner &objects = selection->Objects();
    int sel_size = objects.Size();

    for(auto x: objects)
    {
        SelectMgr_EntityOwner owner = x;
        owner.SetSelected(true);

        auto selected_object = x->Selectable();
        AIS_InteractiveObject *real_object = dynamic_cast<AIS_InteractiveObject*>(selected_object.get());
        wxString res = m_object_pool.Contains(real_object) ? "OK" : "Not found";
    }

    if(objects.Size()==1)
    {
//        auto selected_object = objects.First()->Selectable();
//        AIS_InteractiveObject *real_object = dynamic_cast<AIS_InteractiveObject*>(selected_object.get());
//        const SelectMgr_SequenceOfSelection& selections = real_object->Selections();
//        for(auto x: selections)
//        {
//            for(auto y: x.get()->Entities())
//            {
//                wxString res = "Number:";
//                //res<<selections.Size();
//                res<<typeid(y.get()).name();
//                wxMessageBox(res);
//            }
//        }
    }



    if(m_context->HasDetected() && m_context->MainSelector()->NbPicked()>0)
    {
        //auto x = m_context->MainSelector()->OnePicked();
        //wxMessageBox("QQQQ");
    }
    */

    m_context->Activate(TopAbs_FACE, Standard_True);
    ShowGrid(!IsGridShown());
}

void wxOccPanel::ShowGrid(bool show)
{
    if(!show)
    {
        m_viewer->DeactivateGrid();
        m_view->Redraw();
        return;
    }
    gp_Pnt pnt(0,0,50);
    gp_Dir dir(0, 0.5, 0.5);
    gp_Pln plane(pnt, dir);
    gp_Ax3 ax(plane.Location(), plane.Axis().Direction());
    m_viewer->SetPrivilegedPlane(ax);
    m_viewer->SetRectangularGridValues(0, 1, 10, 10, 0);
    m_viewer->SetRectangularGridGraphicValues(100, 100, 0);
    m_viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Points);
    Graphic3d_Vertex vert(0.0f,0.0f,0.0f);
    //m_viewer->ShowGridEcho(m_view, vert);
    //m_viewer->HideGridEcho(m_view);
    m_view->Redraw();
}

void wxOccPanel::OnMouseWheel(wxMouseEvent &event)
{
    Graphic3d_Vec2i pos(event.GetX(), event.GetY());
    double param = static_cast<double>(event.GetWheelRotation())/m_scale_factor;
    Aspect_ScrollDelta ascroll(pos, param, Aspect_VKeyFlags_NONE);
    AIS_ViewController::UpdateMouseScroll(ascroll);
    AIS_ViewController::FlushViewEvents (m_context, m_view, true);
}

void wxOccPanel::OnLeftMouseButtonDown(wxMouseEvent &event)
{
    m_mouse_lb_clicked = true;
    Graphic3d_Vec2i pos(event.GetX(), event.GetY());
    Aspect_VKeyFlags flags = GetPressedKey();
    AIS_ViewController::PressMouseButton(pos, Aspect_VKeyMouse_LeftButton,
                                         flags, true);
}

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Aspect_Grid.hxx>
void wxOccPanel::OnRightMouseButtonDown(wxMouseEvent &event)
{
    Graphic3d_Vec2i pos(event.GetX(), event.GetY());
    Standard_Real x, y, z, vx, vy, vz;
    //m_view->ConvertToGrid(event.GetX(), event.GetY(), x, y, z);
    m_view->Convert(event.GetX(), event.GetY(), x, y, z);

    // Default plane
    gp_Pnt pnt(0,0,50);
    gp_Dir dir(0, 0.5, 0.5);
    gp_Pln plane(pnt, dir);

    // Eye
    Standard_Real ex, ey, ez;
    m_view->Eye(ex, ey, ez);

    wxString tmp = "EYE: ";
    tmp<<"x = "<<ex<<", y = "<<ey<<", z = "<<ez;
    //wxMessageBox(tmp);

    TopoDS_Face face = BRepBuilderAPI_MakeFace(plane);
    BRepAdaptor_Surface surface(face);
    const GeomAdaptor_Surface& geomAdapSurf = surface.Surface();
    const Handle(Geom_Surface)& geomSurf = geomAdapSurf.Surface();
    gp_Pnt picked(x, y, z);
    GeomAPI_ProjectPointOnSurf prj(picked, geomSurf);
    Standard_Integer index = prj.NbPoints();
    gp_Pnt ResultPoint = prj.Point(index);


    if(m_context->HasDetected() && m_context->MainSelector()->NbPicked()>0)
    {
        auto x = m_context->MainSelector()->OnePicked();
        Graphic3d_Vec2i pos(event.GetX(), event.GetY());
        //wxMessageBox("QQQQ");
    }

    //Handle(Geom_Point) cpoint1 = new Geom_CartesianPoint(x, y, z);
    Handle(Geom_Point) cpoint1 = new Geom_CartesianPoint(ResultPoint.X(), ResultPoint.Y(), ResultPoint.Z());
    Handle(AIS_Point) point1 = new AIS_Point(cpoint1);
    Handle(Geom_Point) cpoint2 = new Geom_CartesianPoint(0,0,0);
    Handle(AIS_Point) point2 = new AIS_Point(cpoint2);
    Handle(AIS_Line) line = new AIS_Line(cpoint1, cpoint2);
    AddShape(line);


}

void wxOccPanel::OnLeftMouseButtonUp(wxMouseEvent &event)
{
    m_mouse_lb_clicked = false;
    Graphic3d_Vec2i pos(event.GetX(), event.GetY());
    Aspect_VKeyFlags flags = GetPressedKey();
    AIS_ViewController::ReleaseMouseButton(pos, Aspect_VKeyMouse_LeftButton,
                                         flags, true);
}

void wxOccPanel::OnMouseMove(wxMouseEvent &event)
{
    Graphic3d_Vec2i pos(event.GetX(), event.GetY());
    Aspect_VKeyMouse buttons = GetMouseButton(event);
    Aspect_VKeyFlags flags = GetPressedKey();

    AIS_ViewController::UpdateMousePosition(pos, buttons, flags, false);
    AIS_ViewController::FlushViewEvents(m_context, m_view, true);
}

Aspect_VKeyMouse wxOccPanel::GetMouseButton(wxMouseEvent &event) const
{
    int button = event.GetButton();
    switch(button)
    {
        case wxMOUSE_BTN_LEFT:
            return Aspect_VKeyMouse_LeftButton;
        case wxMOUSE_BTN_RIGHT:
            return Aspect_VKeyMouse_RightButton;
    }
    return Aspect_VKeyMouse_NONE;
}

Aspect_VKeyFlags wxOccPanel::GetPressedKey() const
{
    if(wxGetKeyState(WXK_RAW_CONTROL))
        return Aspect_VKeyFlags_CTRL;
    else if(wxGetKeyState(WXK_SHIFT))
        return Aspect_VKeyFlags_SHIFT;
    else if(wxGetKeyState(WXK_ALT))
        return Aspect_VKeyFlags_ALT;

    return Aspect_VKeyFlags_NONE;
}

bool wxOccPanel::IsGridShown() const
{
    return m_viewer->IsGridActive();
}
