#include <QTranslator>
#include <QApplication>
#include "Encodings.h"
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Shortcuts are Cmd-based on Macs and Alt-based on others for us!!
    // DO NOT use Qt::META and Qt::ALT, but use UJ::MOD and UJ::ALT in
    // Globals.h instead.
    a.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta);

    YL::init_table();

    QTranslator trans;
    trans.load(":/translations/UJ_Translate");
    a.installTranslator(&trans);

    UJ::Qelly::MainWindow w;
    w.show();

    return a.exec();
}
