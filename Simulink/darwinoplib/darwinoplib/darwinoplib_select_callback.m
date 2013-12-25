function darwinop_select_callback(hDlg, hSrc)

% Setup the hardware configuration
slConfigUISetVal(hDlg, hSrc, 'ProdHWDeviceType', 'Generic->32-bit Embedded Processor');
        
% Set the target language to C and disable modification
slConfigUISetVal(hDlg, hSrc, 'TargetLang', 'C++');
slConfigUISetEnabled(hDlg, hSrc, 'TargetLang', 0);

% Use our own ert_main.c
slConfigUISetVal(hDlg, hSrc, 'ERTCustomFileTemplate', 'darwinoplib_main.tlc');
slConfigUISetVal(hDlg, hSrc, 'GenerateSampleERTMain', 'off');
slConfigUISetEnabled(hDlg, hSrc, 'GenerateSampleERTMain',0);

% DarwinOP is Little Endian
slConfigUISetVal(hDlg, hSrc, 'ProdEndianess', 'LittleEndian');

% The target is model reference compliant
slConfigUISetVal(hDlg, hSrc, 'ModelReferenceCompliant', 'on');
slConfigUISetEnabled(hDlg, hSrc, 'ModelReferenceCompliant', false);

% Set the solver type to fixed step
slConfigUISetVal(hDlg, hSrc, 'SolverType', 'Fixed-Step');
slConfigUISetVal(hDlg, hSrc, 'Solver', 'FixedStepDiscrete');

% Generate report
slConfigUISetVal(hDlg, hSrc, 'GenerateReport', 'off');
slConfigUISetVal(hDlg, hSrc, 'LaunchReport', 'off');
slConfigUISetVal(hDlg, hSrc, 'GenerateCodeMetricsReport', 'off');
