/*=========================================================================

  Program:   Visualization Toolkit
  Module:    GPURenderDemo.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// projcet includes
#include "InputParser.h"
#include "RenderWindowUISingleInheritance.h"

// QT includes
#include <QApplication>

int main(int argc, char *argv[])
{

  // Parse the parameters
  InputParser *inputParser = new InputParser(argc, argv);

  // QT Stuff
  QApplication app(argc, argv);

  RenderWindowUISingleInheritance renderWindowUISingleInheritance(inputParser);
  renderWindowUISingleInheritance.show();

  delete inputParser;

  return app.exec();
}