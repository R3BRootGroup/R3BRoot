// ROOT programs tend not to produce coredumps, because root
// helpfully catches SIGSEGV and tries to display its own
// idea of a stack trace, which really makes post-mortem
// debugging impossible.
// run root as 'LD_PRELOAD=libfixsignals.so root -l whatever.C+'
// to get core dumps. 

// we overwrite the sigaction syscall so ROOT does not fsck 
// with important system handlers

// regarding API hooking, see e.g. 
// http://stackoverflow.com/questions/69859

// As root (5?) cleverly uses sigusr, we have to allow that signal
//  to be overwritten. 

// TODO: Because Geant4 tends to enable FPEs, (G4FPEDetection.hh)
// hooking fesetenv (or Geant4's function InvalidOperationDetection)
// is probably up next. 

// Note: exporting LD_PRELOAD=libfixsignals.so will mess up all other sorts of
// programs and is probably not a great idea. 

#define _GNU_SOURCE
#include <dlfcn.h>
//#define _FCNTL_H
//#include <bits/fcntl.h>
#define _SIGNAL_H
typedef int __pid_t;
typedef unsigned int __uid_t;
typedef long int __clock_t;
#include <bits/sigset.h>
#include <bits/siginfo.h>
#include <asm-generic/signal-defs.h>
#include <bits/sigaction.h>
#include <bits/signum.h>
#include <stdio.h>


int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact)
{
  FILE* f=fopen("fixsignals.log", "a");
  if (f)
    {
      fprintf(f, "caught sigaction(%2d, %20p, %20p)\n", signum, act, oldact);
      fclose(f);
    }
  if (signum != SIGUSR1 && signum != SIGUSR2)
    return 0;

  int (* _sigaction)(int, const struct sigaction *,
                     struct sigaction *)=dlsym(RTLD_NEXT, "sigaction");
  return _sigaction(signum, act, oldact);
}
