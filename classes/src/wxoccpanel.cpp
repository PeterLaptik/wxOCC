#include "../include/wxoccpanel.h"
#include <BRepPrimAPI_MakeCylinder.hxx>

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

    m_view->SetBackgroundColor(Quantity_NOC_DARKSLATEGRAY4);
    m_view->MustBeResized();
    m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.1, V3d_ZBUFFER);

//    BRepPrimAPI_MakeCylinder cylinder(10., 50.);
//    const TopoDS_Shape &shape = cylinder.Shape();
//    aisthing = new AIS_Shape(shape);
//    m_context->Display(aisthing, AIS_Shaded, 0, true);

    m_context->DisplayAll(true);
    m_view->Redraw();
}

wxOccPanel::~wxOccPanel()
{

}

void wxOccPanel::AddShape(Handle(AIS_Shape) shape)
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
    wxString tmp = "Selected: ";


    //m_context->ClearSelected(false);

    const Handle(AIS_Selection) &selection = m_context->Selection();
    if(selection->IsEmpty())
        return;

//    AIS_Selection f;
//    f.Objects();

    const AIS_NListOfEntityOwner& objects = selection->Objects();
    int sel_size = objects.Size();
    tmp<<sel_size;

    for(auto x: objects)
    {
        auto selected_object = x->Selectable();
        AIS_InteractiveObject *real_object = dynamic_cast<AIS_InteractiveObject*>(selected_object.get());
        wxString res = m_object_pool.Contains(real_object) ? "OK" : "Not found";
        wxMessageBox(res);
    }
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

Aspect_VKeyMouse wxOccPanel::GetMouseButton(wxMouseEvent &event)
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

Aspect_VKeyFlags wxOccPanel::GetPressedKey()
{
    if(wxGetKeyState(WXK_RAW_CONTROL))
        return Aspect_VKeyFlags_CTRL;
    else if(wxGetKeyState(WXK_SHIFT))
        return Aspect_VKeyFlags_SHIFT;
    else if(wxGetKeyState(WXK_ALT))
        return Aspect_VKeyFlags_ALT;

    return Aspect_VKeyFlags_NONE;
}
