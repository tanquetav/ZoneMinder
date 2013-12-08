//
// ZoneMinder Remote RTSP Camera Class Interface, $Date$, $Revision$
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

#ifndef ZM_REMOTE_CAMERA_VLC_H
#define ZM_REMOTE_CAMERA_VLC_H

#include "zm_remote_camera.h"

#include "zm_buffer.h"
#include "zm_utils.h"

#include "vlc/vlc.h"

//
// Class representing 'remote' cameras, i.e. those which are
// accessed over a network connection.
//
class RemoteCameraVlc ;

struct context {
    uint8_t* pixel;
    RemoteCameraVlc * rcl;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int n;
};

class RemoteCameraVlc : public RemoteCamera
{
protected:
	struct sockaddr_in rtsp_sa;
	struct sockaddr_in rtcp_sa;
	int rtsp_sd;
	int rtp_sd;
	int rtcp_sd;

	Buffer buffer;


    int frameCount;

    libvlc_instance_t *libvlc;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;

    
    struct context  ctx;

    static void *lock(void *data, void **p_pixels) ;
    static void unlock(void *data, void *id, void *const *p_pixels);
    static void display(void *data, void *id) ;

public:
	RemoteCameraVlc( int p_id,  const std::string &path, int p_width, int p_height, int p_colours, int p_brightness, int p_contrast, int p_hue, int p_colour, bool p_capture );
	~RemoteCameraVlc();

	void Initialise();
	void Terminate();
    int Connect();
    int Disconnect();

	int PrimeCapture();
	int PreCapture();
	int Capture( Image &image );
	int PostCapture();
        std::string path;
};

#endif // ZM_REMOTE_CAMERA_VLC_H
