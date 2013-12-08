//
// ZoneMinder Remote Camera Class Implementation, $Date$, $Revision$
// Copyright (C) 2001-2008 Philip Coombes
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

#include "zm.h"

#include "zm_remote_camera_vlc.h"
#include "zm_mem_utils.h"

#include <sys/types.h>
#include <sys/socket.h>


RemoteCameraVlc::RemoteCameraVlc( int p_id , const std::string &p_path, int p_width, int p_height, int p_colours, int p_brightness, int p_contrast, int p_hue, int p_colour, bool p_capture ) :
    RemoteCamera( p_id, "rtsp", "", "", "", p_width, p_height, p_colours, p_brightness, p_contrast, p_hue, p_colour, p_capture )
   ,path(p_path)
{

//	if ( capture )
//	{
		Initialise();
//	}
	frameCount = 0;
	
	
}

RemoteCameraVlc::~RemoteCameraVlc()
{

}

void RemoteCameraVlc::Initialise()
{
//    RemoteCamera::Initialise();

//	int max_size = width*height*colours;

//	buffer.size( max_size );


    Connect();
}

void RemoteCameraVlc::Terminate()
{
    Disconnect();
}
void *RemoteCameraVlc:: lock(void *data, void **p_pixels)
{
  struct context *c = (context *)data;
  pthread_mutex_lock(&c->mutex);
    *p_pixels = c->pixel;

//    SDL_LockMutex(c->mutex);
//    SDL_LockTexture(c->texture, NULL, p_pixels, &pitch);

  return NULL; // Picture identifier, not needed here.

}

void RemoteCameraVlc::unlock(void *data, void *id, void *const *p_pixels)
{
  struct context *c = (context *)data;
c->n++;
pthread_cond_signal(&c->cond);

  pthread_mutex_unlock(&c->mutex);
}

void RemoteCameraVlc::display(void *data, void *id)
{

}

int RemoteCameraVlc::Connect()
{
  char const *vlc_argv[] = {

      "--no-audio", // Don't play audio.
      "--no-xlib", // Don't use Xlib.
      "--no-one-instance",
      "--no-started-from-file",
      "--no-syslog",
      "--no-one-instance-when-started-from-file"

      // Apply a video filter.
      //"--video-filter", "sepia",
      //"--sepia-intensity=200"
  };
	Info("VLC Connect id: %d width: %d height: %d colours: %d subpixelorder: %d capture: %d",id,width,height,colours,subpixelorder,capture);
//logTerm();
  int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

  libvlc = libvlc_new(vlc_argc, vlc_argv);
  if(NULL == libvlc) {
      printf("LibVLC initialization failure.\n");
      return EXIT_FAILURE;
  }

//  m = libvlc_media_new_path(libvlc, "rtsp://179.223.187.28:5767/0/");
  m = libvlc_media_new_path(libvlc, path.c_str());
  mp = libvlc_media_player_new_from_media(m);
  libvlc_media_release(m);

  ctx.pixel=(uint8_t *)malloc(width * height * colours);
  ctx.rcl=this;

  pthread_mutex_init(&ctx.mutex,NULL);
  pthread_cond_init(&ctx.cond,NULL);
ctx.n=0;

  libvlc_video_set_callbacks(mp, lock, unlock, display, &ctx);
  if (colours==1) {
  Info( "Iniciando  Captura mod grey" );
     libvlc_video_set_format(mp, "GREY", width, height, width* colours);
  }
  else if (colours==3) {
  Info( "Iniciando  Captura mod 24" );
    libvlc_video_set_format(mp, "RV24", width, height, width* colours);
  }
  else if (colours==4) {
  Info( "Iniciando  Captura mod 32" );
     libvlc_video_set_format(mp, "RV32", width, height, width* colours);
  }
  else {
  Info( "IF RUIM!!!!!");
  }
  libvlc_media_player_play(mp);

    return( 0 );
}

int RemoteCameraVlc::Disconnect()
{
  libvlc_media_player_stop(mp);
  libvlc_media_player_release(mp);
  libvlc_release(libvlc);

  return( 0 );
}

int RemoteCameraVlc::PrimeCapture()
{

    return( 0 );
}

int RemoteCameraVlc::PreCapture()
{
    return( 0 );
}

int RemoteCameraVlc::Capture( Image &image )
{
pthread_mutex_lock(&ctx.mutex);

while (ctx.n==0) {
   pthread_cond_wait(&ctx.cond,&ctx.mutex);  
}

//    uint8_t * img = image.WriteBuffer(p_width,p_height,ZM_COLOUR_RGB24,ZM_SUBPIX_ORDER_ARGB);
//    memcpy(img,ctx.pixel,p_width * p_height * 3);
  image.Assign(width,height,colours,ZM_SUBPIX_ORDER_RGB,ctx.pixel,width*height*colours);
//Box b(10,10,200,200);
//image.Fill(RGB_ORANGE,&b);
// image.WriteJpeg("/tmp/image.jpg");
pthread_mutex_unlock(&ctx.mutex);
ctx.n=0;
    return (0) ;
}

int RemoteCameraVlc::PostCapture()
{
   return 0;
}
