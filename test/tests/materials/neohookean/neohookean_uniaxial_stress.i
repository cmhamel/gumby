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
  [bottom_fixed_y]
    type = DirichletBC
    variable = displ_y
    boundary = 2
    value = 0.0
  []
  [back_fixed_z]
    type = DirichletBC
    variable = displ_z
    boundary = 3
    value = 0.0
  []
  [top_displace_y]
    type = FunctionDirichletBC
    variable = displ_y
    boundary = 5
    function = ramp
  []
[]

[AuxVariables]
  [./deformation_gradient_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./deformation_gradient_yy]
    order = CONSTANT
    family = MONOMIAL
  []
  [./deformation_gradient_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./pk1_stress_xx]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./pk1_stress_yy]
    order = CONSTANT
    family = MONOMIAL
  []
  [./pk1_stress_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [./deformation_gradient_xx]
    type = ADRankTwoAux
    rank_two_tensor = 'rubber_deformation_gradient_new'
    variable = deformation_gradient_xx
    index_i = 0
    index_j = 0
  [../]
  [./deformation_gradient_yy]
    type = ADRankTwoAux
    rank_two_tensor = 'rubber_deformation_gradient_new'
    variable = deformation_gradient_yy
    index_i = 1
    index_j = 1
  [../]
  [./deformation_gradient_zz]
    type = ADRankTwoAux
    rank_two_tensor = 'rubber_deformation_gradient_new'
    variable = deformation_gradient_zz
    index_i = 2
    index_j = 2
  [../]
  [./pk1_stress_xx]
    type = ADRankTwoAux
    rank_two_tensor = 'rubber_pk1_stress'
    variable = pk1_stress_xx
    index_i = 0
    index_j = 0
  [../]
  [./pk1_stress_yy]
    type = ADRankTwoAux
    rank_two_tensor = 'rubber_pk1_stress'
    variable = pk1_stress_yy
    index_i = 1
    index_j = 1
  [../]
  [./pk1_stress_zz]
    type = ADRankTwoAux
    rank_two_tensor = 'rubber_pk1_stress'
    variable = pk1_stress_zz
    index_i = 2
    index_j = 2
  [../]
[]

[Postprocessors]
  [./F_xx]
    type = PointValue
    point = '0 0 0'
    variable = deformation_gradient_xx
  [../]
  [./F_yy]
    type = PointValue
    point = '0 0 0'
    variable = deformation_gradient_yy
  [../]
  [./F_zz]
    type = PointValue
    point = '0 0 0'
    variable = deformation_gradient_zz
  [../]
  [./P_xx]
    type = PointValue
    point = '0 0 0'
    variable = pk1_stress_xx
  [../]
  [./P_yy]
    type = PointValue
    point = '0 0 0'
    variable = pk1_stress_yy
  [../]
  [./P_zz]
    type = PointValue
    point = '0 0 0'
    variable = pk1_stress_zz
  [../]
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
  file_base = neohookean_uniaxial_stress_out
  perf_graph = true
  print_linear_residuals = false
  execute_on = 'INITIAL TIMESTEP_END'
  [./csv]
    type = CSV
  [../]
[]
