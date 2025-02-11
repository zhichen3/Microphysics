# a network for He/C burning that includes rates that
# bypass the C12(a,g)O16 rate.  This does not make (a,p)(p,g)
# approximations.

import pynucastro as pyna
from pynucastro.networks import AmrexAstroCxxNetwork

def get_library():

    reaclib_lib = pyna.ReacLibLibrary()

    all_reactants = ["p",
                     "he4", "c12", "o16", "ne20", "mg24", "si28", "s32",
                     "ar36", "ca40", "ti44", "cr48", "fe52", "ni56",
                     "al27", "p31", "cl35", "k39", "sc43", "v47", "mn51", "co55",
                     "n13", "n14", "f18", "ne21", "na22", "na23"]

    subch = reaclib_lib.linking_nuclei(all_reactants)

    # in this list, we have the reactants, the actual reactants,
    # and modified products that we will use instead
    other_rates = [(("c12", "c12"), ("mg23", "n"), ("mg24")),
                   (("o16", "o16"), ("s31", "n"), ("s32")),
                   (("c12", "o16"), ("si27", "n"), ("si28"))]

    for r, p, mp in other_rates:
        rfilter = pyna.rates.RateFilter(reactants=r, products=p)
        _library = reaclib_lib.filter(rfilter)
        r = _library.get_rates()[0]
        r.modify_products(mp)
        subch += _library

    return subch

def doit():

    subch = get_library()

    rc = pyna.RateCollection(libraries=[subch])

    print(f"number of nuclei: {len(rc.unique_nuclei)}")
    print(f"number of rates: {len(rc.rates)}")

    comp = pyna.Composition(rc.get_nuclei())
    comp.set_all(0.1)
    comp.set_nuc("he4", 0.95)
    comp.normalize()

    rc.plot(outfile="subch_full.png", rho=1.e6, T=1.e9, comp=comp,
            rotated=True, hide_xalpha=True, curved_edges=True,
            size=(1500, 450),
            node_size=500, node_font_size=11, node_color="#337dff", node_shape="s",
            Z_range=(1,29))

    # these are the rates that we are going to allow to be optionally
    # zeroed
    r1 = subch.get_rate("p_c12__n13")
    r2 = subch.get_rate("he4_n13__p_o16")

    net = AmrexAstroCxxNetwork(libraries=[subch], symmetric_screening=True, disable_rate_params=[r1, r2])
    net.write_network()


if __name__ == "__main__":
    doit()
