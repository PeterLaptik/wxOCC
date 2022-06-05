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
    //m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.1, V3d_ZBUFFER);
    m_context->DisplayAll(true);
    m_view->Redraw();

    gp_Pnt point(0, 0, 0);
    gp_Dir direction(0, 0, 1);
    gp_Ax1 axis(point, direction);
    m_plane.SetAxis(axis);
    // Set for sketch mode
    //m_context->Activate(TopAbs_FACE, Standard_True);
    //AIS_Shape::SelectionType(TopAbs_EDGE);

    CreateViewCube();
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
    //m_context->Activate(TopAbs_FACE, Standard_True);
    ShowGrid(!IsGridShown());
}

void wxOccPanel::ShowGrid(bool show)
{
    if(!show)
    {
        //m_viewer->DisplayPrivilegedPlane(false);
        m_viewer->DeactivateGrid();
        m_view->Redraw();
        return;
    }
    gp_Pnt pnt(0,0,50);
    gp_Dir dir(0, 0, 1);
    gp_Pln plane(pnt, dir);


    gp_Ax3 ax(plane.Location(), plane.Axis().Direction());
    m_viewer->SetPrivilegedPlane(ax);
    //m_viewer->DisplayPrivilegedPlane(true);

    m_viewer->SetRectangularGridValues(0, 1, 10, 10, 0);
    m_viewer->SetRectangularGridGraphicValues(100, 100, 0);
    m_viewer->ActivateGrid(Aspect_GT_Rectangular, Aspect_GDM_Lines);
    //Graphic3d_Vertex vert(0.0f,0.0f,0.0f);
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
    Standard_Real x, y, z;//, vx, vy, vz;
    //m_view->ConvertToGrid(event.GetX(), event.GetY(), x, y, z);
    //m_view->Proj(vx, vy, vz);
    //m_view->ConvertWithProj(event.GetX(), event.GetY(), x, y, z, vx, vy, vz);
    m_view->ConvertToGrid(event.GetX(), event.GetY(), x, y, z);


    // Default plane
    gp_Pnt pnt(0,0,0);
    gp_Dir dir(0, 0, 1);
    gp_Pln plane(pnt, dir);

    // Eye
    Standard_Real ex, ey, ez;
    m_view->At(ex, ey, ez);

    wxString tmp = "EYE: ";
    tmp<<"x = "<<ex<<", y = "<<ey<<", z = "<<ez;
    //wxMessageBox(tmp);

    TopoDS_Face face = BRepBuilderAPI_MakeFace(m_viewer->PrivilegedPlane());
    BRepAdaptor_Surface surface(face);
    const GeomAdaptor_Surface& geomAdapSurf = surface.Surface();
    const Handle(Geom_Surface)& geomSurf = geomAdapSurf.Surface();

    gp_Pnt picked = gp_Pnt(x, y, z);
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

#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <ElSLib.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GC_MakeLine.hxx>
void wxOccPanel::OnMouseMove(wxMouseEvent &event)
{
    Graphic3d_Vec2i pos(event.GetX(), event.GetY());
    Aspect_VKeyMouse buttons = GetMouseButton(event);
    Aspect_VKeyFlags flags = GetPressedKey();

    AIS_ViewController::UpdateMousePosition(pos, buttons, flags, false);
    AIS_ViewController::FlushViewEvents(m_context, m_view, true);


    /** tmp **/
    Standard_Real x, y, z, vx, vy, vz;
    Standard_Real px, py, pz;
    //m_view->ConvertToGrid(event.GetX(), event.GetY(), x, y, z);
    m_view->ConvertWithProj(event.GetX(), event.GetY(), x, y, z, vx, vy, vz);
    m_view->Proj(px, py, pz);
    if(tmp_line.get()!=nullptr)
        m_context->Remove(tmp_line, true);


    Handle(Geom_Surface) theSurf = new Geom_Plane(m_plane.Location(), m_plane.Axis().Direction());

    GeomAdaptor_Surface surface(theSurf);
    const GeomAdaptor_Surface& geomAdapSurf = surface.Surface();
    const Handle(Geom_Surface)& geomSurf = geomAdapSurf.Surface();

    Standard_Real ex, ey, ez;
    m_view->At(ex, ey, ez);

    Standard_Real cosa, cosb, cosc, sum;

    gp_Pnt picked(x, y, z);
    GeomAPI_ProjectPointOnSurf prj(picked, geomSurf);
    Standard_Integer index = prj.NbPoints();
    gp_Pnt ResultPoint = prj.Point(index);


    Handle(Geom_Point) cpoint1 = new Geom_CartesianPoint(ResultPoint.X(), ResultPoint.Y(), ResultPoint.Z());
    Handle(AIS_Point) point1 = new AIS_Point(cpoint1);
    Handle(Geom_Point) cpoint2 = new Geom_CartesianPoint(0,0,0);
    Handle(AIS_Point) point2 = new AIS_Point(cpoint2);
    tmp_line = new AIS_Line(cpoint1, cpoint2);
    AddShape(tmp_line);

}

gp_Pnt wxOccPanel::GetIntersectionPoint(int mouse_x, int mouse_y)
{
    gp_Pnt intersection_point;
    const gp_Dir direction = m_plane.Axis().Direction();
    Standard_Real A = direction.X();
    Standard_Real B = direction.Y();
    Standard_Real C = direction.Z();
    return intersection_point;
}

void wxOccPanel::CreateViewCube()
{
    m_view_cube = new AIS_ViewCube;
    m_view_cube->SetBoxColor(Quantity_NOC_BLUE2);
    m_view_cube->SetDrawAxes(true);
    m_view_cube->SetSize(40);
    m_view_cube->SetFontHeight(12);
    /*
    m_view_cube->SetTransformPersistence(
        new Graphic3d_TransformPers(
            Graphic3d_TMF_TriedronPers,
            Aspect_TOTP_LEFT_UPPER,
            Graphic3d_Vec2i(85, 85)));
    */
    m_context->Display(m_view_cube, false);
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
