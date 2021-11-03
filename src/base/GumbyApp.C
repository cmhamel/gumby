#include "GumbyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
GumbyApp::validParams()
{
  InputParameters params = MooseApp::validParams();

  // Do not use legacy material output, i.e., output properties on INITIAL as well as TIMESTEP_END
  params.set<bool>("use_legacy_material_output") = false;

  return params;
}

GumbyApp::GumbyApp(InputParameters parameters) : MooseApp(parameters)
{
  GumbyApp::registerAll(_factory, _action_factory, _syntax);
}

GumbyApp::~GumbyApp() {}

void
GumbyApp::registerAll(Factory & f, ActionFactory & af, Syntax & syntax)
{
  ModulesApp::registerAll(f, af, syntax);
  Registry::registerObjectsTo(f, {"GumbyApp"});
  Registry::registerActionsTo(af, {"GumbyApp"});

  /* register custom execute flags, action syntax, etc. here */

  registerSyntax("GumbyAddVariablesAction", "GumbyVariables");
  registerSyntax("GumbyAddStressDivergenceKernelsAction",
                 "GumbyStressDivergenceKernels/*");
}

void
GumbyApp::registerApps()
{
  registerApp(GumbyApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
GumbyApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  GumbyApp::registerAll(f, af, s);
}
extern "C" void
GumbyApp__registerApps()
{
  GumbyApp::registerApps();
}
