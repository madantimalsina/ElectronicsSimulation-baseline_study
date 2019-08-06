<a href="https://scan.coverity.com/projects/electronicssimulation">
  <img alt="Coverity Scan Build Status"
       src="https://img.shields.io/coverity/scan/16034.svg"/>
</a>

Repository for LZ Detector Electronics Simulation Code.
====

Generated documentation: http://luxzeplin.web.cern.ch/luxzeplin/docs/der/master/

Introduction
===
This documentation summarises the content of the simulation, which is the
detector electronics response software of the LZ Experiment.

This document applies to all documents, documentation, information, code,
ideas, and information - both in part and wholly, as well as both implicitly and
explicitly - uploaded to the ElectronicsSimulation, and regardless of its
current state.

SPECIAL NOTICE: The QE factors have been re-scaled
to comply with BACCARAT rescaling parameters. This is set in the DERCONFIG file.
Users should be aware of this change when they are analysing results. Once BACCARAT
provides this as an output variable, this process will be obviated.

Simulation Inputs and Outputs
===
The DER input is a DER-ready .root file containing times of arrival of photons incident on PMT Photocathodes.
The DER output is a .root file containing a time and the response in the
current format of the Event Builder.

Installation and Usage (PDSF)
===
Step 1: Download the files
==
Obtain the Electronics Simulation code by doing

    git clone git@lz-git.ua.edu:sim/ElectronicsSimulation.git

Step 2: Compile
==
Load the currently supported version of gcc and ROOT (`bash`):

    source setup.sh

The software can be compiled & installed using the supplied makefile. To do so, simply type

    make

The compilation process indicates which components are being compiled. There should be no errors or warnings.

The result will be an executable called

    DER

in the `${DER_INSTALL_DIR}/bin` directory. Since `${DER_INSTALL_DIR}/bin` will be included in the `$PATH`,
you can call the executable from within any folder.

Step 3: Run
==
The DER assumes its input is DER-ready. To obtain such a file, please follow
the steps below. Please also ensure that the aforementioned modules and
environment variables have been set.

1. A .bin file must first be produced using BACCARAT. Run BACCARAT, ensuring
the macro is set-up to track the following volumes and record levels using

        /Bacc/detector/recordLevel LiquidXenonTarget 3
        /Bacc/detector/recordLevelOptPhot LiquidXenonTarget 5
        /Bacc/detector/recordLevelThermElec LiquidXenonTarget 5
        /Bacc/detector/recordLevelOptPhot PMT_Photocathode 3
2. Then process the output using BaccRootConverter (this produces a .root file with the same name).

        ./BaccRootConverter output.bin
3. Then process with BaccMCTruth (which creates a new .root file).

        ./BaccMCTruth output.root
4. Run the DER using the format

        DER --source file.root

Where `output.bin` is the output of BACCARAT,
`output.root` is the output of BaccRootConverter, and
`file.root` is the output of BaccMCTruth and the input path for the DER.

By default, the output of the DER will be a new .root file located in the same
directory in which the DER was executed. This output file is compliant with the
event builder format and is ready to be used in LZap.

The DERCONFIG.txt file is used for setting or changing options. By definition,
the DER executable looks for the DERCONFIG.txt file in the same directory as the
executable. The DERCONFIG.txt file is required in order to run the DER.

Please ensure that the same version of ROOT is used throughout production.

Additional Options
==
A number of settings are available in DERCONFIG.txt. It is recommended to leave them as default.
All settings in DERCONFIG.txt can be overriden in the CLI using the format:

    --settingname value

where `settingname` should be replaced by the exact setting name in DERCONFIG.txt, and `value`
should be replaced by a suitable value, as per DERCONFIG.txt. The complete format is therefore:

    DER --setting value --source /path/to/inputfile.root

Failure to provide a valid input will stop execution.

A custom DERCONFIG path can be specified using the following options

    --DERCONFIGPath /path/to/DERCONFIG.txt

Further Documentation
===
More detailed documentation can be found on the LZ TWiki page.
http://teacher.pas.rochester.edu:8080/wiki/bin/view/Lz/DetectorElectronicsResponse

Contact and Support
===
If you notice any errors or have suggestions,
please contact https://luxzeplin.slack.com/messages/C04UMD0BM.

Created by Cees Carels on 01/01/2016.
Copyright © 2016 Cees Carels. All rights reserved.
