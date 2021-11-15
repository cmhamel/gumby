#pragma once
#include "Action.h"

class GumbyAddSectionAction : public Action
{
public:
  GumbyAddSectionAction(InputParameters params);

  virtual void act() override;

protected:
  std::vector<VariableName> _displacements;
  std::string _base_name;
  std::vector<SubdomainName> _blocks;
};

template <>
InputParameters validParams<GumbyAddSectionAction>();
