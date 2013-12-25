function darwinoplib_setup()

curpath = pwd;
tgtpath = curpath(1:end-length('\darwinoplib'));
addpath(tgtpath);
addpath(fullfile(tgtpath, 'darwinoplib'));
addpath(fullfile(tgtpath, 'blocks'));
addpath(fullfile(tgtpath, 'darwinoplibdemos'));
addpath(fullfile(tgtpath, 'putty'));

savepath;

disp('DarwinOP Target Path Setup Complete.');
