/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Module:    $RCSfile$
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "mitkPicFileReader.h"
#include <itkImageFileReader.h>

//##ModelId=3E1878400265
void mitk::PicFileReader::GenerateOutputInformation()
{
    mitk::Image::Pointer output = this->GetOutput();

    if ((output->IsInitialized()) && (this->GetMTime() <= m_ReadHeaderTime.GetMTime()))
        return;

    itkDebugMacro(<<"Reading file for GenerateOutputInformation()" << m_FileName);

    // Check to see if we can read the file given the name or prefix
    //
    if ( m_FileName == "" && m_FilePrefix == "" )
    {
        throw itk::ImageFileReaderException(__FILE__, __LINE__, "One of FileName or FilePrefix must be non-empty");
    }

    if( m_FileName != "")
    {
        ipPicDescriptor* header=ipPicGetHeader(const_cast<char *>(m_FileName.c_str()), NULL);

        header=ipPicGetTags(const_cast<char *>(m_FileName.c_str()), header);

        if( header == NULL)
        {
            itk::ImageFileReaderException e(__FILE__, __LINE__);
            itk::OStringStream msg;
            msg << " Could not read file "
                << m_FileName.c_str();
            e.SetDescription(msg.str().c_str());
            throw e;
            return;
        }

        output->Initialize(header);
    }
    else
    {
        int numberOfImages=0;
        m_StartFileIndex=0;

        ipPicDescriptor* header=NULL;

        char fullName[1024];

        while(m_StartFileIndex<10)
        {
            sprintf(fullName, m_FilePattern.c_str(), m_FilePrefix.c_str(), m_StartFileIndex+numberOfImages);
            FILE * f=fopen(fullName,"r");
            if(f==NULL) 
            {
                //already found an image?
                if(numberOfImages>0)
                    break;
                //no? let's increase start
                ++m_StartFileIndex;
            }
            else
            {
                fclose(f);
                //only open the header of the first file found,
                //@warning what to do when images do not have the same size??
                if(header==NULL) 
                {
                    header=ipPicGetHeader(fullName, NULL);
                    header=ipPicGetTags(fullName, header);
                }
                ++numberOfImages;
            }
        }

        printf("\n numberofimages %d\n",numberOfImages);

        if(numberOfImages==0)
        {
            itk::ImageFileReaderException e(__FILE__, __LINE__);
            itk::OStringStream msg;
            msg << "no images found";
            e.SetDescription(msg.str().c_str());
            throw e;
            return;
        }

        //@FIXME: was ist, wenn die Bilder nicht alle gleich gross sind?
        if(numberOfImages>1)
        {  
            printf("\n numberofimages %d > 1\n",numberOfImages);
            header->dim=3;
            header->n[2]=numberOfImages;
        }

        printf(" \ninitialisize output\n");
        output->Initialize(header);
    }

    m_ReadHeaderTime.Modified();
}

void mitk::PicFileReader::ConvertHandedness(ipPicDescriptor* pic)
{
  //left to right handed conversion
  if(pic->dim>=3)
  {
      ipPicDescriptor* slice=ipPicCopyHeader(pic, NULL);
      slice->dim=2;
      int size=_ipPicSize(slice);
      slice->data=malloc(size);

      int v, volumes = (pic->dim>3? pic->n[3] : 1);
      int volume_size = size*pic->n[2];

      for(v=0; v<volumes; ++v)
      {
        unsigned char *p_first=(unsigned char *)pic->data;
        unsigned char *p_last=(unsigned char *)pic->data;
        p_first+=v*volume_size;
        p_last+=size*(pic->n[2]-1)+v*volume_size;

        int i, smid=pic->n[2]/2;
        for(i=0; i<smid;++i,p_last-=size, p_first+=size)
        {
            memcpy(slice->data, p_last, size);
            memcpy(p_last, p_first, size);
            memcpy(p_first, slice->data, size);
        }
      }
      ipPicFree(slice);
  }
}

//##ModelId=3E187255016C
void mitk::PicFileReader::GenerateData()
{
    mitk::Image::Pointer output = this->GetOutput();

    // Check to see if we can read the file given the name or prefix
    //
    if ( m_FileName == "" && m_FilePrefix == "" )
    {
        throw itk::ImageFileReaderException(__FILE__, __LINE__, "One of FileName or FilePrefix must be non-empty");
    }

    if( m_FileName != "")
    {
        ipPicDescriptor* pic=ipPicGet(const_cast<char *>(m_FileName.c_str()), NULL);
        ConvertHandedness(pic);
        output->SetPicChannel(pic);
        ipPicFree(pic);

        //slice-wise reading
        //currently much too slow.
        //else
        //{
        //  int sstart, smax;
        //  int tstart, tmax;

        //  sstart=output->GetRequestedRegion().GetIndex(2);
        //  smax=sstart+output->GetRequestedRegion().GetSize(2);

        //  tstart=output->GetRequestedRegion().GetIndex(3);
        //  tmax=tstart+output->GetRequestedRegion().GetSize(3);

        //  int s,t;
        //  for(s=sstart; s<smax; ++s)
        //  {
        //    for(t=tstart; t<tmax; ++t)
        //    {
        //      ipPicDescriptor* pic=ipPicGetSlice(const_cast<char *>(m_FileName.c_str()), NULL, t*smax+s+1);
        //      output->SetPicSlice(pic,s,t);
        //    }
        //  }
        //}
    }
    else
    {
        int position;
        ipPicDescriptor*  pic=NULL;

        int zDim=(output->GetDimension()>2?output->GetDimensions()[2]:1);
        printf("\n zdim is %u \n",zDim);

        for (position = 0; position < zDim; ++position) 
        {
            char fullName[1024];

            sprintf(fullName, m_FilePattern.c_str(), m_FilePrefix.c_str(), m_StartFileIndex+position);

            pic=ipPicGet(fullName, pic);
            if(pic==NULL)
            {
                itkDebugMacro("Pic file '" << fullName << "' does not exist."); 
            }
            else
            if(output->SetPicSlice(pic, position)==false)
            {
                itkDebugMacro("Image '" << fullName << "' could not be added to Image."); 
            }
       }
       if(pic!=NULL)
         ipPicFree(pic);
    }
}

void mitk::PicFileReader::EnlargeOutputRequestedRegion(itk::DataObject *output)
{
  output->SetRequestedRegionToLargestPossibleRegion();
}

//##ModelId=3E1874D202D0
mitk::PicFileReader::PicFileReader()
    : m_FileName(""), m_FilePrefix(""), m_FilePattern("")
{
}

//##ModelId=3E1874E50179
mitk::PicFileReader::~PicFileReader()
{
}
