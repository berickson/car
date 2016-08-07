#include <QCoreApplication>

int main2(int argc,char*argv[]);

int main(int argc, char *argv[])
{
  return main2(argc, argv);
  QCoreApplication a(argc, argv);

  return a.exec();
}

