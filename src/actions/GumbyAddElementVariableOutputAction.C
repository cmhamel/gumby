#include "GumbyAddElementVariableOutputAction.h"
#include "Factory.h"
#include "Parser.h"
#include "FEProblem.h"

registerMooseAction("GumbyApp", GumbyAddElementVariableOutputAction, "add_aux_variable");
registerMooseAction("GumbyApp", GumbyAddElementVariableOutputAction, "add_aux_kernel");

template<>
InputParameters validParams<GumbyAddElementVariableOutputAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::vector<std::string>>("base_names",
    "base names of the ElementVariableOutput, e.g. aluminum, rubber, etc.");
  params.addRequiredParam<std::vector<std::string>>("blocks",
    "block numbers");
  params.addParam<std::string>("deformation_gradient",
    "set to name of output if you want to output all 9 components of the deformation gradient");
  params.addParam<std::string>("pk1_stress",
    "set to name of output if you want to output all 9 components of the pk1 stress");

  return params;
}

GumbyAddElementVariableOutputAction::
GumbyAddElementVariableOutputAction(InputParameters params)
  : Action(params),
    _base_names(getParam<std::vector<std::string>>("base_names")),
    _blocks(getParam<std::vector<std::string>>("blocks"))
{}

void GumbyAddElementVariableOutputAction::act()
{
  if (_current_task == "add_aux_variable")
  {
    if (isParamValid("deformation_gradient"))
    {
      std::string variable_base_name = getParam<std::string>("deformation_gradient");
      addFullTensorOutputVariables(variable_base_name);
    }
    if (isParamValid("pk1_stress"))
    {
      std::string variable_base_name = getParam<std::string>("pk1_stress");
      addFullTensorOutputVariables(variable_base_name);
    }
  }
  else if (_current_task == "add_aux_kernel")
  {
    for (unsigned int i = 0; i < _base_names.size(); ++i)
    {
      if (isParamValid("deformation_gradient"))
      {
        std::string section_base_name = _base_names[i] + "_deformation_gradient_new";
        std::string variable_base_name = getParam<std::string>("deformation_gradient");
        addFullTensorOutputAuxKernels(section_base_name, variable_base_name, _blocks[i]);
      }
      if (isParamValid("pk1_stress"))
      {
        std::string section_base_name = _base_names[i] + "_pk1_stress";
        std::string variable_base_name = getParam<std::string>("pk1_stress");
        addFullTensorOutputAuxKernels(section_base_name, variable_base_name, _blocks[i]);
      }
    }
  }
}

void GumbyAddElementVariableOutputAction::
addFullTensorOutputVariables(std::string variable_base_name)
{
  auto params = _factory.getValidParams("MooseVariable");
  params.set<MooseEnum>("order") = "FIRST";
  params.set<MooseEnum>("family") = "MONOMIAL";
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_xx", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_xy", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_xz", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_yx", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_yy", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_yz", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_zx", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_zy", params);
  _problem->addAuxVariable("MooseVariable", variable_base_name + "_zz", params);
}

void GumbyAddElementVariableOutputAction::
addFullTensorOutputAuxKernels(std::string section_base_name,
                              std::string variable_base_name,
                              std::string block)
{
  std::vector<SubdomainName> subdomains;
  subdomains.push_back(block);

  auto params = _factory.getValidParams("ADRankTwoAux");
  params.set<MaterialPropertyName>("rank_two_tensor") = section_base_name;
  params.set<std::vector<SubdomainName>>("block") = subdomains;
  //
  params.set<AuxVariableName>("variable") = variable_base_name + "_xx";
  params.set<unsigned int>("index_i") = 0;
  params.set<unsigned int>("index_j") = 0;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_xx", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_xy";
  params.set<unsigned int>("index_i") = 0;
  params.set<unsigned int>("index_j") = 1;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_xy", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_xz";
  params.set<unsigned int>("index_i") = 0;
  params.set<unsigned int>("index_j") = 2;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_xz", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_yx";
  params.set<unsigned int>("index_i") = 1;
  params.set<unsigned int>("index_j") = 0;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_yx", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_yy";
  params.set<unsigned int>("index_i") = 1;
  params.set<unsigned int>("index_j") = 1;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_yy", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_yz";
  params.set<unsigned int>("index_i") = 1;
  params.set<unsigned int>("index_j") = 2;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_yz", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_zx";
  params.set<unsigned int>("index_i") = 2;
  params.set<unsigned int>("index_j") = 0;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_zx", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_zy";
  params.set<unsigned int>("index_i") = 2;
  params.set<unsigned int>("index_j") = 1;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_zy", params);
  params.set<AuxVariableName>("variable") = variable_base_name + "_zz";
  params.set<unsigned int>("index_i") = 2;
  params.set<unsigned int>("index_j") = 2;
  _problem->addAuxKernel("ADRankTwoAux", variable_base_name + "_" + section_base_name + "_zz", params);
}
