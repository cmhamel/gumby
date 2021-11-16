#include "GumbyAddFixedDisplacementBCAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "Conversion.h"

registerMooseAction("GumbyApp", GumbyAddFixedDisplacementBCAction, "add_bc");

InputParameters
GumbyAddFixedDisplacementBCAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addRequiredParam<std::vector<BoundaryName>>("boundary",
    "the list of boundary IDs, not node set names.");
  params.addRequiredParam<std::vector<VariableName>>("displacements",
    "displacement variables.");
  params.addRequiredParam<std::vector<std::string>>("components",
    "list of components, i.e. x, y, z for this to act on.");
  return params;
}
GumbyAddFixedDisplacementBCAction::
GumbyAddFixedDisplacementBCAction(const InputParameters & params)
  : Action(params),
    _displacements(getParam<std::vector<VariableName>>("displacements")),
    _components(getParam<std::vector<std::string>>("components"))
{}

void GumbyAddFixedDisplacementBCAction::act()
{
  std::string kernel_name = "DirichletBC";
  std::vector<BoundaryName> boundaries = getParam<std::vector<BoundaryName>>("boundary");

  for (unsigned int i = 0; i < _components.size(); ++i)
  {
    InputParameters params = _factory.getValidParams(kernel_name);
    std::string unique_kernel_name = kernel_name + "_" + _name + "_" +
                                     Moose::stringify(i);
    if (_components[i] == "x")
    {
      params.set<std::vector<BoundaryName>>("boundary") = boundaries;
      params.set<NonlinearVariableName>("variable") = _displacements[0];
      params.set<Real>("value") = 0.0;
      _problem->addBoundaryCondition(kernel_name, unique_kernel_name, params);
    }
    else if (_components[i] == "y")
    {
      params.set<std::vector<BoundaryName>>("boundary") = boundaries;
      params.set<NonlinearVariableName>("variable") = _displacements[1];
      params.set<Real>("value") = 0.0;
      _problem->addBoundaryCondition(kernel_name, unique_kernel_name, params);
    }
    else if (_components[i] == "z")
    {
      params.set<std::vector<BoundaryName>>("boundary") = boundaries;
      params.set<NonlinearVariableName>("variable") = _displacements[2];
      params.set<Real>("value") = 0.0;
      _problem->addBoundaryCondition(kernel_name, unique_kernel_name, params);
    }
    else
    {
      mooseError("unsuported coordinate in GumbyFixedDisplacementBC");
    }
  }
}
