#include <iostream>
#include "Application.h"

int main(int argc, char** argv) {
    CoordinatViewer app({argc, argv});
    return app.exec();
}
