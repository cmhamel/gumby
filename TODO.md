# General things

1. Make seperate folder for strain measures to better organize code
	- add simple fast unit tests that check things are calculated correctly

2. Scope out generalizing state variables

3. Add traction BCs
	- also a pressure BC (this might be trickier in total-Lagrangian)

4. Post-processing for nodal forces (essentially an AUX kernel to calculate the P:gradphi if that's possible?)

5. Unit tests for material models
	- make a set of general deformation gradients used as testers
	- generalizing the testing approach for different motions to reduce code in unit tests

6. Scope out hourglassing and other stabilization methods

7. Exemplar models to consider
	- Neohookean (done except for testing)
	- Gent
	- FeFp plasticity (linear hardening to start, then generalize)
	- FeFv viscoelasticity (this will put the state variable interface to the test)

8. Add simple thermal kernels

9. Other multiphysics kernels for convenience?


