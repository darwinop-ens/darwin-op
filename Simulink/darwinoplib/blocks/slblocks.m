function blkStruct = slblocks

% Information for "Blocksets and Toolboxes" subsystem
blkStruct.Name = 'DarwinOP Lib';
blkStruct.OpenFcn = 'lib_darwinop';
blkStruct.MaskInitialization = '';
blkStruct.MaskDisplay = 'disp(''DarwinOP'')';

% Information for Simulink Library Browser
Browser(1).Library = 'lib_darwinop';
Browser(1).Name = 'DarwinOP Lib';
Browser(1).IsFlat = 1;% Is this library "flat" (i.e. no subsystems)?
blkStruct.Browser = Browser;
