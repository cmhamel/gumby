#pragma once
#include "Action.h"

class GumbyAddElementVariableOutputAction : public Action
{
public:
  GumbyAddElementVariableOutputAction(InputParameters params);

  virtual void act() override;

protected:
  void addFullTensorOutputVariables(std::string variable_base_name);
  void addFullTensorOutputAuxKernels(std::string section_base_name,
                                     std::string variable_base_name,
                                     std::string block);

  std::vector<std::string> _base_names;
  std::vector<std::string> _blocks;
};

template <>
InputParameters validParams<GumbyAddElementVariableOutputAction>();
