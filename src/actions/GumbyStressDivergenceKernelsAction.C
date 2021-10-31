#include "GumbyAddStressDivergenceKernelsAction.h"
#include "Factory.h"
#include "Parser.h"
#include "FEProblem.h"

registerMooseAction("GumbyApp", GumbyAddStressDivergenceKernelsAction, "add_variable");

template<>
InputParameters validParams<GumbyAddStressDivergenceKernelsAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::vector<VariableName>>("displacements",
    "variable names for displacemenets, e.g. 'displ_x, displ_y' for 2D");
  params.addRequiredParam<std::string>("base_name",
    "base name of the section, e.g. aluminum, rubber, etc.");
  return params;
}

GumbyAddStressDivergenceKernelsAction::
GumbyAddStressDivergenceKernelsAction(InputParameters params)
  : Action(params),
    _displacements(getParam<std::vector<VariableName>>("displacements")),
    _base_name(getParam<std::string>("base_name"))
{}

void GumbyAddStressDivergenceKernelsAction::act()
{
  std::cout << "Adding stress divergence kernels to Gumby analysis" << std::endl;
  for (unsigned int i = 0; i < _displacements.size(); ++i)
  {
    std::cout << "disp = " << _displacements[i] << std::endl;
    std::string kernel_name = _base_name + "_gumby_stress_divergence_";
    if (i == 0)
      kernel_name += "x";
    else if (i == 1)
      kernel_name += "y";
    else if (i == 2)
      kernel_name += "z";

    auto params = _factory.getValidParams("GumbyStressDivergence");
    params.set<NonlinearVariableName>("variable") = _displacements[i];
    params.set<unsigned int>("component") = i;
    params.set<std::vector<VariableName>>("displacements") = _displacements;
    params.set<std::string>("base_name") = _base_name;
    _problem->addKernel("GumbyStressDivergence", kernel_name, params);
  }

  std::cout << std::endl << std::endl << std::endl;
}
