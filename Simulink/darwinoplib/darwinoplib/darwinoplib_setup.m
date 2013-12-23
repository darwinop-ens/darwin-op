function darwinoplib_setup()

curpath = pwd;
tgtpath = curpath(1:end-length('\darwinoplib'));
addpath(fullfile(tgtpath, 'darwinoplib'));
addpath(fullfile(tgtpath, 'blocks'));
addpath(fullfile(tgtpath, 'darwinoplibdemos'));

savepath;

disp('DarwinOP Target Path Setup Complete.');