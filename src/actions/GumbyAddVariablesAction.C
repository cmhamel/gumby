#include "GumbyAddVariablesAction.h"
#include "Factory.h"
#include "Parser.h"
#include "FEProblem.h"

registerMooseAction("GumbyApp", GumbyAddVariablesAction, "add_variable");

template<>
InputParameters validParams<GumbyAddVariablesAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::vector<VariableName>>("displacements",
    "variable names for displacemenets, e.g. 'displ_x, displ_y' for 2D");
  params.addRequiredParam<std::string>("displacements_order",
    "displacements shape function orders, e.g. first");
  params.addRequiredParam<std::string>("displacements_family",
    "displacements shape function family, e.g. Lagrange");
  return params;
}

GumbyAddVariablesAction::GumbyAddVariablesAction(InputParameters params)
  : Action(params),
    _displacements(getParam<std::vector<VariableName>>("displacements")),
    _displacements_order(getParam<std::string>("displacements_order")),
    _displacements_family(getParam<std::string>("displacements_family"))
{}

void GumbyAddVariablesAction::act()
{
  std::cout << "Adding displacement variables to Gumby analysis" << std::endl;
  for (unsigned int i = 0; i < _displacements.size(); ++i)
  {
    std::cout << "disp = " << _displacements[i] << std::endl;
    auto params = _factory.getValidParams("MooseVariable");
    params.set<MooseEnum>("order") = _displacements_order;
    params.set<MooseEnum>("family") = _displacements_family;
    _problem->addVariable("MooseVariable", _displacements[i], params);
  }
  std::cout << "displacements_order = " << _displacements_order << std::endl;
  std::cout << "displacements_family = " << _displacements_family << std::endl;

  std::cout << std::endl << std::endl << std::endl;
}
