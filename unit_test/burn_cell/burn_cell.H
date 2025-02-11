#include <extern_parameters.H>
#include <eos.H>
#include <network.H>
#include <burner.H>
#include <fstream>
#include <iostream>
#include <iomanip>

void burn_cell_c()
{

    burn_t state;

    Real massfractions[NumSpec] = {-1.0};

    // Make sure user set all the mass fractions to values in the interval [0, 1]
    for (int n = 1; n <= NumSpec; ++n) {
        switch (n) {

        case 1:
            massfractions[n-1] = X1;
            break;
        case 2:
            massfractions[n-1] = X2;
            break;
        case 3:
            massfractions[n-1] = X3;
            break;
        case 4:
            massfractions[n-1] = X4;
            break;
        case 5:
            massfractions[n-1] = X5;
            break;
        case 6:
            massfractions[n-1] = X6;
            break;
        case 7:
            massfractions[n-1] = X7;
            break;
        case 8:
            massfractions[n-1] = X8;
            break;
        case 9:
            massfractions[n-1] = X9;
            break;
        case 10:
            massfractions[n-1] = X10;
            break;
        case 11:
            massfractions[n-1] = X11;
            break;
        case 12:
            massfractions[n-1] = X12;
            break;
        case 13:
            massfractions[n-1] = X13;
            break;
        case 14:
            massfractions[n-1] = X14;
            break;
        case 15:
            massfractions[n-1] = X15;
            break;
        case 16:
            massfractions[n-1] = X16;
            break;
        case 17:
            massfractions[n-1] = X17;
            break;
        case 18:
            massfractions[n-1] = X18;
            break;
        case 19:
            massfractions[n-1] = X19;
            break;
        case 20:
            massfractions[n-1] = X20;
            break;
        case 21:
            massfractions[n-1] = X21;
            break;

        }

        if (massfractions[n-1] < 0 || massfractions[n-1] > 1) {
            amrex::Error("mass fraction for " + short_spec_names_cxx[n-1] + " not initialized in the interval [0,1]!");
        }

    }

    // Echo initial conditions at burn and fill burn state input

    std::cout << "Maximum Time (s): " << tmax << std::endl;
    std::cout << "State Density (g/cm^3): " << density << std::endl;
    std::cout << "State Temperature (K): " << temperature << std::endl;
    for (int n = 0; n < NumSpec; ++n) {
        std::cout << "Mass Fraction (" << short_spec_names_cxx[n] << "): " << massfractions[n] << std::endl;
    }

    state.T = temperature;
    state.rho = density;
    for (int n = 0; n < NumSpec; ++n) {
        state.xn[n] = massfractions[n];
    }

    // normalize -- just in case

    if (! skip_initial_normalization) {
        normalize_abundances_burn(state);
    }

    // update the composition quantities

    eos(eos_input_rt, state);

    // output just the instantaneous RHS

    Array1D<Real, 1, neqs> ydot;
    actual_rhs(state, ydot);

    std::cout << "RHS at t = 0" << std::endl;
    for(int n = 0; n < NumSpec; ++n){
        std::string element = short_spec_names_cxx[n];
        std::cout << std::setw(6) << element << " " << ydot(n+1) << std::endl;
    }

    // output initial burn type data
    Real time = 0.0_rt;

    // call the EOS to set initial e -- it actually doesn't matter to
    // the burn but we need to keep track of e to get a valid
    // temperature for the burn if we substep

    eos(eos_input_rt, state);

    std::ofstream state_over_time("state_over_time.txt");

    // we will divide the total integration time into nsteps that are
    // logarithmically spaced

    if (tfirst == 0.0_rt) {
        if (nsteps == 1) {
            tfirst = tmax;
        } else {
            tfirst = tmax / nsteps;
        }
    }
    Real dlogt = 0.0_rt;
    if (nsteps == 1) {
        dlogt = (std::log10(tmax) - std::log10(tfirst));
    } else {
        dlogt = (std::log10(tmax) - std::log10(tfirst)) / (nsteps - 1);
    }

    // save the initial state -- we'll use this to determine
    // how much things changed over the entire burn

    burn_t state_in = state;

    // output the data in columns, one line per timestep


    state_over_time << std::setw(25) << "# Time";
    state_over_time << std::setw(25) << "Temperature";
    for(int x = 0; x < NumSpec; ++x){
        std::string element = short_spec_names_cxx[x];
	state_over_time << std::setw(25) << element;
    }
    state_over_time << std::endl;
    state_over_time << std::setprecision(15);

    Real t = 0.0;

    state_over_time << std::setw(25) << t;
    state_over_time << std::setw(25) << state.T;
    for (int x = 0; x < NumSpec; ++x){
        state_over_time << std::setw(25) << state.xn[x];
    }
    state_over_time << std::endl;


    // store the initial internal energy -- we'll update this after
    // each substep

    Real energy_initial = state.e;

    // loop over steps, burn, and output the current state

    int nstep_int = 0;

    for (int n = 0; n < nsteps; n++){

        // compute the time we wish to integrate to

        Real tend = std::pow(10.0_rt, std::log10(tfirst) + dlogt * n);
        Real dt = tend - t;

    	burner(state, dt);

        nstep_int += state.n_step;

        // state.e represents the change in energy over the burn (for
        // just this sybcycle), so turn it back into a physical energy

        state.e += energy_initial;

        // reset the initial energy for the next subcycle

        energy_initial = state.e;

        // get the updated T

        if (call_eos_in_rhs) {
            eos(eos_input_re, state);
        }

        t += dt;

	state_over_time << std::setw(25) << t;
	state_over_time << std::setw(25) << state.T;
	for (int x = 0; x < NumSpec; ++x){
	     state_over_time << std::setw(25) << state.xn[x];
	}
	state_over_time << std::endl;
    }
    state_over_time.close();

    // output diagnostics to the terminal

    std::cout << "------------------------------------" << std::endl;
    std::cout << "successful? " << state.success << std::endl;
    std::cout << " - Hnuc = " << (state.e - state_in.e) / tmax << std::endl;
    std::cout << " - added e = " << state.e - state_in.e << std::endl;
    std::cout << " - final T = " << state.T << std::endl;


    std::cout << "------------------------------------" << std::endl;
    std::cout << "e initial = " << state_in.e << std::endl;
    std::cout << "e final =   " << state.e << std::endl;


    std::cout << "------------------------------------" << std::endl;
    std::cout << "new mass fractions: " << std::endl;
    for (int n = 0; n < NumSpec; ++n) {
        std::string element = short_spec_names_cxx[n];
        std::cout << element << " " << state.xn[n] << std::endl;
    }

    std::cout << "------------------------------------" << std::endl;
    std::cout << "species creation rates: " << std::endl;
    for (int n = 0; n < NumSpec; ++n) {
        std::cout << "omegadot(" << short_spec_names_cxx[n] << "): "
                  << (state.xn[n] - state_in.xn[n]) / tmax << std::endl;
    }

    std::cout << "number of steps taken: " << nstep_int << std::endl;

}
