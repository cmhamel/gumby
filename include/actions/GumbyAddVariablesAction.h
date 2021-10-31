#pragma once
#include "Action.h"

class GumbyAddVariablesAction : public Action
{
public:
  GumbyAddVariablesAction(InputParameters params);

  virtual void act() override;

protected:
  std::vector<VariableName> _displacements;
  std::string _displacements_order;
  std::string _displacements_family;
};

template <>
InputParameters validParams<GumbyAddVariablesAction>();
