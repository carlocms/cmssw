#!/bin/bash

base_dir=/gfsvol01/cms/users/giraldin/calib_new/CMSSW_15_0_0_pre2/src/Validation/MtdValidation/test
exec_dir=${PWD}

nome=`sed -n ${1},${1}p ${base_dir}/lista_MU`

cat > exec.sh << EOF
set -o noclobber
cat /etc/redhat-release
source $HOME/.bashrc
cd ${base_dir}
cmsenv

# whatever CMSSW related command

echo ${nome}
cd ${exec_dir}

cmsRun mtdValidation_cfg.py ${nome}

mv step3_inDQM.root ${base_dir}/step3_inDQM_${1}.root

EOF

cmssw-el8 -B $HOME -B /gfsvol01 -B $PWD --command-to-run source ${PWD}/exec.sh
