#include <iostream>
#include "CoordinateViewer.h"

int main(int argc, char** argv) {
    CoordinateViewerMain::CoordinateViewer app({argc, argv});
    return app.exec();
}
