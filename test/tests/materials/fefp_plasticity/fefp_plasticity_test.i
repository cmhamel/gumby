[Mesh]
  [generated_mesh]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 1
    ny = 1
    # nz = 2
    xmin = 0
    ymin = 0
    # zmin = 0
    xmax = 1
    ymax = 5
    # zmax = 1
  []
  [assign_block]
    type = SubdomainBoundingBoxGenerator
    input = generated_mesh
    bottom_left = '0 0 0'
    top_right = '1 5 0'
    block_id = 1
  []
  # [node_set]
  #   type = BoundingBoxNodeSetGenerator
  #   input = assign_block
  #   bottom_left = '0 0 0'
  #   top_right = '0 0 0'
  #   new_boundary = 'corner'
  # []
[]

[GlobalParams]
  #displacements = 'displ_x displ_y displ_z'
  displacements = 'displ_x displ_y'
[]

[GumbyVariables]
  displacements_family = LAGRANGE
  displacements_order = FIRST
[]

[GumbySections]
  [./metal_section]
    base_name = 'metal'
    block = 1
  [../]
[]

[Materials]
  [metal]
    type = GumbyADFeFpPlasticity
    plane_strain = on
    base_name = 'metal'
    bulk_modulus = 58.33e9
    shear_modulus = 26.92e9
    yield_stress = 200.0e6
    hardening_modulus = 0.0
  []
[]

[Functions]
  [ramp]
    type = ParsedFunction
    value = '5 * t'
  []
[]

[BCs]
  [./GumbyFixedDisplacementBC]
    # [./corner_fixed]
    #   components = 'x y z'
    #   boundary = 'corner'
    # [../]
    # [./left_fixed_x]
    #   components = 'x'
    #   boundary = 'left'
    # [../]
    # [./bottom_fixed_y]
    #   components = 'y'
    #   boundary = 'bottom'
    # [../]
    # [./back_fixed_z]
    #   components = 'z'
    #   boundary = 'back'
    # [../]
    [./bottom_fixed]
      components = 'x y'
      boundary = 'bottom'
    [../]
    [./top_fixed_xz]
      components = 'x'
      boundary = 'top'
    [../]
  [../]
  [./top_displace_y]
    type = FunctionDirichletBC
    variable = displ_y
    boundary = 'top'
    function = ramp
  [../]
[]

# [GumbyOutputElementVariables]
#   base_names = 'metal'
#   blocks = '1'
#   deformation_gradient = 'F_field'
#   pk1_stress = 'P_field'
# []

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'
  # solve_type = 'PJFNK'
  start_time = 0.0
  end_time = 1.0
  dt = 0.05
  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-12
  l_max_its = 500
  nl_max_its = 250
[]

[Outputs]
  exodus = true
  file_base = fefp_plasticity_test_out
  perf_graph = true
  print_linear_residuals = false
  execute_on = 'INITIAL TIMESTEP_END'
[]
