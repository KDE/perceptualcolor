#include <colordialog.h>
#include <qapplication.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PerceptualColor::ColorDialog m_colorDialog;
    m_colorDialog.show();
    return app.exec();
}
