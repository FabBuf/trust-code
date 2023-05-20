#!/bin/bash
##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   # qstat inexistente sur les dernieres machines du CCRT/TGCC
   echo "Command qstat created on $HOST"
   cp $TRUST_ROOT/bin/KSH/qstat_wrapper $TRUST_ROOT/bin/KSH/qstat
   # Initialisation de l environnement module $MODULE_PATH
   echo "source /etc/profile" >> $env
   #
   # Load modules
   module="craype-x86-trento craype-network-ofi PrgEnv-cray rocm craype-accel-amd-gfx90a libfabric"
   # Mise a jour du 03/02/23:
   module="craype-x86-trento craype-network-ofi rocm craype-accel-amd-gfx90a libfabric cce/15.0.0 craype/2.7.19 cray-mpich/8.1.21 cray-libsci/22.11.1.2"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env
   echo "PATH=\$CRAY_MPICH_PREFIX/bin:\$PATH"  >> $env # Pour trouver mpicxx
   echo "export TRUST_DEVICES_PER_NODE=8" >> $env # Devices per node
   . $env
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   # Documentation adastra: https://dci.dci-gitlab.cines.fr/webextranet/
   soumission=1
   if [ "$prod" = 1 ]
   then
      qos="" && cpu=86400 # 1 day
   else
      qos="" && cpu=1440 # 24 mn
   fi
   if [ "$gpu" = 1 ]
   then
      # Partition mi250 (4 cartes MI250X par noeud=8 cartes MI100):
      constraint=MI250
      ntasks=64 # Node cores
      cpus_per_task=8 # 1 GPU/MPI (OpenMP choix par default)
      noeuds=`echo "1+($NB_PROCS-1)/8" | bc`
      srun_options="--gpus-per-task=1 --ntasks-per-node=8 --threads-per-core=1 --gpu-bind=verbose,closest"
      #[ $NB_PROCS -gt 8 ] && qos=normal # 2 nodes
   else
      # Partition scalaire
      constraint=genoa
      ntasks=192 # Node cores
      noeuds=`echo "1+($NB_PROCS-1)/$ntasks" | bc`
      srun_options=""
      #[ $NB_PROCS -gt ??? ] && qos=???
   fi
   project="cpa2202"
   node=1 # --exclusive
   mpirun="srun -l $srun_options --mpi=cray_shasta --cpu-bind=verbose,cores"
   sub=SLURM
}

