//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "GumbyTestApp.h"
#include "GumbyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"
#include "ModulesApp.h"

InputParameters
GumbyTestApp::validParams()
{
  InputParameters params = GumbyApp::validParams();
  return params;
}

GumbyTestApp::GumbyTestApp(InputParameters parameters) : MooseApp(parameters)
{
  GumbyTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

GumbyTestApp::~GumbyTestApp() {}

void
GumbyTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  GumbyApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"GumbyTestApp"});
    Registry::registerActionsTo(af, {"GumbyTestApp"});
  }
}

void
GumbyTestApp::registerApps()
{
  registerApp(GumbyApp);
  registerApp(GumbyTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
GumbyTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  GumbyTestApp::registerAll(f, af, s);
}
extern "C" void
GumbyTestApp__registerApps()
{
  GumbyTestApp::registerApps();
}
