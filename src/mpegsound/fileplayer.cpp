/* MPEG Sound library

   (C) 1997 by Jung woo-jae */

// Fileplayer.cc
// It's an example for how to use MPEG Sound library

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mpegsound.h"

// File player superclass
Fileplayer::Fileplayer()
{
  __errorcode=SOUND_ERROR_OK;
  player=NULL;
};

Fileplayer::~Fileplayer()
{
  delete player;
};

// Mpegfileplayer
Mpegfileplayer::Mpegfileplayer()
{
  loader=NULL;
  server=NULL;
};

Mpegfileplayer::~Mpegfileplayer()
{
  if(loader)delete loader;
  if(server)delete server;
}


void Mpegfileplayer::setforcetomono(bool flag)
{
  server->setforcetomono(flag);
};

void Mpegfileplayer::setdownfrequency(int value)
{
  server->setdownfrequency(value);
};

#include <math.h>
char * tominsec(double s)
{
  int min=int(floor(s/60));
  double sec=s-60*min;
  char * buff = new char[15];
  sprintf(buff,"%d:%2.2f",min,sec);
  return buff;
}

bool Mpegfileplayer::playing(int verbose, bool frameinfo, int startframe)
{
  if(!server->run(-1))return false;       // Initialize MPEG Layer 3
  if(verbose>0)showverbose(verbose);

  if (startframe) server->setframe(startframe);

  int pcmperframe=server->getpcmperframe();
  int frequency=server->getfrequency();
  int totframes=server->gettotalframe();
  double tottime=1.0*totframes*pcmperframe/frequency;
  if(frameinfo) {
    cout << "Totalframes " <<  totframes;
    cout << "; Totaltime " << tominsec(tottime)  << endl;
  }

  // Playing
  int err;
  while( err)  { 
    err = server->run(100);
    if(frameinfo) {
      int currframe=server-> getcurrentframe();
      double currtime=1.0*currframe*pcmperframe/frequency;
      cout << "Frame " << currframe << " [" << totframes-currframe << "]; ";
      cout << "Time " << tominsec(currtime) << " [" ;
      cout << tominsec(tottime-currtime) << "]" << endl ;
    }
  }

  seterrorcode(server->geterrorcode());
  if(seterrorcode(SOUND_ERROR_FINISH))return true;
  return false;
}

#ifdef PTHREADEDMPEG
bool Mpegfileplayer::playingwiththread(int verbose,bool frameinfo,
				       int framenumbers, int startframe)
{
  if(framenumbers<20)return playing(verbose,frameinfo,startframe);

  server->makethreadedplayer(framenumbers);

  if(!server->run(-1))return false;       // Initialize MPEG Layer 3
  if(verbose>0)showverbose(verbose);

  if (startframe) server->setframe(startframe);

  int pcmperframe=server->getpcmperframe();
  int frequency=server->getfrequency();
  int totframes=server->gettotalframe();
  double tottime=1.0*totframes*pcmperframe/frequency;
  if(frameinfo) {
    cout << "Totalframes " <<  totframes;
    cout << "; Totaltime " << tominsec(tottime)  << endl;
  }

  // Playing
  int err;
  while( err)  { 
    err = server->run(100);
    if(frameinfo) {
      int currframe=server-> getcurrentframe();
      double currtime=1.0*currframe*pcmperframe/frequency;
      cout << "Frame " << currframe << " [" << totframes-currframe << "]; ";
      cout << "Time " << tominsec(currtime) << " [" ;
      cout << tominsec(tottime-currtime) << "]" << endl ;
    }
  }

  server->freethreadedplayer();
  
  seterrorcode(server->geterrorcode());
  if(seterrorcode(SOUND_ERROR_FINISH))return true;
  return false;
}
#endif

void Mpegfileplayer::showverbose(int )
{
  static char *modestring[4]={"stereo","joint stereo","dual channel","mono"};

  fprintf(stderr,"\tMPEG-%d Layer %d, %s,\n\t%dHz%s, %dkbit/s, ",
	  server->getversion()+1,
	  server->getlayer(),modestring[server->getmode()],
	  server->getfrequency(),server->getdownfrequency()?"//2":"",
	  server->getbitrate());
  fprintf(stderr,server->getcrccheck() 
	  ? "with crc check\n" 
	  : "without crc check\n");
}
