#ifndef WXOCCPANEL_H
#define WXOCCPANEL_H

#include "../include/objectpool.h"
#include <wx/panel.h>
#include <AIS_ViewController.hxx>
//#include <BRepPrimAPI_MakeBox.hxx>
//#include <BRepPrimAPI_MakeCylinder.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_Window.hxx>
#include <AIS_InteractiveContext.hxx>
//#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Line.hxx>

class wxOccPanel: public wxPanel, public AIS_ViewController
{
    public:
        ///\brief Constructor
        ///\param parent - parent window
        ///\param pos - default panel position
        ///\param size - panel size
        ///\param style - panel style (see wxWidgets documentation)
        ///\param name - panel name
        wxOccPanel(wxWindow *parent,
                    wxWindowID winid = wxID_ANY,
                    const wxPoint &pos = wxDefaultPosition,
                    const wxSize &size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString &name = wxPanelNameStr);

        virtual ~wxOccPanel();
        void AddShape(Handle(AIS_InteractiveObject) shape);
        void ShowGrid(bool show = true);
        bool IsGridShown(void) const;
        void DeleteSelected(void);
        void Test(void);

    protected:
        ///
        void OnPaint(wxPaintEvent &event);
        void OnResize(wxSizeEvent &event);
        /// Mouse wheel event handler. Default behavior:
        /// zoom in / zoom out on mouse wheel rotating
        void OnMouseWheel(wxMouseEvent &event);
        void OnMouseMove(wxMouseEvent &event);
        void OnLeftMouseButtonDown(wxMouseEvent &event);
        void OnLeftMouseButtonUp(wxMouseEvent &event);
        void OnRightMouseButtonDown(wxMouseEvent &event);

    private:
        inline Aspect_VKeyMouse GetMouseButton(wxMouseEvent &event) const;
        inline Aspect_VKeyFlags GetPressedKey(void) const;
        inline void CreateViewCube(void);
        inline gp_Pnt GetIntersectionPoint(int mouse_x, int mouse_y);

        gp_Pln m_plane;
        Handle(AIS_Line) tmp_line;

        double m_scale_factor;
        bool m_mouse_lb_clicked;
        int m_last_x, m_last_y;

        TCollection_ExtendedString m_panel_name;
        Handle(Aspect_DisplayConnection) m_display_connection;
        Handle(OpenGl_GraphicDriver) m_graphic_driver;
        Handle_WNT_Window m_window;
        Handle(V3d_Viewer) m_viewer;
        Handle(V3d_View) m_view;
        Handle(AIS_InteractiveContext) m_context;
        Handle(AIS_ViewCube) m_view_cube;

        ObjectPool m_object_pool;
        Handle(AIS_Shape) aisthing;

    DECLARE_EVENT_TABLE()
};

#endif // WXOCCPANEL_H
