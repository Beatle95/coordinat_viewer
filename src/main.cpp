#include <iostream>
#include "CoordinateViewer.h"

int main(int argc, char** argv) {
    CoordinateViewerMain::Renderer app({argc, argv});
    return app.exec();
}
