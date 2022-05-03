#include "main_frame.h"
#include <wx/dcclient.h>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <V3d_Viewer.hxx>
#include <WNT_Window.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <AIS_Shape.hxx>

Handle(V3d_Viewer) viewer;
TCollection_ExtendedString a3DName("Visu3D");
Handle(V3d_View) view;
Handle(AIS_InteractiveContext) context;

Handle(AIS_Shape) aisthing;


void MainFrame::Draw()
{
    BRepPrimAPI_MakeBox mkBox(2., 2., 2.);
    BRepPrimAPI_MakeCylinder cylinder(10., 50.);
    const TopoDS_Shape &shape = cylinder.Shape();

    HWND wnd = this->GetHandle();

    Handle(Aspect_DisplayConnection) aDisplayConnection = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) myGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
    Handle_WNT_Window wind = new WNT_Window((Aspect_Handle) wnd);


    viewer = new V3d_Viewer(myGraphicDriver/*, (short* const)"viewer"*/); // manages views
    view = viewer->CreateView(); // view is the orientation, mapping etc of your actual display
    view->SetWindow(wind); // attach the view to the window
    if(!wind->IsMapped()) wind->Map();
    context = new AIS_InteractiveContext(viewer); // for selection management i.e neutral point or open local context
    viewer->SetDefaultLights(); // makes everything come alive
    viewer->SetLightOn(); // makes everything come alive

    view->SetBackgroundColor(Quantity_NOC_GRAY11);
    view->MustBeResized();
    view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);

    aisthing = new AIS_Shape(shape);
    context->Display(aisthing, true);
    context->DisplayAll(true);

    view->Redraw();
}

void MainFrame::OnPaint(wxPaintEvent &evt)
{
    view->Redraw();
}



