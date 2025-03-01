exec=$TCHEM_INSTALL_PATH/example/TChem_IgnitionZeroD.x
inputs=$TCHEM_INSTALL_PATH/example/data/nbutane

this="$exec --chemfile=$inputs/chem_nbutane_T.inp \
            --thermfile=$inputs/therm_nbutane.dat \
            --samplefile=$inputs/sample.dat \
            --outputfile=IgnSolutionT_cvode.dat \
            --use-cvode=true \
            --atol-newton=1e-18 \
            --rtol-newton=1e-8\
            --max-newton-iterations=20 \
            --tol-time=1e-8 \
            --rtol-time=1e-18 \
            --dtmax=1e-3 \
            --dtmin=1e-20 \
            --tend=2 \
            --time-iterations-per-interval=10 \
            --max-time-iterations=2000 \
            --ignition-delay-time-file=IgnitionDelayTimeT_cvode.dat \
            --ignition-delay-time-w-threshold-temperature-file=IgnitionDelayTimeTthresholdT_cvode.dat
            --threshold-temperature=1500"

echo $this
eval $this
