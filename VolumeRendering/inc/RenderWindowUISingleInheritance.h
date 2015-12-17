#ifndef RenderWindowUISingleInheritance_H
#define RenderWindowUISingleInheritance_H

#include <vtkCommand.h>

#include <QMainWindow>

// Forward Qt class declarations
class Ui_RenderWindowUISingleInheritance;
class InputParser;

class RenderWindowUISingleInheritance : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  RenderWindowUISingleInheritance(InputParser *inputParser);
  ~RenderWindowUISingleInheritance() {};

  public slots:

  virtual void slotExit();

private:

  // Designer form
  Ui_RenderWindowUISingleInheritance *ui;
};

#endif