#include "stdafx.h"

#include <stdexcept>
#include <rxmaptools/RXBitmap.h>

#ifndef RX_NO_JPEG

extern "C"
{
#ifdef _WIN32
#include <libjpeg/jpeglib.h>
#include <libjpeg/jerror.h>
#pragma comment(lib, "libjpeg")
#else
//Linux build
#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>
#endif
}



//chrisbk - Follow some ugly code I ripped from one of my old application
//I'm too lazy to try to do it better right now.

//Please, forgive. I was young (or maybe it is just libjpeg that looks so bad ?)

struct SJPEGDestManager
{
	struct jpeg_destination_mgr pub;

	std::ostream* outfile;		
	JOCTET * buffer;		
};



#define OUTPUT_BUF_SIZE  4096	


METHODDEF(void) init_destination (j_compress_ptr cinfo)
{
	SJPEGDestManager * dest = (SJPEGDestManager *) cinfo->dest;

	dest->buffer = (JOCTET *)     (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		OUTPUT_BUF_SIZE * sizeof(JOCTET));

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}




METHODDEF(boolean) empty_output_buffer (j_compress_ptr cinfo)
{
	SJPEGDestManager * dest = (SJPEGDestManager *) cinfo->dest;


	dest->outfile->write((const char *)dest->buffer,OUTPUT_BUF_SIZE);
	if (dest->outfile->fail())
		ERREXIT(cinfo, JERR_FILE_WRITE);

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

	return TRUE;
}




METHODDEF(void) term_destination (j_compress_ptr cinfo)
{
	SJPEGDestManager * dest = (SJPEGDestManager *) cinfo->dest;
	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;


	if (datacount > 0) {
		dest->outfile->write((const char *)dest->buffer, (std::streamsize)datacount);
		
		if (dest->outfile->fail())
			ERREXIT(cinfo, JERR_FILE_WRITE);
	}

}




METHODDEF(void) jpeg_flow_dest (j_compress_ptr cinfo, std::ostream * outfile)
{
	SJPEGDestManager * dest;


	if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(SJPEGDestManager));
	}

	dest = (SJPEGDestManager *) cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outfile = outfile;
}













void RXBitmap::saveToJPEG(const std::string &dest, int compression)  const
{
	std::ofstream str(dest.c_str(),std::ios::out | std::ios::binary | std::ios::trunc);
	if (str.is_open()==false)
		throw std::runtime_error("Could not save to  : " + dest);

	saveToJPEG(str, compression);

	str.close();
}

/*! \brief save the pic in JPG format
*  \param out : flow where to store the JPG
*  \param quality  : 0 = awful , 127 = best quality
*  \return 0 if OK, -1 otherwise
*/

static void jpegError(j_common_ptr ptr)
{
	throw std::runtime_error("error while writing to jpeg");
}

void RXBitmap::saveToJPEG(std::ostream &out, int compression)  const
{


	if(compression<0)
		compression = 0;
	else
	{
		//we take a value mapped between 0-100 and libjpeg takes one between 1-127
		compression = (int) ( (float)compression * 1.27f);

		if (compression > 127)
			compression = 127;
	}


	jpeg_compress_struct ccinfo;
	jpeg_error_mgr jerr;    

	ccinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&ccinfo);

	try
	{
		ccinfo.err->error_exit = jpegError;
		jpeg_flow_dest(&ccinfo, &out);

		// Pass image details to libjpeg
		ccinfo.image_width = width;             // Set width + height
		ccinfo.image_height = height;
		ccinfo.input_components = 3;              // Set bits per pixel
		ccinfo.in_color_space = JCS_RGB;          // RGB format

		// Set all options to the default
		jpeg_set_defaults(&ccinfo);

		// Change the settings depending on our defaults (or user parameters)
		jpeg_set_quality(&ccinfo, (int)compression, true);

		// Write header to file
		jpeg_start_compress(&ccinfo, true);



		// Temporary pointer to current scanline

		// If pixel format needs to be flipped
		unsigned char *data;
		data = new unsigned char[width*3];


		const unsigned char *image  = (const unsigned char *)color;


		int a;
		for (int i=0; i < height; i++)
		{
			a=0;
			for (int j=0;j<width*4;j+=4)
			{
				data[a]   = image[j+2];
				data[a+1] = image[j+1];
				data[a+2] = image[j+0];
				a+=3;
			}
			jpeg_write_scanlines(&ccinfo, &data, 1);
			image += (width*4);
		}

		delete []data;
		jpeg_finish_compress(&ccinfo);
		jpeg_destroy_compress(&ccinfo);
	}
	catch(...)
	{
		jpeg_destroy_compress(&ccinfo);
		throw;
	}
}



#else 

//If no libjpeg, do nothing
void RXBitmap::saveToJPEG(const std::string &destName, int compression) const
{
	throw std::runtime_error("Not implemented - no libjpeg support provided");
}

void RXBitmap::saveToJPEG(std::ostream &out, int compression)  const
{
	throw std::runtime_error("Not implemented - no libjpeg support provided");
}
#endif

