[Mesh]
  type = FileMesh
  file = cube.e
[]

[GumbyVariables]
  displacements = 'displ_x displ_y displ_z'
  displacements_family = LAGRANGE
  displacements_order = FIRST
[]

[GumbySections]
  [./rubber_section]
    displacements = 'displ_x displ_y displ_z'
    base_name = 'rubber'
    block = 1
  [../]
[]

[Materials]
  [rubber]
    type = GumbyADNeohookean
    displacements = 'displ_x displ_y displ_z'
    base_name = 'rubber'
    bulk_modulus = 1000.0e6
    shear_modulus = 1.0e6
  []
[]

[Functions]
  [ramp]
    type = ParsedFunction
    value = 't'
  []
[]

[BCs]
  [corner_fixed_x]
    type = DirichletBC
    variable = displ_x
    boundary = 7
    value = 0.0
  []
  [corner_fixed_y]
    type = DirichletBC
    variable = displ_y
    boundary = 7
    value = 0.0
  []
  [corner_fixed_z]
    type = DirichletBC
    variable = displ_z
    boundary = 7
    value = 0.0
  []
  [left_fixed_x]
    type = DirichletBC
    variable = displ_x
    boundary = 1
    value = 0.0
  []
  [left_fixed_y]
    type = DirichletBC
    variable = displ_y
    boundary = 1
    value = 0.0
  []
  [left_fixed_z]
    type = DirichletBC
    variable = displ_z
    boundary = 1
    value = 0.0
  []
  [right_displace_y]
    type = FunctionDirichletBC
    variable = displ_y
    boundary = 4
    function = ramp
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'newton'
  start_time = 0.0
  end_time = 1.0
  dt = 0.01
  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10
  l_max_its = 100
  nl_max_its = 250
[]

[Outputs]
  exodus = true
  file_base = neohookean_simple_shear_out
  perf_graph = true
  print_linear_residuals = false
  execute_on = 'INITIAL TIMESTEP_END'
  [./csv]
    type = CSV
  [../]
[]
